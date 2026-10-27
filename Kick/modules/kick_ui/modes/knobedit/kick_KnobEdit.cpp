#include "kick_KnobEdit.h"
namespace kick_ui
{
    class screenContent;
    KnobEdit::KnobEdit()
        : kick_ui::IScreen( kick_ui::Screen::MenuKnobEdit )
    {
        mt::utils::addAndMakeVisible( this, header, custom, customContent, screen, knob, content, customContent, tracks, label, trackLabel );
        header.onBackButtonPressed = [this]( bool ) { closeScreen(); };
        header.showSettingsButton( true );

        screen.setRadioGroupId( (int)kick_ui::RadioGroupId::Knobedit );
        custom.setRadioGroupId( (int)kick_ui::RadioGroupId::Knobedit );
        screen.triggerClick();
        tracks.onSelectionChanged = [&]( int Index, const juce::String& value ) {
            auto& ae    = mt::getAudioInterface();
            auto  pProg = ae.getProgramDataForTrack( ae.getSelectedSequence(), Index );
            auto  type  = pProg ? pProg->type.get() : mt::MTProgramType::Unknown;
            label.setText( getProgramType( type ), juce::dontSendNotification );
            customContent.middleControls.updateButtons( type );
            customContent.topControls.click( type );
            customContent.setIndex( Index );
            knob.setIndex( Index );
        };
        label.setFontHeight( 16 );

        screen.onClick = [&] {
            knob.setVisible( true );
            knob.setInfoText2( "Q-Link controls are fixed to the mode you are in" );
            knob.labelBounds( false );
            content.setVisible( true );
            customContent.setVisible( false );
            knobControl.setMode( 1 );
            knob.repaint();
        };
        custom.onClick = [&] {
            knob.setVisible( true );
            knob.setInfoText2( "SELECTED Q-LINK" );
            knob.labelBounds( true );
            content.setVisible( false );
            customContent.setVisible( true );
            knobControl.setMode( 0 );
            knob.repaint();
        };

        tracks.setJustificationType( juce::Justification::centredLeft );
        tracks.setTitleColour( ui::Theme::textColour( 1 ) );
        label.setValueBackgroundColour( ui::Theme::backgroundColour( 9 ) );

        trackLabel.setValueBackgroundColour( ui::Theme::backgroundColour( 9 ) );
        trackLabel.setValueColour( ui::Theme::textColour( 1 ) );

        custom.setValueColour( ui::Theme::textColour( 1 ) );
        custom.setValueFontHeight( 18 );
        screen.setValueColour( ui::Theme::textColour( 1 ) );
        screen.setValueFontHeight( 18 );

        ui::themeChanged.connect( [&]() {
            custom.setToggleButtonText( "Custom", "Custom" );
            screen.setToggleButtonText( "Screen", "Screen" );
            header.setIcon( "Icon_qlink" );
        } );

        knob.onknobSelection = [&]( int index ) {
            customContent.setSelectedKnob( index );
        };
    }

    KnobEdit::~KnobEdit() {}

    void KnobEdit::resized()
    {
        auto b = getLocalBounds();
        header.setBounds( b.removeFromTop( ui::layout::header.getHeight() ) );
        tracks.setBoundsRelative( .2250f, 0.0503f, .1956f, .0535f );
        label.setBoundsRelative( .4305f, 0.0503f, .1956f, .0535f );
        trackLabel.setBoundsRelative( .2294f, 0.0000f, .0648f, .0583f );
        custom.setBoundsRelative( .0097f, .9297f, .1653f, .0568f );
        screen.setBoundsRelative( .1790f, .9297f, .1653f, .0568f );
        knob.setBoundsRelative( .0097f, .1438f, .3346f, .7692f );
        content.setBoundsRelative( .3542f, .1438f, .6379f, .7692f );
        customContent.setBoundsRelative( .3542f, .1438f, .6399f, .7692f );
    }

    juce::String KnobEdit::getProgramType( mt::MTProgramType type )
    {
        switch( type )
        {
            case mt::MTProgramType::Drum: return "Drum Program"; break;
            case mt::MTProgramType::Keygroup: return "Keyboard Program"; break;
            case mt::MTProgramType::Midi: return "Midi Program"; break;
            case mt::MTProgramType::Audio: return "Audio Program"; break;
            case mt::MTProgramType::Clip: return "Clip Program"; break;
            case mt::MTProgramType::Plugin: return "Plugin Program"; break;
            case mt::MTProgramType::Cv: return "CV Program"; break;
            case mt::MTProgramType::Unknown: return "Unknown Program"; break;
            default: return "Unknown Program"; break;
        }
    }
    void KnobEdit::knobCommand( int knobnum, int value )
    {
        knob.knobCommand( knobnum, value );
    }

    void KnobEdit::handleCommand( CPktMgr::HWMsg_t& f_HwMsg )
    {
        if( contracts::DclContractsHelper::isButtonMessage( f_HwMsg ) && ( contracts::DclContractsHelper::getButtonState( f_HwMsg ) == contracts::DeviceControlContracts::BTN_PRESSED ) )
        {
            auto  buttonCode = contracts::DclContractsHelper::getButtonCode( f_HwMsg );
            auto& ae         = mt::getAudioInterface();
            auto& aeSel      = ae.getSelected();
            switch( buttonCode )
            {
                case contracts::DeviceControlContracts::EN_PLAY: ae.setSequencePlay( aeSel.sequence, false, true ); break;
                case contracts::DeviceControlContracts::EN_PLAYSTART: ae.setSequencePlay( aeSel.sequence, true, true ); break;
                case contracts::DeviceControlContracts::EN_STOP: ae.setSequencePause(); break;
            }
        }
    }

    void KnobEdit::knobLongPress()
    {
        startTimer( timeinms );

        if( screen.getToggleState() )
            comp = &content;
        else if( custom.getToggleState() )
            comp = &customContent;

        juce::Component tempBounds;
        tempBounds.setSize( 500, 450 );
    }
    void KnobEdit::timerCallback()
    {
        if( auto dw = getCurrentlyModalComponent() )
            dw->exitModalState( 0 );
        stopTimer();
    }
    void KnobEdit::visibilityChanged()
    {
        if( isVisible() )
        {
            auto& ae  = mt::getAudioInterface();
            auto& sel = ae.getSelected();

            std::vector<mt::MTITrackData*> trackList;
            ae.getTrackListData( sel.sequence, trackList );

            std::vector<juce::String> options;
            {
                for( auto& i : trackList )
                    if( i )
                        options.push_back( i->name.get() );

                tracks.setListOptions( options );
            }
            tracks.setSelected( sel.track, juce::sendNotification );
        }
    }
}
