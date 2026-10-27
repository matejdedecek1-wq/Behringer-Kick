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
    AppTopbar::AppTopbar()
    {
        ae.addListener( this );
        setName( "Topbar" );

        project.setValueJustification( juce::Justification::centred );
        project.setValueBackgroundColour( ui::Theme::backgroundColour( 9 ) );
        project.setValueFontHeight( 24 );

        mt::utils::addAndMakeVisible( this, menuButton, mainModeButton, gridButton, stepSeqButton, axisButton, settingsButton, newFileButton, kickBrowseButton, kickPlayButton, metronomeButton, project );
        mt::utils::setRadioGroupID( (int)RadioGroupId::AppTopMenu, menuButton, mainModeButton, gridButton, stepSeqButton, axisButton, settingsButton, kickBrowseButton, newFileButton );

        mainModeButton.setToggleState( true, juce::dontSendNotification );

        setPaintingIsUnclipped( true );
        setInterceptsMouseClicks( false, true );

        signals::onScreenLoaded.connect( [&]( Screen Scrn ) {
            switch( Scrn )
            {
                case Screen::HomeOptions: menuButton.setToggleState( true, juce::dontSendNotification ); break;
                case Screen::HomeListView: mainModeButton.setToggleState( true, juce::dontSendNotification ); break;
                case Screen::HomeGridView: gridButton.setToggleState( true, juce::dontSendNotification ); break;
                case Screen::HomeSequencerView: stepSeqButton.setToggleState( true, juce::dontSendNotification ); break;
                case Screen::HomePreferencesView: settingsButton.setToggleState( true, juce::dontSendNotification ); break;
                case Screen::MenuFileBrowser: kickBrowseButton.setToggleState( true, juce::dontSendNotification ); break;
                case Screen::RecentProject: newFileButton.setToggleState( true, juce::dontSendNotification ); break;
                case Screen::HomeXYView: axisButton.setToggleState( true, juce::dontSendNotification ); break;
                default: break;
            }
        } );

        mt::signals::playStateChanged.connect( [&]( mt::TransportState State ) {
            bool state = ( State == mt::TransportState::Playing );
            juce::MessageManager::callAsync( [this, state] {
                kickPlayButton.setToggleState( state, juce::dontSendNotification );
            } );
        } );

        kickPlayButton.onClick = [&]() {
            if( kickPlayButton.getToggleState() )
                ae.setSequencePlay( ae.getSelectedSequence(), false, true );
            else
                ae.setSequencePause();
        };

        ui::themeChanged.connect( [&]() {
            menuButton.setImages( "Main_normal", "Main_active" );
            mainModeButton.setImages( "Home_normal", "Home_active" );
            gridButton.setImages( "Main Edit_normal", "Main Edit_active" );
            stepSeqButton.setImages( "Step Seq_normal", "Step Seq_active" );
            axisButton.setImages( "XY_normal", "XY_active" );
            metronomeButton.setImages( "Metronome_normal", "Metronome_active" );
            kickPlayButton.setImages( "Play Control_normal", "Play Control_active" );
            kickBrowseButton.setImages( "File_normal", "File_active" );
            newFileButton.setImages( "New File_normal", "New File_active" );
            settingsButton.setImages( "Settings_normal", "Settings_active" );
        } );

        menuButton.onClick     = []() { signals::sigScreenClose( Screen::HomeOptions ); };
        mainModeButton.onClick = [this]() {
            if( mainModeButton.getToggleState() )
                signals::sigScreenClose( Screen::HomeListView );
        };
        gridButton.onClick       = []() { signals::sigScreenClose( Screen::HomeGridView ); };
        stepSeqButton.onClick    = []() { signals::sigScreenClose( Screen::HomeSequencerView ); };
        settingsButton.onClick   = []() { signals::sigScreenClose( Screen::HomePreferencesView ); };
        kickBrowseButton.onClick = []() { signals::sigScreenClose( Screen::MenuFileBrowser ); };
        newFileButton.onClick    = []() { signals::sigScreenClose( Screen::RecentProject ); };
        axisButton.onClick       = []() { signals::sigScreenClose( Screen::HomeXYView ); };

        metronomeButton.onClick = []() { MetronomePopup::Instance().Show(); };
    }

    void AppTopbar::resized()
    {
        auto b = getLocalBounds().reduced( 8, 0 );

        // From left...
        menuButton.setBounds( b.removeFromLeft( buttonWidth ).reduced( buttonMargin, 0 ) );
        mainModeButton.setBounds( b.removeFromLeft( buttonWidth ).reduced( buttonMargin, 0 ) );
        gridButton.setBounds( b.removeFromLeft( buttonWidth ).reduced( buttonMargin, 0 ) );
        stepSeqButton.setBounds( b.removeFromLeft( buttonWidth ).reduced( buttonMargin, 0 ) );
        axisButton.setBounds( b.removeFromLeft( buttonWidth ).reduced( buttonMargin, 0 ) );
        //From right
        settingsButton.setBounds( b.removeFromRight( iconButtonWidth ).reduced( buttonMargin, 0 ) );
        newFileButton.setBounds( b.removeFromRight( iconButtonWidth ).reduced( buttonMargin, 0 ) );
        metronomeButton.setBounds( b.removeFromRight( iconButtonWidth ).reduced( buttonMargin, 0 ) );
        kickBrowseButton.setBounds( b.removeFromRight( iconButtonWidth ).reduced( buttonMargin, 0 ) );
        kickPlayButton.setBounds( b.removeFromRight( iconButtonWidth ).reduced( buttonMargin, 0 ) );

        project.setBounds( b.removeFromLeft( 300 ).reduced( buttonMargin, 0 ) );
    }

    void AppTopbar::onProjectLoaded( juce::File projectFile )
    {
        juce::MessageManager::callAsync( [&, projectFile]() {
            project.setText( projectFile.getFileNameWithoutExtension(), juce::dontSendNotification );
        } );
    }

    void AppTopbar::visibilityChanged()
    {
    }
}
