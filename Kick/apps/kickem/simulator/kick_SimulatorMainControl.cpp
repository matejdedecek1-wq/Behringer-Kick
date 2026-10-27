#include "kick_SimulatorMainControl.h"
#include "kick_SimulatorInterfaceImpl.h"

#define MAIN_CTRL_BUTTON_WIDTH 93
#define MAIN_CTRL_BUTTON_HEIGHT 63
namespace sim
{
SimulatorMainControl1::SimulatorMainControl1()
{
    fullLevels.makeVisibleAndAddListner(this);
    levels16.makeVisibleAndAddListner(this);
    erase.makeVisibleAndAddListner(this);
}

void SimulatorMainControl1::resized()
{
    fullLevels.setBounds(0, 0, MAIN_CTRL_BUTTON_WIDTH, MAIN_CTRL_BUTTON_HEIGHT);
    levels16.setBounds(fullLevels.getRight() + 48, 0,  MAIN_CTRL_BUTTON_WIDTH, MAIN_CTRL_BUTTON_HEIGHT);
    erase.setBounds(levels16.getRight() + 48, 0, MAIN_CTRL_BUTTON_WIDTH, MAIN_CTRL_BUTTON_HEIGHT);
}

void SimulatorMainControl1::drawSubFunc(juce::Graphics& g, juce::Component& control, const juce::String& subFunctext)
{
    juce::Rectangle<int> subFuncRect(control.getX(), control.getBottom() + 2, control.getWidth(), 16);
    g.drawText(subFunctext, subFuncRect, juce::Justification::centred);
}

void SimulatorMainControl1::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::yellow);
    drawSubFunc(g, fullLevels, "HALF LEVEL");
    drawSubFunc(g, levels16, "NOTES");
}

void SimulatorMainControl1::buttonClicked(Button* button)
{
    if (button == &fullLevels)
    {
        ;
    }
    else if (button == &levels16)
    { 
        ;
    }
    else if (button == &erase)
    {
        ;
    }
}

SimulatorMainControl::SimulatorMainControl()
    : impl(new SimulatorIntrefaceImpl)
{
    bankA       .makeVisibleAddListnerAndAddRadioGroup(this, 2);
    bankB       .makeVisibleAddListnerAndAddRadioGroup(this, 2);
    bankC       .makeVisibleAddListnerAndAddRadioGroup(this, 2);
    bankD       .makeVisibleAddListnerAndAddRadioGroup(this, 2);
    bankA.setToggleState(true, juce::dontSendNotification);
    
    mainBtn.setToggleState(true, juce::dontSendNotification);
    mainBtn     .makeVisibleAddListnerAndAddRadioGroup(this, 1);
    browseBtn   .makeVisibleAddListnerAndAddRadioGroup(this, 1);
    trackMixBtn .makeVisibleAddListnerAndAddRadioGroup(this, 1);
    trackMuteBtn.makeVisibleAddListnerAndAddRadioGroup(this, 1);
    nextSeqBtn.makeVisibleAddListnerAndAddRadioGroup(this, 1);
    
    stepSeq.makeVisibleAddListnerAndAddRadioGroup(this, 1);
    tc.makeVisibleAddListnerAndAddRadioGroup(this, 1);
    sampler.makeVisibleAddListnerAndAddRadioGroup(this, 1);
    sampleEdit.makeVisibleAddListnerAndAddRadioGroup(this, 1);
    programEdit.makeVisibleAddListnerAndAddRadioGroup(this, 1);

    tapTempoBtn.makeVisibleAddListnerAndAddRadioGroup(this, 1);
    noteRepeatBtn.makeVisibleAddListnerAndAddRadioGroup(this, 1);

    menuBtn.makeVisibleAddListnerAndAddRadioGroup(this, 1);
    addAndMakeVisible(shiftBtn);

    backBtn.makeVisibleAndAddListner(this);
    copyBtn.makeVisibleAndAddListner(this);
    plusBtn.makeVisibleAndAddListner(this);
    minusBtn.makeVisibleAndAddListner(this);
    addAndMakeVisible(centralRotary);
    centralKnobClick.makeVisibleAndAddListner(this);
    centralRotary.showTitleAndValue(false, Slider::TextBoxBelow);
    centralRotary.setDisplayProportions(.6f, .2f, .2f);
    centralRotary.setSelectionEnabled(false);
    centralRotary.setRange(0, 127);
    centralRotary.onValueChanged = [this](double value)
    {
        std::vector<char> data;
        int currentValue = (int)std::round(value);
        data.push_back((int8)(currentValue - centralRotaryValue));
        centralRotaryValue = currentValue;
        DBG(currentValue);
        if (data[0] < 0)
            impl->hwInputReceived.emit(mt::Kick::HwControlCommands::centralKnobCCW, data);
        else
            impl->hwInputReceived.emit(mt::Kick::HwControlCommands::centralKnobCW, data);
    };

}

