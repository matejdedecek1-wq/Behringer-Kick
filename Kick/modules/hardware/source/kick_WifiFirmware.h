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

#include <string>

namespace mt
{
    class WifiManager
    {
    public:
        WifiManager();
        ~WifiManager();
        bool                            enableWiFi();
        bool                            disableWiFi();
        const std::vector<juce::String> scanWiFiNetworks();
        void                            displayWiFiNetworks( const std::vector<juce::String>& networks );
        bool                            connectToWiFi( const juce::String& ssid, const juce::String& password, int priority = 5 );
        bool                            disconnectWiFi();

        static WifiManager* getWifiInterface()
        {
            static WifiManager* _wifiInterface;
            if( NULL == _wifiInterface )
            {
                _wifiInterface = new WifiManager();
            }

            return _wifiInterface;
        }

    private:
        std::vector<juce::String>          networks;
        juce::String                       executeCommand( const juce::String& command );
        void                               addSSID( const juce::String& ssid, const juce::String& password, int priority = 5 );
        std::map<std::string, std::string> loadWifiCredentials();
        void                               saveWifiCredential( const std::string& ssid, const std::string& password );
        bool                               fileExists( const std::string& filename );
    };
}
