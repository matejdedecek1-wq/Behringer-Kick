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

namespace kick_ui
{
    class ProgramMixer : public kick_ui::IRowView
        , public mt::MTAudioInterfaceListener
    {
    public:
        ProgramMixer();
        ~ProgramMixer() = default;

        void populate();
        void knobCommand( int knob, int value );

    private:
        void setupProvider() override;
        void visibilityChanged() override;

        void onProgramListChanged() override;
        void meterUpdate() override;
        //==============================================================================
        std::atomic_bool _initialized{ false };

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( ProgramMixer )
    };

    class ProgramMixerDisplay : public juce::Component
    {
    public:
        std::function<void()> onSoloChanged;

        ProgramMixerDisplay();
        ~ProgramMixerDisplay() = default;

        void setProgram( int Prog );
        void updateData();

        inline int  getProgNumber( void ) { return progNo; }
        inline bool isProgSelected( void ) { return isHighlight; }

        void                 meterUpdate();
        void                 linkRotated( int ID, int value );
        juce::Rectangle<int> getElementBounds( int state );

    private:
        mt::MTAudioInterface&                   ae{ mt::getAudioInterface() };
        std::shared_ptr<mt::Kick::ScopeTracker> scope;

        bool            isHighlight{ false };
        int             progNo{ -1 };
        mt::MTIProgram* pProg{ nullptr };

        ui::KickSerialNumber    srNo{ "" };
        ui::KickLabel           programName{ "" };
        ui::KickLabel           programHeader{ "PROGRAM" };
        ui::KickButton          programInserts{ " Inserts", "Insert" };
        ui::KickImageButton     monitor{ "Monitor", "Monitor_normal", "Monitor_active" };
        ui::KickImageButton     solo{ "Solo", "Solo_normal", "Solo_active" };
        ui::KickImageButton     mute{ "Mute", "Mute_normal", "Mute_active" };
        ui::KickMeterHorizantal levelMeter1;
        ui::KickMeterHorizantal levelMeter2;
        ui::KickListBox         ioListBox{ "OUT" };

        ui::KickLevelSlider levelSlider;
        ui::KickPanRotary   panSlider;

        void clear();
        void resized() override;
        void mouseUp( const juce::MouseEvent& e ) override;
        void visibilityChanged() override;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( ProgramMixerDisplay )
    };
}
