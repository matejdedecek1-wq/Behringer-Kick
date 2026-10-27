#pragma once
namespace kick_ui
{
    class MidiMessageDisplay : public juce::Component
    {
    public:
        MidiMessageDisplay();
        struct MessageData
        {
            int          Index{ -1 };
            juce::String type{ "" };
            juce::String port{ "" };
            int          channel{ -1 };
            juce::String note{ "" };
            juce::String details{ "" };
        };

        void updateData( MessageData );

    private:
        void resized() override;

        ui::KickLabel index{ "" };
        ui::KickLabel type{ "" };
        ui::KickLabel port{ "" };
        ui::KickLabel channel{ "" };
        ui::KickLabel note{ "" };
        ui::KickLabel details{ "" };
    };

    class MidiMonitor : public juce::Component
    {
    public:
        MidiMonitor();

        void addMidiMessage( juce::MidiMessage& Msg, juce::String& Port );
        void refresh()
        {
            tableView.refresh();
        }

        struct Header : public juce::Component
        {
        public:
            Header()
            {
                setInterceptsMouseClicks( false, false );
                mt::utils::addAndMakeVisible( this, index, type, port, channel, note, details );
                mt::utils::setValueColour( ui::Theme::textColour( 2 ), index, type, port, channel, note, details );
                mt::utils::setValueFontHeight( 18, index, type, port, channel, note, details );
            }

        private:
            void resized() override
            {
                auto b = getLocalBounds();
                b.removeFromLeft( 10 );
                index.setBounds( b.removeFromLeft( 60 ) );
                b.removeFromLeft( 5 );
                type.setBounds( b.removeFromLeft( 180 ) );
                b.removeFromLeft( 5 );
                port.setBounds( b.removeFromLeft( 180 ) );
                b.removeFromLeft( 5 );
                channel.setBounds( b.removeFromLeft( 60 ) );
                b.removeFromLeft( 5 );
                note.setBounds( b.removeFromLeft( 60 ) );
                b.removeFromLeft( 5 );
                details.setBounds( b );
            }

            void paint( juce::Graphics& G ) override
            {
                G.fillAll( ui::Theme::backgroundColour( 3 ) );
            }

            ui::KickLabel index{ "INDEX" };
            ui::KickLabel type{ "TYPE" };
            ui::KickLabel port{ "PORT" };
            ui::KickLabel channel{ "CH" };
            ui::KickLabel note{ "NOTE" };
            ui::KickLabel details{ "DETAILS" };
        };

    private:
        void resized() override;
        void paint( juce::Graphics& ) override;

        ui::KickTableViewProvider provider;
        ui::KickTableView         tableView{ "MIDI MONITOR", provider };

        ui::KickToggleButton in{ "IN", "IN", "IN" };
        ui::KickToggleButton out{ "OUT", "OUT", "OUT" };
        ui::KickButton       clear{ "CLEAR" };
        ui::KickButton       close{ "CLOSE" };

        std::vector<MidiMessageDisplay::MessageData> midiOutData;
        std::vector<MidiMessageDisplay::MessageData> midiInData;
    };

    const static std::vector<int> controlEnums = {
        contracts::DeviceControlContracts::BtnLedSequence::EN_STEP_SEQ, contracts::DeviceControlContracts::BtnLedSequence::EN_NEXT_SEQ, contracts::DeviceControlContracts::BtnLedSequence::EN_TRACK_MUTE,
        contracts::DeviceControlContracts::BtnLedSequence::EN_SAMPLE_EDIT, contracts::DeviceControlContracts::BtnLedSequence::EN_PROG_EDIT, contracts::DeviceControlContracts::BtnLedSequence::EN_BROWSE,
        contracts::DeviceControlContracts::BtnLedSequence::EN_PLUS, contracts::DeviceControlContracts::BtnLedSequence::EN_TAP_TEMPO, contracts::DeviceControlContracts::BtnLedSequence::EN_KNOB_ENTER,
        contracts::DeviceControlContracts::BtnLedSequence::EN_MINUS, contracts::DeviceControlContracts::BtnLedSequence::EN_ERASE, contracts::DeviceControlContracts::BtnLedSequence::EN_TRACK_MIX,
        contracts::DeviceControlContracts::BtnLedSequence::EN_TC, contracts::DeviceControlContracts::BtnLedSequence::EN_PAD_MUTE, contracts::DeviceControlContracts::BtnLedSequence::EN_COPY,
        contracts::DeviceControlContracts::BtnLedSequence::EN_PAD_MIX, contracts::DeviceControlContracts::BtnLedSequence::EN_SAMPLER, contracts::DeviceControlContracts::BtnLedSequence::EN_FX
    };
    const static std::vector<int> controlPadEnums = {
        contracts::DeviceControlContracts::PadSequence::EN_PAD1, contracts::DeviceControlContracts::PadSequence::EN_PAD2, contracts::DeviceControlContracts::PadSequence::EN_PAD3,
        contracts::DeviceControlContracts::PadSequence::EN_PAD4, contracts::DeviceControlContracts::PadSequence::EN_PAD5, contracts::DeviceControlContracts::PadSequence::EN_PAD6,
        contracts::DeviceControlContracts::PadSequence::EN_PAD7, contracts::DeviceControlContracts::PadSequence::EN_PAD8, contracts::DeviceControlContracts::PadSequence::EN_PAD9,
        contracts::DeviceControlContracts::PadSequence::EN_PAD10, contracts::DeviceControlContracts::PadSequence::EN_PAD11, contracts::DeviceControlContracts::PadSequence::EN_PAD12,
        contracts::DeviceControlContracts::PadSequence::EN_PAD13, contracts::DeviceControlContracts::PadSequence::EN_PAD14, contracts::DeviceControlContracts::PadSequence::EN_PAD15, contracts::DeviceControlContracts::PadSequence::EN_PAD16
    };

    class MidiControl : public kick_ui::IScreen
        , public juce::Component

    {
    public:
        MidiControl();
        ~MidiControl();
        bool canClose() override { return true; }

        bool requiresControlBar() override { return false; }
        bool requiresStatusBar() override { return true; }
        bool requiresHandleCommand() override { return true; }

        void handleCommand( CPktMgr::HWMsg_t& ) override;
        int  screenIndex( int screenName );
        int  screenIndexInPad( int screenName );

        ui::KickToggleButton enableMapping{ "Enable Mapping", "Learn", "Enable Mapping" };

    private:
        AppStatusbar& statusBar{ AppStatusbar::Instance() };

        kick_ui::KickModeHeader header{ "MIDI Control", "Icon_MidiControl" };
        ui::KickListBox         output{ ui::KickListBox::withoutTitleBar, "", {} };
        ui::KickLabel           outPutLabel{ "MIDI Output Port" };
        ui::KickToggleButton    hardware{ "Hardware", "Hardware", "Hardware" };
        ui::KickToggleButton    xy{ "XY", "XY", "XY" };
        ui::KickToggleButton    midiLearn{ "MIDI Learn", "MIDI Learn", "MIDI Learn" };
        ui::KickButton          monitor{ "Midi Monitor" };
        MidiMonitor             midiMonitor;

        MidiControlHardware  hardWareTab;
        MidiControlXY        xyMidiTab;
        MidiControlMidiLearn midiLearnTab;

        std::vector<MidiRightData> listEntry;

        void resized() override;
        void visibilityChanged() override;
        void paint( juce::Graphics& g ) override;
    };

}
