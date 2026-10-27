#include "kick_SimulatorControls.h"
#include "kick_SimulatorInterfaceImpl.h"
namespace sim
{
#define SIM_TRANSPORT_CONTROL_WIDTH 81
#define SIM_TRANSPORT_CONTROL_HEIGHT 81

SimQlinkControls::SimQlinkControls()
{
    impl = new SimulatorIntrefaceImpl;
    qlink1.showTitleAndValue(true, Slider::TextBoxBelow, Justification::centred);
    qlink2.showTitleAndValue(true, Slider::TextBoxBelow, Justification::centred);
    qlink3.showTitleAndValue(true, Slider::TextBoxBelow, Justification::centred);
    qlink4.showTitleAndValue(true, Slider::TextBoxBelow, Justification::centred);
    
    qlink1.setDisplayProportions(.6f, .2f, .2f);
    qlink2.setDisplayProportions(.6f, .2f, .2f);
    qlink3.setDisplayProportions(.6f, .2f, .2f);
    qlink4.setDisplayProportions(.6f, .2f, .2f);
    
    qlink1.setRange(0, 127);
    qlink2.setRange(0, 127);
    qlink3.setRange(0, 127);
    qlink4.setRange(0, 127);
    
    addAndMakeVisible(qlink1);
    addAndMakeVisible(qlink2);
    addAndMakeVisible(qlink3);
    addAndMakeVisible(qlink4);
    
    auto handleValueChange = [this](ui::KickRotary* rotary, int qlinkType)
    {
        std::vector<char> data(2);
        if ((int)rotary->getValue() - currValues[qlinkType] >= 0)
            data[0] = 1;
        else
            data[0] = -1;

        data[1] = 0;
        currValues[qlinkType] = (int)rotary->getValue();
        impl->hwInputReceived.emit(mt::Kick::HwControlCommands::qlink1 + qlinkType, data);
    };
    qlink1.onValueChanged = [handleValueChange, this](double)
    {
        handleValueChange(&qlink1, 0);
    };
    
    qlink2.onValueChanged = [handleValueChange, this](double)
    {
        handleValueChange(&qlink2, 1);
    };

    qlink3.onValueChanged = [handleValueChange, this](double)
    {
        handleValueChange(&qlink3, 2);
    };
   
    qlink4.onValueChanged = [handleValueChange, this](double)
    {
        handleValueChange(&qlink4, 3);
    };

}

void SimQlinkControls::resized()
{
    qlink1.setBounds(0, 0, 109, 109);
    qlink2.setBounds(0, qlink1.getBottom() + 20, 109, 109);
    qlink3.setBounds(0, qlink2.getBottom() + 20, 109, 109);
    qlink4.setBounds(0, qlink3.getBottom() + 20, 109, 109);
}

SimTransportControl::SimTransportControl()
{
    impl = new SimulatorIntrefaceImpl;
    rec.makeVisibleAddListnerAndAddRadioGroup(this, 1);
    overDub.makeVisibleAddListnerAndAddRadioGroup(this, 1);
    stop.makeVisibleAddListnerAndAddRadioGroup(this, 1);
    play.makeVisibleAddListnerAndAddRadioGroup(this, 1);
    playStart.makeVisibleAddListnerAndAddRadioGroup(this, 1);
}

void SimTransportControl::resized()
{
    rec.setBounds(0, 0, SIM_TRANSPORT_CONTROL_WIDTH, SIM_TRANSPORT_CONTROL_HEIGHT);
    overDub.setBounds(0, rec.getBottom() + 8, SIM_TRANSPORT_CONTROL_WIDTH, SIM_TRANSPORT_CONTROL_HEIGHT);
    stop.setBounds(0, overDub.getBottom() + 8, SIM_TRANSPORT_CONTROL_WIDTH, SIM_TRANSPORT_CONTROL_HEIGHT);
    play.setBounds(0, stop.getBottom() + 8, SIM_TRANSPORT_CONTROL_WIDTH, SIM_TRANSPORT_CONTROL_HEIGHT);
    playStart.setBounds(0, play.getBottom() + 8, SIM_TRANSPORT_CONTROL_WIDTH, SIM_TRANSPORT_CONTROL_HEIGHT);
}

void SimTransportControl::buttonClicked(Button* button)
{
    std::vector<char> data;
    if (button == &rec)
        impl->hwInputReceived.emit(mt::Kick::HwControlCommands::Record, data);
    else if (button == &play)
        impl->hwInputReceived.emit(mt::Kick::HwControlCommands::Play, data);
    else if (button == &overDub)
        impl->hwInputReceived.emit(mt::Kick::HwControlCommands::OverDub, data);
    else if (button == &stop)
        impl->hwInputReceived.emit(mt::Kick::HwControlCommands::Stop, data);
    else if (button == &playStart)
        impl->hwInputReceived.emit(mt::Kick::HwControlCommands::PlayStart, data);
}
}
