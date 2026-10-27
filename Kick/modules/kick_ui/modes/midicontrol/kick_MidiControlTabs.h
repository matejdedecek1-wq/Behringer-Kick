#pragma once

namespace kick_ui
{
    struct MidiRightData
    {
        juce::String source;
        juce::String target;
        juce::String type{ "" };
        int          data{ 0 };
        juce::String flip{ "" };
        int          channel{ 0 };
        int          ccNum{ 0 };
    };

    class MidiControlMidiLearnLeft : public juce::Component
    {
    public:
        MidiControlMidiLearnLeft();
        ~MidiControlMidiLearnLeft();
        void setleftOptionsVisible( bool visible )
        {
            sourceList.setVisible( visible );
            clear.setVisible( visible );
            del.setVisible( visible );
            source.setVisible( visible );
        }
        void setTrackviewVisible( bool visible )
        {
            track.setVisible( visible );
            tracklabel.setVisible( visible );
        }
        void setPgmVisible( bool visible )
        {
            pgm.setVisible( visible );
            pgmlabel.setVisible( visible );
        }
        void setReturnVisible( bool visible )
        {
            ret.setVisible( visible );
            retlabel.setVisible( visible );
        }
        void setSubmixVisible( bool visible )
        {
            sub.setVisible( visible );
            sublabel.setVisible( visible );
        }
        void setMainVisible( bool visible )
        {
            main.setVisible( visible );
            mainlabel.setVisible( visible );
        }
        void setpadVisible( bool visible )
        {
            setAll.setVisible( visible );
            padEnable.setVisible( visible );
        }

        ui::KickListBox    sourceList{ ui::KickListBox::BoxType::withoutTitleBar, "Source", { "None", "MIDI Track", "Audio Track", "Program", "Return", "Submix", "Main" } };
        ui::KickListBox    track{ ui::KickListBox::BoxType::withoutTitleBar, "track", {} };
        ui::KickListBox    pgm{ ui::KickListBox::BoxType::withoutTitleBar, "pgm", {} };
        ui::KickListBox    ret{ ui::KickListBox::BoxType::withoutTitleBar, "ret", {} };
        ui::KickListBox    main{ ui::KickListBox::BoxType::withoutTitleBar, "main", {} };
        ui::KickListBox    sub{ ui::KickListBox::BoxType::withoutTitleBar, "submix", {} };
        ui::KickListBox    banklist{ ui::KickListBox::BoxType::withoutTitleBar, "BANKS", mt::Kick::padBankNames };
        ui::KickListBox    padnolist{ ui::KickListBox::BoxType::withoutTitleBar, "Channel", mt::Kick::midiOutChannels };
        ui::KickLabel      source{ "Source" };
        ui::KickLabel      padEnable{ "Pad" };
        juce::ToggleButton setAll{ "" };
        ui::KickLabel      bank{ "Bank" };
        ui::KickLabel      padno{ "Pad" };
        ui::KickButton     clear{ "Clear" };
        ui::KickButton     del{ "Delete" };

    private:
        void                 paint( juce::Graphics& g ) override;
        void                 resized() override;
        ui::KickToggleButton learn{ "Learn" };
        ui::KickToggleButton enableMapping{ "Enable Mapping" };

        ui::KickLabel tracklabel{ "Track" };
        ui::KickLabel pgmlabel{ "Program" };

        ui::KickLabel retlabel{ "Return" };
        ui::KickLabel sublabel{ "SubMix" };
        ui::KickLabel mainlabel{ "Main" };
    };

    class MidiControlMidiLearnTableView : public juce::Component
    {
    public:
        MidiControlMidiLearnTableView();
        ~MidiControlMidiLearnTableView();
        std::unique_ptr<ui::KickTableView> table;
        ui::KickTableViewProvider          provider;
        std::vector<MidiRightData>         midiRightDataStruct;
        void                               addStructure( MidiRightData, int row );

