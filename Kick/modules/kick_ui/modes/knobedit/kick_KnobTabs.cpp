#include "kick_KnobTabs.h"
namespace kick_ui
{
    Content::Content()
    {}

    Content::~Content()
    {}

    void Content::resized()
    {
        auto content = getLocalBounds();
    }

    void Content::paint( juce::Graphics& g )
    {
        mt::utils::fillRoundedRect( g, { 0, 0, 652, 460 }, ui::Theme::backgroundColour( 4 ) );
    }

    CustomTab::CustomTab( KnobGrid& Knob )
        : knob( Knob )
    {
        mt::utils::addAndMakeVisible( this, topControls, middleControls, bottomControls );
        middleControls.onButtonClick            = [&]( CustomButtons button ) { type( button ); };
        bottomControls.level.onSelectionChanged = [&]( int index, const juce::String value ) {
            if( knobnum != -1 )
            {
                auto selectedParameter = bottomControls.level.getSelectedValue();
                setParameter( knobnum, selectedParameter );
            }
        };
        knob.parameterSelection = [&]( juce::String parameter, int value ) -> int {
            auto parameterIndex = bottomControls.level.getIndex( parameter );
            if( parameterIndex >= 0 )
            {
                auto& ae{ mt::getAudioInterface() };
                auto  valueTree    = ae.getProgramVT( ae.getSelectedProgram() );
                int   newValue     = valueTree.getProperty( "CC" + juce::String( parameterIndex ) );
                auto  updatedValue = juce::jlimit( MIDI::MIN, MIDI::MAX, newValue + value );
                valueTree.setProperty( "CC" + juce::String( parameterIndex ), updatedValue, nullptr );
                return updatedValue;
            }
            return 0;
        };
        topControls.isPad = [&]() {
            auto padState = topControls.type.getToggleState();
            knob.setPadState( padState );
        };
        topControls.onPadChange = [&]( int bankIndex, int padIndex ) {
            knob.setPadIndex( bankIndex, padIndex );
        };
    }

    void CustomTab::resized()
    {
        topControls.setBoundsRelative( .0f, .0f, .9969f, .2217f );
        bottomControls.setBoundsRelative( .0f, .3391f, .9969f, .6608f );
        middleControls.setBoundsRelative( .0f, .2434f, 1.0f, .0739f );
    }

    void CustomTab::setIndex( int index )
    {
        indexNum = index;
    }

    void CustomTab::setInsertsList( int insertIndex )
    {
        auto&                     ae          = mt::getAudioInterface();
        auto                      program     = ae.getProgram( ae.getSelectedProgram() );
        auto                      insertsInfo = program->getInsertsInfo( insertIndex );
        std::vector<juce::String> list;

        for( auto it : insertsInfo.params )
            list.push_back( it.name );

        if( list.empty() )
            list.push_back( "OFF" );

        bottomControls.level.setListOptions( list );
    }

