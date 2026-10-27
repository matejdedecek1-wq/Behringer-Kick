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

// Module include files.
namespace kick_ui
{
    class SettingsMidiSync : public SettingsInterface
    {
    public:
        SettingsMidiSync();

        // Virtual functions
        void refresh() override;
        void setupHeader( kick_ui::KickModeHeader& Hdr ) override;
        void handleCommand( CPktMgr::HWMsg_t& ) override;

    private:
        mt::KickSettings& settings{ mt::getSettings() };

        ui::KickListBox inputList{ ui::KickListBox::withoutTitleBar, "MidiInput", {}, 0, 1 };
        ui::KickListBox outputList{ ui::KickListBox::withoutTitleBar, "MidiOutput", {}, 0, 1 };

        ui::KickToggleButton itrack{ "TRACK", "TRACK", "TRACK" };
        ui::KickToggleButton icontrol{ "CONTROL", "CONTROL", "CONTROL" };

        ui::KickToggleButton otrack{ "TRACK", "TRACK", "TRACK" };
        ui::KickToggleButton ocontrol{ "CONTROL", "CONTROL", "CONTROL" };
    };
}
