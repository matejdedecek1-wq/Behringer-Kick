#include "kick_MidiControl.h"

namespace kick_ui
{
#define padIndex 22
    MidiControl::MidiControl()
        : kick_ui::IScreen( kick_ui::Screen::MenuMidiControl )
    {
        mt::utils::addAndMakeVisible( this, midiLearn, header, hardware, xy, hardWareTab, xyMidiTab, midiLearnTab, outPutLabel, output, enableMapping, monitor );
        mt::utils::makeHidden( xyMidiTab, midiLearnTab );
        mt::utils::setRadioGroupID( (int)kick_ui::RadioGroupId::midiControl, hardware, xy, midiLearn );

        header.onBackButtonPressed = [this]( bool ) { closeScreen(); };
        header.showOptions( true, false );
        mt::utils::setValueColour( ui::Theme::textColour( 1 ), midiLearn, hardware, xy, monitor, enableMapping );
        mt::utils::setValueFontHeight( 18, midiLearn, hardware, xy, monitor, enableMapping );

        output.setJustificationType( juce::Justification::centredLeft );
        output.setTitleColour( ui::Theme::textColour( 1 ) );
        outPutLabel.setJustificationType( juce::Justification::centredLeft );
        outPutLabel.setValueBackgroundColour( ui::Theme::backgroundColour( 9 ) );
        outPutLabel.setValueColour( ui::Theme::textColour( 1 ) );
        output.setBackGroundColours( ui::Theme::textColour( 4 ), ui::Theme::backgroundColour() );
        hardware.toFront( true );
        xy.toFront( true );
        midiLearn.toFront( true );

        hardware.onClick = [&]() {
            mt::utils::makeVisible( hardWareTab, output, outPutLabel );
            mt::utils::makeHidden( xyMidiTab, midiLearnTab, enableMapping );
        };

        xy.onClick = [&]() {
            mt::utils::makeVisible( xyMidiTab );
            mt::utils::makeHidden( hardWareTab, midiLearnTab, enableMapping, output, outPutLabel );
        };

        midiLearn.onClick = [&]() {
            mt::utils::makeVisible( midiLearnTab, enableMapping );
            mt::utils::makeHidden( xyMidiTab, hardWareTab, output, outPutLabel );
        };

        monitor.onClick = [&]() {
            mt::utils::showDialog( "MIDI MONITOR", &midiMonitor, [&]( int R ) {

            },
                                   { ui::layout::keyBoardPopup } );

            midiMonitor.refresh();
        };

        enableMapping.onClick = [&]() {
            if( enableMapping.getToggleState() == true )
            {
                midiLearnTab.startLearning();
            }

            else if( enableMapping.getToggleState() == false )
            {
                DBG( " Learn STOPPED" );
                midiLearnTab.stopMidiInput();
            }
        };

        xyMidiTab.onGridCoordinateChanged = [&]( juce::MidiMessage& xaxis, juce::MidiMessage& yaxis ) {
            hardWareTab.sendMidiMassages( xaxis );
            hardWareTab.sendMidiMassages( yaxis );
        };

        output.onSelectionChanged = [&]( int index, const juce::String value ) {
            hardWareTab.setOutput( value );
        };

        hardWareTab.onMessageSend = [&]( juce::MidiMessage& Msg, juce::String& Port ) {
            midiMonitor.addMidiMessage( Msg, Port );
        };

        ui::themeChanged.connect( [&]() {
            header.setIcon( "Icon_MidiControl" );
            midiLearn.setToggleButtonText( "MIDI Learn", "MIDI Learn" );
            hardware.setToggleButtonText( "Hardware", "Hardware" );
            xy.setToggleButtonText( "XY", "XY" );
        } );

        hardware.triggerClick();
    }

    MidiControl::~MidiControl() {}

    void MidiControl::resized()
    {
        auto b = getLocalBounds(); //1024,536
        header.setBounds( b.removeFromTop( ui::layout::header.getHeight() ) );
        output.setBoundsRelative( .2636f, 0.04833f, .15625f, .04833f );
        outPutLabel.setBoundsRelative( .264f, 0.01033f, .15625f, .04833f );

        monitor.setBoundsRelative( 0.5153f, 0.917f, 0.1650f, 0.063f );
        midiLearn.setBoundsRelative( 0.347f, 0.917f, 0.1650f, 0.063f );
        hardware.setBoundsRelative( 0.0097f, 0.917f, 0.1650f, 0.063f );
        xy.setBoundsRelative( 0.178f, 0.917f, 0.1650f, 0.063f );
        hardWareTab.setBoundsRelative( 0.0f, 0.1399f, 1.0f, 1.0f );
        xyMidiTab.setBoundsRelative( 0.0f, 0.1399f, 1.0f, 1.0f );
        midiLearnTab.setBoundsRelative( 0.0f, 0.1399f, 1.0f, 1.0f );
        enableMapping.setBoundsRelative( 0.7207f, 0.0333f, 0.1220f, 0.05666f );
    }

