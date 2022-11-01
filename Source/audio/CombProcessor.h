/*
  ==============================================================================

    CombProcessor.h
    Created: 7 Oct 2022 2:54:57pm
    Author:  Kevin Kopczynski

  ==============================================================================
*/

#ifndef COMBPROCESSOR_H
#define COMBPROCESSOR_H

#include <JuceHeader.h>
#include "../config.h"

namespace audio
{

class CombProcessor
{
public:
    CombProcessor(unsigned int _maxNumFilters, int _numChannels);
    
    void prepare(const dsp::ProcessSpec& spec);
    void reset();
    void process(AudioBuffer<float> &buffer, int numSamples);
    void updateParams(float freq, float q, float timbre, float curve, float detune);
    
private:
    void updateFilterSettings(int i);
    void updateCurve(int i);
    void updateTimbre(int i);
    float getCurFreq(int i);
    
    using SVFilter = dsp::StateVariableTPTFilter<float>;
    using Gain = dsp::Gain<float>;
    using Chain = dsp::ProcessorChain<SVFilter, Gain, Gain>;
    std::vector<std::array<Chain, MAX_NUM_FILTERS>> chain;
    
    enum ChainPositions
    {
        Filter,
        Timbre,
        Curve
    };
    
    AudioBuffer<float> tempBuffer, outBuffer;
    float freq, q, timbre, curve, detune;
    unsigned int maxNumFilters;
    int numChannels, numFilters;
    double sampleRate;
};

}

#endif
