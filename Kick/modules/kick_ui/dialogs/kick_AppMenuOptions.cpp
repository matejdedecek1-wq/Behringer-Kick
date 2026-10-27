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
    AppMenuOptions::AppMenuOptions()
        : KickMenuComponent( "MenuOptions", 4 ), IScreen( Screen::HomeOptions )
    {
        addOption( { "Browser", "Browser" }, "" );
        addOption( { "Track Mixer", "Track Mixer" }, "" );
        addOption( { "Track Mutes", "Track Mutes" }, "" );
        addOption( { "Sequences", "Sequences" }, "" );

        addOption( { "listedit", "listedit" }, "" );
        addOption( { "Padmixer", "Padmixer" }, "" );
        addOption( { "Padmutes", "Padmutes" }, "" );
        addOption( { "songs", "songs" }, "" );

        addOption( { "Coloreditor", "Coloreditor" }, "" );
        addOption( { "Sampler", "Sampler" }, "" );
        addOption( { "Sample editor", "Sample editor" }, "" );
        addOption( { "Program edit", "Program edit" }, "" );

        addOption( { "Padnotes", "Padnotes" }, "" );
        addOption( { "Looper", "Looper" }, "" );
        addOption( { "Qlinks", "Qlinks" }, "" );
        addOption( { "Midicontrol", "Midicontrol" }, "" );

        setVisible( true );
        setSelectionEnabled( false );

        onMenuOptionClicked = [&]( const juce::String& /*Option*/, int Index, const juce::String& /*Group*/ ) {
            loadScreen( Index );
        };
    }

    AppMenuOptions::~AppMenuOptions()
    {
        setVisible( false );
    }

    void AppMenuOptions::handleCommand( CPktMgr::HWMsg_t& f_HwMsg )
    {
        if( contracts::DclContractsHelper::isButtonMessage( f_HwMsg ) && ( contracts::DclContractsHelper::getButtonState( f_HwMsg ) == contracts::DeviceControlContracts::BTN_PRESSED ) )
        {
            auto  buttonCode = contracts::DclContractsHelper::getButtonCode( f_HwMsg );
            auto& ae         = mt::getAudioInterface();
            auto& aeSel      = ae.getSelected();
            switch( buttonCode )
            {
                case contracts::DeviceControlContracts::EN_PLAY: ae.setSequencePlay( aeSel.sequence, false, true ); return;
                case contracts::DeviceControlContracts::EN_PLAYSTART: ae.setSequencePlay( aeSel.sequence, true, true ); return;
                case contracts::DeviceControlContracts::EN_STOP: ae.setSequencePause(); return;
            }
        }
        KickMenuComponent::handleCommand( f_HwMsg );
    }

    void AppMenuOptions::visibilityChanged()
    {
        KickMenuComponent::visibilityChanged();
    }
    void AppMenuOptions::loadScreen( int index )
    {
        switch( index )
        {
            case 0: signals::sigScreenOpen( kick_ui::Screen::MenuFileBrowser ); break;
            case 1: signals::sigScreenOpen( kick_ui::Screen::MenuChannelMixer ); break;
            case 2: signals::sigScreenOpen( kick_ui::Screen::MenuTrackMute ); break;
            case 3: signals::sigScreenOpen( kick_ui::Screen::MenuNextSequence ); break;

            case 4: signals::sigScreenOpen( kick_ui::Screen::MenuListEdit ); break;
            case 5: signals::sigScreenOpen( kick_ui::Screen::MenuPadMixer ); break;
            case 6: signals::sigScreenOpen( kick_ui::Screen::MenuPadMute ); break;
            case 7: signals::sigScreenOpen( kick_ui::Screen::MenuSong ); break;

            case 8: signals::sigScreenOpen( kick_ui::Screen::MenuColorEdit ); break;
            case 9: signals::sigScreenOpen( kick_ui::Screen::MenuSampler ); break;
            case 10: signals::sigScreenOpen( kick_ui::Screen::MenuSampleEdit ); break;
            case 11: signals::sigScreenOpen( kick_ui::Screen::MenuProgramEdit ); break;

            case 12: signals::sigScreenOpen( kick_ui::Screen::MenuPadPerform ); break;
            case 13: signals::sigScreenOpen( kick_ui::Screen::MenuLooper ); break;
            case 14: signals::sigScreenOpen( kick_ui::Screen::MenuKnobEdit ); break;
            case 15: signals::sigScreenOpen( kick_ui::Screen::MenuMidiControl ); break;
            default: break;
        }
    }
}
