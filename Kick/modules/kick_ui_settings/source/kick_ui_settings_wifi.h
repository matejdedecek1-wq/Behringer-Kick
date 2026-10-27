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
    class SettingsWifi : public SettingsInterface
    {
    public:
        SettingsWifi();

        // Virtual functions
        void refresh() override;
        void setupHeader( kick_ui::KickModeHeader& Hdr ) override;
        void handleCommand( CPktMgr::HWMsg_t& ) override;

    private:
        void handleEnabledClick();

        void initializeInterfaceState();
        void connectWifi( bool connect );

    private:
        mt::KickSettings&    settings{ mt::getSettings() };
        ui::KickToggleButton enabled{ "Wifi", "ENABLED", "DISABLED" };

        ui::KickLabel        password{ "", "" };
        ui::KickListBox      wifiList{ "availableWifi" };
        ui::KickToggleButton scanWifi{ "scanWifi", "ScanWifi", "ScanWifi" };
        ui::KickToggleButton connect{ "connect", "connect", "disconnected" };

        ui::KickLabel wifIP{ "IpAddress", "" };
        ui::KickLabel gateway{ "gatway", "" };
        ui::KickLabel subnet{ "subnet", "" };
    };
}
