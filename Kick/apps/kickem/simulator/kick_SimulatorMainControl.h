#pragma once

#include <kick_ui/kick_ui.h>

namespace sim
{
class SimulatorIntrefaceImpl;
class KickSimButton : public ui::KickButton
{
public: 
    KickSimButton(const juce::String& buttonName, const juce::String& buttonText)
        :KickButton(buttonName, buttonText)
    {
        setClickingTogglesState(true);
        setColour(juce::TextButton::buttonColourId, ui::Theme::backgroundColour(1));
        setColour(juce::TextButton::buttonOnColourId, ui::Theme::backgroundColour(1));
        setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        setColour(juce::TextButton::textColourOnId, juce::Colours::red);
    }
    void paint(juce::Graphics& g)
    {
        juce::TextButton::paint(g);
        g.setColour(juce::Colours::white);
        g.drawRoundedRectangle(getLocalBounds().toFloat(), 3, 5);
    }
private:

};

class SimulatorMainControl1 : public juce::Component,
    public juce::Button::Listener
{
public:
    SimulatorMainControl1();
    void resized();
    void paint(juce::Graphics& g) override;

private:
    void drawSubFunc(juce::Graphics& g, juce::Component& control, const juce::String& subFunctext);
    
private:
    void buttonClicked(Button* button);

private:
    KickSimButton fullLevels{ "", "FULL LEVELS" };
    KickSimButton levels16{ "", "16 LEVELS" };
    KickSimButton erase{ "", "ERASE" };
};

class SimulatorMainControl : public juce::Component, 
    public juce::Button::Listener
{
public:
    SimulatorMainControl();
    ~SimulatorMainControl();
    void buttonClicked(Button*);
    void paint(juce::Graphics& paint);
    void resized() override;

SIGNALS:
    sig::Signal<int> bankChanged;

private:
    void drawSubFunc(juce::Graphics& g, juce::Component& control, const juce::String& subFunctext);

private:
    SimulatorIntrefaceImpl* impl = nullptr;
    ui::KickImageButton bankA{"", "Sim Button A.png", "Sim Button A Enabled.png"};
    ui::KickImageButton bankB{"", "Sim Button B.png", "Sim Button B Enabled.png"};
    ui::KickImageButton bankC{"", "Sim Button C.png", "Sim Button C Enabled.png"};
    ui::KickImageButton bankD{"", "Sim Button D.png", "Sim Button D Enabled.png"};
    
    KickSimButton mainBtn{ "", "MAIN" };
    KickSimButton browseBtn{"", "BROWSE" };
    KickSimButton trackMixBtn{"", "TRACK MIX" };
    KickSimButton trackMuteBtn{"", "TRACK MUTE" };
    KickSimButton nextSeqBtn{"", "NEXT SEQ" };

    KickSimButton tapTempoBtn{"", "TAP TEMPO" };
    KickSimButton noteRepeatBtn{"", "NOTE REPEAT" };

    KickSimButton stepSeq{"", "STEP SEQ" };
    KickSimButton tc{"", "TC" };
    KickSimButton sampler{"", "SAMPLER" };
    KickSimButton sampleEdit{"", "SAMPLE EDIT" };
    KickSimButton programEdit{"", "PROGRAM EDIT" };

    KickSimButton menuBtn{"", "MENU" };
    KickSimButton shiftBtn{"", "SHIFT" };

    ui::KickImageButton backBtn{ "", "Sim Back.png" , "" };
    ui::KickImageButton copyBtn{ "", "Sim Copy.png" , "" };
    ui::KickImageButton plusBtn{ "", "Sim Plus.png" , "" };
    ui::KickImageButton minusBtn{ "", "Sim Minus.png" , ""};
    ui::KickRotary centralRotary{"x"};
    ui::KickButton centralKnobClick{"", "B"};
    int centralRotaryValue = 0;
};
}