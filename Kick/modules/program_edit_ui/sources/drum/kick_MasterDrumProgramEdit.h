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

    class MasterDrumProgramEdit : public ui::KickGrid
        , public DrumInterface
    {
    public:
        explicit MasterDrumProgramEdit( std::vector<int>& Pads, mt::MTDrumProgram*& PDrum );
        ~MasterDrumProgramEdit() = default;

        void refresh() override;
        void handleCommand( CPktMgr::HWMsg_t& ) override;
        void refreshPads();

    private:
        std::vector<int>&                       pads;
        mt::MTDrumProgram*&                     pDrum;
        std::shared_ptr<mt::Kick::ScopeTracker> scope;

        ui::KickRotary        masterLevel{ "Level", juce::Slider::TextBoxAbove, true };
        ui::KickRotary        masterPan{ "Pan", juce::Slider::TextBoxAbove, true };
        ui::KickRotary        masterSemi{ "Semi", juce::Slider::TextBoxAbove, true };
        ui::KickRotary        masterFine{ "Fine", juce::Slider::TextBoxAbove, true };
        ui::KickRotary        masterPoly{ "Poly", juce::Slider::TextBoxAbove, true };
        ui::KickRotary        padLevel{ "Level", juce::Slider::TextBoxAbove, true };
        ui::KickRotary        padPan{ "Pan", juce::Slider::TextBoxAbove, true };
        ui::KickRotary        padSemi{ "Semi", juce::Slider::TextBoxAbove, true };
        ui::KickRotary        padFine{ "Fine", juce::Slider::TextBoxAbove, true };
        ui::KickToggleOptions layerPlay{ "Layer Play", { "Cycle", "Velocity", "Random" } };
        ui::KickToggleOptions samplePlay{ "Sample Play", { "One Shot", "Note Off", "Note On" } };
        ui::KickNumberT<int>  muteGroup{ "Mute Group", MUTEGROUP::MIN, MUTEGROUP::MAX, MUTEGROUP::DEFAULT, true };

        void visibilityChanged() override;
    };
}