    void CustomTab::setListInfo( mt::MTProgramType type, CustomButtons buttonName, bool topState )
    {
        if( type == mt::MTProgramType::Drum )
        {
            if( !topState )
            {
                if( buttonName == mix )
                    bottomControls.level.setListOptions( { "OFF", "Pan", "Velocity", "Solo", "Mute", "sends01", "sends02", "sends03", "sends04" } );
                else if( buttonName == program )
                    bottomControls.level.setListOptions( { "OFF", "GlobalSemiTune", "GlobalFineTune" } );
                else if( buttonName == ins1 || buttonName == ins2 || buttonName == ins3 || buttonName == ins4 )
                {
                    int insertIndex = ( buttonName == ins1 ) ? 0 : ( buttonName == ins2 ) ? 1 :
                                                               ( buttonName == ins3 )     ? 2 :
                                                                                            3;
                    setInsertsList( insertIndex );
                }
            }
            else
            {
                if( buttonName == mix )
                    bottomControls.level.setListOptions( { "OFF", "Level", "Pan", "PadSolo", "PadMute" } );
                else if( buttonName == program )
                    bottomControls.level.setListOptions( { "OFF", "Polyphony", "Glide All", "Bend Range", "Doubling", "Doubling Detune", "Glide Time", "Osc 1 Octave", "Osc 1 Fine", "Osc 1 Shape", "Osc 1 F_Envtoshape", "Osc 1 Quard", "Quard_Detune", "Sub Osc Shape", "Osc 2 Octave", "Osc 2 Micro Detune", "Osc 2 Shape", "Osc 2 F_EnvtoShape", "Osc 2 Sync", "Osc 2 EQ Gain", " Osc 2 EQ Freq", "Osc 2 EQ Keytrack", "Osc 2 EQDrive", "Osc 1 Level", "Osc 2 Level", "Sub Osc Level", "Ring  ModLevel", "Mixer Drive", "LPF Cutoff", "LPF Reso", " LPF Slop", "LPF Saturation", "LPF Env", "LPF Keytrack", "Filter Attack", "Filter Decay", "Filter Sustain", "Filter Release", "Filter Release", "Filter Hold", "Amp Attack", "Amp Decay", "Amp Sustain", "Amp Release", "Amp Hold", "Env 3 StartLevel", "Env 3 StartTime", "Env 3 Slop Hold", "Env 3 Slop Release", " Env 3 Destination", "Lfo To Pitch", " Lfo To Filter", " Lfo To Amp ", "Lfo To PAN", "Lfo Wave", "Lfo Rate", "Layer 1 Semi", "Layer 1 PAN", "Layer 1 Fine", "Layer 1 Level", "Layer 2 Semi", "Layer 2 PAN", "Layer 2 Fine", "Layer 2 Level", "Layer 3 Semi", "Layer 3 PAN", "Layer 3 Fine", "Layer 3 Level", "Layer 4 Semi", "Layer 1 PAN", "Layer 4 Fine", "Layer 4 Level" } );
                else if( buttonName == ins1 || buttonName == ins2 || buttonName == ins3 || buttonName == ins4 )
                {
                    int insertIndex = ( buttonName == ins1 ) ? 0 : ( buttonName == ins2 ) ? 1 :
                                                               ( buttonName == ins3 )     ? 2 :
                                                                                            3;
                    setInsertsList( insertIndex );
                }
            }
        }
        else if( type == mt::MTProgramType::Clip )
        {
            if( !topState )
            {
                if( buttonName == mix )
                    bottomControls.level.setListOptions( { "OFF", "Pan", "Velocity", "Solo", "Mute", "sends01", "sends02", "sends03", "sends04" } );
                else if( buttonName == program )
                    bottomControls.level.setListOptions( { "OFF", "GlobalSemiTune", "GlobalFineTune", "Launch Quantize" } );
                else if( buttonName == ins1 || buttonName == ins2 || buttonName == ins3 || buttonName == ins4 )
                {
                    int insertIndex = ( buttonName == ins1 ) ? 0 : ( buttonName == ins2 ) ? 1 :
                                                               ( buttonName == ins3 )     ? 2 :
                                                                                            3;
                    setInsertsList( insertIndex );
                }
            }
            else
            {
                if( buttonName == mix )
                    bottomControls.level.setListOptions( { "OFF", "Level", "Pan", "PadSolo", "PadMute", "sends01", "sends02", "sends03", "sends04" } );
                else if( buttonName == program )
                    bottomControls.level.setListOptions( { "OFF", "SemiTune", "FineTune", "Reverse", "Wrap", "Fade" } );
                else if( buttonName == ins1 || buttonName == ins2 || buttonName == ins3 || buttonName == ins4 )
                {
                    int insertIndex = ( buttonName == ins1 ) ? 0 : ( buttonName == ins2 ) ? 1 :
                                                               ( buttonName == ins3 )     ? 2 :
                                                                                            3;
                    setInsertsList( insertIndex );
                }
            }
        }
        else if( type == mt::MTProgramType::Keygroup )
        {
            if( !topState )
            {
                if( buttonName == mix )
                    bottomControls.level.setListOptions( { "OFF", "Pan", "Velocity", "Solo", "Mute", "sends01", "sends02", "sends03", "sends04" } );
                else if( buttonName == program )
                    bottomControls.level.setListOptions( { "OFF", "Program Polyphony", "Global SemiTune", "Global FineTune", "Portamento Time", "PortLegato", "PortQuantize" } );
                else if( buttonName == ins1 || buttonName == ins2 || buttonName == ins3 || buttonName == ins4 )
                {
                    int insertIndex = ( buttonName == ins1 ) ? 0 : ( buttonName == ins2 ) ? 1 :
                                                               ( buttonName == ins3 )     ? 2 :
                                                                                            3;
                    setInsertsList( insertIndex );
                }
            }
        }
        else if( type == mt::MTProgramType::Midi )
        {
            if( !topState && buttonName == midi )
                bottomControls.level.setListOptions( { "Bank Select", "Modulation", " Breath", " CC3", " Foot", " Portamento ", " DataEntry ", " Volume ", " Balance ", " CC9 ", " Pan ", " Expression ", " Effect1 ", " Effect2 ", " CC14 ", " CC15 ", " GenPurpose1 ", " GenPurpose2 ", " genpurpose3 ", " genpurpose4 ", " CC20 ", " CC21 ", "CC21", " CC23 ", " CC24 ", " CC25 ", " CC26 ", " CC27 ", " CC28 ", " CC29 ", " CC30 ", " CC31 ", " BankSelLSB ", " ModulationLSB ", " BreathLSB ", " CC35 ", " FootLSB ", " PortamentoLSB ", " DataEntryLSB ", " VolumeLSB ", " BalanceLSB ", " CC41 ", " PanLSB ", " ExpressionLSB ", " Effect1LSB ", " Effect2LSB ", " CC46 ", " CC47 ", " GenPur1LSB ", " GenPur2LSB ", " GenPur3LSB ", " GenPur4LSB ", " CC52 ", " CC53 ", " CC54 ", " CC55 ", " CC56 ", " CC57 ", " CC58 ", " CC59 ", " CC60 ", " CC61 ", " CC62 ", " CC63 ", " SustainOnOff ", " PmentoOnOff ", " SostOnOff ", " SoftPedOnOff ", " LegatoAwitch ", " Hold2 ", " SndControl1 ", " SndControl2 ", " SndControl3 ", " SndControl4 ", " SndControl5 ", " SndControl6 ", " SndControl7 ", " SndControl8 ", " SndControl9 ", " SndControl10 ", " GenPurpose5 ", " GenPurpose6 ", " GenPurpose7 ", " GenPurpose8 ", " PmentoControl ", " CC85 ", " CC86 ", " CC87 ", " HiResVelocity ", " CC89 ", " CC90 ", " Send1 ", " Send2 ", " Send3 ", " Send4 ", " Send4 ", " DataIncrement ", " DataDecrement ", " NRPNLSB ", " NRPNMSB ", " RPNLSB ", " RPNMSB ", " CC102 ", " CC103 ", " CC104 ", " CC105 ", " CC106 ", " CC107 ", " CC108 ", " CC109 ", " CC110 ", " CC111 ", " CC112 ", " CC113 ", " CC114 ", " CC115 ", " CC116 ", " CC117 ", " CC118 ", " CC119 ", " AllSoundOff ", " ResetAll ", " Localonoff ", " AllnoteOff ", " OmniModeOff ", " OmniModeOn ", " MonoMode ", " PolyMode " } );
        }
        /*else if( type == mt::MTProgramType::Cv )
        {
            if( !topState )
            {
                if( buttonName == mix )
                    bottomControls.level.setListOptions( { "OFF", "Solo", "Mute" } );
                else if( buttonName == program )
                    bottomControls.level.setListOptions( { "CV Out1", "CV Out2", "CV Out3", "CV Out4", "CV Out5", "CV Out6", "CV Out7", "CV Out8" } );
            }
        }*/
    }

