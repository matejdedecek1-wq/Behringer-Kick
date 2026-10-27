#include "kick_Simulator.h"

namespace sim
{
#define padSpacing  20
KickSimulatorButton::KickSimulatorButton(int p)
    :pos(p)
{
}

KickSimulatorButton::~KickSimulatorButton()
{
}

void KickSimulatorButton::paint(juce::Graphics& g)
{
    g.setColour(juce::Colour(34, 34, 34));
    g.fillAll();

    juce::Colour clr;
    if (mousePressed)
        clr = padColour.withAlpha(0.75f);
    else if (state == mt::Kick::PadButtonState::StateOff || state == mt::Kick::PadButtonState::StateOff)
        clr = juce::Colours::transparentBlack;
    else
        clr = padColour;

    g.setColour(clr);
    g.drawRect(getLocalBounds(), 3);
}

void KickSimulatorButton::mouseDown(const MouseEvent&)
{
    if (state == mt::Kick::PadButtonState::StateOff)
        return;

    padStateChanged.emit(pos, true);
    mousePressed = true;
    repaint();
}

void KickSimulatorButton::mouseUp(const MouseEvent&)
{
    if (state == mt::Kick::PadButtonState::StateOff)
        return;

    padStateChanged.emit(pos, false);
    mousePressed = false;
    repaint();
}

SimulatorButtonGrid::SimulatorButtonGrid()
{
    padColour.resize(128, mt::Kick::PadColourIds::PadColourTrans);
    padState.resize(128, mt::Kick::PadButtonState::StateOff);
    auto triggerHwEvent = [this](int pos, bool pressed)
    {
        std::vector<char> data(2);
        int pad = mt::Kick::HwControlCommands::Pad1 + pos;
        data[1] = 127; //velocity
        if (pressed)
            data[0] = PAD_PRESSED;
        else
            data[0] = PAD_RELEASED;

        impl->hwInputReceived.emit((mt::Kick::HwControlCommands)pad, data);
    };
    
    impl = new SimulatorIntrefaceImpl();
    for (int row = 0; row < 4; row++)
    {
        for (int column = 0; column < 4; column++)
        {
            auto& padBtn = buttons.emplace_back(std::unique_ptr<KickSimulatorButton>(new KickSimulatorButton( row * 4 + column)));
            addAndMakeVisible(*padBtn);
            padBtn->padStateChanged.connect(triggerHwEvent);
        }
    }
    
    impl->hwMessageReceived.connect([this](int hwId, const std::vector<juce::int8>& data)
    {
        if (!(hwId >= mt::Kick::HwControlCommands::Pad1 && hwId <= mt::Kick::HwControlCommands::Pad16))
            return;

        mt::Kick::PadAction pdAction = (mt::Kick::PadAction)data[0];
        int pad = hwId - (int)mt::Kick::HwControlCommands::Pad1;
        if (pdAction == mt::Kick::PadAction::SetPadColour)
        {
            int bank = (int)data[1];

            mt::Kick::PadColourIds selPadClr = (mt::Kick::PadColourIds)data[2];
            padColour[(rsize_t)bank * TOTAL_DRUM_PADS + pad] = (mt::Kick::PadColourIds)data[2];

            if (currentBank == bank)
            {
                for (auto& button : buttons)
                {
                    if (button->getPos() == pad)
                    {
                        button->setPadColour(mt::Kick::getPadColourFromId(selPadClr));
                        break;
                    }
                }
            }
            repaint();
        }
        else if (pdAction == mt::Kick::PadAction::SetPadState)
        {
            padState[currentBank * TOTAL_DRUM_PADS + pad] = (mt::Kick::PadButtonState)data[1];

            //int startPadAndBank = (int)data[1];
            //int ensPadAndBank = (int)data[2];
            //for (int i = startPadAndBank; i <= ensPadAndBank; i++)
            //    padState[i] = state;

            setCurrentBank(currentBank);
        }
        else if (hwId == pdAction == mt::Kick::PadAction::SetPadState)
        {
            //int startPadAndBank = (int)data[0];
            //int ensPadAndBank = (int)data[1];
            //for (int i = startPadAndBank; i <= ensPadAndBank; i++)
            //    padsOn[i] = false;
            //
            //setCurrentBank(currentBank);
        }
    });
}

SimulatorButtonGrid::~SimulatorButtonGrid()
{
}

void SimulatorButtonGrid::resized()
{
    int buttonWidth = (int)(std::round(getWidth() - (5 * padSpacing)) / 4);
    int buttonHeight = (int)(std::round(getHeight() - (5 * padSpacing)) / 4);
    int yPos = getHeight() - padSpacing - buttonHeight;
    int xPos = padSpacing;
    
    int pos = 0;
    for (std::unique_ptr<KickSimulatorButton>& padBtn : buttons)
    {
        padBtn->setBounds(xPos, yPos, buttonWidth, buttonHeight);
        xPos += buttonWidth + padSpacing;
        pos++;
        if (pos / 4 == 1)
        {
            yPos -= buttonHeight + padSpacing;
            pos = 0;
            xPos = padSpacing;
        }
    }
}

void SimulatorButtonGrid::setCurrentBank(int cBank)
{
    currentBank = cBank;
    int buttonStartValue = cBank * 16;
    for (auto& padButton : buttons)
    {
        padButton->setPadColour(mt::Kick::getPadColourFromId(padColour[buttonStartValue]));
        padButton->setState(padState[buttonStartValue]);
        buttonStartValue++;
    }

    repaint();
}

void SimulatorButtonGrid::paint(juce::Graphics& g)
{
    g.setColour(juce::Colour(40, 39, 37));
    g.fillAll();

    g.setColour(juce::Colours::white);
    int buttonWidth = (int)(std::round(getWidth() - (5 * padSpacing)) / 4);
    int buttonHeight = (int)(std::round(getHeight() - (5 * padSpacing)) / 4);
    int yPos = getHeight() - padSpacing;
    int xPos = padSpacing;

    int pos = 0;
    for (std::unique_ptr<KickSimulatorButton>& padBtn : buttons)
    {
        juce::Rectangle<int> textRect(xPos, yPos, buttonWidth, 16);
        juce::String text = juce::String().formatted("Pad %d", padBtn->pos);
        g.drawText(text, textRect, juce::Justification::centredLeft);

        xPos += buttonWidth + padSpacing;
        pos++;
        if (pos / 4 == 1)
        {
            yPos -= buttonHeight + padSpacing;
            pos = 0;
            xPos = padSpacing;
        }
    }
}

Simulator::Simulator()
{
    sif = new SimulatorIntrefaceImpl;
    const Displays::Display* primary = juce::Desktop::getInstance().getDisplays().getPrimaryDisplay();
    setBounds(primary->userArea);

    slider.setRange(1, 100, 1);
    addAndMakeVisible(master);
    master.setDisplayProportions(.6f, .2f, .2f);
    master.showTitleAndValue(true, Slider::TextBoxBelow, Justification::centred);

    addAndMakeVisible(rec);
    rec.setDisplayProportions(.6f, .2f, .2f);
    rec.showTitleAndValue(true, Slider::TextBoxBelow, Justification::centred);

    addAndMakeVisible(slider);
    addAndMakeVisible(brand);
    addAndMakeVisible(logo);
    addAndMakeVisible(AppHome::Instance());
    addAndMakeVisible(pad);
    addAndMakeVisible(mainControl);
    addAndMakeVisible(mainControl1);
    addAndMakeVisible(transport);
    addAndMakeVisible(qlinks);
    mainControl.bankChanged.connect([this](int bankEnum)
        {
            int currentBank = bankEnum >= mt::Kick::HwControlCommands::BankA && bankEnum <= mt::Kick::HwControlCommands::BankD ?
                bankEnum - mt::Kick::HwControlCommands::BankA : bankEnum  - mt::Kick::HwControlCommands::BankE;
            pad.setCurrentBank(currentBank);
        }
    );
}

Simulator::~Simulator()
{
    AppHome::deletInstance();
    delete sif;
}

void Simulator::paint(juce::Graphics &g)
{
    g.setColour(juce::Colour(53, 53, 53));
    g.fillAll();

    g.setColour(juce::Colour(27, 27, 27));
    g.fillRect(getSliderRect());
    g.fillRect(getMasterRect());
    g.fillRect(getRecRect());
    g.fillRect(getSliderLinkRect());
    g.fillRect(getSliderControlsRect());
    g.fillRect(getQlinkControlsRect());
}

void Simulator::resized()
{
    brand.setBounds(20, 20, 275, 37);
    logo.setBounds(20, brand.getBottom() + 10, 275, 50);
    AppHome::Instance()->setTopLeftPosition(683, 20);
    pad.setBounds(40, getSliderControlsRect().getBottom() + 10, 614, 490);
    mainControl.setBounds(692, AppHome::Instance()->getBottom() + 2, 1018, 458);
    transport.setBounds(1767, 578, 84, 437);
    qlinks.setBounds(1753, 40, 110, 526);

    slider.setBounds(446, 40, 16, 280);
    master.setBounds(512, 30, 148, 137);
    rec.setBounds(512, 197, 148, 137);

    mainControl1.setBounds(79, 395, 442 - 68, 63);
}

juce::Rectangle<int> Simulator::getSliderRect()
{
    return juce::Rectangle<int>(335, 20, 147, 324);
}

juce::Rectangle<int> Simulator::getMasterRect()
{
    return juce::Rectangle<int>(502, 20, 168, 157);
}

juce::Rectangle<int> Simulator::getRecRect()
{
    return juce::Rectangle<int>(502, 187, 168, 157);
}

juce::Rectangle<int> Simulator::getSliderLinkRect()
{
    return juce::Rectangle<int>(502, 354, 168, 156);
}

juce::Rectangle<int> Simulator::getSliderControlsRect()
{
    return juce::Rectangle<int>(40, 354, 442, 156);
}

juce::Rectangle<int> Simulator::getQlinkControlsRect()
{
    return juce::Rectangle<int>(1734, 20, 147, 524);
}

}
