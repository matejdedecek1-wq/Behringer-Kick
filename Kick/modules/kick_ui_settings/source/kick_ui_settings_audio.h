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
    class SettingsAudio : public SettingsInterface
    {
    public:
        SettingsAudio();

        // Virtual functions
        void refresh() override;
        void setupHeader( kick_ui::KickModeHeader& Hdr ) override;
        void handleCommand( CPktMgr::HWMsg_t& ) override;

    private:
    private:
        ui::KickListBox sampleRate{ ui::KickListBox::withTitleBar, "Thresold", { "1", "2", "3", "4", "5", "6", "7", "8" }, 4 };
        ui::KickListBox bitDepth{ ui::KickListBox::withTitleBar, "Sensitivity", { "1", "2", "3", "4", "5", "6", "7", "8" }, 4 };
    };
}