    private:
        void                 resized() override;
        void                 paint( juce::Graphics& g ) override;
        int                  numRows{ 0 };
        ui::KickLabel        name{ "TARGET" }, type{ "TYPE" }, ch{ "CH" }, data{ "DATA" }, flip{ "FLIP" };
        juce::Rectangle<int> rightTopBounds = { 0, 0, 442, 54 };
    };
    class MidiControlMidiLearnRight : public juce::Component
    {
    public:
        MidiControlMidiLearnRight();
        ~MidiControlMidiLearnRight(){};
        void setTargetVisible( bool visible )
        {
            target.setVisible( visible );
            targetList.setVisible( visible );

            flip.setVisible( visible );
            fliplabel.setVisible( visible );
        }
        void setTypeVisible( bool visible )
        {
            type.setVisible( visible );
            typeList.setVisible( visible );
        }
        ui::KickListBox      targetList{ ui::KickListBox::BoxType::withoutTitleBar, "Target", {} };
        ui::KickListBox      typeList{ ui::KickListBox::BoxType::withoutTitleBar, "Type", {} };
        ui::KickToggleButton flip{ "Enable", "Enable", "Enable" };
        ui::KickLabel        target{ "Target" };

    private:
        ui::KickLabel type{ "Type" };
        ui::KickLabel fliplabel{ "Flip" };
        void          resized() override;
        void          paint( juce::Graphics& g ) override{};
    };
    class MidiLearnTableEntry : public juce::Component
    {
    public:
        MidiLearnTableEntry();
        ~MidiLearnTableEntry(){};
        void setData( MidiRightData& data );

    private:
        ui::Kick2LineNameLabel target{ "" };
        ui::KickLabel          type;
        ui::KickLabel          data;
        ui::KickLabel          ch;
        ui::KickLabel          flip;

        void resized() override;
        void paint( juce::Graphics& g ) override;
    };

    class MidiControlMidiLearn : public juce::Component
        , public juce::MidiInputCallback

    {
    public:
        MidiControlMidiLearn();
        ~MidiControlMidiLearn();

        struct MidiMessageData
        {
            juce::MidiMessage message;
            juce::Time        timeStamp;
        };
        void                             refresh();
        void                             handleIncomingMidiMessage( juce::MidiInput* source, const juce::MidiMessage& message ) override;
        ui::KickImageButton              add{ "Add", "Add_normal", "Add_normal", "Add_active" };
        void                             startLearning();
        void                             stopMidiInput();
        void                             ledUpdate( int ccNumber, int midiChannel );
        void                             processBufferedMessages();
        void                             parameterMapping( const juce::MidiMessage& );
        bool                             isLearning;
        bool                             isMappingEnabled;
        std::unique_ptr<juce::MidiInput> midiInput;
        std::unique_ptr<juce::MidiInput> activeMidiInput;
        std::vector<MidiMessageData>     messageBuffer;
        mt::MTClipProgram*               pClip = { nullptr };
        mt::MTDrumProgram*               pDrum{ nullptr };

    private:
        void populateMaps();

    private:
        mt::MTAudioInterface&         ae = mt::getAudioInterface();
        MidiControlMidiLearnRight     midComp;
        MidiControlMidiLearnTableView rightComp;
        MidiControlMidiLearnLeft      leftComp;
        std::vector<MidiRightData>    midiTableData;
        juce::Rectangle<int>          topBounds = { 10, 10, 905, 44 };
        ui::KickButton                clear1{ "Clear All" };
        int                           numRows{ 0 };
        void                          refreshTable();
        void                          resized();
        void                          paint( juce::Graphics& g ) override;
        void                          visibilityChanged() override;

        void                  saveData( int index );
        juce::ValueTree       projectVTree;
        juce::CriticalSection lock;
        juce::ValueTree       midilearn;

        mt::MTBusManager&           busMgr{ ae.getBusManager() };
        juce::String                busNameReturn;
        juce::String                busNameSubmix;
        int                         busNo{ -1 };
        int                         busNos{ -1 };
        juce::ValueTree             childTree;
        juce::ValueTree             vTree;
        std::map<int, juce::String> midiMapping;

        juce::HashMap<juce::String, int> padLedToButtonId;
        juce::HashMap<juce::String, int> buttonLedToButtonId;
    };

    class MidiControlXYFXOptions : public juce::Component
    {
    public:
        MidiControlXYFXOptions();
        ~MidiControlXYFXOptions();
        ui::KickToggleButton mute1{ "M", "M", "M" };
        ui::KickToggleButton mute2{ "M", "M", "M" };

