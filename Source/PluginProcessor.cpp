/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TheFineToothAudioProcessor::TheFineToothAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts(*this, nullptr, "PARAMETERS", createParameterLayout())
#endif
{
}

TheFineToothAudioProcessor::~TheFineToothAudioProcessor()
{
}

//==============================================================================
const juce::String TheFineToothAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool TheFineToothAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool TheFineToothAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool TheFineToothAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double TheFineToothAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int TheFineToothAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int TheFineToothAudioProcessor::getCurrentProgram()
{
    return 0;
}

void TheFineToothAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String TheFineToothAudioProcessor::getProgramName (int index)
{
    return {};
}

void TheFineToothAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void TheFineToothAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // contstruct the filter processor
    filterProcessor = std::make_unique<CombProcessor>(MAX_NUM_FILTERS, getTotalNumOutputChannels());
    
    // prepare the filter processor
    dsp::ProcessSpec spec;
    spec.numChannels = 1;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    filterProcessor->prepare(spec);
    
    // initialize filter params
    filterProcessor->updateParams(CombProcessor::Parameters(A4_FREQ, RESONANCE_DEFAULT, TIMBRE_DEFAULT, CURVE_DEFAULT, SPREAD_DEFAULT, RETUNE_DEFAULT));
    
    // initialize buffers
    tempBuffer1.setSize(getTotalNumOutputChannels(), samplesPerBlock);
    auto data1 = tempBuffer1.getArrayOfWritePointers();
    for (int ch = 0; ch < getTotalNumOutputChannels(); ++ch)
    {
        SIMD::fill(data1[ch], 0.0f, samplesPerBlock);
    }
    
    // pitch tracking setup
    pitchMPM = std::make_unique<adamski::PitchMPM>((int)sampleRate, samplesPerBlock);
    
    freqParam.reset(sampleRate, 0.005);
    freqParam.setCurrentAndTargetValue(FREQ_DEFAULT);
    
    // initialize retune and tonic
    auto settings = getChainSettings(apvts);
    updateTonic(settings);
    updateRetune(settings);
    
    // initialize pitch array
    pitchArray = {0.0f, 2.0f, 4.0f, 5.0f, 7.0f, 9.0f, 11.0f};
    
    // initialize last freq
    lastFc = tonic;
}

void TheFineToothAudioProcessor::releaseResources()
{
    if (filterProcessor)
        filterProcessor->reset();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TheFineToothAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void TheFineToothAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    int numSamples = buffer.getNumSamples();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    auto settings = getChainSettings(apvts);
    updateTonic(settings);
    updateRetune(settings);
    
    float pitch = pitchMPM->getPitch(buffer.getReadPointer(0));
    
    float fc;
    if (pitch < 0)
        fc = lastFc;
    else
        fc = snapFreq(pitch);
    
    freqParam.skip(numSamples - 1);
    
    filterProcessor->updateParams(CombProcessor::Parameters(fc, settings.resonance, settings.timbre, settings.curve, settings.spread, settings.retuneSpeed));
    
    // update temp buffers
    auto data1 = tempBuffer1.getArrayOfWritePointers();
    auto outData = buffer.getArrayOfWritePointers();
    for (int ch = 0; ch < totalNumOutputChannels; ++ch)
    {
        SIMD::copy(data1[ch], outData[ch], numSamples);
    }
    
    filterProcessor->process(tempBuffer1, numSamples);
    
    for (int ch = 0; ch < totalNumOutputChannels; ++ch)
    {
        SIMD::copy(outData[ch], data1[ch], numSamples);
    }
    
    lastFc = fc;
}

float TheFineToothAudioProcessor::snapFreq(float freq)
{
    int octave = 0;
    
    // !!!round to nearest interval in pitch array!!!
    float interval = log2(freq / tonic) * 12.0f;
//    DBG(interval);
    
    while ( ! (interval >= -0.5f && interval <= 11.5f))
    {
        if (interval < -0.5f)
        {
            interval += 12.f;
            octave--;
        }
        else if (interval > 11.5f)
        {
            interval -= 12.f;
            octave++;
        }
    }
    
    float snapInt = *std::min_element(pitchArray.begin(), pitchArray.end(),
                               [interval](float a, float b)
                               { return std::abs(a - interval) < std::abs(b - interval); });

    snapInt += (octave * 12.0f);
    
    return tonic * pow(2.0f, snapInt / 12.0f);
}

void TheFineToothAudioProcessor::updateTonic(const ChainSettings &settings)
{
    tonic = keyFreqs[settings.key];
}

void TheFineToothAudioProcessor::updateRetune(const ChainSettings &settings)
{
    if (settings.retuneSpeed != lastRetune)
    {
        freqParam.reset(getSampleRate(), settings.retuneSpeed / 1000.0f);
        freqParam.setCurrentAndTargetValue(lastFc);
        
        lastRetune = settings.retuneSpeed;
    }
}

float TheFineToothAudioProcessor::getFreq()
{
    return lastFc;
}

//==============================================================================
bool TheFineToothAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* TheFineToothAudioProcessor::createEditor()
{
    return new TheFineToothAudioProcessorEditor (*this);
//    return new GenericAudioProcessorEditor (*this);
}

//==============================================================================
void TheFineToothAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void TheFineToothAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TheFineToothAudioProcessor();
}
