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

#include "kick_ui_settings_info.h"
#include "../modules/hardware/source/kick_Hwpref.h"
#include "../modules/engine/interface/source/mt_audio_ids.h"

namespace kick_ui
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////  KickChannelMixer
    ///////////////////////////////////////////////////////////////////////////////////////////////////////

    SettingsInfo::SettingsInfo()
        : SettingsInterface( 7, 5, false )
    {
        ui::KickGrid::setBackgroundColour( juce::Colours::transparentBlack );

        fReset.setValueColour( ui::Theme::textColour( 1 ) );
        fwUpdate.setValueColour( ui::Theme::textColour( 1 ) );
        rDefaults.setValueColour( ui::Theme::textColour( 1 ) );

        fReset.setValueFontHeight( 20 );
        fwUpdate.setValueFontHeight( 20 );
        rDefaults.setValueFontHeight( 20 );

        addComponent( logo, 0, 0, 3, 4 );
        addComponent( srNo, 0, 3, 2, 1 );
        addComponent( fVer, 1, 3, 2, 1 );
        addComponent( cVer, 2, 3, 2, 1 );
        addComponent( date, 3, 3, 2, 1 );
        addComponent( fReset, 5, 1, 1, 1 );
        addComponent( fwUpdate, 5, 2, 1, 1 );
        addComponent( rDefaults, 5, 3, 1, 1 );

        ui::themeChanged.connect( [&]() {
            logo.setImage( "LOGO" );
        } );

        fwUpdate.onClick = [&]() { signals::sigScreenOpen( kick_ui::Screen::SWUpdateView ); };
        fReset.onClick   = [&]() {
            auto p = new ConfirmationPopup{ " Are You Sure You Want To Do Factory Reset ?", false, false };
            mt::utils::showOptions(
                "CONFIRMATION!", p, [&, p]( int R ) {
                   if( R == UI::APPLY )
                     mt::HwPref::getHwPref().factoryReset();
                   delete p; }, ui::layout::warningPopup, ui::Theme::backgroundColour( 1 ), ui::Theme::titleColour( 1 ), ui::Theme::backgroundColour( 2 ) );
        };
        rDefaults.onClick = [&]() {
            auto p = new ConfirmationPopup{ "Are You Sure You Want To Restore Defaults ?", false, false };
            mt::utils::showOptions(
                "CONFIRMATION!", p, [&, p]( int R ) {
                   if( R == UI::APPLY )
                    mt::getSettings().makeDefaults();
                   delete p; }, ui::layout::warningPopup, ui::Theme::backgroundColour( 1 ), ui::Theme::titleColour( 1 ), ui::Theme::backgroundColour( 2 ) );
        };

        updateInfo();
    }

    void SettingsInfo::updateInfo()
    {
        auto& hwpref = mt::HwPref::getHwPref();

        juce::String serialNumber          = hwpref.getDevSerial();
        juce::String ControlSurfaceVersion = hwpref.getSurfaceVersion();
        juce::String firmWareVersion       = hwpref.getFirmwareVersion();

        srNo.value.setText( serialNumber, juce::dontSendNotification );
        cVer.value.setText( ControlSurfaceVersion, juce::dontSendNotification );
        fVer.value.setText( firmWareVersion, juce::dontSendNotification );

#if defined( TIMESTAMP ) && ( TIMESTAMP > 0 )
        juce::String buildStamp = juce::String( BUILD_TIMESTAMP_STR ) + " :: " + juce::String( GIT_SHORT_HASH_STR ) + " :: " + juce::String( GIT_BRANCH_STR );
#else
        juce::String buildStamp = juce::String( __DATE__ ) + " :: " + juce::String( __TIME__ );
#endif
        date.value.setText( buildStamp, juce::dontSendNotification );
    }

    void SettingsInfo::refresh() {}

    void SettingsInfo::setupHeader( kick_ui::KickModeHeader& Hdr ) {}

    void SettingsInfo::handleCommand( CPktMgr::HWMsg_t& ) {}

    ConfirmationPopup::ConfirmationPopup( juce::String Info, bool isYes, bool isNo, int x, int y, int width )
        : info( Info ), startX( x ), startY( y ), width( width )
    {
        mt::utils::addChildComponents( this, yes, no, info );
        yes.setVisible( isYes );
        no.setVisible( isNo );
        info.setVisible( true );
        yes.onClick = [&]() { close( UI::APPLY ); };
        no.onClick  = [&]() { close( UI::CANCEL ); };
        info.setFontHeight( 24.0f );
        info.setTextColour( ui::Theme::textColour( 2 ) );
        info.setValueBackgroundColour( ui::Theme::backgroundColour( 9 ) );
        info.setValueJustification( juce::Justification::centred );

        ui::themeChanged.connect( [&]() {
            no.setImages( "No" );
            yes.setImages( "Yes" );
        } );
    }
    void ConfirmationPopup::close( int State )
    {
        if( auto dw = getCurrentlyModalComponent() )
            dw->exitModalState( State );
    }
    void ConfirmationPopup::resized()
    {
        info.setBounds( startX, startY, width, 25 );
        no.setBoundsRelative( 0.05f, 0.86f, 0.4f, 0.106f );
        yes.setBoundsRelative( 0.55f, 0.86f, 0.4f, 0.106f );
    }
}
