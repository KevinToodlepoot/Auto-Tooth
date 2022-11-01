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
    float resonance {0};
    
    /* Secondary Section */
    float curve {0};
    float spread {0};
    
    float retuneSpeed {0};
    int key {0};
};

inline ChainSettings getChainSettings(APVTS& apvts)
{
    ChainSettings settings;
    
    //==============================================================================
    settings.timbre = apvts.getRawParameterValue("Timbre")->load();
    settings.resonance = apvts.getRawParameterValue("Resonance")->load();
    
    //==============================================================================
    settings.curve = apvts.getRawParameterValue("Curve")->load();
    settings.spread = apvts.getRawParameterValue("Spread")->load();
    
    settings.retuneSpeed = apvts.getRawParameterValue("Retune Speed")->load();
    settings.key = apvts.getRawParameterValue("Key")->load();
    
    return settings;
}

static APVTS::ParameterLayout createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;
    
    auto pTimbre = std::make_unique<AudioParameterFloat>
        (ParameterID ("Timbre", 1), "Timbre", TIMBRE_MIN, TIMBRE_MAX, TIMBRE_DEFAULT);
    auto pResonance = std::make_unique<AudioParameterFloat>
        (ParameterID ("Resonance", 1), "Resonance", RESONANCE_MIN, RESONANCE_MAX, RESONANCE_DEFAULT);
    auto pCurve = std::make_unique<AudioParameterFloat>
        (ParameterID ("Curve", 1), "Curve", NormalisableRange<float>(CURVE_MIN, CURVE_MAX, 0.01f, 4.0f), CURVE_DEFAULT);
    auto pSpread = std::make_unique<AudioParameterFloat>
        (ParameterID ("Spread", 1), "Spread", SPREAD_MIN, SPREAD_MAX, SPREAD_DEFAULT);
    
    auto pRetuneSpeed = std::make_unique<AudioParameterFloat>
        (ParameterID ("Retune Speed", 1), "Retune Speed", RETUNE_MIN, RETUNE_MAX, RETUNE_DEFAULT);
    
    AudioParameterIntAttributes keyAttributes;
    auto pKey = std::make_unique<AudioParameterInt>
        (ParameterID ("Key", 1), "Key", 0, 11, 0, keyAttributes.withAutomatable(false));
        
    params.push_back(std::move(pTimbre));
    params.push_back(std::move(pResonance));
    params.push_back(std::move(pCurve));
    params.push_back(std::move(pSpread));
    params.push_back(std::move(pRetuneSpeed));
    params.push_back(std::move(pKey));
    
    return { params.begin(), params.end() };
}

#endif
