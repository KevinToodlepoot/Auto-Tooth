/*
  ==============================================================================

    Params.h
    Created: 7 Oct 2022 2:58:54pm
    Author:  Kevin Kopczynski

  ==============================================================================
*/

#ifndef PARAMS_H
#define PARAMS_H

#include "config.h"

using namespace audio;

struct ChainSettings
{
    /* Main Section */
    float timbre {0};
    float freq {0};
    float q {0};
    
    /* Secondary Section */
    float curve {0};
    float detune {0};
    
    float retuneSpeed {0};
    Key key {0};
};

inline ChainSettings getChainSettings(APVTS& apvts)
{
    ChainSettings settings;
    
    //==============================================================================
    settings.timbre = apvts.getRawParameterValue("Timbral Parity")->load();
    settings.freq = apvts.getRawParameterValue("Fundamental Frequency")->load();
    settings.q = apvts.getRawParameterValue("Resonance")->load();
    
    //==============================================================================
    settings.curve = apvts.getRawParameterValue("Magnitude Curve")->load();
    settings.detune = apvts.getRawParameterValue("Harmonic Spread")->load();
    
    settings.retuneSpeed = apvts.getRawParameterValue("Retune Speed")->load();
    settings.key = static_cast<Key>(apvts.getRawParameterValue("Key")->load());
    
    return settings;
}

static APVTS::ParameterLayout createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;
    
    auto pTimbre = std::make_unique<AudioParameterFloat>
        (ParameterID ("Timbral Parity", 1), "Timbral Parity", TIMBRE_MIN, TIMBRE_MAX, TIMBRE_DEFAULT);
    auto pFreq = std::make_unique<AudioParameterFloat>
        (ParameterID ("Fundamental Frequency", 1), "Fund Freq", NormalisableRange<float>(FREQ_MIN, FREQ_MAX, 1.0f, 0.25f), FREQ_DEFAULT);
    auto pQ = std::make_unique<AudioParameterFloat>
        (ParameterID ("Resonance", 1), "Resonance", Q_MIN, Q_MAX, Q_DEFAULT);
    auto pCurve = std::make_unique<AudioParameterFloat>
        (ParameterID ("Magnitude Curve", 1), "Curve", CURVE_MIN, CURVE_MAX, CURVE_DEFAULT);
    auto pDetune = std::make_unique<AudioParameterFloat>
        (ParameterID ("Harmonic Spread", 1), "Detune", SPREAD_MIN, SPREAD_MAX, SPREAD_DEFAULT);
    
    auto pRetuneSpeed = std::make_unique<AudioParameterFloat>
        (ParameterID ("Retune Speed", 1), "Retune Speed", RETUNE_MIN, RETUNE_MAX, RETUNE_DEFAULT);
    
    auto pKey = std::make_unique<AudioParameterChoice>
        (ParameterID ("Key", 1), "Key",
         StringArray("C Major", "Db Major", "D Major", "Eb Major", "E Major", "F Major",
                    "Gb Major", "G Major", "Ab Major", "A Major", "Bb Major", "B Major"), 0);
        
    params.push_back(std::move(pTimbre));
    params.push_back(std::move(pFreq));
    params.push_back(std::move(pQ));
    params.push_back(std::move(pCurve));
    params.push_back(std::move(pDetune));
    params.push_back(std::move(pRetuneSpeed));
    params.push_back(std::move(pKey));
    
    return { params.begin(), params.end() };
}

#endif
