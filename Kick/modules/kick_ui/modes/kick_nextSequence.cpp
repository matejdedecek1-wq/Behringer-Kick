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

namespace kick_ui
{
    KickNextSequenceDisplay::KickNextSequenceDisplay( SequenceData data )
    {
        mt::utils::addAndMakeVisible( this, srNo, seqName, length, bpm );
        mt::utils::setValueBackgroundColour( ui::Theme::backgroundColour( 9 ), srNo, seqName, length, bpm );
        mt::utils::setValueJustification( juce::Justification::centred, srNo, seqName, length, bpm );
        mt::utils::setValueColour( ui::Theme::textColour( 1 ), srNo, seqName, length, bpm );
        mt::utils::setValueFontHeight( 17, srNo, seqName, length, bpm );

        srNo.setEditable( false );
        length.setEnabled( false );
        bpm.setEnabled( false );

        seqData = data;
        updateData( seqData );
    }

    void KickNextSequenceDisplay::updateData( SequenceData Data )
    {
        seqData = Data;
        srNo.setText( seqData.srNo, juce::dontSendNotification );
        seqName.setText( seqData.name, juce::dontSendNotification );
        bpm.setValue( seqData.bpm, juce::dontSendNotification );
        length.setValue( seqData.length, juce::dontSendNotification );
    }

    void KickNextSequenceDisplay::resized()
    {
        srNo.setBoundsRelative( .018f, .077f, .12f, .846f );
        seqName.setBoundsRelative( .15f, .077f, .42f, .846f );
        length.setBoundsRelative( .581f, .077f, .2f, .846f );
        bpm.setBoundsRelative( .79f, .077f, .2f, .846f );
    }

