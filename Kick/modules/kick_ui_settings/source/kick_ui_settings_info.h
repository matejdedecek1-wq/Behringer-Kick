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
    class SettingsInfo : public SettingsInterface
    {
    public:
        SettingsInfo();

        // Virtual functions
        void refresh() override;
        void setupHeader( kick_ui::KickModeHeader& Hdr ) override;
        void handleCommand( CPktMgr::HWMsg_t& ) override;

    private:
        mt::KickSettings& settings{ mt::getSettings() };
        TextValue         srNo{ "Serial Number", "t374t" };
        TextValue         fVer{ "Firmware Version", "y7344875y" };
        TextValue         cVer{ "Control Version", "sghdfjgdg" };
        TextValue         date{ "Build Date", "jdkfljlekw" };

        ui::KickImage  logo{ "Logo", "LOGO" };
        ui::KickButton fReset{ "Factory Reset" };
        ui::KickButton fwUpdate{ "Firmware Update" };
        ui::KickButton rDefaults{ "Restore Defaults" };

        void updateInfo();
    };
    class ConfirmationPopup : public juce::Component
    {
    public:
        ConfirmationPopup( juce::String, bool isYes = true, bool isNo = true, int x = 100, int y = 130, int width = 400 );

    private:
        void close( int state = 0 );
        void resized() override;

        ui::KickImageButton no{ "No", "No" };
        ui::KickImageButton yes{ "Yes", "Yes" };
        ui::KickImageButton cancel{ "Cancel", "Cancel_Browser" };
        ui::KickLabel       info;
        int                 startX, startY, width;
    };
}
