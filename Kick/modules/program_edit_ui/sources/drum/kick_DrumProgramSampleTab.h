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

namespace kick_ui
{
#define DP_MASTER_ROTARIES ( 9 )
#define DP_MASTER_NUMERICS ( 1 )
#define DP_MASTER_RADIOS   ( 2 )

    class DPSampleTab : public ui::KickGrid
        , private ui::KickToggleButton::Listener
        , public DrumInterface
    {
    public:
        explicit DPSampleTab( std::vector<int>& Pads, mt::MTDrumProgram*& PDrum );
        ~DPSampleTab() = default;

        void refresh() override;
        void handleCommand( CPktMgr::HWMsg_t& ) override;
        void refreshPads();

    private:
        void buttonClicked( juce::Button* ) override{};
        void buttonStateChanged( juce::Button* ) override;

    private:
        std::vector<int>&   pads;
        mt::MTDrumProgram*& pDrum;

        ui::WaveRenderer wav;
        int              selectedLayer{ 0 };

        std::array<ui::KickToggleButton, 4> layer;

        ui::KickNumberT<juce::int64> sStart{ "START" };
        ui::KickNumberT<juce::int64> sEnd{ "END" };
        ui::KickNumberT<juce::int64> lStart{ "LOOP START" };
        ui::KickNumberT<juce::int64> lEnd{ "LOOP END" };

        ui::KickRotary semi{ "Semi", juce::Slider::TextBoxAbove, true };
        ui::KickRotary fine{ "Fine", juce::Slider::TextBoxAbove, true };

        ui::KickToggleButton loop{ "LOOP", "LOOPING", "NOT LOOPING" };
        ui::KickToggleButton reverse{ "REVERSE", "REVERSE", "FORWARD" };

        void visibilityChanged() override;
    };
}