    KickNextSequenceContent::KickNextSequenceContent()
        : ae( mt::getAudioInterface() )
        , pads( std::make_unique<ui::KickPadButtonGroupComponent>( ui::KickPadButton::PadButtonType::with1Row ) )
        , playingSequence( ae.getSelectedSequence() )
    {
        ae.addListener( this );

        tableView.reset( new ui::KickTableView{ "Sequences", provider } );

        mt::utils::addChildComponents( this, tableView.get(), pads.get(), nextBar, sudden, hold, clear, srNo, sequence, length, bpm );

        tableView->setRowSpacing( 1 );
        tableView->setRowHeight( 52 );
        tableView->setMargin( 0, 0, 2, 0 );
        tableView->setRowBackgroundColour( ui::Theme::backgroundColour( 3 ) );
        tableView->setSelectedRowBackgroundColour( ui::Theme::backgroundColour( 0 ) );
        tableView->setAutoScrollToSelection( true );

        clear.setEnabled( false );
        nextBar.setEnabled( false );
        pads->setActive( -1, false );
        pads->setHighlight( -1 );
        pads->enableBank( true );
        pads->setPadButtonSpacer( 12 );
        pads->setFocusOutline( false );

        mt::utils::setValueBackgroundColour( ui::Theme::backgroundColour( 9 ), srNo, sequence, length, bpm );
        mt::utils::setValueJustification( juce::Justification::centred, srNo, sequence, length, bpm );
        mt::utils::setValueColour( ui::Theme::textColour( 1 ), nextBar, sudden, clear, hold );
        mt::utils::setValueFontHeight( 18, srNo, sequence, length, bpm, nextBar, sudden, clear, hold );

        provider.numRows = [&]() {
            return ae.getNumSequences();
        };

        provider.cellComponent = [&]( int Row, juce::Component* Existing ) -> juce::Component* {
            auto comp = dynamic_cast<KickNextSequenceDisplay*>( Existing );
            auto data = ae.getSequenceData( Row );

            seqData.srNo   = juce::String( Row + 1 );
            seqData.name   = data->name.get();
            seqData.length = data->length.get();
            seqData.bpm    = data->tempo;
            seqData.index  = Row;

            if( juce::isPositiveAndBelow( Row, ae.getNumSequences() ) )
            {
                if( comp == nullptr )
                    comp = new KickNextSequenceDisplay( seqData );
                comp->updateData( seqData );
                return comp;
            }
            return nullptr;
        };

        provider.onRowSelected = [&]( int Index ) {
            selectSequence( Index );
            ae.setSelectedSequence( Index );
            auto startID = pads->getBankOffset();
            auto endID   = startID + _NUM_PADS_IN_COMPONENT;
            if( Index >= startID && Index < endID )
            {
                //current bank : No need to change
            }
            else
            {
                auto bankIndex = Index / _NUM_PADS_IN_COMPONENT;
                ae.setBank( pads->getBank( bankIndex ) );
            }
        };

        pads->OnButtonClicked = [&]( int Id ) {
            if( isPlaying )
            {
                auto startId = ae.getBankOffset();
                auto endId   = startId + 16;
                if( Id >= startId && Id < endId )
                {
                    selectSequence( Id );
                    if( !hold.getToggleState() )
                        ae.setNextSequence( nextSequence );
                }
            }
            else
            {
                tableView->selectRow( Id );
            }
        };

        hold.onClick = [&]() {
            if( hold.getToggleState() && isPlaying )
            {
                if( nextSequence >= 0 )
                {
                    pads->removeBreathing( nextSequence );
                    setPadBlinking( nextSequence, contracts::DeviceControlContracts::StateOffBlink, contracts::DeviceControlContracts::PadColourOrange );
                    nextSequence = -1;
                }
                ae.setNextSequence( -1 );
            }
            else
            {
                if( nextSequence >= 0 )
                    ae.setNextSequence( nextSequence );
            }
        };

        sudden.onClick = [&]() {
            if( !hold.getToggleState() )
            {
                if( nextSequence >= 0 )
                {
                    ae.playSequence( nextSequence );
                    changePlayingPad( playingSequence, nextSequence );
                }
            }
            sudden.setToggleState( false, juce::dontSendNotification );
        };

        nextBar.onClick = [&]() {
            if( hold.getToggleState() )
            {
                if( nextSequence >= 0 )
                {
                    pads->removeBreathing( nextSequence );
                    setPadBlinking( nextSequence, contracts::DeviceControlContracts::StateOffBlink, contracts::DeviceControlContracts::PadColourOrange );
                    nextSequence = -1;
                    ae.setNextSequence( -1 );
                }
            }
            if( isPlaying )
            {
                ae.setNextBarFlag( true );
                nextBar.setToggleState( false, juce::dontSendNotification );
            }
        };

        clear.onClick = [&]() {
            if( nextSequence >= 0 )
            {
                ae.setNextSequence( -1 );
                clearNextSequence();
            }
            clear.setToggleState( false, juce::dontSendNotification );
        };

        mt::signals::onNextSequencePlay.connect( [&]( int Sequence ) {
            juce::MessageManager::callAsync( [this, Sequence] {
                changePlayingPad( playingSequence, Sequence );
            } );
        } );

        nextSequence.onValueChanged = [&]( int value ) {
            if( nextSequence == -1 )
                clear.setEnabled( false );
            else if( !clear.isEnabled() )
                clear.setEnabled( true );
        };
        ui::themeChanged.connect( [&]() {
            nextBar.setToggleButtonText( "Next Bar", "Next Bar" );
            sudden.setToggleButtonText( "Sudden", "Sudden" );
            clear.setToggleButtonText( "Clear", "Clear" );
            hold.setToggleButtonText( "Hold", "Hold" );
        } );

        mt::signals::bankChanged.connect( [&]( int Index, juce::String Bank ) {
            if( kick_ui ::KickHome::Instance().getCurrentScreen() == kick_ui::Screen::MenuNextSequence )
            {
                ui::fileUtils::lastPressedPad = -1;
                pads->setBank( Index );
                if( pads->isBreathing( playingSequence ) ) //while not Playing
                    setPadBlinking( playingSequence, contracts::DeviceControlContracts::PadButtonState::StateOffBlink, contracts::DeviceControlContracts::PadColourTrans );
                else
                    setPadBlinking( nextSequence, contracts::DeviceControlContracts::PadButtonState::StateOffBlink, contracts::DeviceControlContracts::PadColourTrans );
                updateData();
                restoreData();
            }
        } );
    }

    KickNextSequenceContent::~KickNextSequenceContent()
    {
        ae.removeListener( this );
        tableView.reset();
        pads.reset();
    }

