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

#include "kick_ui_settings_wifi.h"
#include "../modules/hardware/source/kick_Hwpref.h"
#include "../modules/hardware/source/kick_WifiFirmware.h"

namespace kick_ui
{

    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////  KickChannelMixer
    ///////////////////////////////////////////////////////////////////////////////////////////////////////

    SettingsWifi::SettingsWifi()
        : SettingsInterface( 6, 3, true )
    {
        setRequiredFeature( mt::hw::Feature::wifi, "Wi-Fi",
                            "On the unit Wi-Fi is wpa_supplicant and dhclient on its wlan0 interface. This system has no wlan0 for the app to manage: nothing is scanned or changed." );
        ui::KickGrid::setBackgroundColour( juce::Colours::transparentBlack );
        setRowTitles( { "", "WIFI SETTINGS", "SSID", "PASSWORD", "Connection/Status" } );

        addComponent( enabled, 1, 0 );
        addComponent( scanWifi, 2, 0 );
        addComponent( wifiList, 2, 1 );
        addComponent( password, 3, 0 );
        addComponent( connect, 4, 0 );
        addComponent( wifIP, 4, 1 );
        addComponent( gateway, 4, 2 );
        addComponent( subnet, 4, 3 );

        password.setEditable( false, true );
        enabled.onClick = [&]() {
            settings.wifi.enabled = ( enabled.getToggleState() );
        };

        scanWifi.onClick = [&]() {
            if( scanWifi.getToggleState() )
            {
                DBG( "Scanning Wifi" );
                std::vector<juce::String> list = mt::WifiManager().scanWiFiNetworks();
                wifiList.setListOptions( list );
            }
        };

        password.onTextChange = [&]() {
            juce::String text = password.getText();
            settings.wifi.pwd = ( text );
        };

        wifiList.onSelectionChanged = [&]( int value, juce::String str ) {
            settings.wifi.ssid = ( str );
        };

        connect.onClick = [&]() {
            connectWifi( connect.getToggleState() );
        };
    }

    void SettingsWifi::connectWifi( bool Connect )
    {
        if( Connect )
            mt::WifiManager().getWifiInterface()->connectToWiFi( wifiList.getSelectedValue(), settings.wifi.pwd() );
        else
            mt::WifiManager().getWifiInterface()->disconnectWiFi();
    }

    void SettingsWifi::handleEnabledClick()
    {
        bool status = enabled.getToggleState();
        if( status )
        {
            mt::WifiManager().enableWiFi();
        }
        else
            mt::WifiManager().disableWiFi();
    }

    void SettingsWifi::refresh()
    {
        mt::sysEthInfo info;
        mt::HwPref::getHwPref().getIP( "wlan0", info );

        wifIP.setText( info.IP, juce::dontSendNotification );
        subnet.setText( info.subnet, juce::dontSendNotification );
        gateway.setText( info.subnet, juce::dontSendNotification );
#if 0
        // Assuming wifiList is a custom UI component or structure managing the list of networks
        juce::String previouslySelectedSSID = wifiList.getSelected(); // Step 1: Store the currently selected SSID

        std::vector<juce::String> list = mt::WifiManager().scanWiFiNetworks(); // Refresh the list of networks

        wifiList.setListOptions( list ); // Update the UI or list with the new set of networks

        // Step 2: Reselect the previously selected SSID if it's still available
        if( std::find( list.begin(), list.end(), previouslySelectedSSID ) != list.end() )
        {
            wifiList.setSelected( previouslySelectedSSID );
        }
        else
        {
            // Optionally, handle the case where the previously selected SSID is no longer available
            wifiList.setSelected( "" ); // or any default behavior
        }
#endif
    }

    void SettingsWifi::initializeInterfaceState()
    {
        if( settings.wifi.enabled() )
        {
            mt::WifiManager().getWifiInterface()->connectToWiFi( settings.wifi.ssid(), settings.wifi.pwd() );
        }
    }

    void SettingsWifi::setupHeader( kick_ui::KickModeHeader& Hdr ) {}

    void SettingsWifi::handleCommand( CPktMgr::HWMsg_t& ) {}

}
