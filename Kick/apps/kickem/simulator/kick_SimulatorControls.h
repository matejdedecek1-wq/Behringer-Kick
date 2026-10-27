#pragma once

#include <kick_ui/kick_ui.h>
namespace sim
{
class SimulatorIntrefaceImpl;
class SimQlinkControls : public juce::Component
{
public:
    SimQlinkControls();
    void resized() override;

private:
    SimulatorIntrefaceImpl* impl = nullptr;
    ui::KickRotary qlink1{ "P1" };
    ui::KickRotary qlink2{ "P2" };
    ui::KickRotary qlink3{ "P3" };
    ui::KickRotary qlink4{ "P4" };
    std::vector<int> currValues { 0,0,0,0 };
    int qlink1CurrValue = 0;
    int qlink2CurrValue = 0;
    int qlink3CurrValue = 0;
    int qlink4CurrValue = 0;
};

class SimTransportControl : public juce::Component,
    public juce::Button::Listener
{
public:
    SimTransportControl();
    void resized() override;
    void buttonClicked(Button* button);

private:
    SimulatorIntrefaceImpl* impl = nullptr;
    ui::KickImageButton rec{ "", "Sim Transport Rec.png", "Sim Transport Rec Enabled.png" };
    ui::KickImageButton overDub{ "", "Sim Transport OverDub.png", "Sim Transport OverDub Enabled.png" };
    ui::KickImageButton stop{ "", "Sim Transport Stop.png", "Sim Transport Stop Enabled.png" };
    ui::KickImageButton play{ "", "Sim Transport Play.png", "Sim Transport Play Enabled.png" };
    ui::KickImageButton playStart{ "", "Sim Transport PlayStart.png", "Sim Transport PlayStart Enabled.png" };
};
}