    void MidiControl::paint( juce::Graphics& g )
    {
    }

    void MidiControl::visibilityChanged()
    {
        if( isVisible() )
        {
            auto& outPorts = mt::getAudioInterface().getMidiDeviceNames( true );
            auto  it       = std::find( outPorts.begin(), outPorts.end(), hardWareTab.getOutput() );
            auto  selIndex = it != outPorts.end() ? std::distance( outPorts.begin(), it ) : 0;
            output.setListOptions( outPorts, selIndex );
        }
    }

    void MidiControl::handleCommand( CPktMgr::HWMsg_t& f_HWMsg )
    {
        if( contracts::DclContractsHelper::isButtonMessage( f_HWMsg ) && ( contracts::DclContractsHelper::getButtonState( f_HWMsg ) == contracts::DeviceControlContracts::BTN_PRESSED ) )
        {
            auto  buttonCode = contracts::DclContractsHelper::getButtonCode( f_HWMsg );
            auto& ae         = mt::getAudioInterface();
            auto& aeSel      = ae.getSelected();
            switch( buttonCode )
            {
                case contracts::DeviceControlContracts::EN_PLAY: ae.setSequencePlay( aeSel.sequence, false, true ); return;
                case contracts::DeviceControlContracts::EN_PLAYSTART: ae.setSequencePlay( aeSel.sequence, true, true ); return;
                case contracts::DeviceControlContracts::EN_STOP: ae.setSequencePause(); return;
            }
        }

        {
            bool isPadControl = contracts::DclContractsHelper::isPadButtonMessage( f_HWMsg );
            int  index        = isPadControl ? screenIndexInPad( f_HWMsg.data[0] ) : screenIndex( f_HWMsg.data[0] );
            if( isPadControl && index != -1 )
            {
                index += padIndex;
            }

            if( contracts::DclContractsHelper::isBankMessage( f_HWMsg ) )
            {
                switch( f_HWMsg.data[0] )
                {
                    case contracts::DeviceControlContracts::BtnLedSequence::EN_BANK_A:
                        hardWareTab.leftComp.bank.setSelected( 0 );
                        break;
                    case contracts::DeviceControlContracts::BtnLedSequence::EN_BANK_B:
                        hardWareTab.leftComp.bank.setSelected( 1 );
                        break;
                    case contracts::DeviceControlContracts::BtnLedSequence::EN_BANK_C:
                        hardWareTab.leftComp.bank.setSelected( 2 );
                        break;
                    case contracts::DeviceControlContracts::BtnLedSequence::EN_BANK_D:
                        hardWareTab.leftComp.bank.setSelected( 3 );
                        break;
                    case contracts::DeviceControlContracts::BtnLedSequence::EN_BANK_E:
                        hardWareTab.leftComp.bank.setSelected( 4 );
                        break;
                    case contracts::DeviceControlContracts::BtnLedSequence::EN_BANK_F:
                        hardWareTab.leftComp.bank.setSelected( 5 );
                        break;
                    case contracts::DeviceControlContracts::BtnLedSequence::EN_BANK_G:
                        hardWareTab.leftComp.bank.setSelected( 6 );
                        break;
                    case contracts::DeviceControlContracts::BtnLedSequence::EN_BANK_H:
                        hardWareTab.leftComp.bank.setSelected( 7 );
                        break;
                }
            }
            else if( index != -1 )
                hardWareTab.leftComp.option1List.setSelected( index );
            hardWareTab.handleCommand( f_HWMsg );
        }
    }

    int MidiControl::screenIndex( int screenName )
    {
        auto it = std::find( controlEnums.begin(), controlEnums.end(), screenName );

        if( it != controlEnums.end() )
        {
            return it - controlEnums.begin();
        }
        return -1;
    }

    int MidiControl::screenIndexInPad( int screenName )
    {
        auto it = std::find( controlPadEnums.begin(), controlPadEnums.end(), screenName );

        if( it != controlPadEnums.end() )
        {
            return it - controlPadEnums.begin();
        }
        return -1;
    }

#undef padIndex

    ////////////////////////////////////////////////////////////////////////////
    ///////             MIDI MONITOR                                ///////////
    ///////////////////////////////////////////////////////////////////////////