SimulatorMainControl::~SimulatorMainControl()
{
}

void SimulatorMainControl::buttonClicked(Button* button)
{
    if (button == &centralKnobClick)
    {
        impl->hwInputReceived.emit(mt::Kick::HwControlCommands::CentralKnobClick, {});
        return;
    }

    bool shiFtPressed = shiftBtn.getToggleState();
    if (button == &bankA)
    {
        if (shiFtPressed)
            bankChanged.emit(mt::Kick::HwControlCommands::BankE);
        else
            bankChanged.emit(mt::Kick::HwControlCommands::BankA);
    }
    else if (button == &bankB)
    {
        if (shiFtPressed)
            bankChanged.emit(mt::Kick::HwControlCommands::BankF);
        else
            bankChanged.emit(mt::Kick::HwControlCommands::BankB);
    }
    else if (button == &bankC)
    {
        if (shiFtPressed)
            bankChanged.emit(mt::Kick::HwControlCommands::BankG);
        else
            bankChanged.emit(mt::Kick::HwControlCommands::BankC);
    }
    else if (button == &bankD)
    {
        if (shiFtPressed)
            bankChanged.emit(mt::Kick::HwControlCommands::BankH);
        else
            bankChanged.emit(mt::Kick::HwControlCommands::BankD);
    }

    if (!button->getToggleState())
        return;

    std::vector<char> data;
    if (shiftBtn.getToggleState())
    {
        if (button == &bankA) impl->hwInputReceived.emit(mt::Kick::HwControlCommands::BankE, data);
        else if (button == &bankB) impl->hwInputReceived.emit(mt::Kick::HwControlCommands::BankF, data);
        else if (button == &bankC) impl->hwInputReceived.emit(mt::Kick::HwControlCommands::BankG, data);
        else if (button == &bankD) impl->hwInputReceived.emit(mt::Kick::HwControlCommands::BankH, data);
        else if (button == &mainBtn) impl->hwInputReceived.emit(mt::Kick::HwControlCommands::Grid, data);
        else if (button == &browseBtn) impl->hwInputReceived.emit(mt::Kick::HwControlCommands::Save, data);
        else if (button == &trackMixBtn) impl->hwInputReceived.emit(mt::Kick::HwControlCommands::PadMix, data);
        else if (button == &trackMuteBtn) impl->hwInputReceived.emit(mt::Kick::HwControlCommands::PadMute, data);
        else if (button == &nextSeqBtn) impl->hwInputReceived.emit(mt::Kick::HwControlCommands::Fx, data);
        else if (button == &tapTempoBtn) impl->hwInputReceived.emit(mt::Kick::HwControlCommands::Master, data);
        else if (button == &noteRepeatBtn) impl->hwInputReceived.emit(mt::Kick::HwControlCommands::Latch, data);
        else if (button == &stepSeq) impl->hwInputReceived.emit(mt::Kick::HwControlCommands::Automation, data);
        else if (button == &tc) impl->hwInputReceived.emit(mt::Kick::HwControlCommands::OnOff, data);
        else if (button == &sampler) impl->hwInputReceived.emit(mt::Kick::HwControlCommands::Looper, data);
        else if (button == &sampleEdit) impl->hwInputReceived.emit(mt::Kick::HwControlCommands::LinkEdit, data);
        else if (button == &programEdit) impl->hwInputReceived.emit(mt::Kick::HwControlCommands::MidiControl, data);
        else if (button == &menuBtn) impl->hwInputReceived.emit(mt::Kick::HwControlCommands::Prefs, data);
        else if (button == &backBtn) impl->hwInputReceived.emit(mt::Kick::HwControlCommands::Copy, data);
    }
    else
    {
        if (button == &bankA) impl->hwInputReceived.emit(mt::Kick::HwControlCommands::BankA, data);
        else if (button == &bankB) impl->hwInputReceived.emit(mt::Kick::HwControlCommands::BankB, data);
        else if (button == &bankC) impl->hwInputReceived.emit(mt::Kick::HwControlCommands::BankC, data);
        else if (button == &bankD) impl->hwInputReceived.emit(mt::Kick::HwControlCommands::BankD, data);
        else if (button == &mainBtn) impl->hwInputReceived.emit(mt::Kick::HwControlCommands::Main, data);
        else if (button == &browseBtn) impl->hwInputReceived.emit(mt::Kick::HwControlCommands::Browse, data);
        else if (button == &trackMixBtn) impl->hwInputReceived.emit(mt::Kick::HwControlCommands::TrackMix, data);
        else if (button == &trackMuteBtn) impl->hwInputReceived.emit(mt::Kick::HwControlCommands::TrackMute, data);
        else if (button == &nextSeqBtn) impl->hwInputReceived.emit(mt::Kick::HwControlCommands::NextSeq, data);
        else if (button == &tapTempoBtn) impl->hwInputReceived.emit(mt::Kick::HwControlCommands::TapTempo, data);
        else if (button == &noteRepeatBtn) impl->hwInputReceived.emit(mt::Kick::HwControlCommands::NoteRepeat, data);
        else if (button == &stepSeq) impl->hwInputReceived.emit(mt::Kick::HwControlCommands::StepSeq, data);
        else if (button == &tc) impl->hwInputReceived.emit(mt::Kick::HwControlCommands::Tc, data);
        else if (button == &sampler) impl->hwInputReceived.emit(mt::Kick::HwControlCommands::Sampler, data);
        else if (button == &sampleEdit) impl->hwInputReceived.emit(mt::Kick::HwControlCommands::SampleEdit, data);
        else if (button == &programEdit) impl->hwInputReceived.emit(mt::Kick::HwControlCommands::ProgramEdit, data);
        else if (button == &menuBtn) impl->hwInputReceived.emit(mt::Kick::HwControlCommands::Menu, data);
        else if (button == &backBtn) impl->hwInputReceived.emit(mt::Kick::HwControlCommands::Undo, data);
    }
}

