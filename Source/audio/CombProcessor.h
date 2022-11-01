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

using SVFilter = dsp::StateVariableTPTFilter<float>;
using Gain = dsp::Gain<float>;
using Chain = dsp::ProcessorChain<SVFilter, Gain, Gain>;

enum ChainPositions
{
    Filter,
    Timbre,
    Curve
};

namespace audio
{

class CombProcessor
{
public:
    enum class FreqOutOfBoundsMode
    {
        Ignore,
        Wrap,
        Fold
    };
    
    struct Parameters
    {
        Parameters(float freq, float resonance, float timbre, float curve, float spread, float glide, FreqOutOfBoundsMode mode = FreqOutOfBoundsMode::Ignore)
        {
            this->freq = freq;
            this->resonance = resonance;
            this->timbre = timbre;
            this->curve = curve;
            this->spread = spread;
            this->glide = glide;
            this->mode = mode;
        }
        
        float freq, resonance, timbre, curve, spread, glide;
        FreqOutOfBoundsMode mode;
    };
    
    CombProcessor(unsigned int _maxNumFilters);
    CombProcessor(unsigned int _maxNumFilters, int _numChannels);
    ~CombProcessor() {;}
    
    void prepare(const dsp::ProcessSpec& spec);
    void reset();
    void process(AudioBuffer<float> &buffer, int numSamples, int startSample = 0);
    void updateParams(Parameters params);
    Parameters& getParams();
    void setFrequency(float freq);
    void setCurveOffset(float offset);
    
private:
    bool updateFilterSettings(float curFreq, float curQ, float curSpread, int i);
    void updateTimbre(float curTimbre, int i);
    void updateCurve(float curCurve, float curQ, int i);
    void updateParamsObject(float freq, float resonance, float timbre, float curve, float spread);
    
    std::vector< std::array<Chain, MAX_NUM_FILTERS> > chain;
    
    SmoothedValue<float, ValueSmoothingTypes::Multiplicative> freq, q, spread;
    SmoothedValue<float, ValueSmoothingTypes::Linear> timbre, curve;
    FreqOutOfBoundsMode mode;
    Parameters curParams;
    float lastGlide = RETUNE_DEFAULT / 1000.0f, glide = RETUNE_DEFAULT / 1000.0f;
    
    AudioBuffer<float> tempBuffer, outBuffer;
    unsigned int maxNumFilters;
    int numFilters;
    double sampleRate;
    int numChannels;
};

}

#endif