    void KickNextSequenceContent::selectSequence( int Index ) //engine side index - 128
    {
        if( isPlaying )
        {
            // turn off old next seq
            if( nextSequence >= 0 && nextSequence != Index ) //remove the old blinking next sequence,no change if we press the same next seq pad
            {
                pads->removeBreathing( nextSequence );
                pads->setBorderColor( nextSequence, ui::Theme::outlineColour( 1 ), 1, juce::dontSendNotification );
                setPadBlinking( nextSequence, contracts::DeviceControlContracts::PadButtonState::StateOffBlink, contracts::DeviceControlContracts::DeviceControlContracts::PadColourIds::PadColourOrange );
                nextSequence = -1;
            }
            // turn on new next seq
            if( !pads->isBreathing( Index ) && playingSequence != Index ) //should not allow to blink the already blinking pad and no need to blink when we press the playing seq
            {
                pads->setBreathing( Index ); //no matter whether nextseq is there or not , this will work
                setPadBlinking( Index, contracts::DeviceControlContracts::PadButtonState::StateOnBlink, contracts::DeviceControlContracts::DeviceControlContracts::PadColourIds::padColourlightGreen1, contracts::DeviceControlContracts::DeviceControlContracts::PadColourIds::PadColourOrange );
                nextSequence = Index;
            }
        }

        else if( !isPlaying )
        {
            if( playingSequence == Index )
            {
                //Don't do anything
            }

            if( clear.isEnabled() )
            {
                pads->setBorderColor( nextSequence, ui::Theme::outlineColour( 1 ), 1, juce::dontSendNotification ); //Orange
                pads->removeBreathing( nextSequence );
                setPadBlinking( nextSequence, contracts::DeviceControlContracts::StateOffBlink, contracts::DeviceControlContracts::DeviceControlContracts::PadColourIds::PadColourOrange );

                pads->setBreathing( Index );
                setPadBlinking( Index, contracts::DeviceControlContracts::DeviceControlContracts::StateOnBlink, contracts::DeviceControlContracts::DeviceControlContracts::PadColourIds::padColourlightGreen1, contracts::DeviceControlContracts::DeviceControlContracts::PadColourIds::PadColourOrange );

                playingSequence = Index;
                nextSequence    = -1;
            }

            if( playingSequence != Index && !clear.isEnabled() )
            {
                pads->setBorderColor( playingSequence, ui::Theme::outlineColour( 1 ), 1, juce::dontSendNotification ); //Orange
                pads->removeBreathing( playingSequence );
                setPadBlinking( playingSequence, contracts::DeviceControlContracts::StateOffBlink, contracts::DeviceControlContracts::DeviceControlContracts::PadColourIds::PadColourOrange );

                pads->setBreathing( Index );
                setPadBlinking( Index, contracts::DeviceControlContracts::DeviceControlContracts::StateOnBlink, contracts::DeviceControlContracts::DeviceControlContracts::PadColourIds::padColourlightGreen1, contracts::DeviceControlContracts::DeviceControlContracts::PadColourIds::PadColourOrange );

                playingSequence = Index;
            }
        }
    }

    void KickNextSequenceContent::changePlayingPad( int OldPlayingPad, int NewPlayingPad )
    {
        pads->setBorderColor( OldPlayingPad, ui::Theme::outlineColour( 1 ), 1, juce::dontSendNotification ); //Orange
        setPadLED( OldPlayingPad, contracts::DeviceControlContracts::PadButtonState::StateOn, contracts::DeviceControlContracts::DeviceControlContracts::PadColourIds::PadColourOrange );

        pads->removeBreathing( NewPlayingPad );
        pads->setBorderColor( NewPlayingPad, ui::Theme::selectionColour( 2 ), 1, juce::dontSendNotification ); //Green
        setPadBlinking( NewPlayingPad, contracts::DeviceControlContracts::StateOffBlink, contracts::DeviceControlContracts::DeviceControlContracts::PadColourIds::padColourlightGreen1 );

        playingSequence = NewPlayingPad; //updating the playing index
        nextSequence    = -1;

        tableView->selectRow( NewPlayingPad, juce::dontSendNotification );
        ae.setSelectedSequence( NewPlayingPad ); //update the engine sequence
    }

    void KickNextSequenceContent::clearNextSequence()
    {
        if( isPlaying )
        {
            pads->removeBreathing( nextSequence ); //orange
            setPadBlinking( nextSequence, contracts::DeviceControlContracts::StateOffBlink, contracts::DeviceControlContracts::DeviceControlContracts::PadColourIds::PadColourOrange );
            nextSequence = -1;
        }
        else
        {
            if( playingSequence >= 0 )
            {
                pads->removeBreathing( nextSequence ); //orange
                setPadBlinking( nextSequence, contracts::DeviceControlContracts::StateOffBlink, contracts::DeviceControlContracts::DeviceControlContracts::PadColourIds::PadColourOrange );

                pads->setBreathing( playingSequence );
                setPadBlinking( playingSequence, contracts::DeviceControlContracts::DeviceControlContracts::StateOnBlink, contracts::DeviceControlContracts::DeviceControlContracts::PadColourIds::padColourlightGreen1, contracts::DeviceControlContracts::PadColourIds::PadColourOrange );

                nextSequence = -1;
            }
            else
                std::abort(); // Forces a crash here
        }
    }

