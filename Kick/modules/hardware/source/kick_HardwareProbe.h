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

#include <juce_core/juce_core.h>

namespace mt::hw
{
    /// Device features that some screens and helpers drive directly (shell commands, device nodes).
    enum class Feature
    {
        wifi,      ///< wlan0: iwlist, ip link, wpa_supplicant, dhclient
        ethernet,  ///< eth0: ifconfig, udhcpc
        cv,        ///< CV / gate DAC on /dev/i2c-3
        update,    ///< firmware update with swupdate
        clock,     ///< setting the system date, time and time zone
        resources, ///< CPU, RAM and storage meters (/proc/stat, sysinfo, df)
    };

    /// "wifi", "ethernet", "cv", "update", "clock", "resources": the names KICK_HARDWARE uses.
    juce::String getName( Feature F );

    /// Whether this system has the feature, so that the code driving it may run.
    ///
    /// Without KICK_HARDWARE the answer comes from the system itself, on Linux only (never on
    /// other platforms): wifi needs /sys/class/net/wlan0, ethernet /sys/class/net/eth0, cv
    /// /dev/i2c-3, update swupdate on PATH; clock and resources are always there. It is checked
    /// on every call, so an interface that appears after start-up is picked up. On a unit every
    /// feature is present and this is always true.
    ///
    /// KICK_HARDWARE overrides the probe on any platform: "none" makes every feature unavailable,
    /// a comma-separated list ("wifi,clock") makes exactly those available. It is read once.
    bool isAvailable( Feature F );
} // namespace mt::hw