    void CustomTab::type( CustomButtons buttonName )
    {
        auto& ae       = mt::getAudioInterface();
        auto  pProg    = ae.getProgramDataForTrack( ae.getSelectedSequence(), indexNum );
        auto  type     = pProg ? pProg->type.get() : mt::MTProgramType::Unknown;
        bool  topState = topControls.type.getToggleState();
        setListInfo( type, buttonName, topState );
    }

    void CustomTab::setParameter( int Knobnum, juce::String parameter )
    {
        auto param = parameter;
        knob.labelInfo( Knobnum, parameter );
    }

    TopContent::TopContent()
    {
        mt::utils::addAndMakeVisible( this, type, typeLabel, pad, bank );
        mt::utils::makeHidden( pad, bank );
        typeLabel.setValueBackgroundColour( ui::Theme::backgroundColour( 9 ) );
        typeLabel.setValueColour( ui::Theme::textColour( 1 ) );
        typeLabel.setValueJustification( juce::Justification::centred );
        type.setValueColour( ui::Theme::textColour( 1 ) );
        type.setValueFontHeight( 18 );
        type.onClick = [&] {
            padContent();
            if( isPad )
                isPad();
        };

        bank.onSelectionChanged = [&]( int index, juce::String name ) {
            if( onPadChange )
                onPadChange( index, pad.getSelectedIndex() );
        };
        pad.onSelectionChanged = [&]( int index, juce::String name ) {
            if( onPadChange )
                onPadChange( bank.getSelectedIndex(), index );
        };

        ui::themeChanged.connect( [&]() {
            type.setToggleButtonText( "PROGRAM", "PAD" );
        } );
    }
    void TopContent::paint( juce::Graphics& g )
    {
        mt::utils::fillRoundedRect( g, { 0, 0, 652, 102 }, ui::Theme::backgroundColour( 4 ) );
    }

