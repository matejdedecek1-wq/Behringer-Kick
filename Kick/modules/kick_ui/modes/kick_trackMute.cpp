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
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////    KickTrackMute
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    KickTrackMute::KickTrackMute()
        : kick_ui::IScreen( kick_ui::Screen::MenuTrackMute ), tab( std::make_unique<KickTrackMuteTab>() )
    {
        mt::utils::addChildComponents( this, header, trackMute, trackGroup, *tab, hdrComp, sequenceLabel );
        tab->setVisible( false );
        trackMute.setToggleState( true, juce::sendNotification );
        trackMute.toFront( true );
        trackGroup.toFront( true );

        header.showSettingsButton( true );
        header.onBackButtonPressed = [this]( bool ) { closeScreen(); };
        hdrComp.sequence.setDisplayFormat( ui::KickListBox::withoutTitleBar );

        hdrComp.sequence.onSelectionChanged = [&]( int Sel, const juce::String& /*SelStr*/ ) {
            auto& ae  = mt::getAudioInterface();
            auto  sel = ae.getSelected();

            ae.setSelectedSequence( Sel );
            DBG( "On Selection Changed" );
            tab->refreshData();
            tab->refreshAllLEDs();
        };

        trackMute.onClick = [&]() {
            tab->showMiniMap();
            trackGroup.setToggleState( false, juce::dontSendNotification );
            if( !trackMute.getToggleState() )
                trackMute.setToggleState( true, juce::dontSendNotification );
        };

        trackGroup.onClick = [&]() {
            tab->showGroups();
            trackMute.setToggleState( false, juce::dontSendNotification );
            if( !trackGroup.getToggleState() )
                trackGroup.setToggleState( true, juce::dontSendNotification );
        };

        header.onSettingsClick = [&]() {
            auto p = new TrackMuteSettings();
            mt::utils::showOptions( "TRACK MUTE AUTOMATION SETTINGS", p, [p, this]( int R ) { delete p; } );
        };

        ui::themeChanged.connect( [&]() {
            trackMute.setToggleButtonText( "Track Mute", "Track Mute" );
            trackGroup.setToggleButtonText( "Track Group", "Track Group" );
            header.setIcon( "Icon_TrackMute" );
        } );
    }

    void KickTrackMute::paint( juce::Graphics& g )
    {
        mt::utils::fillRoundedRect( g, rect, ui::Theme::backgroundColour( 3 ), 5.0f );
        mt::utils::renderRoundedRect( g, rect, ui::Theme::outlineColour( 3 ), 1, 6.0f );

        sequenceLabel.setValueBackgroundColour( ui::Theme::backgroundColour( 9 ) );
        sequenceLabel.setValueColour( ui::Theme::textColour( 1 ) );
        trackMute.setValueColour( ui::Theme::textColour( 1 ) );
        trackMute.setValueFontHeight( 18 );
        trackGroup.setValueColour( ui::Theme::textColour( 1 ) );
        trackGroup.setValueFontHeight( 18 );
    }

    void KickTrackMute::resized()
    {
        auto b = getLocalBounds();
        header.setBounds( b.removeFromTop( ui::layout::header.getHeight() ) );

        trackMute.setBoundsRelative( 0.0097f, 0.9230f, 0.1653f, 0.063f );
        trackGroup.setBoundsRelative( 0.1790f, 0.9230f, 0.1653f, 0.063f );
        tab->setBoundsRelative( 0, 0.1270f, 1.00f, 0.8662f );
        hdrComp.setBoundsRelative( 0.225f, 0.060f, 0.1956f, 0.053f );
        sequenceLabel.setBoundsRelative( .2294f, 0.03f, .0648f, .0283f );
    }

    void KickTrackMute::handleCommand( CPktMgr::HWMsg_t& f_HwMsg )
    {
        if( contracts::DclContractsHelper::isButtonMessage( f_HwMsg ) && contracts::DclContractsHelper::getButtonState( f_HwMsg ) == contracts::DeviceControlContracts::BTN_PRESSED )
        {
            int   bankId;
            int   btnId = contracts::DclContractsHelper::getButtonCode( f_HwMsg );
            auto& ae    = mt::getAudioInterface();
            auto& aeSel = ae.getSelected();
            switch( btnId )
            {
                    // Bank buttons...
                case contracts::DeviceControlContracts::BtnLedSequence::EN_BANK_A: tab->changeBank( "A" ); break;
                case contracts::DeviceControlContracts::BtnLedSequence::EN_BANK_B: tab->changeBank( "B" ); break;
                case contracts::DeviceControlContracts::BtnLedSequence::EN_BANK_C: tab->changeBank( "C" ); break;
                case contracts::DeviceControlContracts::BtnLedSequence::EN_BANK_D: tab->changeBank( "D" ); break;
                case contracts::DeviceControlContracts::BtnLedSequence::EN_BANK_E: tab->changeBank( "E" ); break;
                case contracts::DeviceControlContracts::BtnLedSequence::EN_BANK_F: tab->changeBank( "F" ); break;
                case contracts::DeviceControlContracts::BtnLedSequence::EN_BANK_G: tab->changeBank( "G" ); break;
                case contracts::DeviceControlContracts::BtnLedSequence::EN_BANK_H:
                    tab->changeBank( "H" );
                    break;
                    //Play  Buttons..
                case contracts::DeviceControlContracts::EN_PLAY: ae.setSequencePlay( aeSel.sequence, false, true ); break;
                case contracts::DeviceControlContracts::EN_PLAYSTART: ae.setSequencePlay( aeSel.sequence, true, true ); break;
                case contracts::DeviceControlContracts::EN_STOP: ae.setSequencePause(); break;
                default: break; // DBG( "Unknown" ); break;
            }
        }
        else if( contracts::DclContractsHelper::isKnobMessage( f_HwMsg ) )
        {
            auto knob  = contracts::DclContractsHelper::getKnobCode( f_HwMsg );
            auto value = contracts::DclContractsHelper::getKnobValue( f_HwMsg );
            if( knob != contracts::DeviceControlContracts::EN_KNOB_CENTER )
                tab->knobCommand( knob, value );
        }
    }

    void KickTrackMute::visibilityChanged()
    {
        for( auto it : getChildren() )
            it->setVisible( isVisible() );
        tab->setVisible( isVisible() );

        if( isVisible() )
        {
            contracts::DclSignals::sendCommandToDcl( contracts::DclContractsHelper::
                                                         setPadColorModeMessage( mt::PadColorMode::DISABLE_PAD_COLOUR_MODE ) );
        }
        else
        {
            auto MTIProg = mt::getAudioInterface().getProgramData( mt::getAudioInterface().getSelectedProgram() );
            contracts::DclSignals::sendCommandToDcl( contracts::DclContractsHelper::
                                                         setPadColorModeMessage( MTIProg ? MTIProg->colourMode.get() : mt::OFF ) );

            contracts::DclSignals::refreshPadLeds();
        }
    }

    KickTrackMuteTab::KickTrackMuteTab()
        : pads( std::make_unique<ui::KickPadButtonGroupComponent>( ui::KickPadButton::PadButtonType::muteGroup ) )
    {
        mt::utils::addChildComponents( this, miniMap, groups, pads.get(), pads->getBankComponent(), clearSoloBtn, timeDivList, soloBtn, instantAndTD );
        miniMap.setColours( -1, ui::Theme::highlightColour( 3 ), ui::Theme::highlightColour( 2 ), ui::Theme::backgroundColour( 14 ) );
        mt::utils::setValueColour( ui::Theme::textColour( 1 ), clearSoloBtn, instantAndTD, soloBtn );
        mt::utils::setValueFontHeight( 18, clearSoloBtn, instantAndTD, soloBtn );

        timeDivList.setEnabled( false );
        timeDivList.setJustificationType( juce::Justification::centred );

        pads->setActive( -1, false );
        pads->setPadButtonSpacer( padButtonSpacer );
        pads->setEnableTrackPrefix( true );
        pads->setFocusOutline( false );

        pads->OnButtonClicked = [this]( int Id ) {
            if( Id >= tracksMax ) return;
            auto seq = ae.getSelectedSequence();
            if( isMiniMap )
            {
                if( soloBtn.getToggleState() )
                {
                    auto seq    = ae.getSelectedSequence();
                    auto isSolo = ae.getTrackSolo( seq, Id );
                    if( isSolo )
                    {
                        ae.setTrackSolo( seq, Id, false );
                    }
                    else
                    {
                        for( auto id = 0; id < ae.getNumTracks( seq ); id++ )
                        {
                            if( id != Id && ae.getTrackSolo( seq, id ) )
                            {
                                ae.setTrackSolo( seq, id, false );
                            }
                        }
                        ae.setTrackSolo( seq, Id, true );
                    }
                }
                else
                {
                    ae.setTrackMute( seq, Id, !pads->getMute( Id ) );
                }
            }
            else
            {
                // Add to group or remove from group...
                if( 0 == groups.getSelectedIndex() )
                    ae.seqRemoveTrackFromGroup( seq, Id );
                else
                    ae.seqAddTrackToGroup( seq, Id, groups.getSelectedIndex() );
            }
            refreshData();
            refreshAllLEDs();
        };

        pads->OnBankChanged = [&]( const juce::String& Bank ) {
            int idx = *Bank.getCharPointer() - 'A';
            miniMap.select( idx, true );
            contracts::DclSignals::sendCommandToDcl( contracts::DclContractsHelper::clearAllPadsMessage() );
            contracts::DclSignals::sendCommandToDcl( contracts::DclContractsHelper::setBankMessage( idx ) );
            refreshData();
            refreshAllLEDs();
        };

        soloBtn.onClick = [&]() {
            if( !soloBtn.getToggleState() )
            {
                auto seq = ae.getSelectedSequence();
                ae.getTrackListData( seq, list );
                tracksMax = (int)list.size();
                for( int index = 0; index < tracksMax; ++index )
                    ae.setTrackSolo( seq, index, false );
                refreshData();
                refreshAllLEDs();
            }
        };

        instantAndTD.onClick = [&]() {
            timeDivList.setEnabled( instantAndTD.getToggleState() );
        };

        clearSoloBtn.onClick = [&]() {
            auto value = ae.resetTrackSolos();
            if( !value )
            {
                refreshData();
                refreshAllLEDs();
            }
        };

        miniMap.onClicked = [&]( juce::String Bank, int ) //Pad
        {
            pads->setBank( Bank );
        };

        ui::themeChanged.connect( [&]() {
            soloBtn.setToggleButtonText( "Solo", "Solo" );
            groups.setContent( groupNumbers, groupActiveNumbers, groups.getSelectedIndex() );
        } );
    }

    void KickTrackMuteTab::changeBank( juce::String bank )
    {
        int idx = *bank.getCharPointer() - 'A';
        pads->setBank( bank );
    }

    void KickTrackMuteTab::knobCommand( int knob, int value )
    {
        pads->setKnobBorder( knob );
        assignKnobToPad( knob, value );
    }

    void KickTrackMuteTab::displayPadState( const int id, const juce::String& text, const bool state, int muteGroup )
    {
        pads->setActive( id, true );
        pads->setRow1Text( id, text, juce::dontSendNotification );
        pads->setMute( id, state, juce::dontSendNotification );
        auto seq = ae.getSelectedSequence();
        if( ae.getTrackSolo( seq, id ) )
        {
            pads->setBorderColor( id, ui::Theme::padButtonColour( 8 ), borderThickness, juce::dontSendNotification );
            miniMap.setColours( id, ui::Theme::highlightColour( 3 ), ui::Theme::highlightColour( 2 ), ui::Theme::backgroundColour( 14 ), ui::Theme::padButtonColour( 8 ), true ); //solo - blue
        }
        else
        {
            if( ae.getNumSoloTracks( seq ) )
            {
                pads->setBorderColor( id, state ? ui::Theme::highlightColour( 2 ) : ui::Theme::padButtonColour( 11 ), borderThickness, juce::dontSendNotification );
                miniMap.setColours( id, ui::Theme::padButtonColour( 11 ), ui::Theme::highlightColour( 2 ), ui::Theme::backgroundColour( 14 ) ); //solo normal - light pink
            }
            else
            {
                pads->setBorderColor( id, state ? ui::Theme::highlightColour( 2 ) : ui::Theme::highlightColour( 3 ), borderThickness, juce::dontSendNotification );
                miniMap.setColours( id, ui::Theme::highlightColour( 3 ), ui::Theme::highlightColour( 2 ), ui::Theme::backgroundColour( 14 ) );
            }
        }
        pads->setGroupText( id, 0 < muteGroup ? juce::String( muteGroup ) : juce::String( "" ), juce::dontSendNotification );

        miniMap.setActive( id, true );
        miniMap.setHighlight( id, state );
    }

    void KickTrackMuteTab::refreshData()
    {
        pads->clearAllPadData();
        pads->setActive( -1, false );

        miniMap.setActive( -1, false );
        miniMap.clearAll();

        ae.getTrackListData( ae.getSelectedSequence(), list );
        tracksMax  = (int)list.size();
        auto index = 0;
        for( const auto& it : list )
        {
            if( it )
            {
                displayPadState( index++, it->name.get(), it->mute.get(), it->muteGroup.get() );
            }
        }

        // Highlight selected bank on minimap
        int idx = *pads->getBank().getCharPointer() - 'A';
        miniMap.select( idx, true );
    }

    void KickTrackMuteTab::refreshAllLEDs()
    {
        std::vector<contracts::PadData> padData;
        int                             padIndex = 0, padInc = 0;
        auto                            seq = ae.getSelectedSequence();
        ae.getTrackListData( seq, list );

        for( const auto& it : list )
        {
            if( it )
            {
                padIndex = it->index - pads->getBankOffset();
                if( juce::isPositiveAndBelow( padIndex, 16 ) )
                {
                    if( ae.getTrackSolo( seq, it->index ) )
                    {
                        padData.push_back( { padIndex, contracts::DeviceControlContracts::PadButtonState::StateOn, contracts::DeviceControlContracts::PadColourIds::PadColourBlue1 } );
                    }
                    else
                    {
                        auto colourId = ae.getNumSoloTracks( seq ) ? contracts::DeviceControlContracts::PadColourIds::PadColourPink : contracts::DeviceControlContracts::PadColourIds::padColourlightGreen1;
                        padData.push_back( { padIndex, contracts::DeviceControlContracts::PadButtonState::StateOn, it->mute.get() ? contracts::DeviceControlContracts::PadColourIds::PadColourRed : colourId } );
                    }
                    padInc++;
                }
            }
        }
        //making remaining pads as trans
        for( padInc; padInc < NUM_PADS_IN_BANK; padInc++ )
        {
            padData.push_back( { padInc, contracts::DeviceControlContracts::PadButtonState::StateOff, contracts::DeviceControlContracts::PadColourIds::PadColourTrans } );
        }

        contracts::DclSignals::sendCommandToDcl( contracts::DclContractsHelper::setPadColourAndStateInfoMessage( padData ) );
    }

    void KickTrackMuteTab::visibilityChanged()
    {
        for( auto it : getChildren() )
            it->setVisible( isVisible() );
        if( isVisible() )
        {
            auto seq = ae.getSelectedSequence();
            soloBtn.setToggleState( false, false );

            for( auto index = 0; index < ae.getNumTracks( seq ); index++ )
            {
                if( ae.getTrackSolo( seq, index ) )
                {
                    //no need to update Engine as we are getting data from engine to reflect UI
                    soloBtn.setToggleState( true, false );
                    break;
                }
            }
        }
        resized();
    }

    void KickTrackMuteTab::showMiniMap()
    {
        isMiniMap = true;
        resized();
    }

    void KickTrackMuteTab::showGroups()
    {
        isMiniMap = false;
        groups.toFront( !isMiniMap );
        resized();
    }

    void KickTrackMuteTab::assignKnobToPad( int knobId, int value )
    {
        if( knobId < 1 || knobId > tracksMax ) return;

        int trackId               = knobId - 1;
        int currentGroupNumberStr = pads->getGroupNumber( trackId );
        int currentGroupNumber    = currentGroupNumberStr + value;
        currentGroupNumber        = juce::jlimit( 1, 16, currentGroupNumber );
        ae.seqAddTrackToGroup( ae.getSelectedSequence(), trackId, currentGroupNumber );
        pads->setGroupText( trackId, juce::String( currentGroupNumber ), juce::dontSendNotification );
    }

    void KickTrackMuteTab::resized()
    {
        groups.setVisible( !isMiniMap );
        miniMap.setVisible( isMiniMap );

        miniMap.setBoundsRelative( 0.0097f, 0.0393f, 0.3346f, 0.8644f );
        groups.setBoundsRelative( 0.0097f, 0.0389f, 0.3346f, 0.8644f );
        pads->setBoundsRelative( 0.3581f, 0.0389f, 0.6301f, 0.8690f );

        clearSoloBtn.setBoundsRelative( .504f, 0.9230f, 0.1174f, 0.0732f );
        instantAndTD.setBoundsRelative( 0.626f, 0.9230f, 0.1174f, 0.0732f );
        timeDivList.setBoundsRelative( 0.7480f, 0.9230f, 0.1174f, 0.0732f );
        soloBtn.setBoundsRelative( 0.869f, 0.9230f, 0.1174f, 0.0732f );
    }

    TrackMuteSettings::TrackMuteSettings()
    {
        mt::utils::addAndMakeVisible( this, label1, label2, label3, instantTrkMute, playTrkMuteSoloEvents, recordTrkMuteSoloEvents );
        mt::utils::setValueColour( ui::Theme::textColour( 1 ), label1, label2, label3 );
        mt::utils::setValueBackgroundColour( ui::Theme::backgroundColour( 9 ), label1, label2, label3 );
        mt::utils::setValueFontHeight( 20, label1, label2, label3, instantTrkMute, playTrkMuteSoloEvents, recordTrkMuteSoloEvents );
    }

    void TrackMuteSettings::resized()
    {
        label1.setBoundsRelative( .1083f, .3538f, .3846f, .054f );
        label2.setBoundsRelative( .1083f, .5135f, .3846f, .054f );
        label3.setBoundsRelative( .1083f, .6732f, .3846f, .054f );

        instantTrkMute.setBoundsRelative( .5538f, .3292f, .2461f, .1081f );
        playTrkMuteSoloEvents.setBoundsRelative( .5538f, .4889f, .2461f, .1081f );
        recordTrkMuteSoloEvents.setBoundsRelative( .5538f, .6486f, .2461f, .1081f );
    }
}