    void KickNextSequenceContent::startPlaying()
    {
        pads->removeBreathing( playingSequence );
        pads->setBorderColor( playingSequence, ui::Theme::selectionColour( 2 ), 1, juce::dontSendNotification ); //Green
        setPadBlinking( playingSequence, contracts::DeviceControlContracts::StateOffBlink, contracts::DeviceControlContracts::DeviceControlContracts::PadColourIds::padColourlightGreen1 );
        setPadLED( playingSequence, contracts::DeviceControlContracts::DeviceControlContracts::StateOn, contracts::DeviceControlContracts::DeviceControlContracts::PadColourIds::padColourlightGreen1 );

        if( nextSequence >= 0 )
        {
            pads->setBreathing( nextSequence );
            setPadBlinking( nextSequence, contracts::DeviceControlContracts::StateOnBlink, contracts::DeviceControlContracts::PadColourIds::padColourlightGreen1, contracts::DeviceControlContracts::PadColourIds::PadColourOrange );
        }
        ae.playSequence( playingSequence );
    }

    void KickNextSequenceContent::stopPlaying()
    {
        if( nextSequence >= 0 )
        {
            pads->setBorderColor( playingSequence, ui::Theme::outlineColour( 1 ), 1, juce::dontSendNotification ); //Orange
            setPadLED( playingSequence, contracts::DeviceControlContracts::StateOn, contracts::DeviceControlContracts::PadColourIds::PadColourOrange );
        }
        else
        {
            pads->setBorderColor( playingSequence, ui::Theme::outlineColour( 1 ), 1, juce::dontSendNotification ); //Orange - for toggeling
            pads->setBreathing( playingSequence );
            setPadBlinking( playingSequence, contracts::DeviceControlContracts::StateOnBlink, contracts::DeviceControlContracts::PadColourIds::padColourlightGreen1, contracts::DeviceControlContracts::PadColourIds::PadColourOrange );
        }
        ae.stopSequence();
    }

    void KickNextSequenceContent::restoreData()
    {
        if( playingSequence == -1 )
            playingSequence = ae.getSelectedSequence();

        isPlaying = ae.isSequencePlaying();
        if( isPlaying )
        {
            pads->removeBreathing( playingSequence );
            pads->setBorderColor( playingSequence, ui::Theme::selectionColour( 2 ), 1, juce::dontSendNotification ); //Green
            setPadLED( playingSequence, contracts::DeviceControlContracts::StateOn, contracts::DeviceControlContracts::PadColourIds::padColourlightGreen1 );
        }
        else
        {
            pads->setBreathing( playingSequence );
            setPadBlinking( playingSequence, contracts::DeviceControlContracts::StateOnBlink, contracts::DeviceControlContracts::PadColourIds::padColourlightGreen1, contracts::DeviceControlContracts::PadColourIds::PadColourOrange );
        }

        if( isPlaying )
        {
            if( nextSequence >= 0 )
            {
                pads->setBreathing( nextSequence );
                setPadBlinking( nextSequence, contracts::DeviceControlContracts::StateOnBlink, contracts::DeviceControlContracts::PadColourIds::padColourlightGreen1, contracts::DeviceControlContracts::PadColourIds::PadColourOrange );
            }
        }
        else if( nextSequence >= 0 )
        {
            std::abort();
        }

        tableView->selectRow( playingSequence, juce::dontSendNotification );
        ae.setSelectedSequence( playingSequence );
    }

    void KickNextSequenceContent::onPlay()
    {
        isPlaying = true;
        startPlaying();
        nextBar.setEnabled( true );
        tableView->setInterceptsMouseClicks( false, false );
    }

    void KickNextSequenceContent::onStop()
    {
        isPlaying = false;
        stopPlaying();
        nextBar.setEnabled( false );
        tableView->setInterceptsMouseClicks( true, true );
    }