    void TopContent::resized()
    {
        type.setBoundsRelative( .0306f, .3725f, .1840f, .4314f );
        typeLabel.setBoundsRelative( .0306f, .196f, .184f, .196f );
        pad.setBoundsRelative( .7852f, .196f, .184f, .6078f );
        bank.setBoundsRelative( .5858f, .196f, .184f, .6078f );
    }

    void TopContent::click( mt::MTProgramType types )
    {
        switch( types )
        {
            default:
                type.setToggleState( false, juce::dontSendNotification );
                bank.setVisible( false );
                pad.setVisible( false );
                type.setInterceptsMouseClicks( false, false );
                break;
            case mt::MTProgramType::Drum:
            case mt::MTProgramType::Clip:
                type.setInterceptsMouseClicks( true, true );
                break;
        }
    }

    void TopContent::padContent()
    {
        pad.setTitleColour( ui::Theme::textColour( 1 ) );
        bank.setTitleColour( ui::Theme::textColour( 1 ) );
        if( type.getToggleState() )
        {
            pad.setVisible( true );
            bank.setVisible( true );
        }
        else
        {
            pad.setVisible( false );
            bank.setVisible( false );
        }
    }

    MiddleContent::MiddleContent()
    {
        mt::utils::addAndMakeVisible( this, mix, program, midi, ins1, ins2, ins3, ins4 );
        mix.setRadioGroupId( (int)kick_ui::RadioGroupId::QlinkTab, juce::dontSendNotification );
        program.setRadioGroupId( (int)kick_ui::RadioGroupId::QlinkTab, juce::dontSendNotification );
        ins1.setRadioGroupId( (int)kick_ui::RadioGroupId::QlinkTab, juce::dontSendNotification );
        ins2.setRadioGroupId( (int)kick_ui::RadioGroupId::QlinkTab, juce::dontSendNotification );
        ins3.setRadioGroupId( (int)kick_ui::RadioGroupId::QlinkTab, juce::dontSendNotification );
        ins4.setRadioGroupId( (int)kick_ui::RadioGroupId::QlinkTab, juce::dontSendNotification );
        midi.setRadioGroupId( (int)kick_ui::RadioGroupId::QlinkTab, juce::dontSendNotification );

        auto onClickHandler = [this]( CustomButtons button ) { return [&, button]() { onClick( button ); }; };

        mix.onClick     = onClickHandler( CustomButtons::mix );
        program.onClick = onClickHandler( CustomButtons::program );
        ins1.onClick    = onClickHandler( CustomButtons::ins1 );
        ins2.onClick    = onClickHandler( CustomButtons::ins2 );
        ins3.onClick    = onClickHandler( CustomButtons::ins3 );
        ins4.onClick    = onClickHandler( CustomButtons::ins4 );
        midi.onClick    = onClickHandler( CustomButtons::midi );

        mix.setValueColour( ui::Theme::textColour( 1 ) );
        mix.setValueFontHeight( 18 );
        midi.setValueColour( ui::Theme::textColour( 1 ) );
        midi.setValueFontHeight( 18 );
        program.setValueColour( ui::Theme::textColour( 1 ) );
        program.setValueFontHeight( 18 );
        ins1.setValueColour( ui::Theme::textColour( 1 ) );
        ins1.setValueFontHeight( 18 );
        ins2.setValueColour( ui::Theme::textColour( 1 ) );
        ins2.setValueFontHeight( 18 );
        ins3.setValueColour( ui::Theme::textColour( 1 ) );
        ins3.setValueFontHeight( 18 );
        ins4.setValueColour( ui::Theme::textColour( 1 ) );
        ins4.setValueFontHeight( 18 );

        ui::themeChanged.connect( [&]() {
            mix.setToggleButtonText( "Mix", "Mix" );
            midi.setToggleButtonText( "MIDI CC", "MIDI CC" );
            program.setToggleButtonText( "Program", "Program" );
            ins1.setToggleButtonText( "Ins 1", "Ins 1" );
            ins2.setToggleButtonText( "Ins 2", "Ins 2" );
            ins3.setToggleButtonText( "Ins 3", "Ins 3" );
            ins4.setToggleButtonText( "Ins 4", "Ins 4" );
        } );
    }
    void MiddleContent::resized()
    {
        mix.setBoundsRelative( .0f, .0f, .158f, 1.0f );
        midi.setBoundsRelative( .0f, .0f, .158f, 1.0f );
        program.setBoundsRelative( .167f, .0f, .158f, 1.0f );
        ins1.setBoundsRelative( .3358f, .0f, .158f, 1.0f );
        ins2.setBoundsRelative( .5038f, .0f, .158f, 1.0f );
        ins3.setBoundsRelative( .671f, .0f, .158f, 1.0f );
        ins4.setBoundsRelative( .839f, .0f, .158f, 1.0f );
    }

