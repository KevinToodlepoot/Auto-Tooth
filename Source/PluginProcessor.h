/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "audio/CombProcessor.h"
#include "config.h"
#include "Params.h"

using namespace audio;

//==============================================================================
/**
*/
class TheFineToothAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    TheFineToothAudioProcessor();
    ~TheFineToothAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    APVTS apvts;

private:
    //==============================================================================
    float snapFreq(float freq);
    void updateTonic(const ChainSettings& settings);
    void updateRetune(const ChainSettings& settings);
    
    //==============================================================================
    std::unique_ptr<CombProcessor> filterProcessor;
    
    AudioBuffer<float> tempBuffer1;
    SmoothedValue<float, ValueSmoothingTypes::Multiplicative> freqParam;
    std::array<float, 7> pitchArray;
    float tonic = A4_FREQ, lastFc = A4_FREQ, lastRetune = -1.0f;
    
    std::unique_ptr<adamski::PitchMPM> pitchMPM;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TheFineToothAudioProcessor)
};