    void KickNextSequenceContent::enableRowSelection( bool State )
    {
        tableView->enableRowSelection( State );
    }

    void KickNextSequenceContent::onSequenceSelectionChange( const mt::MTSelected& Selected )
    {
        if( kick_ui::KickHome::Instance().getCurrentScreen() != kick_ui::Screen::MenuNextSequence )
            juce::MessageManager::callAsync( [this, Selected] { selectSequence( Selected.sequence ); } );
    }

    void KickNextSequenceContent::updateData()
    {
        pads->clearAllPadData();
        contracts::DclSignals::sendCommandToDcl( contracts::DclContractsHelper::clearAllPadsMessage() );
        tableView->refresh();

        auto startId = ae.getBankOffset();
        auto endId   = startId + 16;

        for( auto id = startId, row = 0; id < endId; ++id, ++row )
        {
            if( juce::isPositiveAndBelow( id, ae.getNumSequences() ) )
            {
                auto data = ae.getSequenceData( id );
                pads->setActive( id, true );
                pads->setPadId( id + 1, id % 16 );
                pads->setHeaderText( id, "S" + juce::String( id + 1 ).paddedLeft( '0', 2 ), juce::dontSendNotification );
                pads->setRow1Text( id, data->name.get(), juce::dontSendNotification );
                if( id != playingSequence && id != nextSequence )
                    setPadLED( id, contracts::DeviceControlContracts::PadButtonState::StateOn, contracts::DeviceControlContracts::PadColourIds::PadColourOrange );
            }
            else
            {
                pads->setActive( id, false );
                pads->setHeaderText( id, "S" + juce::String( id + 1 ).paddedLeft( '0', 2 ), juce::dontSendNotification );
                pads->setRow1Text( id, " ", juce::dontSendNotification );
            }
        }
    }

    void KickNextSequenceContent::setPadLED( const int Id, int State, int Color )
    {
        if( kick_ui::KickHome::Instance().getCurrentScreen() == kick_ui::Screen::MenuNextSequence )
        {
            //Make sure we are  setting LED for the current Bank
            int idx = Id - pads->getBankOffset();
            if( juce::isPositiveAndBelow( idx, 16 ) )
                contracts::DclSignals::sendCommandToDcl( contracts::DclContractsHelper::setPadDataMessage( idx, State, Color ) );
        }
    }

    void KickNextSequenceContent::setPadBlinking( const int Id, int State, int OnColorID, int OffColorID )
    {
        if( ae.isProjectLoaded() && kick_ui::KickHome::Instance().getCurrentScreen() == kick_ui::Screen::MenuNextSequence )
        {
            auto startId = ae.getBankOffset();
            auto endId   = startId + 16;
            if( Id >= startId && Id < endId )
            {
                int idx = Id - pads->getBankOffset();
                if( juce::isPositiveAndBelow( idx, 16 ) )
                    contracts::DclSignals::sendCommandToDcl( contracts::DclContractsHelper::setPadBlinkingMessage( idx, State, OnColorID, OffColorID ) );
            }
        }
    }

    void KickNextSequenceContent::paint( juce::Graphics& g )
    {
        mt::utils::fillRoundedRect( g, { 10, 10, 342, 396 }, ui::Theme::backgroundColour( 3 ) );
        mt::utils::fillRoundedRect( g, { 10, 10, 342, 54 }, ui::Theme::backgroundColour( 1 ), 5.0f, true, true, false, false );
        mt::utils::renderRoundedRect( g, { 10, 10, 342, 396 }, ui::Theme::backgroundColour( 0 ), 1.f, 6.f );
    }

    void KickNextSequenceContent::resized()
    {
        srNo.setBoundsRelative( 0.0156f, 0.033f, 0.04f, 0.096f );
        sequence.setBoundsRelative( 0.059f, 0.033f, 0.137f, 0.096f );
        length.setBoundsRelative( 0.2f, 0.033f, 0.0636f, 0.096f );
        bpm.setBoundsRelative( 0.272f, 0.033f, 0.0636f, 0.096f );

        tableView->setBoundsRelative( .0097f, .1391f, .3346f, .7434f );
        pads->setBoundsRelative( .3581f, .0304f, .6301f, .8434f );

        nextBar.setBoundsRelative( .5068f, .9083f, .1174f, .0742f );
        sudden.setBoundsRelative( .6282f, .9083f, .1174f, .0742f );
        hold.setBoundsRelative( .7495f, .9083f, .1174f, .0742f );
        clear.setBoundsRelative( .8708f, .9083f, .1174f, .0742f );
    }

