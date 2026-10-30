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

#include "kick_ui_settings_etnernet.h"
#include "../modules/hardware/source/kick_Hwpref.h"

namespace kick_ui
{

    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////  KickChannelMixer
    ///////////////////////////////////////////////////////////////////////////////////////////////////////

    SettingsEthernet::SettingsEthernet()
        : SettingsInterface( 6, 3, true )
    {
        setRequiredFeature( mt::hw::Feature::ethernet, "Ethernet",
                            "On the unit eth0 is configured with ifconfig and udhcpc. This system has no eth0 for the app to manage, and its network is left as it is." );
        ui::KickGrid::setBackgroundColour( juce::Colours::transparentBlack );
        setRowTitles( { "ETHERNET SETTINGS", "IP ADDRESS", "SUBNET MASK", "GATEWAY", "DNS SERVER" } );

        addComponent( enabled, 0, 0 );
        addComponent( method, 0, 1 );

        addComponent( ipAddr, 1, 0, 2 );
        addComponent( subnet, 2, 0, 2 );
        addComponent( gateway, 3, 0, 2 );
        addComponent( dns, 4, 0, 2 );

        enabled.onClick = [&]() {
            handleEnabledClick();
        };

        method.onClick = [&]() {
            handleMethodClick();
        };

        ipAddr.onTextChange = [&]() {
            settings.eth.ipAddress = ( ipAddr.getTextValue().toString() );
            update();
            DBG( "IPAddress -> " << ipAddr.getTextValue().toString() );
        };

        gateway.onTextChange = [&]() {
            settings.eth.gateway = ( gateway.getTextValue().toString() );
            update();
            DBG( "gateWay -> " << gateway.getTextValue().toString() );
        };

        subnet.onTextChange = [&]() {
            settings.eth.subnetMask = ( subnet.getTextValue().toString() );
            update();
            DBG( "subnet -> " << subnet.getTextValue().toString() );
        };
        dns.onTextChange = [&]() {
            update();
            DBG( "dns -> " << dns.getTextValue().toString() );
        };
        initializeInterfaceState();
    }

    void SettingsEthernet::handleEnabledClick()
    {
        bool status          = enabled.getToggleState();
        settings.eth.enabled = ( status );
        setComponentsEnabled( status );

        mt::HwPref::getHwPref().ethernetEnable( status );
    }

    void SettingsEthernet::handleMethodClick()
    {
        bool useDhcp      = method.getToggleState();
        settings.eth.mode = ( useDhcp ? "MANUAL" : "DHCP" );

        if( useDhcp )
        {
            setComponentsEnabled( false );
            mt::HwPref::getHwPref().startDhcp();
        }
        else
        {
            setComponentsEnabled( true );
            mt::HwPref::getHwPref().stopDhcp();
        }
    }

    void SettingsEthernet::setComponentsEnabled( bool Enabled )
    {
        ipAddr.setEditable( Enabled );
        gateway.setEditable( Enabled );
        subnet.setEditable( Enabled );
        dns.setEditable( Enabled );
    }

    void SettingsEthernet::refresh()
    {
        bool status          = mt::HwPref::getHwPref().isInterfaceUp();
        settings.eth.enabled = ( status );
        enabled.setToggleState( status, juce::dontSendNotification );

        mt::sysEthInfo info;
        mt::HwPref::getHwPref().getIP( "eth0", info );

        ipAddr.setText( info.IP, juce::dontSendNotification );

        subnet.setText( info.subnet, juce::dontSendNotification );
        gateway.setText( info.gateway, juce::dontSendNotification );
        dns.setText( "0.0.0.0", juce::dontSendNotification );

        settings.eth.ipAddress  = ( info.IP );
        settings.eth.gateway    = ( info.gateway );
        settings.eth.subnetMask = ( info.subnet );
    }

    void SettingsEthernet::initializeInterfaceState()
    {
        bool status          = mt::HwPref::getHwPref().isInterfaceUp();
        settings.eth.enabled = ( status );
        enabled.setToggleState( status, juce::dontSendNotification );

        juce::String mode = settings.eth.mode();
        DBG( "i####################################################3mode -> " << mode );
        if( mode == "DHCP" )
        {
            mt::HwPref::getHwPref().startDhcp();
        }
        else
        {
            //here it will update based on value tree.
            update();
            mt::HwPref::getHwPref().stopDhcp();
        }
    }

    void SettingsEthernet::update()
    {
        juce::String ip   = settings.eth.ipAddress();
        juce::String gate = settings.eth.gateway();
        juce::String sub  = settings.eth.subnetMask();
        mt::HwPref::getHwPref().setIP( "eth0", ip.toStdString(), sub.toStdString(), gate.toStdString() );
    }

    void SettingsEthernet::setupHeader( kick_ui::KickModeHeader& Hdr ) {}

    void SettingsEthernet::handleCommand( CPktMgr::HWMsg_t& ) {}

}
