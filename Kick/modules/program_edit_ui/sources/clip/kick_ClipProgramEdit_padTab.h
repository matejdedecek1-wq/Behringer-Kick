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
    class ClipPadTab : public juce::Component
    {
    public:
        explicit ClipPadTab( int& SelectedPad, const std::shared_ptr<mt::MTClipProgram>& PClip );
        ~ClipPadTab() = default;

        void refresh();
        void handleCommand( CPktMgr::HWMsg_t& );

    private:
        void resized() override;
        void visibilityChanged() override;
        void paint( juce::Graphics& ) override;

    private:
        int&                                      selectedPad;
        const std::shared_ptr<mt::MTClipProgram>& pClip;

        ui::WaveRenderer wav;

        ui::KickLabel  nameLabel{ "Timing and Pitch" };
        ui::KickNumber start{ "START" };
        ui::KickNumber end{ "END" };
        ui::KickNumber semi{ "SEMI", SEMI::MIN, SEMI::MAX, SEMI::DEFAULT };
        ui::KickNumber fine{ "FINE", FINE::MIN, FINE::MAX, FINE::DEFAULT };

        ui::KickListBox muteGroup{ "Mute Group", ui::KickListBox::generateOptions( 1, 32, true ), 0, 3 };

        ui::KickToggleButton wrap{ "Warp", "Warp", "Warp" };
        ui::KickToggleButton reverse{ "Reverse", "Reverse", "Reverse" };
        ui::KickToggleButton fade{ "Fade", "Fade", "Fade" };
    };

}
