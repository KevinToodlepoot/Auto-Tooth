/*
  ==============================================================================

    config.h
    Created: 7 Oct 2022 2:56:48pm
    Author:  Kevin Kopczynski

  ==============================================================================
*/

#ifndef CONFIG_H
#define CONFIG_H

// PITCH DEFINES
#define C4_FREQ             261.626f
#define Db4_FREQ            277.183f
#define D4_FREQ             293.665f
#define Eb4_FREQ            311.127f
#define E4_FREQ             329.628f
#define F4_FREQ             349.228f
#define Gb4_FREQ            369.994f
#define G4_FREQ             391.995f
#define Ab4_FREQ            415.305f
#define A4_FREQ             440.0f
#define Bb4_FREQ            466.164f
#define B4_FREQ             493.883f

namespace audio
{
    using SIMD = FloatVectorOperations;
    using APVTS = AudioProcessorValueTreeState;

    static float Pi = MathConstants<float>::pi;
    static float Tau = MathConstants<float>::twoPi;

    template<typename Float>
    inline float msInSamples(Float ms, Float Fs) noexcept
    {
        return ms * Fs * static_cast<Float>(.001);
    }

    template<typename Float>
    inline float pitchToScalar(Float pitch) noexcept
    {
        return pow(2.f, pitch / 12.f);
    }
}

#define LEFT                0
#define RIGHT               0
#define MAX_NUM_FILTERS     200
#define SMOOTH_SEC          0.005f

// PARAMETER DEFINES
#define TIMBRE_MIN          0.0f
#define TIMBRE_MAX          1.0f
#define TIMBRE_DEFAULT      0.5f
#define FREQ_MIN            20.0f
#define FREQ_MAX            10000.0f
#define FREQ_DEFAULT        440.0f
#define RESONANCE_MIN       20.0f
#define RESONANCE_MAX       200.0f
#define RESONANCE_DEFAULT   60.0f
#define SPREAD_MIN          0.5f
#define SPREAD_MAX          2.0f
#define SPREAD_DEFAULT      1.0f
#define CURVE_MIN           -12.0f
#define CURVE_MAX           0.0f
#define CURVE_DEFAULT       -1.0f
#define RETUNE_MIN          0.01f
#define RETUNE_MAX          100.0f
#define RETUNE_DEFAULT      10.0f

#endif
