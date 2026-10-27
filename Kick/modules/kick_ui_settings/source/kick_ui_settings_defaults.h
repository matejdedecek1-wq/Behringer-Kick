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
    class SettingsPDefault : public SettingsInterface

    {
    public:
        SettingsPDefault();

        // Virtual functions
        void refresh() override;
        void setupHeader( kick_ui::KickModeHeader& Hdr ) override;
        void handleCommand( CPktMgr::HWMsg_t& ) override;

    private:
    private:
        mt::KickSettings& settings{ mt::getSettings() };

        ui::KickListBox ssLaunch{ ui::KickListBox::withTitleBar, "ON LAUNCH", { "Selector", "New Project", "Last Used" } };
        ui::KickNumber  ssRecProjects{ "RECENT PROJECTS", 1, 5, 5, 1 };

        ui::KickToggleButton asEnabled{ "AUTO SAVE", "ENABLED", "DISABLED" };
        ui::KickListBox      asTimeout{ ui::KickListBox::withTitleBar, "SAVE AFTER", { "15 Sec", "30 Sec", "1 min", "2 min", "5 min" }, 1 };

        ui::KickTempo   pdTempo{ "TEMPO", (int)BPM::MIN, (int)BPM::MAX, (int)BPM::DEFAULT, (int)BPM::STEP };
        ui::KickNumber  pdBars{ "BARS", BAR::MIN, BAR::MAX, BAR::DEFAULT, BAR::STEP };
        ui::KickListBox pdNum{ ui::KickListBox::withTitleBar, "NUMERATOR", { "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16" }, 3, 2 };
        ui::KickListBox pdDen{ ui::KickListBox::withTitleBar, "DENOMINATOR", { "4", "8", "16", "32" }, 0 };

        ui::KickListBox pdRootNote{ ui::KickListBox::withTitleBar, "ROOT NOTE", mt::Note::rootNames(), 0, 3 };
        ui::KickListBox pdScale{ ui::KickListBox::withTitleBar, "SCALE", mt::ScaleAndNotes::scaleNames, 0, 3 };
        ui::KickNumber  pdSeq{ "SEQUENCES", 1, 64, 4, 1 };
        ui::KickNumber  pdTracks{ "TRACKS/SEQ", 1, 64, 4, 1 };
        ui::KickListBox bitDepath{ ui::KickListBox::withTitleBar, "BIT Depth", { "16", "24" }, 1 };
        ui::KickListBox enableList{ ui::KickListBox::BoxType::withoutTitleBar, "state", { "DISABLE", "TIME STAMP", "CUSTOM" } };
        ui::KickLabel   defaultName{ "" };
    };
}
