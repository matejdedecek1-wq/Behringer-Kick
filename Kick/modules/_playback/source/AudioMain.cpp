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

#include <functional>

namespace kick_ui
{
    static juce::String fileSelected;
    static juce::File   fSelected;

    AppWindow::AppWindow()
        : fc( "Open File", getAppDirs( MTSamples ), "*mp3;*wav" )
    {
        // Setup default look and feel.
        setLookAndFeel( ui::KickLooknFeel::getLnf() );

        // Setup input command handler.
        auto commandHandler = [this]( CPktMgr::HWMsg_t& f_HwMsg ) {
            if( ( f_HwMsg.enPktType == CPktMgr::enumPacketType::EN_PKT_TYPE_BUTTON ) && ( f_HwMsg.pktId == Controls::BtnCmd::EN_BTN_STATUS ) )
            {
                uint8_t ui8BtnNum = f_HwMsg.data[0];
                switch( ui8BtnNum )
                {
                    case Controls::BtnLedSequence::EN_PLAY:
                        Controls::InputEvents::setLEDON( Controls::BtnLedSequence::EN_PLAY );
                        ae.setSequencePlay( sel.sequence, false, true );
                        break;
                    case Controls::BtnLedSequence::EN_PLAYSTART:
                        Controls::InputEvents::setLEDON( Controls::BtnLedSequence::EN_PLAYSTART );
                        ae.setSequencePlay( sel.sequence, true, true );
                        break;
                    case Controls::BtnLedSequence::EN_STOP:
                        Controls::InputEvents::setLEDON( Controls::BtnLedSequence::EN_STOP );
                        ae.setSequenceStop();
                        break;
                }
            }

            // Send input to focused component...
            if( auto b = ui::getFocusComponent() )
                b->handleCommand( f_HwMsg );
        };

        Controls::InputEvents::initialize();
        Controls::InputEvents::hardwareInputTriggered.connect( commandHandler );

        isInitialized.store( true );

        setSize( ui::appWidth, ui::appHeight );

        setBufferedToImage( true );
        setPaintingIsUnclipped( true );
        setWantsKeyboardFocus( true );
        addKeyListener( &Controls::_keysDetector );

        mt::utils::addAndMakeVisible( this, loadFile, playFile, stopFile, loopFile, meter );

        auto OnFileSelected = [&]( const juce::FileChooser& fc ) {
            fileSelected = fc.getResult().getFullPathName();
            fSelected    = fc.getResult();
            DBG( "FileToOpen::" << fc.getResult().getFullPathName() );
        };

        loadFile.onClick = [&]() {
            fc.launchAsync( juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::useTreeView,
                            OnFileSelected );
        };

        playFile.onClick = [&]() {
            if( fSelected.existsAsFile() )
            {
                ae.createPreview( fSelected, 1.f );
                ae.loopPreview( loopFile.getToggleState() );
                startTimerHz( 30 );
            }
        };

        stopFile.onClick = [&]() {
            ae.stopPreview();
            stopTimer();
        };

        loopFile.onClick = [&]() {
            ae.loopPreview( loopFile.getToggleState() );
        };
    }

    AppWindow::~AppWindow()
    {
        removeKeyListener( &Controls::_keysDetector );
        Controls::InputEvents::hardwareInputTriggered.disconnect_all();
    }

    void AppWindow::onFileLoaded( const juce::FileChooser& fc )
    {
        fileSelected = fc.getResult().getFullPathName();
        DBG( "FileToOpen::" << fileSelected );
    }

    void AppWindow::timerCallback()
    {
        auto m = ae.readPreviewMeter();
        meter.setSignalStrength( { m.rms[0], m.rms[1], m.peak[0], m.peak[1] } );
    }

    void AppWindow::paint( juce::Graphics& g )
    {
        g.fillAll( juce::Colours::ghostwhite );
    }

    void AppWindow::resized()
    {
        auto b = getLocalBounds().reduced( 50 );
        auto w = b.getWidth() / 4;

        loadFile.setBounds( b.removeFromLeft( w ).reduced( 30, 0 ) );
        meter.setBounds( b.removeFromRight( w ).reduced( 30, 0 ) );

        auto b1 = b.removeFromTop( (int)( 0.75 * b.getHeight() ) );
        playFile.setBounds( b1.removeFromLeft( w ).reduced( 30, 0 ) );
        stopFile.setBounds( b1.removeFromLeft( w ).reduced( 30, 0 ) );
        b.removeFromTop( 30 );
        loopFile.setBounds( b.reduced( 30, 0 ) );
    }

    void AppWindow::visibilityChanged()
    {
    }
} // namespace