        ui::KickNumberWithAttachedValue<int> midCh1{ "", MIDI_CH::MIN, MIDI_CH::MAX, 1 };
        ui::KickNumberWithAttachedValue<int> midCh2{ "", MIDI_CH::MIN, MIDI_CH::MAX, 1 };
        ui::KickNumberWithAttachedValue<int> ccNum1{ "", MIDI::MIN, MIDI::MAX, 1 };
        ui::KickNumberWithAttachedValue<int> ccNum2{ "", MIDI::MIN, MIDI::MAX, 1 };

        ui::KickNumberWithAttachedValue<int> lowRang1{ "", MIDI::MIN, MIDI::MAX, 1 };
        ui::KickNumberWithAttachedValue<int> lowRang2{ "", MIDI::MIN, MIDI::MAX, 1 };
        ui::KickNumberWithAttachedValue<int> highRang1{ "", MIDI::MIN, MIDI::MAX, 1 };
        ui::KickNumberWithAttachedValue<int> highRang2{ "", MIDI::MIN, MIDI::MAX, 1 };

    private:
        void resized() override;

        ui::KickLabel xLabel{ "X-Axis" };
        ui::KickLabel midiCh{ "MIDI Channel" };
        ui::KickLabel ccNumber{ "CC Number" };
        ui::KickLabel lowRang{ "Low Range" };
        ui::KickLabel highRang{ "High Range" };
        ui::KickLabel yLabel{ "Y-Axis" };
    };

    class MidiControlXY : public juce::Component
    {
    public:
        std::function<void( juce::MidiMessage&, juce::MidiMessage& )> onGridCoordinateChanged;
        MidiControlXY();
        void            refresh();
        void            visibilityChanged() override;
        void            midiMessages( float x, float y );
        juce::ValueTree vTree;

    private:
        ui::XYFXGrid           leftComp;
        MidiControlXYFXOptions rightComp;
        void                   resized();
        void                   paint( juce::Graphics& g ) override;
        int                    midiXyIndex{ 0 };
    };
    class MidiControlHardwareButtons : public ui::KickViewport
    {
    public:
        MidiControlHardwareButtons();
        void                                 resized() override;
        ui::KickListBox                      lightLedOptions{ ui::KickListBox::BoxType::withoutTitleBar, "Never", {} };
        ui::KickNumberWithAttachedValue<int> midiChannelValue{ "", MIDI_CH::MIN, MIDI_CH::MAX, 1 };
        ui::KickNumberWithAttachedValue<int> ccNumberValue{ "", MIDI::MIN, MIDI::MAX, 1 };
        ui::KickToggleButton                 typeOptions{ "Momentary", "Toggle" };

    protected:
        ui::KickLabel lightLed{ "Light Led" };
        ui::KickLabel midiChannelLabel{ "MIDI Channel" };
        ui::KickLabel ccNumberLabel{ "CC Number" };
        ui::KickLabel type{ "Type" };
    };

    class MidiControlHardwareQlink : public MidiControlHardwareButtons
    {
    public:
        MidiControlHardwareQlink();
        void                                 resized() override;
        ui::KickNumberWithAttachedValue<int> note1{ "", MIDI::MIN, MIDI::MAX, 1 };
        ui::KickListBox                      note2{ ui::KickListBox::BoxType::withoutTitleBar, "Note", mt::ScaleAndNotes::midiNoteNames, 0, 8 };
        ui::KickToggleButton                 typeOptions{ "Relative", "Absolute", "Relative" };
        ui::KickNumberWithAttachedValue<int> lowRangeValue{ "", MIDI::MIN, MIDI::MAX, 1 };
        ui::KickNumberWithAttachedValue<int> highRangeValue{ "", MIDI::MIN, MIDI::MAX, 1 };
        ui::KickToggleButton                 touchSenceOptions{ "Off", "On", "Off" };

    private:
        ui::KickLabel   lowRangeLabel{ "Low Range" };
        ui::KickLabel   highRangeLabel{ "High Range" };
        ui::KickLabel   touchSenceLabel{ "Touch Sense" };
        ui::KickLabel   noteLabel{ "Note" };
        juce::Component listComp;
    };

