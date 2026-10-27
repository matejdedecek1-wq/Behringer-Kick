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

#pragma once

class AppSequence : public kick_ui::IRowView
    , public mt::MTAudioInterfaceListener
{
public:
    //==============================================================================
    std::function<int( int )> getSelectedSeqBPM;
    std::function<void()>     onSeqEdit;

    AppSequence();
    ~AppSequence();

    void populate( void );
    void onTransControlChange( juce::String, juce::String );
    void handleCommand( CPktMgr::HWMsg_t& f_HwMsg );
    void knobCommand( int knob, int value );

private:
    void setupProvider() override;
    void visibilityChanged() override;
    void onSequenceListChanged() override;
    void onSequenceSelectionChange( const mt::MTSelected& Selected ) override;
    //==============================================================================
    std::atomic_bool _initialized{ false };

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( AppSequence )
};

//[EndFile] You can add extra defines here...
//[/EndFile]