    void MiddleContent::paint( juce::Graphics& g )
    {}

    void MiddleContent::visibilityChanged()
    {}
    void MiddleContent::updateButtons( mt::MTProgramType type )
    {
        for( auto it : getChildren() )
            it->setVisible( false );
        switch( type )
        {
            default:
                mt::utils::makeVisible( mix, program, ins1, ins2, ins3, ins4 );
                mt::utils::makeHidden( midi );
                break;
            case mt::MTProgramType::Keygroup:
                mt::utils::makeHidden( midi );
                mt::utils::makeVisible( mix, program, ins1, ins2, ins3, ins4 );
                break;
            case mt::MTProgramType::Cv:
                mt::utils::makeVisible( mix, program );
                mt::utils::makeHidden( ins1, ins2, ins3, ins4 );
                break;
            case mt::MTProgramType::Midi:
                mt::utils::makeVisible( midi );
                mt::utils::makeHidden( mix, program, ins1, ins2, ins3, ins4 );
                break;
        }
    }

    void MiddleContent::onClick( CustomButtons button )
    {
        if( onButtonClick )
            onButtonClick( button );
    }

    BottomContent::BottomContent()
    {
        mt::utils::addAndMakeVisible( this, parameter, level, momentary );
        parameter.setValueBackgroundColour( ui::Theme::backgroundColour( 9 ) );
        parameter.setFontHeight( 18 );
        momentary.setValueColour( ui::Theme::textColour( 1 ) );
        momentary.setValueFontHeight( 18 );
    }

    void BottomContent::paint( juce::Graphics& g )
    {
        mt::utils::fillRoundedRect( g, { 0, 0, 652, 304 }, ui::Theme::backgroundColour( 4 ) );
    }

    void BottomContent::resized()
    {
        parameter.setBoundsRelative( .0168f, .0657f, .2453f, .1447f );
        level.setBoundsRelative( .5858f, .0657f, .3834f, .1447f );
        momentary.setBoundsRelative( .784f, .2697f, .1837f, .1447f );
    }
}
