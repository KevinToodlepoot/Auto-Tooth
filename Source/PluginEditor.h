/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GUI/FilterDisplay.h"
#include "GUI/FineToothLNF.h"
#include "GUI/MultiChoiceButton.h"


class CustomRotarySlider : public Slider
{
public:
    CustomRotarySlider() : Slider(Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                                  Slider::TextEntryBoxPosition::NoTextBox)
    {
        setLookAndFeel(&lnf);
    }
    
    ~CustomRotarySlider()
    {
        setLookAndFeel(nullptr);
    }
    
private:
    FineToothLNF lnf;
};

//==============================================================================
/**
*/
class TheFineToothAudioProcessorEditor  : public juce::AudioProcessorEditor, public Timer
{
public:
    TheFineToothAudioProcessorEditor (TheFineToothAudioProcessor&);
    ~TheFineToothAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void timerCallback() override;

private:
    std::vector<Component*> getComps();
    
    FineToothLNF lookAndFeel;
    MultiChoiceButton keyButton;
    FilterDisplay filterDisplay;
    CustomRotarySlider timbre, resonance, curve, spread, retuneSpeed;
    APVTS::SliderAttachment timbreAttachment, resonanceAttachment, curveAttachment, spreadAttachment, retuneAttachment;
    
    class CustomFontLookAndFeel : public juce::LookAndFeel_V4
    {
    public:
        CustomFontLookAndFeel()
        {
            // without this custom Fonts won't work!!
            juce::LookAndFeel::setDefaultLookAndFeel (this);

            // This can be used as one way of setting a default font
            setDefaultSansSerifTypeface (getCustomFont().getTypefacePtr());
            
        }

        static const juce::Font getCustomFont()
        {
            static auto typeface = juce::Typeface::createSystemTypefaceFor (BinaryData::ComfortaaRegular_ttf, BinaryData::ComfortaaRegular_ttfSize);
            return juce::Font (typeface);
        }

        juce::Typeface::Ptr getTypefaceForFont (const juce::Font& f) override
        {
            // This can be used to fully change/inject fonts.
            // For example: return different TTF/OTF based on weight of juce::Font (bold/italic/etc)
            return getCustomFont().getTypefacePtr();
        }
    private:
    } customLookAndFeel;
    
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    TheFineToothAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TheFineToothAudioProcessorEditor)
};
