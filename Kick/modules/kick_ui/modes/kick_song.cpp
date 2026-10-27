/*
 * Copyright (c) 2021 MUSIC Tribe
 *
 * This file is part of Kick.
 *
 * Kick is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Affero General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * Kick is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with Kick. If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "contracts/dcl/dcl_contracts_helper.h"
#include "contracts/dcl/dcl_signals.h"

namespace kick_ui
{
    struct CreateNewSong : public juce::Component
    {
        ui::KickLabel Name{ "Enter Name" };

        CreateNewSong()
        {
            Name.setFontHeight( 32 );
            Name.setPadding( 20 );
            addAndMakeVisible( Name );
            Name.setEditable( true, true );
        }

        void resized() override
        {
            Name.setBounds( getLocalBounds().reduced( 20 ) );
        }
    };

    SaveFile::SaveFile( const juce::String FileName )
    {
        addAndMakeVisible( fileName );
        fileName.setValueColour( ui::Theme::textColour( 1 ) );
        fileName.setValueJustification( juce::Justification::centred );
        fileName.setValueBackgroundColour( ui::Theme::backgroundColour( 9 ) );
        fileName.setValueFontHeight( 24 );
        fileName.setEditable( true );
        fileName.setText( "Do You Want To Export " + FileName + " Song", juce::dontSendNotification );
    }

    void SaveFile::resized()
    {
        fileName.setBoundsRelative( .15f, .4f, .7f, .2f );
    }

    ///////////////////   Song Content   ////////////////////

    KickSongContent::KickSongContent()
        : ae( mt::getAudioInterface() )
        , pads( std::make_unique<ui::KickPadButtonGroupComponent>( ui::KickPadButton::PadButtonType::with3Rows ) )

    {
        setLookAndFeel( ui::KickLooknFeel::getLnf() );

        tableView.setRowHeight( 52 );
        tableView.setRowSpacing( 1 );
        tableView.setMargin( 0, 0, 1, 0 );
        tableView.setSelectedRowBackgroundColour( ui::Theme::backgroundColour( 0 ) );
        tableView.setRowBackgroundColour( ui::Theme::backgroundColour( 3 ) );
        tableView.setHeaderHeight( 40 );
        tableView.showHeader( true );
        tableView.setTableEndInfo( "END OF SONG", ui::Theme::textColour( 1 ), 18, true );

        pads->setActive( -1, false );
        pads->setHighlight( -1 );
        pads->setPadButtonSpacer( 4 );

        mt::utils::setValueFontHeight( 18, convert, insert, clr, del );
        mt::utils::setValueColour( ui::Theme::textColour( 1 ), convert, insert, clr, del );
        mt::utils::addChildComponents( this, tableView, pads.get(), insert, convert, clr, del );

        mt::signals::bankChanged.connect( [&]( int index, juce::String bankName ) {
            if( kick_ui::KickHome::Instance().getCurrentScreen() == kick_ui::Screen::MenuSong )
            {
                pads->setBank( bankName );
                pads->refreshAllPads();
            }
        } );

        provider.numRows      = [&]() -> int { return (int)info.steps.size(); };
        provider.hdrComponent = [&]( juce::Component* Existing ) -> juce::Component* { return dynamic_cast<KickSongContent::Header*>( Existing ) ? Existing : new KickSongContent::Header(); };

        provider.cellComponent = [&]( int Row, juce::Component* Existing ) -> juce::Component* {
            auto seqData            = ae.getSequenceData( info.steps[Row].seqNo );
            info.steps[Row].bars    = seqData->length.get();
            info.steps[Row].bpm     = isSeqSong ? bpmValue : seqData->tempo;
            info.steps[Row].loopEnd = seqData->loopEnd.get();
            auto comp               = dynamic_cast<KickSongTableDisplay*>( Existing );
            if( comp == nullptr )
                comp = new KickSongTableDisplay();

            comp->refreshStep( info.steps[Row] );
            comp->onRepeatChanged = [&, Row]( int Value ) {
                info.steps[Row].repeatCount = Value;
                ae.editSongStep( songName, info.steps[Row], Row );
            };

            return comp;
        };

        pads->OnButtonClicked = [this]( int Index ) {
            if( !isPlaying )
            {
                if( selectedPadSeq != Index )
                {
                    setPadLED( selectedPadSeq, contracts::DeviceControlContracts::PadButtonState::StateOn, contracts::DeviceControlContracts::PadColourIds::PadColourOrange );
                    setPadLED( Index, contracts::DeviceControlContracts::PadButtonState::StateOn, contracts::DeviceControlContracts::PadColourIds::padColourlightGreen1 );
                }
                else //if user press the same selected pad again
                    setPadLED( Index, contracts::DeviceControlContracts::PadButtonState::StateOn, contracts::DeviceControlContracts::PadColourIds::padColourlightGreen1 );
                pads->clearAllBreathing();
                pads->setBreathing( Index );
                pads->refreshAllPads();
                selectedPadSeq = Index;
            }
        };

        clr.onClick = [&]() {
            if( info.steps.size() )
            {
                if( isPlaying )
                    stopOrPause();
                ae.deleteAllSongSteps( songName );
                updateSong( songName );
            }
        };

        insert.onClick = [&]() {
            auto idx = pads->getSelected();
            if( idx < seqList.size() )
            {
                mt::MTSongStepInfo _step;
                _step.seqNo = idx;

                auto selStep = tableView.getSelectedRow();
                auto insPos  = ( selStep < info.steps.size() ) ? selStep : -1;
                if( ( selStep < info.steps.size() ) && ( idx == info.steps[selStep].seqNo ) )
                { // Edit Step.
                    _step.repeatCount = info.steps[selStep].repeatCount + 1;
                    ae.editSongStep( songName, _step, insPos );
                    selStep -= 1;
                }
                else
                { // Add new step.
                    ae.addSongStep( songName, _step, insPos + 1 );
                }

                updateSong( songName );
                tableView.selectRow( ( selStep + 1 ) < info.steps.size() ? selStep + 1 : 0 );
            }
        };

        del.onClick = [&]() {
            auto selStep = tableView.getSelectedRow();
            ae.deleteSongStep( songName, selStep );
            updateSong( songName );
        };

        mt::signals::onSongSequencePlay.connect( [this]( int CurrentSeq, int NextSeq, int CurrentSeqIndex ) {
            juce::MessageManager::callAsync( [this, CurrentSeq, NextSeq, CurrentSeqIndex] {
                setSequenceToPlay( CurrentSeq, NextSeq );
                if( CurrentSeqIndex != -1 )
                    tableView.selectRow( CurrentSeqIndex );
            } );
        } );

        mt::signals::onNextSeqAboutToPlayInSong.connect( [this]( int NextSeq ) {
            juce::MessageManager::callAsync( [this, NextSeq] {
                setSequenceToPlay( selectedId, NextSeq );
            } );
        } );

        pads->setSelected( 0, juce::dontSendNotification );
        pads->setBreathing( 0 );
    }

    void KickSongContent::updateSong( const juce::String& Song )
    {
        songName = Song;
        info     = ae.getSongInfo( songName );
        displayState( songName );

        if( isSeqSong )
            for( auto& step : info.steps )
                step.bpm = bpmValue;

        if( tableView.getNumRows() == 1 )
            tableView.selectRow( 0 );
        refreshTable();
    }

    void KickSongContent::displayState( juce::String& Song )
    {
        for( int index = info.steps.size() - 1; index >= 0; --index )
            if( info.steps[index].dispName.isEmpty() )
                ae.deleteSongStep( songName, index );

        info = ae.getSongInfo( songName );
    }

    void KickSongContent::playOrResume()
    {
        isPlaying = true;
        tableView.setInterceptsMouseClicks( false, false );
        if( isPlaying && info.steps.size() )
        {
            pads->clearAllBreathing();
            ae.playSong( songName, tableView.getSelectedRow() );
        }
        else
            stopOrPause();
        del.setEnabled( !isPlaying );
    }

    void KickSongContent::stopOrPause()
    {
        isPlaying = false;
        del.setEnabled( !isPlaying );
        tableView.setInterceptsMouseClicks( true, true );
        ae.stopSong();
        pads->resetBorderColor( selectedId );
        pads->clearAllBreathing();
        pads->setSelected( 0, juce::sendNotification );
        setPadLED( selectedId + pads->getBankOffset(), contracts::DeviceControlContracts::PadButtonState::StateOn, contracts::DeviceControlContracts::PadColourIds::PadColourOrange );

        selectedId = -1;
        tableView.selectRow( 0 );
    }

    void KickSongContent::refreshSequenceList()
    {
        ae.getSequenceDataList( seqList );

        pads->clearAllPadData();
        pads->setBank( "A" );
        for( int index = 0; index < seqList.size(); ++index )
        {
            pads->setActive( index, true );
            pads->setRow1Text( index, seqList[index]->name, juce::dontSendNotification );
            pads->setRow2Text( index, juce::String( (int)seqList[index]->length.get() ) + " Bar", juce::dontSendNotification );
            pads->setRow3Text( index, isSeqSong ? juce::String( bpmValue ) + " BPM" : juce::String( seqList[index]->tempo ) + " BPM", juce::dontSendNotification );
            setPadLED( index % 16, contracts::DeviceControlContracts::PadButtonState::StateOn, contracts::DeviceControlContracts::PadColourIds::PadColourOrange );
        }
        pads->refreshAllPads();
    }

    void KickSongContent::Header::resized()
    {
        sequenceLabel.setBoundsRelative( .018f, .09f, .18f, .82f );
        repeatLabel.setBoundsRelative( .272f, .09f, .18f, .82f );
        bpmLabel.setBoundsRelative( .523f, .09f, .18f, .82f );
        barLabel.setBoundsRelative( .748f, .09f, .18f, .82f );
    }

    void KickSongContent::paint( juce::Graphics& g )
    {
        mt::utils::fillRoundedRect( g, tableView.getBounds(), ui::Theme::backgroundColour( 3 ) );
        mt::utils::renderRoundedRect( g, tableView.getBounds(), ui::Theme::outlineColour( 3 ) );
    }

    void KickSongContent::resized()
    {
        tableView.setBoundsRelative( 0.f, 0.f, .54f, .887f );
        pads->setBoundsRelative( 0.55f, 0.f, 0.45f, 0.9f );

        clr.setBoundsRelative( 0.f, 0.91f, 0.1174f, 0.09f );

        //convert.setBoundsRelative( 0.6321f, 0.91f, 0.1174f, 0.09 );
        insert.setBoundsRelative( 0.7534f, 0.91f, 0.1174f, 0.09f );
        del.setBoundsRelative( 0.8747f, 0.91f, 0.1174f, 0.09f );
    }

    void KickSongContent::visibilityChanged()
    {
        bool visible = isVisible();
        for( auto it : getChildren() )
            it->setVisible( visible );
        if( !visible )
        {
            contracts::DclSignals::sendCommandToDcl( contracts::DclContractsHelper::clearAllPadsMessage() );

            contracts::DclSignals::refreshPadLeds();
        }
        else
        {
            contracts::DclSignals::sendCommandToDcl( contracts::DclContractsHelper::clearAllPadsMessage() );
            refreshSequenceList();
        }
    }

    void KickSongContent::setSequenceToPlay( int CurrentPlaying, int NextSequence )
    {
        if( CurrentPlaying >= 0 )
        {
            pads->resetBorderColor( selectedId );
            setPadLED( selectedId, contracts::DeviceControlContracts::PadButtonState::StateOn, contracts::DeviceControlContracts::PadColourIds::PadColourOrange );
            pads->removeBreathing( CurrentPlaying );
            setPadLED( CurrentPlaying, contracts::DeviceControlContracts::PadButtonState::StateOn, contracts::DeviceControlContracts::PadColourIds::padColourlightGreen1 );
            pads->setBorderColor( CurrentPlaying, ui::Theme::selectionColour( 2 ), 2, juce::dontSendNotification );
            if( NextSequence != CurrentPlaying && NextSequence >= 0 )
            {
                pads->removeBreathing( selectedId );
                pads->setBreathing( NextSequence );
                setPadLED( NextSequence, contracts::DeviceControlContracts::PadButtonState::StateOn, contracts::DeviceControlContracts::PadColourIds::padColourlightGreen1 );
            }
            selectedId = CurrentPlaying;
        }
    }

    void KickSongContent::setPadLED( const int id, int state, int color )
    {
        int idx = id - pads->getBankOffset();
        if( juce::isPositiveAndBelow( idx, 16 ) )
        {
            contracts::DclSignals::sendCommandToDcl( contracts::DclContractsHelper::
                                                         setPadDataMessage( idx, state, color ) );
        }
    }

    void KickSongContent::seqSongState( bool state )
    {
        isSeqSong = state;
    }

    void KickSongContent::setBpmValue( float bpm )
    {
        bpmValue = bpm;
        if( isSeqSong )
        {
            refreshSequenceList();
            updateSong( songName );
        }
    }

    ///
    /// Kick Song Mode...
    ///

    KickSongMode::KickSongMode()
        : kick_ui::IScreen( kick_ui::Screen::MenuSong )
    {
        setLookAndFeel( ui::KickLooknFeel::getLnf() );

        mt::utils::addChildComponents( this, header, content, songListBox, bpmNumber, exportt, seqSongBpm );

        // header.showSettingsButton( true );
        songListBox.setJustificationType( juce::Justification::centredLeft );
        songListBox.setBackGroundColours( ui::Theme::backgroundColour( 13 ), ui::Theme::backgroundColour( 2 ) );
        bpmNumber.setColours( ui::Theme::textColour( 2 ), ui::Theme::textColour( 1 ) );
        bpmNumber.setValueBackgroundColour( ui::Theme::backgroundColour( 13 ) );
        bpmNumber.setTitleProportion( .35f );
        bpmNumber.setTitleFontHeight( 16 );
        songListBox.setTitleFontHeight( 16 );

        seqSongBpm.setValueColour( ui::Theme::textColour( 1 ) );

        header.onBackButtonPressed = [this]( bool ) { closeScreen(); };

        mt::signals::onSongStopped.connect( [&]() {
            juce::MessageManager::callAsync( [this] {
                header.setPlayState( false, juce::sendNotification );
            } );
        } );

        songListBox.onSelectionChanged = [&]( int Index, const juce::String& SongName ) {
            if( SongName.equalsIgnoreCase( "create new song" ) )
            {
                auto& _cns          = ui::KickVirtualKeyBoard::getWithText( "Enter Name" );
                _cns.onEnterPressed = [&_cns, this]( int v ) {
                    mt::utils::exitDialog( _cns.getCurrentlyFocusedComponent(), v );
                };
                _cns.show(
                    "CREATE NEW SONG", ui::KickVirtualKeyBoard::Type::KEYS_LOWERCASE_ALPHABETIC,
                    [&_cns, this]( int R ) {
                        if( R == UI::APPLY )
                        {
                            juce::String newSongName = _cns.getInputText();

                            if( ae.isSongExists( newSongName ) )
                                warningPopup();
                            else
                            {
                                mt::getAudioInterface().createSong( newSongName );
                                triggerAsyncUpdate();
                            }
                        }
                        else
                            songListBox.setSelected( 0 );
                    },
                    ui::layout::keyBoardPopup );
            }
            else
                content.updateSong( SongName );
        };

        exportt.onClick = [&]() {
            auto savePopUp = new SaveFile( songListBox.getSelected() );
            mt::utils::showOptions( "EXPORT", savePopUp, [this, savePopUp]( int R ) {
                if( savePopUp )
                {
                    if( R == UI::APPLY )
                    {
                    }
                }
                delete savePopUp;
            } );
            exportt.setToggleState( false, juce::dontSendNotification );
        };
        seqSongBpm.onClick = [&]() {
            content.seqSongState( seqSongBpm.getToggleState() );
            content.updateSong( songListBox.getSelected() );
            content.refreshSequenceList();
        };

        bpmNumber.onValueChanged = [this]( juce::int64 Value ) {
            bpmValue = bpmNumber.getValue();
            content.setBpmValue( bpmValue );
        };

        ui::themeChanged.connect( [&]() {
            exportt.setImages( "Song_Export", "Song_Export_Active" );
            header.setIcon( "Icon_SongEdit" );
        } );
    }

    void KickSongMode::handleAsyncUpdate()
    {
        auto list     = mt::getAudioInterface().getSongsList();
        auto selected = songListBox.getSelectedIndex();

        list.push_back( "CREATE NEW SONG" );
        songListBox.setListOptions( list, selected );
    }

    void KickSongMode::handleCommand( CPktMgr::HWMsg_t& f_HwMsg )
    {
        if( ae.isProjectLoaded() && contracts::DclContractsHelper::isButtonMessage( f_HwMsg ) && ( contracts::DclContractsHelper::getButtonState( f_HwMsg ) == contracts::DeviceControlContracts::BTN_PRESSED ) )
        {
            auto buttonCode = contracts::DclContractsHelper::getButtonCode( f_HwMsg );
            setPlayState( buttonCode );
        }
        else if( ae.isProjectLoaded() && contracts::DclContractsHelper::isKnobMessage( f_HwMsg ) )
        {
            auto knob  = contracts::DclContractsHelper::getKnobCode( f_HwMsg );
            auto value = contracts::DclContractsHelper::getKnobValue( f_HwMsg );
            if( knob != contracts::DeviceControlContracts::EN_KNOB_CENTER )
                knobCommand( knob, value );
        }
    }

    void KickSongMode::knobCommand( int knob, int value )
    {
        auto seqData   = ae.getSequenceData( ae.getSelectedSequence() );
        auto trackData = ae.getTrackData( ae.getSelectedSequence(), ae.getSelectedTrack() );
        switch( knob )
        {
            //sequence
            case contracts::DeviceControlContracts::KnobSequence::EN_KNOB_1:
                ae.setSequenceBPM( ae.getSelectedSequence(), (int)seqData->tempo + value );
                break;
            case contracts::DeviceControlContracts::KnobSequence::EN_KNOB_2:
                ae.setSequenceLength( ae.getSelectedSequence(), seqData->length.get() + value );
                break;
            case contracts::DeviceControlContracts::KnobSequence::EN_KNOB_3:
                ae.setSequenceLoopPoints( ae.getSelectedSequence(), seqData->loopStart.get() + value, seqData->loopEnd.get() );
                break;
            case contracts::DeviceControlContracts::KnobSequence::EN_KNOB_4:
                ae.setSequenceLoopPoints( ae.getSelectedSequence(), seqData->loopStart.get(), seqData->loopEnd.get() + value );
                break;
            case contracts::DeviceControlContracts::KnobSequence::EN_KNOB_5:
                ae.setSelectedSequence( ae.getSelectedSequence() + value );
                break;
            //track
            case contracts::DeviceControlContracts::KnobSequence::EN_KNOB_6:
                ae.setTrackMidiChannel( ae.getSelectedSequence(), ae.getSelectedTrack(), trackData ? trackData->midiChannel.get() + value : value );
                break;
            case contracts::DeviceControlContracts::KnobSequence::EN_KNOB_7:
                ae.setTrackLength( ae.getSelectedSequence(), ae.getSelectedTrack(), trackData ? trackData->length.get() + value : value );
                break;
            case contracts::DeviceControlContracts::KnobSequence::EN_KNOB_8:
                ae.setTrackLevel( ae.getSelectedSequence(), ae.getSelectedTrack(), trackData ? trackData->level.get() + value : value );
                break;
            case contracts::DeviceControlContracts::KnobSequence::EN_KNOB_9:
                ae.setTrackTranspose( ae.getSelectedSequence(), ae.getSelectedTrack(), trackData ? trackData->transpose.get() + value : value );
                break;
            case contracts::DeviceControlContracts::KnobSequence::EN_KNOB_10:
                ae.setSelectedTrack( ae.getSelectedTrack() + value );
                break;
                //metronome
            case contracts::DeviceControlContracts::KnobSequence::EN_KNOB_11:
            case contracts::DeviceControlContracts::KnobSequence::EN_KNOB_12:
            case contracts::DeviceControlContracts::KnobSequence::EN_KNOB_13:
                metronome.linkRotated( knob % 10, value );
                break;
        }
    }

    void KickSongMode::resized()
    {
        auto b = getLocalBounds();

        header.setBounds( b.removeFromTop( ui::layout::header.getHeight() ) );

        songListBox.setBoundsRelative( 0.225f, .029f, 0.1956f, .082f );
        bpmNumber.setBoundsRelative( 0.43f, .029f, 0.0724f, 0.082f );
        seqSongBpm.setBoundsRelative( .513f, .029f, .049f, .082f );

        exportt.setBoundsRelative( .738f, .029f, .049f, .082f );

        content.setBounds( b.reduced( 10 ) );
    }

    void KickSongMode::setPlayState( int Type )
    {
        switch( Type )
        {
            case contracts::DeviceControlContracts::EN_PLAY:
            case contracts::DeviceControlContracts::EN_PLAYSTART:
                content.playOrResume();
                header.setPlayState( true, juce::dontSendNotification );
                break;
            case contracts::DeviceControlContracts::EN_STOP:
                content.stopOrPause();
                header.setPlayState( false, juce::dontSendNotification );
                break;
        }
    }

    void KickSongMode::warningPopup()
    {
        auto p = new ui::FilePopup( "A song with the same name already exists", false, false );
        mt::utils::showDialog(
            "WARNING!", p, [&, p]( int R ) {
                delete p;
            },
            { ui::layout::warningPopup }, ui::Theme::backgroundColour( 1 ), ui::Theme::titleColour( 1 ), ui::Theme::backgroundColour( 2 ) );
        songListBox.setSelected( 0 );
    }

    void KickSongMode::visibilityChanged()
    {
        bool visible = isVisible();
        songListBox.setFocus( visible );
        for( auto it : getChildren() )
            it->setVisible( visible );
        header.setPlayState( false );
        if( visible )
        {
            contracts::DclSignals::sendCommandToDcl( contracts::DclContractsHelper::
                                                         setPadColorModeMessage( mt::PadColorMode::DISABLE_PAD_COLOUR_MODE ) );
            triggerAsyncUpdate();
        }
        else
        {
            auto MTIProg = ae.getProgramData( ae.getSelectedProgram() );
            contracts::DclSignals::sendCommandToDcl( contracts::DclContractsHelper::
                                                         setPadColorModeMessage( MTIProg ? MTIProg->colourMode.get() : mt::OFF ) );
        }
    }

    ///
    /// KickSongTableDisplay
    ///

    KickSongTableDisplay::KickSongTableDisplay()
    {
        mt::utils::setValueFontHeight( 16, seqName, repeat, bpm, bars );
        mt::utils::setValueJustification( juce::Justification::centred, seqName, repeat, bpm, bars );
        mt::utils::setValueColour( ui::Theme::textColour( 1 ), seqName, repeat, bpm, bars );
        mt::utils::setValueBackgroundColour( ui::Theme::backgroundColour( 9 ), seqName, repeat, bpm, bars );
        mt::utils::addAndMakeVisible( this, seqName, repeat, bpm, bars );
        mt::utils::setEnabled( false, bpm, bars );

        repeat.onValueChanged = [&]( const int value ) {
            step.repeatCount = value;
            if( onRepeatChanged )
                onRepeatChanged( value );
        };
    }

    void KickSongTableDisplay::refreshStep( const mt::MTSongStepInfo& Step )
    {
        step = Step;
        seqName.setText( step.dispName, juce::dontSendNotification );
        repeat.setValue( step.repeatCount, juce::sendNotification );
        bpm.setValue( step.bpm, juce::dontSendNotification );
        bars.setValue( step.bars, juce::dontSendNotification );
    }

    void KickSongTableDisplay::resized()
    {
        seqName.setBoundsRelative( .018f, .09f, .18f, .82f );
        repeat.setBoundsRelative( .272f, .09f, .18f, .82f );
        bpm.setBoundsRelative( .523f, .09f, .18f, .82f );
        bars.setBoundsRelative( .748f, .09f, .18f, .82f );
    }
}