void SimulatorMainControl::drawSubFunc(juce::Graphics& g, juce::Component& control, const juce::String& subFunctext)
{
    juce::Rectangle<int> subFuncRect(control.getX(), control.getBottom() + 2, control.getWidth(), 16);
    g.drawText(subFunctext, subFuncRect, juce::Justification::centred);
}

void SimulatorMainControl::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::white);
    drawSubFunc(g, bankA, "E");
    drawSubFunc(g, bankB, "F");
    drawSubFunc(g, bankC, "G");
    drawSubFunc(g, bankD, "H");

    drawSubFunc(g, mainBtn, "GRID");
    drawSubFunc(g, browseBtn, "SAVE");
    drawSubFunc(g, trackMixBtn, "PAD MIX");
    drawSubFunc(g, trackMuteBtn, "PAD MUTE");
    drawSubFunc(g, nextSeqBtn, "FX");
    drawSubFunc(g, tapTempoBtn, "MASTER");
    drawSubFunc(g, noteRepeatBtn, "LATCH");
    
    drawSubFunc(g, stepSeq, "AUTOMATION");
    drawSubFunc(g, tc, "ON/OFF");
    drawSubFunc(g, sampler, "LOOPER");
    drawSubFunc(g, sampleEdit, "LINKEDIT");
    drawSubFunc(g, programEdit, "MIDI CONTROL");

    drawSubFunc(g, menuBtn, "PREFS");

    g.drawLine((float)bankA.getX(), 17.0f, bankB.getX() + (MAIN_CTRL_BUTTON_WIDTH /2.0f), 17.0f);
    g.drawLine((float)bankC.getX() + (MAIN_CTRL_BUTTON_WIDTH / 2.0f), 17.0f, (float)bankD.getRight(), 17.0f);
    juce::Rectangle<int> banksLabelRect(bankB.getX() + (MAIN_CTRL_BUTTON_WIDTH / 2), 0, 123, 34);
    g.drawText("BANKS", banksLabelRect, juce::Justification::centred);
}

