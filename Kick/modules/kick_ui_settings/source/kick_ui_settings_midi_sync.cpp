/*
 * Copyright (c) 2024 MUSIC Tribe
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

#pragma once

#include "kick_ui_settings_midi_sync.h"

namespace kick_ui
{

    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////  KickChannelMixer
    ///////////////////////////////////////////////////////////////////////////////////////////////////////

    SettingsMidiSync::SettingsMidiSync()
        : SettingsInterface( 6, 5, true )
    {
        ui::KickGrid::setBackgroundColour( juce::Colours::transparentBlack );
        setRowTitles( { "", "MidiInput", "MidiOutput" } );

        addComponent( inputList, 1, 1 );
        addComponent( itrack, 1, 2 );
        addComponent( icontrol, 1, 3 );

        addComponent( outputList, 2, 1 );
        addComponent( otrack, 2, 2 );
        addComponent( ocontrol, 2, 3 );

        itrack.onClick = [&]() {
            settings.midiControl.ipTrackState = ( itrack.getToggleState() );
        };

        icontrol.onClick = [&]() {
            settings.midiControl.ipControlState = ( icontrol.getToggleState() );
        };

        otrack.onClick = [&]() {
            settings.midiControl.opTrackState = ( otrack.getToggleState() );
        };

        ocontrol.onClick = [&]() {
            settings.midiControl.opControlState = ( ocontrol.getToggleState() );
        };

        auto list = mt::getAudioInterface().getMidiDeviceNames( true );

        inputList.setListOptions( mt::getAudioInterface().getMidiDeviceNames( true ) );
        outputList.setListOptions( mt::getAudioInterface().getMidiDeviceNames( false ) );

        inputList.onSelectionChanged = [&]( int index, juce::String name ) {
            settings.midiControl.ipPortName = ( name );
        };

        outputList.onSelectionChanged = [&]( int index, juce::String name ) {
            settings.midiControl.opPortName = ( name );
        };
    }

    void SettingsMidiSync::refresh()
    {
        auto inputIndex  = inputList.getSelectedIndex();
        auto outputIndex = outputList.getSelectedIndex();

        const auto& input = mt::getAudioInterface().getMidiDeviceNames( true );

        for( auto& it : input )
            DBG( "InputName " << it );

        const auto& output = mt::getAudioInterface().getMidiDeviceNames( false );

        for( auto& it : output )
            DBG( "OutputName " << it );

        inputList.setListOptions( input, inputIndex );
        outputList.setListOptions( output, outputIndex );

        itrack.setToggleState( settings.midiControl.ipTrackState(), juce::dontSendNotification );
        icontrol.setToggleState( settings.midiControl.ipControlState(), juce::dontSendNotification );

        otrack.setToggleState( settings.midiControl.opTrackState(), juce::dontSendNotification );
        ocontrol.setToggleState( settings.midiControl.opControlState(), juce::dontSendNotification );

        inputList.setSelected( settings.midiControl.ipPortName(), juce::dontSendNotification );
    }

    void SettingsMidiSync::setupHeader( kick_ui::KickModeHeader& Hdr ) {}

    void SettingsMidiSync::handleCommand( CPktMgr::HWMsg_t& ) {}
}
