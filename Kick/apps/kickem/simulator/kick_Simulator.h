#pragma once

#include <kick_ui/kick_ui.h>
#include "kick_SimulatorInterfaceImpl.h"
#include "kick_SimulatorMainControl.h"
#include "kick_SimulatorControls.h"
namespace sim
{
class KickSimulatorButton : public juce::Component
{
public:
    KickSimulatorButton(int pos);
    ~KickSimulatorButton();
    int getPos() { return pos; }
    void paint(juce::Graphics& g);
    void setState(mt::Kick::PadButtonState newState) { state = newState; }
    void setPadColour(const juce::Colour& clr) { padColour = clr; }

SIGNALS:
    sig::Signal<int, bool> padStateChanged;

private:
    void mouseDown(const MouseEvent& event) override;
    void mouseUp(const MouseEvent& event) override;

public:
    int pos = 0;
    bool mousePressed = false;
    juce::Colour padColour;
    mt::Kick::PadButtonState state = mt::Kick::PadButtonState::StateOff;
};

class SimulatorButtonGrid : public juce::Component
{
public:
    SimulatorButtonGrid();
    ~SimulatorButtonGrid();
    void resized() override;
    void paint(juce::Graphics& g) override;
    void setCurrentBank(int cBank);

private:
    SimulatorIntrefaceImpl* impl = nullptr;
    std::list<std::unique_ptr<KickSimulatorButton>> buttons;
    std::vector<mt::Kick::PadButtonState> padState;
    int currentBank = 0;
    std::vector<mt::Kick::PadColourIds> padColour;
};

class Simulator  : public juce::Component
{
public:
    Simulator ();
    ~Simulator();

    void paint (juce::Graphics& g) override;
    void resized() override;
private:
    juce::Rectangle<int> getSliderRect();
    juce::Rectangle<int> getMasterRect();
    juce::Rectangle<int> getRecRect();
    juce::Rectangle<int> getSliderLinkRect();
    juce::Rectangle<int> getSliderControlsRect();
    juce::Rectangle<int> getQlinkControlsRect();

private:
    SimulatorIntrefaceImpl* sif = nullptr;
    ui::KickImageButton brand{"", "Sim Brands dt_KlarkTeknik.png", ""};
    ui::KickImageButton logo{ "", "Sim KICK logo.png", "" };
    SimulatorButtonGrid pad;
    SimulatorMainControl mainControl;
    SimulatorMainControl1 mainControl1;
    SimTransportControl transport;
    SimQlinkControls qlinks;
    ui::KickSlider slider;
    ui::KickRotary master{"Master"};
    ui::KickRotary rec{ "Rec" };
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Simulator)
};
}