    void KickNextSequenceContent::visibilityChanged()
    {
        bool visible = isVisible();
        for( auto it : getChildren() )
            it->setVisible( visible );

        if( visible )
        {
            contracts::DclSignals::sendCommandToDcl( contracts::DclContractsHelper::clearAllPadsMessage() );

            tableView->refresh();
            updateData();
            restoreData();
        }
        else
        {
            if( isPlaying )
            {
                if( nextSequence >= 0 )
                    setPadBlinking( nextSequence, contracts::DeviceControlContracts::StateOffBlink, contracts::DeviceControlContracts::PadColourIds::PadColourOrange );
            }
            else
            {
                pads->clearAllBreathing();

                if( playingSequence >= 0 )
                    setPadBlinking( playingSequence, contracts::DeviceControlContracts::StateOffBlink, contracts::DeviceControlContracts::PadColourIds::PadColourOrange );

                if( nextSequence >= 0 )
                {
                    ae.setSelectedSequence( nextSequence );

                    playingSequence = nextSequence;
                    nextSequence    = -1;
                }
            }
            contracts::DclSignals::sendCommandToDcl( contracts::DclContractsHelper::clearAllPadsMessage() );
            contracts::DclSignals::refreshPadLeds();
        }
    }

    void KickNextSequenceContent::handleCommand( CPktMgr::HWMsg_t& f_HwMsg )
    {
    }

    //
    //
    //
    //

    KickNextSequenceMode::KickNextSequenceMode()
        : kick_ui::IScreen( kick_ui::Screen::MenuNextSequence )
    {
        mt::utils::addChildComponents( this, header, content );

        header.onBackButtonPressed = [this]( bool ) { closeScreen(); };

        ui::themeChanged.connect( [&]() {
            header.setIcon( "Icon_NextSeq" );
        } );
    }

    void KickNextSequenceMode::resized()
    {
        auto b = getLocalBounds();
        header.setBounds( b.removeFromTop( ui::layout::header.getHeight() ) );
        content.setBounds( b );
    }

    void KickNextSequenceMode::visibilityChanged()
    {
        bool visible = isVisible();

        if( visible )
        {
            contracts::DclSignals::sendCommandToDcl( contracts::DclContractsHelper::
                                                         setPadColorModeMessage( mt::PadColorMode::DISABLE_PAD_COLOUR_MODE ) );
        }
        else
        {
            auto MTIProg = ae.getProgramData( ae.getSelectedProgram() );
            contracts::DclSignals::sendCommandToDcl( contracts::DclContractsHelper::setPadColorModeMessage( MTIProg ? MTIProg->colourMode.get() : mt::OFF ) ); //update the mode only for current screen
        }
        for( auto it : getChildren() )
            it->setVisible( visible );
    }

    void KickNextSequenceMode::handleCommand( CPktMgr::HWMsg_t& f_HwMsg )
    {
        if( contracts::DclContractsHelper::isKnobMessage( f_HwMsg ) )
        {
            auto knob  = contracts::DclContractsHelper::getKnobCode( f_HwMsg );
            auto value = contracts::DclContractsHelper::getKnobValue( f_HwMsg );
            if( knob != contracts::DeviceControlContracts::EN_KNOB_CENTER )
                knobCommand( knob, value );
        }
        else
        {
            if( ae.isProjectLoaded() && contracts::DclContractsHelper::isButtonMessage( f_HwMsg ) && ( contracts::DclContractsHelper::getButtonState( f_HwMsg ) == contracts::DeviceControlContracts::BTN_PRESSED ) )
            {
                auto buttonCode = contracts::DclContractsHelper::getButtonCode( f_HwMsg );
                setPlayState( buttonCode );
            }
            content.handleCommand( f_HwMsg );
        }
    }

    void KickNextSequenceMode::setPlayState( int PlayType )
    {
        switch( PlayType )
        {
            case contracts::DeviceControlContracts::EN_PLAY:
            case contracts::DeviceControlContracts::EN_PLAYSTART:
                if( !ae.isSequencePlaying() )
                    content.onPlay();
                break;
            case contracts::DeviceControlContracts::EN_STOP:
                content.onStop();
                break;
        }
    }

    void KickNextSequenceMode::knobCommand( int knob, int value )
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
}
