/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TheFineToothAudioProcessorEditor::TheFineToothAudioProcessorEditor (TheFineToothAudioProcessor& p)
    : AudioProcessorEditor (&p),
    keyButton(StringArray("C Maj", "C# Maj", "D Maj", "D# Maj", "E Maj", "F Maj", "F# Maj",
                          "G Maj", "G# Maj", "A Maj", "A# Maj", "B Maj"), p),
    filterDisplay(p),
    timbreAttachment(p.apvts, "Timbre", timbre),
    resonanceAttachment(p.apvts, "Resonance", resonance),
    curveAttachment(p.apvts, "Curve", curve),
    spreadAttachment(p.apvts, "Spread", spread),
    retuneAttachment(p.apvts, "Retune Speed", retuneSpeed),
    audioProcessor (p)
{
    for ( auto* comp : getComps() )
        addAndMakeVisible(comp);
    
    startTimerHz(60);
    
    setSize (700, 500);
}

TheFineToothAudioProcessorEditor::~TheFineToothAudioProcessorEditor()
{
}

//==============================================================================
void TheFineToothAudioProcessorEditor::paint (juce::Graphics& g)
{
    ColourGradient gradient(Colour(20, 116, 111), Point<float>(getWidth() * 0.5f, getHeight() * 0.75f), Colour(3, 102, 102), Point<float>(getWidth() * 0.5f, getHeight() * 0.25f), false);
    g.setGradientFill(gradient);
    
    g.fillAll ();
        
    g.setColour(Colour(86, 171, 145));
    
    auto bounds = getLocalBounds().reduced(20.0f);
    auto titleBounds = bounds.removeFromTop(bounds.getHeight() * 0.1f);
    bounds.removeFromTop(bounds.getHeight() * 0.08f); // keyBounds
    bounds.removeFromTop(bounds.getHeight() * 0.5f); // displayBounds
    auto mainBounds = bounds.removeFromTop(bounds.getHeight() * 0.5f).reduced(10.0f);
    auto mainLabelBounds = mainBounds.removeFromTop(mainBounds.getHeight() * 0.1f);
    auto secondaryBounds = bounds.reduced(10.0f);
    auto secondaryLabelBounds = secondaryBounds.removeFromTop(secondaryBounds.getHeight() * 0.1f);
    
    g.setFont(36.0f);
    g.drawText("Auto-Tooth", titleBounds, Justification::centredTop);
    
    g.setFont(18.0f);
    g.drawText("Timbre", mainLabelBounds.removeFromLeft(mainLabelBounds.getWidth() * 0.5f), Justification::centredBottom);
    g.drawText("Resonance", mainLabelBounds, Justification::centredBottom);
    
    g.drawText("Curve", secondaryLabelBounds.removeFromLeft(secondaryLabelBounds.getWidth() * 0.33f), Justification::centredBottom);
    g.drawText("Spread", secondaryLabelBounds.removeFromLeft(secondaryLabelBounds.getWidth() * 0.5f), Justification::centredBottom);
    g.drawText("Retune Speed", secondaryLabelBounds, Justification::centredBottom);
}

void TheFineToothAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds().reduced(20.0f);
    bounds.removeFromTop(bounds.getHeight() * 0.1f); // titleBounds
    auto keyBounds = bounds.removeFromTop(bounds.getHeight() * 0.08f);
    auto displayBounds = bounds.removeFromTop(bounds.getHeight() * 0.5f);
    auto mainBounds = bounds.removeFromTop(bounds.getHeight() * 0.5f).reduced(10.0f);
    mainBounds.removeFromTop(mainBounds.getHeight() * 0.1f); // mainLabelBounds
    auto secondaryBounds = bounds.reduced(10.0f);
    secondaryBounds.removeFromTop(secondaryBounds.getHeight() * 0.1f); // secondaryLabelBounds
    
    keyButton.setBounds(keyBounds);
    filterDisplay.setBounds(displayBounds.reduced(10.0f));
    timbre.setBounds(mainBounds.removeFromLeft(mainBounds.getWidth() * 0.5f));
    resonance.setBounds(mainBounds);
    curve.setBounds(secondaryBounds.removeFromLeft(secondaryBounds.getWidth() * 0.33f));
    spread.setBounds(secondaryBounds.removeFromLeft(secondaryBounds.getWidth() * 0.5f));
    retuneSpeed.setBounds(secondaryBounds);
}

void TheFineToothAudioProcessorEditor::timerCallback()
{
    filterDisplay.updateAll();
}

std::vector<Component*> TheFineToothAudioProcessorEditor::getComps()
{
    return
    {
        &keyButton,
        &filterDisplay,
        &timbre,
        &resonance,
        &curve,
        &spread,
        &retuneSpeed
    };
}