    MidiMonitor::MidiMonitor()
    {
        mt::utils::addAndMakeVisible( this, in, out, clear, close, tableView );
        mt::utils::addChildComponents( this, tableView );
        mt::utils::setRadioGroupID( 410, in, out );
        mt::utils::setValueColour( ui::Theme::textColour( 1 ), in, out, clear, close );
        mt::utils::setValueFontHeight( 16, in, out, clear, close );

        tableView.setInterceptsMouseClicks( false, false );
        tableView.setRowHeight( 44 );
        tableView.setRowSpacing( 1 );
        tableView.setMargin( 0, 0, 1, 0 );
        tableView.setSelectedRowBackgroundColour( ui::Theme::backgroundColour( 9 ) );
        tableView.setRowBackgroundColour( ui::Theme::backgroundColour( 9 ) );
        tableView.setHeaderHeight( 40 );
        tableView.showHeader( true );
        tableView.clearSelection();

        provider.numRows = [&]() {
            if( in.getToggleState() )
                return (int)midiInData.size() > 8 ? 8 : (int)midiInData.size();
            else if( out.getToggleState() ) return (int)midiOutData.size() > 8 ? 8 : (int)midiOutData.size();
            return 0;
        };

        provider.hdrComponent = [&]( juce::Component* Existing ) -> juce::Component* { return dynamic_cast<MidiMonitor::Header*>( Existing ) ? Existing : new MidiMonitor::Header(); };

        provider.cellComponent = [&]( int Index, juce::Component* Existing ) -> juce::Component* {
            auto comp = dynamic_cast<MidiMessageDisplay*>( Existing );

            if( comp == nullptr )
            {
                comp = new MidiMessageDisplay();
            }
            if( in.getToggleState() )
            {
                if( midiInData.size() > 8 )
                    comp->updateData( midiInData[midiInData.size() - 8 + Index] );
                else
                    comp->updateData( midiInData[Index] );
            }
            else if( out.getToggleState() )
            {
                if( midiOutData.size() > 8 )
                    comp->updateData( midiOutData[midiOutData.size() - 8 + Index] );
                else
                    comp->updateData( midiOutData[Index] );
            }
            return comp;
        };

        in.onClick  = [&]() { refresh(); };
        out.onClick = [&]() { refresh(); };

        clear.onClick = [&]() {
            if( out.getToggleState() )
                midiOutData.clear();
            else if( in.getToggleState() )
                midiOutData.clear();
            refresh();
        };

        close.onClick = [&]() {
            if( auto dw = getCurrentlyModalComponent() )
                dw->exitModalState( 0 );
        };

        in.triggerClick();
    }

    void MidiMonitor::addMidiMessage( juce::MidiMessage& Msg, juce::String& Port )
    {
        static int                      counter = 1;
        MidiMessageDisplay::MessageData midiMessage;
        midiMessage.port    = Port;
        midiMessage.Index   = counter;
        midiMessage.channel = Msg.getChannel();
        auto velocity       = juce::String( (int)Msg.getVelocity() );
        if( Msg.isNoteOnOrOff() )
        {
            midiMessage.type = Msg.isNoteOn() ? "Note On" : "Note Off";
            midiMessage.note = Msg.getMidiNoteName( Msg.getNoteNumber(), true, true, 0 );

            auto perc           = juce::String( ( (int)( Msg.getVelocity() ) / MIDI::MAX ) * 100 );
            midiMessage.details = juce::String( "Velocity  " + perc + "% ( " + velocity + " )" );
        }
        else
        {
            auto ccName         = Msg.getControllerName( Msg.getControllerNumber() );
            midiMessage.type    = "Control Change " + juce::String( (int)Msg.getControllerNumber() );
            midiMessage.details = ( ccName ? ccName : "" ) + juce::String( " :" ) + juce::String( Msg.getControllerValue() );
        }

        midiOutData.push_back( midiMessage );
        refresh();
        counter++;
    }

    void MidiMonitor::paint( juce::Graphics& G )
    {
        G.fillAll( ui::Theme::backgroundColour( 1 ) );
    }

    void MidiMonitor::resized()
    {
        auto b = getLocalBounds();

        auto bottomBounds = b.removeFromBottom( 50 );
        tableView.setBounds( b );
        bottomBounds.removeFromBottom( 10 );
        bottomBounds.removeFromLeft( 10 );
        bottomBounds.removeFromRight( 10 );
        auto w = ( bottomBounds.getWidth() - 30 ) / 4;
        in.setBounds( bottomBounds.removeFromLeft( w ) );
        bottomBounds.removeFromLeft( 10 );
        out.setBounds( bottomBounds.removeFromLeft( w ) );
        bottomBounds.removeFromLeft( 10 );
        clear.setBounds( bottomBounds.removeFromLeft( w ) );
        bottomBounds.removeFromLeft( 10 );
        close.setBounds( bottomBounds );
    }

    MidiMessageDisplay::MidiMessageDisplay()
    {
        mt::utils::addAndMakeVisible( this, index, type, port, channel, note, details );
        mt::utils::setValueColour( ui::Theme::textColour( 1 ), index, type, port, channel, note, details );
        mt::utils::setValueFontHeight( 16, index, type, port, channel, note, details );
    }

    void MidiMessageDisplay::updateData( MessageData Data )
    {
        index.setText( juce::String( Data.Index ), juce::dontSendNotification );
        type.setText( Data.type, juce::dontSendNotification );
        port.setText( Data.port, juce::dontSendNotification );
        channel.setText( juce::String( Data.channel ), juce::dontSendNotification );
        note.setText( Data.note, juce::dontSendNotification );
        details.setText( Data.details, juce::dontSendNotification );
    }

    void MidiMessageDisplay::resized()
    {
        auto b = getLocalBounds().reduced( 10 );
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
}
