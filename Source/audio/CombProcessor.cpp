/*
  ==============================================================================

    CombProcessor.cpp
    Created: 7 Oct 2022 2:54:57pm
    Author:  Kevin Kopczynski

  ==============================================================================
*/

#include "CombProcessor.h"

namespace audio
{

CombProcessor::CombProcessor(unsigned int _maxNumFilters, int _numChannels) :
    chain(_numChannels),
    maxNumFilters(_maxNumFilters),
    numChannels(_numChannels),
    numFilters(_maxNumFilters)
{
    freq = FREQ_DEFAULT;
    q = Q_DEFAULT;
    timbre = TIMBRE_DEFAULT;
    curve = CURVE_DEFAULT;
    detune = SPREAD_DEFAULT;
}

void CombProcessor::prepare(const dsp::ProcessSpec& spec)
{
    for (int ch = 0; ch < numChannels; ++ch)
    {
        for (int i = 0; i < maxNumFilters; ++i)
        {
            chain[ch][i].prepare(spec);
            chain[ch][i].get<ChainPositions::Filter>().setType(dsp::StateVariableTPTFilterType::bandpass);
        }
    }
    
    sampleRate = spec.sampleRate;
    tempBuffer.setSize(numChannels, spec.maximumBlockSize);
    outBuffer.setSize(numChannels, spec.maximumBlockSize);
}

void CombProcessor::reset()
{
    for (int ch = 0; ch < numChannels; ++ch)
        for (int i = 0; i < maxNumFilters; ++i)
            chain[ch][i].reset();
    
}

void CombProcessor::process(AudioBuffer<float> &buffer, int numSamples)
{
    auto inWrite = buffer.getArrayOfWritePointers();
    auto tempWrite = tempBuffer.getArrayOfWritePointers();
    auto outWrite = outBuffer.getArrayOfWritePointers();
    
    // clear output buffer
    for (int ch = 0; ch < numChannels; ++ch)
        SIMD::fill(outWrite[ch], 0.0f, numSamples);
    
    // process bandpass filter for each harmonic
    for (int i = 0; i < numFilters; ++i)
    {
        // copy contents of input buffer to temp buffer
        for (int ch = 0; ch < numChannels; ++ch)
            SIMD::copy(tempWrite[ch], inWrite[ch], numSamples);
        
        // process the temp buffer and add it to output buffer
        dsp::AudioBlock<float> block(tempBuffer);
        for (int ch = 0; ch < numChannels; ++ch)
        {
            auto channelBlock = block.getSingleChannelBlock(ch);
            dsp::ProcessContextReplacing<float> context(channelBlock);
            chain[ch][i].process(context);
            
            // add to output buffer
            SIMD::add(outWrite[ch], tempWrite[ch], numSamples);
        }
    }
     
    // write contents of output buffer to input buffer
    for (int ch = 0; ch < numChannels; ++ch)
        SIMD::copy(inWrite[ch], outWrite[ch], numSamples);
}

void CombProcessor::updateParams(float freq, float q, float timbre, float curve, float detune)
{
    this->freq = freq;
    this->q = q;
    this->timbre = timbre;
    this->curve = curve;
    this->detune = detune;
    
    float nyquist = sampleRate / 2.0f;
    
    for (int i = 0; i < maxNumFilters; ++i)
    {
        if (getCurFreq(i) >= nyquist)
        {
            numFilters = i;
            return;
        }
    
        updateFilterSettings(i);
        updateTimbre(i);
        updateCurve(i);
    }
}





void CombProcessor::updateFilterSettings(int i)
{
    float curFreq, curQ;
    curFreq = getCurFreq(i);
    curQ = q * (float(i) / 2.f + 1);
    
    for (int ch = 0; ch < numChannels; ++ch)
    {
        chain[ch][i].get<ChainPositions::Filter>().setCutoffFrequency(curFreq);
        chain[ch][i].get<ChainPositions::Filter>().setResonance(curQ);
    }
}

void CombProcessor::updateTimbre(int i)
{
    float oddGain, evenGain;
    
    oddGain = timbre * -1.f + 1.f;
    evenGain = timbre;
    
    for (int ch = 0; ch < numChannels; ++ch)
    {
        if (i == 0)
            chain[ch][i].get<ChainPositions::Timbre>().setGainLinear(1.f);
        else if (i % 2 == 1)
            chain[ch][i].get<ChainPositions::Timbre>().setGainLinear(oddGain);
        else if (i % 2 == 0)
            chain[ch][i].get<ChainPositions::Timbre>().setGainLinear(evenGain);
    }
}

void CombProcessor::updateCurve(int i)
{
//    float gain, normCurveGain, curveQ;
//
//    curveQ = q * (float(i) / 2.f + 1);
//
//    // compensate gain for tighter q values because nonlinear filter
//    normCurveGain = pow((1.f/curveQ), 0.6);
//
//    gain = pow((-1.f / float(MAX_NUM_FILTERS)) * float(i) + 1.f, (1.f / curve));
//    gain *= normCurveGain;
//
//    for (int ch = 0; ch < numChannels; ++ch)
//        chain[ch][i].get<ChainPositions::Curve>().setGainLinear(gain);
    
    float gain, normCurveGain, curveQ;
    
    gain = Decibels::decibelsToGain( curve * (i + 1) );
    curveQ = q * (float(i) / 2.f + 1);
    normCurveGain = pow((1.f/curveQ), 0.6);
    gain *= normCurveGain;
    
    for (int ch = 0; ch < numChannels; ++ch)
        chain[ch][i].get<ChainPositions::Curve>().setGainLinear(gain);
}

float CombProcessor::getCurFreq(int i)
{
    if ( i )
        return freq * pow(float(i + 1), detune);
    else
        return freq;
}

}
