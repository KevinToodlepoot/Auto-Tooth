#ifndef MULTICHOICEBUTTON_H
#define MULTICHOICEBUTTON_H

#include <JuceHeader.h>
#include "FineToothLNF.h"

//==============================================================================
/*
*/
class MultiChoiceButton  : public Component
{
public:
    MultiChoiceButton(StringArray buttonTexts, TheFineToothAudioProcessor& p) : audioProcessor(p)
    {
        for (auto& button : buttons)
        {
            button.setRadioGroupId (293847);
            button.setClickingTogglesState (true);
        }
        
        auto settings = getChainSettings(audioProcessor.apvts);
        
        buttons[0].setConnectedEdges (Button::ConnectedOnRight);
        buttons[1].setConnectedEdges (3);
        buttons[2].setConnectedEdges (Button::ConnectedOnLeft);
        
        for (int i = 0; i < numButtons; ++i)
        {
            if (i == 0)
                buttons[i].setConnectedEdges (Button::ConnectedOnRight);
            else if (i == numButtons - 1)
                buttons[i].setConnectedEdges (Button::ConnectedOnLeft);
            else
                buttons[i].setConnectedEdges (3);
            
            buttons[i].setButtonText (buttonTexts[i]);
            buttons[i].setToggleState (settings.key == i, dontSendNotification);
            buttons[i].setLookAndFeel (&lookAndFeel);
            
            buttons[i].onClick = [this, i] { buttonClicked(i); };
        }
        
        for (auto& button : buttons)
            addAndMakeVisible (button);
    }

    ~MultiChoiceButton() override
    {
        for (int i = 0; i < numButtons; ++i)
            buttons[i].setLookAndFeel(nullptr);
    }

    void paint (juce::Graphics& g) override
    {
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        
        for (int i = 0; i < numButtons; ++i)
            buttons[i].setBounds(bounds.removeFromLeft(bounds.getWidth() * (1.0f / (numButtons - i))));
    }
    
    void buttonClicked(int i)
    {
        auto buttonState = buttons[i].getToggleState();
        
        if (buttonState)
        {
            for (int b = 0; b < numButtons; ++b)
            {
                if (b != i)
                    buttons[b].setToggleState(! buttonState, dontSendNotification);
            }
            
            audioProcessor.apvts.getParameter("Key")->setValue(i);
        }
    }

private:
    TextButton buttons[12];
    
    FineToothLNF lookAndFeel;
    
    TheFineToothAudioProcessor& audioProcessor;
    
    static constexpr int numButtons = 12;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MultiChoiceButton)
};

#endif // MULTICHOICEBUTTON_H