    class MidiControlPadOptions : public juce::Component
    {
    public:
        MidiControlPadOptions();
        void                                 resized() override;
        ui::KickToggleButton                 on{ "Off", "On", "Off" };
        ui::KickListBox                      ledBox{ ui::KickListBox::BoxType::withoutTitleBar, "None", {} };
        ui::KickNumberWithAttachedValue<int> midiBox{ "", MIDI_CH::MIN, MIDI_CH::MAX, 1 };
        ui::KickNumberWithAttachedValue<int> noteBox1{ "", MIDI::MIN, MIDI::MAX, 1 };
        ui::KickListBox                      noteBox2{ ui::KickListBox::BoxType::withoutTitleBar, "Note", mt::ScaleAndNotes::midiNoteNames, 0, 8 };
        ui::KickListBox                      afterTouchBox{ ui::KickListBox::BoxType::withoutTitleBar, "None", {} };

    private:
        ui::KickTableDataProviders     provider;
        std::unique_ptr<ui::KickTable> table;

        ui::KickLabel velocityLabel{ "Velocity" };
        ui::KickLabel led{ "Light Led" };
        ui::KickLabel midiLabel{ "MIDI Channel" };
        ui::KickLabel noteLabel{ "Note" };
        ui::KickLabel afterTouchLabel{ "After Touch" };
    };

    class MidiControlHardwareLeft : public juce::Component
    {
    public:
        MidiControlHardwareLeft();
        ~MidiControlHardwareLeft();
        void resized() override;
        void paint( juce::Graphics& g );
        void setPadOptionsVisible( bool visible )
        {
            bank.setVisible( visible );
            setAll.setVisible( visible );
            bankLabel.setVisible( visible );
            setAllLabel.setVisible( visible );
        }

    public:
        ui::KickListBox option1List{ ui::KickListBox::BoxType::withoutTitleBar, "Control", {} };
        ui::KickListBox bank{ ui::KickListBox::BoxType::withoutTitleBar, "BANKS", mt::Kick::padBankNames, -1, 1 };

        juce::Rectangle<int> leftTopBounds = { 0, 0, 342, 54 };
        ui::KickLabel        Control{ "Control" };
        ui::KickLabel        para{ "Pad Parameters" };
        ui::KickLabel        bankLabel{ "Bank" };
        ui::KickLabel        setAllLabel{ "Set All" };
        juce::ToggleButton   setAll{ "" };
    };

    class MidiControlHardwareRight : public ui::KickStackedComponent
    {
    public:
        MidiControlHardwareRight();
        ~MidiControlHardwareRight();
        MidiControlHardwareButtons button;
        MidiControlPadOptions      pad;
        MidiControlHardwareQlink   qlink;
        MidiControlXYFXOptions     xy;
    };

    class MidiControlHardware : public juce::Component
    {
    public:
        std::function<void( juce::MidiMessage&, juce::String& )> onMessageSend;

        MidiControlHardware();
        void                    refresh();
        void                    midimessages( int knob, int knobValue, float padVelocity, int padAfterTouch, int buttonState );
        MidiControlHardwareLeft leftComp;
        void                    sendMidiMassages( juce::MidiMessage& data );
        void                    knobCommand( int knob, int value );
        void                    updateValue( int knob, int value );
        void                    setOutput( juce::String list ) { outputPort = list; };
        juce::String&           getOutput() { return outputPort; }
        void                    setKnobValue( int value ) { knobValue = value; };
        void                    handleCommand( CPktMgr::HWMsg_t& f_HWMsg );

    private:
        MidiControlHardwareRight rightComp;
        juce::ValueTree          vTree{};
        juce::ValueTree          midiControl;
        bool                     knobRotated{ false };
        void                     saveData();
        void                     padSelect( bool sel );
        void                     saveBankData();
        void                     createNewBankSettings( juce::String bankName );
        void                     loadBankSettings( juce::String bankName );
        void                     loadDefaultSettings();
        int                      Knob, Value;

        void                  resized() override;
        void                  paint( juce::Graphics& g ) override;
        void                  visibilityChanged() override;
        juce::CriticalSection lock;
        juce::String          outputPort{ "None" };
        int                   knobValue{ 0 };
    };

}
