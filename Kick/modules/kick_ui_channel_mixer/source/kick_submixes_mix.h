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

    class SubmixMixer : public kick_ui::IRowView
    {
    public:
        SubmixMixer();
        ~SubmixMixer() = default;

        void populate();
        void knobCommand( int knob, int value );

    private:
        void setupProvider() override;
        void visibilityChanged() override;
        void meterUpdate() override;

        //==============================================================================
        mt::MTBusManager& _busMgr{ ae.getBusManager() };
        std::atomic_bool  _initialized{ false };

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( SubmixMixer )
    };

    class SubmixMixerDisplay : public juce::Component
    {
    public:
        std::function<void()> onSoloChanged;

        SubmixMixerDisplay();
        ~SubmixMixerDisplay() = default;

        void setBusNumber( int BusNo );
        void updateData();

        inline int getNumber( void ) { return busNo; }

        void                 meterUpdate();
        void                 linkRotated( int ID, int value );
        juce::Rectangle<int> getElementBounds( int state );

    private:
        mt::MTAudioInterface&                   ae{ mt::getAudioInterface() };
        mt::MTBusManager&                       busMgr{ ae.getBusManager() };
        std::shared_ptr<mt::Kick::ScopeTracker> scope;

        int         busNo{ -1 };
        mt::MTIBus* pBus{ nullptr };

        ui::KickSerialNumber    srNo{ "" };
        ui::KickImageButton     icon{ "Icon" };
        ui::KickImageButton     mute{ "Mute", "Mute_normal", "Mute_active" };
        ui::KickLabel           subMixName{ "" };
        ui::KickLabel           subMixHeader{ "BUS" };
        ui::KickButton          programInserts{ "Inserts", "Insert" };
        ui::KickMeterHorizantal levelMeter1;
        ui::KickMeterHorizantal levelMeter2;
        ui::KickListBox         outTo{ "OUT" };
        ui::KickLevelSlider     levelSlider;
        ui::KickPanRotary       panSlider;

        void clear();
        void resized() override;
        void mouseUp( const juce::MouseEvent& e ) override;
        void visibilityChanged() override;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( SubmixMixerDisplay )
    };

}