void SimulatorMainControl::resized()
{
    mainBtn.setBounds(0, 34, MAIN_CTRL_BUTTON_WIDTH, MAIN_CTRL_BUTTON_HEIGHT);
    tapTempoBtn.setBounds(0, mainBtn.getBottom() + 28, MAIN_CTRL_BUTTON_WIDTH, MAIN_CTRL_BUTTON_HEIGHT);
    noteRepeatBtn.setBounds(0, tapTempoBtn.getBottom() + 28, MAIN_CTRL_BUTTON_WIDTH, MAIN_CTRL_BUTTON_HEIGHT);
    stepSeq.setBounds(0, noteRepeatBtn.getBottom() + 28, MAIN_CTRL_BUTTON_WIDTH, MAIN_CTRL_BUTTON_HEIGHT);

    browseBtn.setBounds(mainBtn.getRight() + 37, 34, MAIN_CTRL_BUTTON_WIDTH, MAIN_CTRL_BUTTON_HEIGHT);
    trackMixBtn.setBounds(mainBtn.getRight() + 37, browseBtn.getBottom() + 28, MAIN_CTRL_BUTTON_WIDTH, MAIN_CTRL_BUTTON_HEIGHT);
    trackMuteBtn.setBounds(mainBtn.getRight() + 37, trackMixBtn.getBottom() + 28, MAIN_CTRL_BUTTON_WIDTH, MAIN_CTRL_BUTTON_HEIGHT);

    bankA.setBounds(browseBtn.getRight() + 60, 34, MAIN_CTRL_BUTTON_WIDTH, MAIN_CTRL_BUTTON_HEIGHT);
    bankB.setBounds(bankA.getRight() + 30, 34, MAIN_CTRL_BUTTON_WIDTH, MAIN_CTRL_BUTTON_HEIGHT);
    bankC.setBounds(bankB.getRight() + 30, 34, MAIN_CTRL_BUTTON_WIDTH, MAIN_CTRL_BUTTON_HEIGHT);
    bankD.setBounds(bankC.getRight() + 30, 34,  MAIN_CTRL_BUTTON_WIDTH, MAIN_CTRL_BUTTON_HEIGHT);
    
    tc.setBounds(bankD.getRight() + 50, 34, MAIN_CTRL_BUTTON_WIDTH, MAIN_CTRL_BUTTON_HEIGHT);
    sampler.setBounds(bankD.getRight() + 50, tc.getBottom() + 28, MAIN_CTRL_BUTTON_WIDTH, MAIN_CTRL_BUTTON_HEIGHT);
    sampleEdit.setBounds(bankD.getRight() + 50, sampler.getBottom() + 28, MAIN_CTRL_BUTTON_WIDTH, MAIN_CTRL_BUTTON_HEIGHT);

    nextSeqBtn.setBounds(tc.getRight() + 37, 34, MAIN_CTRL_BUTTON_WIDTH, MAIN_CTRL_BUTTON_HEIGHT);
    menuBtn.setBounds(tc.getRight() + 37, nextSeqBtn.getBottom() + 28, MAIN_CTRL_BUTTON_WIDTH, MAIN_CTRL_BUTTON_HEIGHT);
    shiftBtn.setBounds(tc.getRight() + 37, menuBtn.getBottom() + 28, MAIN_CTRL_BUTTON_WIDTH, MAIN_CTRL_BUTTON_HEIGHT);
    programEdit.setBounds(tc.getRight() + 37, shiftBtn.getBottom() + 28, MAIN_CTRL_BUTTON_WIDTH, MAIN_CTRL_BUTTON_HEIGHT);

    backBtn.setBounds (trackMixBtn.getRight() + 128,  bankA.getBottom() + 26, 145, 133);
    copyBtn.setBounds (backBtn.getRight(), bankA.getBottom() + 26, 145, 143);
    minusBtn.setBounds(trackMixBtn.getRight() + 128, bankA.getBottom() + 26 + 133, 145, 133);
    plusBtn.setBounds (minusBtn.getRight(), bankA.getBottom() + 26 + 133, 145, 133);

    juce::Rectangle<int> rotaryRect(backBtn.getX() + 36, backBtn.getY() + 23, 220, 220);
    centralRotary.setBounds(rotaryRect);
    centralKnobClick.setBounds(rotaryRect.getCentreX() - 20, rotaryRect.getCentreY() - 20, 40, 40);
}

}
