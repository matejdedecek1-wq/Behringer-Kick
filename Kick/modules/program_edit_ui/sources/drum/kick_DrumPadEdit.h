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
    class DrumPadEdit : public ui::KickGrid
    {
    public:
        DrumPadEdit( std::vector<int>& Pads, mt::MTDrumProgram*& PDrum );
        ~DrumPadEdit();

        void refresh();

    private:
        std::vector<int>&   pads;
        mt::MTDrumProgram*& pDrum;

        ui::KickLabel modeLabel{ TRANS( "Mode" ) };
        ui::KickLabel padLabel{ TRANS( "Levels" ) };
        ui::KickLabel simultaniousPlayLabel{ TRANS( "Simultanious Play" ) };
        ui::KickLabel muteTarget{ TRANS( "Mute Target" ) };

        ui::KickListBox       muteGroup{ "Mute Group", 4 };
        ui::KickToggleOptions layerPlay{ "Layer Play" };
        ui::KickToggleOptions samplePlay{ "Sample Play" };
        //ui::KickListBox padPolyphony{ "Mute Group", 4 };

        ui::KickRotary padVolume{ "Volume", juce::Slider::TextBoxLeft, true };
        ui::KickRotary padPan{ "Pan", juce::Slider::TextBoxLeft, true };

        ui::KickListBox  sPad[4], mPad[4];
        std::vector<int> sPlay, sMute;

        void visibilityChanged() override;
    };
}
