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

#include "kick_ui_settings_cv.h"

namespace kick_ui
{

    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////  SettingsCV
    ///////////////////////////////////////////////////////////////////////////////////////////////////////

    SettingsCV::SettingsCV()
        : SettingsInterface( 5, 4, true )
    {
        ui::KickGrid::setBackgroundColour( juce::Colours::transparentBlack );
        setRowTitles( { "CV Range Settings", "", "", "" } );

        calMsg.setText( "Note: Before starting calibration process, please disconnect any external gears connected on CV ports.", juce::dontSendNotification );
        calMsg.setValueFontHeight( 16 );
        calMsg.setValueJustification( juce::Justification::centred );
        calMsg.setValueColour( ui::Theme::textColour( 2 ) );

        addComponent( cv0, 0, 0 );
        addComponent( cv1, 0, 1 );
        addComponent( cv2, 0, 2 );
        addComponent( cv3, 0, 3 );

        addComponent( cv4, 1, 0 );
        addComponent( cv5, 1, 1 );
        addComponent( cv6, 1, 2 );
        addComponent( cv7, 1, 3 );

        addComponent( calibrate, 3, 0 );
        addComponent( calMsg, 3, 1, 3 );

        cv0.onSelectionChanged = [this]( int index, const juce::String& str ) {
            settings.cv.setNoteStart( 0, mt::ScaleAndNotes::getNoteNumber( str ) );
            mt::CV::getCVInterface().setChannelNoteMap( 0, mt::ScaleAndNotes::getNoteNumber( str ) );
        };
        cv1.onSelectionChanged = [this]( int index, const juce::String& str ) {
            settings.cv.setNoteStart( 1, mt::ScaleAndNotes::getNoteNumber( str ) );
            mt::CV::getCVInterface().setChannelNoteMap( 1, mt::ScaleAndNotes::getNoteNumber( str ) );
        };
        cv2.onSelectionChanged = [this]( int index, const juce::String& str ) {
            settings.cv.setNoteStart( 2, mt::ScaleAndNotes::getNoteNumber( str ) );
            mt::CV::getCVInterface().setChannelNoteMap( 2, mt::ScaleAndNotes::getNoteNumber( str ) );
        };
        cv3.onSelectionChanged = [this]( int index, const juce::String& str ) {
            settings.cv.setNoteStart( 3, mt::ScaleAndNotes::getNoteNumber( str ) );
            mt::CV::getCVInterface().setChannelNoteMap( 3, mt::ScaleAndNotes::getNoteNumber( str ) );
        };
        cv4.onSelectionChanged = [this]( int index, const juce::String& str ) {
            settings.cv.setNoteStart( 4, mt::ScaleAndNotes::getNoteNumber( str ) );
            mt::CV::getCVInterface().setChannelNoteMap( 4, mt::ScaleAndNotes::getNoteNumber( str ) );
        };
        cv5.onSelectionChanged = [this]( int index, const juce::String& str ) {
            settings.cv.setNoteStart( 5, index );
            mt::CV::getCVInterface().setChannelNoteMap( 5, index );
        };
        cv6.onSelectionChanged = [this]( int index, const juce::String& str ) {
            settings.cv.setNoteStart( 6, index );
            mt::CV::getCVInterface().setChannelNoteMap( 6, index );
        };
        cv7.onSelectionChanged = [this]( int index, const juce::String& str ) {
            settings.cv.setNoteStart( 7, index );
            mt::CV::getCVInterface().setChannelNoteMap( 7, index );
        };
        calibrate.onClick = [&]() {
            signals::sigScreenOpen( Screen::CVClib );
        };
    }

    void SettingsCV::refresh()
    {
        int note = settings.cv.getNoteStart( 0 );
        cv0.setSelected( note, juce::sendNotification );
        mt::CV::getCVInterface().setChannelNoteMap( 0, note );

        note = settings.cv.getNoteStart( 1 );
        cv1.setSelected( note, juce::sendNotification );
        mt::CV::getCVInterface().setChannelNoteMap( 1, note );

        note = settings.cv.getNoteStart( 2 );
        cv2.setSelected( note, juce::sendNotification );
        mt::CV::getCVInterface().setChannelNoteMap( 2, note );

        note = settings.cv.getNoteStart( 3 );
        cv3.setSelected( note, juce::sendNotification );
        mt::CV::getCVInterface().setChannelNoteMap( 3, note );

        note = settings.cv.getNoteStart( 4 );
        cv4.setSelected( note, juce::sendNotification );
        mt::CV::getCVInterface().setChannelNoteMap( 4, note );

        note = settings.cv.getNoteStart( 5 );
        cv5.setSelected( note, juce::sendNotification );
        mt::CV::getCVInterface().setChannelNoteMap( 5, note );

        note = settings.cv.getNoteStart( 6 );
        cv6.setSelected( note, juce::sendNotification );
        mt::CV::getCVInterface().setChannelNoteMap( 6, note );

        note = settings.cv.getNoteStart( 7 );
        cv7.setSelected( note, juce::sendNotification );
        mt::CV::getCVInterface().setChannelNoteMap( 7, note );
    }

    void SettingsCV::setupHeader( kick_ui::KickModeHeader& Hdr ) {}

    void SettingsCV::handleCommand( CPktMgr::HWMsg_t& ) {}

    CVClibRawComponent::CVClibRawComponent( int index )
        : rowIndex( index )
    {
        mt::utils::addAndMakeVisible( this, expectedVoltage, adjustmentMinus, adjustmentPlus, resetBtn, offset, step, expVelLabel, adjLabel );

        expectedVoltage.setText( juce::String( rowIndex ) + " V", juce::dontSendNotification );
        step.setText( "STEP - " + juce::String( rowIndex ), juce::dontSendNotification );
        expectedVoltage.setValueBackgroundColour( ui::Theme::backgroundColour( 3 ) );
        expectedVoltage.setValueJustification( juce::Justification::centred );
        mt::utils::setTitleFontHeight( 12, expVelLabel, adjLabel );
        expVelLabel.setTextColour( ui::Theme::textColour( 1 ) );
        adjLabel.setTextColour( ui::Theme::textColour( 1 ) );
        mt::utils::setValueFontHeight( 20, expectedVoltage, step, resetBtn );
        mt::utils::setValueColour( ui::Theme::textColour( 1 ), adjustmentMinus, adjustmentPlus, resetBtn );
        expectedVoltage.setValueFontHeight( 20 );
        adjustmentMinus.onClick = [this]() { if (onMinusClicked) onMinusClicked(rowIndex); };
        adjustmentPlus.onClick  = [this]() { if (onPlusClicked) onPlusClicked(rowIndex); };
        resetBtn.onClick        = [this]() { if (onResetClicked) onResetClicked(rowIndex); };

        offset.setText( juce::String( Clib ), juce::dontSendNotification );
        /*
        resetBtn.onClick = [&]() {
            DBG( "ID " << rowIndex );
            Clib = 0;
            offset.setText( juce::String( Clib ), juce::dontSendNotification );
            mt::CV::getCVInterface().calibrateCV( rowIndex, Clib );
        };
        adjustmentPlus.onClick = [&]() {
            DBG( "Plus button clicked " << rowIndex );
            Clib += 1;
            mt::CV::getCVInterface().calibrateCV( rowIndex, Clib );
            offset.setText( juce::String( Clib ), juce::dontSendNotification );
        };
        adjustmentMinus.onClick = [&]() {
            DBG( "minus button clicked " << rowIndex );
            Clib -= 1;
            offset.setText( juce::String( Clib ), juce::dontSendNotification );
            mt::CV::getCVInterface().calibrateCV( rowIndex, Clib );
        };
        */
    }

    void CVClibRawComponent::resized()
    {
        expectedVoltage.setBoundsRelative( 0.3048f, 0.285f, 0.1219f, 0.628f );
        step.setBoundsRelative( 0.2032f, 0.285f, 0.081f, 0.628f );
        adjustmentMinus.setBoundsRelative( 0.4674f, 0.285f, 0.0609f, 0.628f );
        adjustmentPlus.setBoundsRelative( 0.5386f, 0.285f, 0.0609f, 0.628f );
        resetBtn.setBoundsRelative( 0.6402f, 0.285f, 0.1016f, 0.628f );
        offset.setBoundsRelative( 0.7621f, 0.285f, 0.1219f, 0.628f );
        expVelLabel.setBoundsRelative( 0.3150f, 0.0f, 0.1219f, 0.2857f );
        adjLabel.setBoundsRelative( 0.4979f, 0.0f, 0.1219f, 0.2857f );
    }

    void CVClibRawComponent::paint( juce::Graphics& g )
    {
        g.setColour( ui::Theme::backgroundColour( 14 ) );
        g.drawHorizontalLine( 79, 50, 940 );
    }

    CVClib::CVClib()
        : kick_ui::IScreen( kick_ui::Screen::CVClib )
    {
        mt::utils::addAndMakeVisible( this, apply, goback, header );
        header.showOptions( false, false );
        header.onBackButtonPressed = [this]( bool ) { closeScreen(); };
        mt::utils::setValueFontHeight( 20, goback, apply );
        goback.setValueColour( ui::Theme::textColour( 1 ) );
        apply.setValueColour( ui::Theme::textColour( 1 ) );
        for( int i = 0; i < 4; ++i )
        {
            auto* rawComponent = new CVClibRawComponent( i );
            addAndMakeVisible( rawComponent );
            rawComponents.add( rawComponent );
            rawComponent->setMinusButtonClickCallback( [this]( int rowIndex ) { handleMinusButtonClick( rowIndex ); } );
            rawComponent->setPlusButtonClickCallback( [this]( int rowIndex ) { handlePlusButtonClick( rowIndex ); } );
            rawComponent->setResetButtonClickCallback( [this]( int rowIndex ) { handleResetButtonClick( rowIndex ); } );
            double offsetValue = settings.cvclib.getOffset( i );
            rawComponent->setOffsetValue( offsetValue );
        }

        apply.onClick = [&]() {
            applyCalibrationValues();
        };

        goback.onClick = [&]() {
            closeScreen();
        };
    }

    void CVClib::resized()
    {
        auto b           = getBounds();
        auto area        = getLocalBounds().reduced( 10 );
        int  totalHeight = ( 80 * rawComponents.size() ) + ( 10 * ( rawComponents.size() - 1 ) ); // Total height of all rows including spacing
        int  startY      = ( getHeight() - totalHeight ) / 2; // Calculate starting Y position to center rows

        for( auto* rawComponent : rawComponents )
        {
            rawComponent->setBounds( area.removeFromTop( 100 ).withY( startY ).reduced( 10 ) );
            startY += 90; // Move to next row position (row height + spacing)
        }

        int buttonWidth  = 120;
        int buttonHeight = 44;

        header.setBounds( b.removeFromTop( ui::layout::header.getHeight() ) );
        goback.setBounds( 10, getHeight() - buttonHeight - 5, buttonWidth, buttonHeight );
        apply.setBounds( ( getWidth() - buttonWidth - 15 ), getHeight() - buttonHeight - 5, buttonWidth, buttonHeight );
    }

    void CVClib::handleCommand( CPktMgr::HWMsg_t& )
    {
    }

    void CVClib::handleMinusButtonClick( int rowIndex )
    {
        DBG( "Minus button clicked in row: " << rowIndex );
        auto*  component = rawComponents[rowIndex];
        double newValue  = component->getOffsetValue() - 1.0; // Decrease offset by 1.0
        settings.cvclib.setOffset( rowIndex, newValue );

        mt::CV::getCVInterface().calibrateCV( rowIndex, newValue );
        component->setOffsetValue( newValue );
    }

    void CVClib::handlePlusButtonClick( int rowIndex )
    {
        DBG( "Plus button clicked in row: " << rowIndex );
        auto*  component = rawComponents[rowIndex];
        double newValue  = component->getOffsetValue() + 1.0; // Increase offset by 1.0
        component->setOffsetValue( newValue );

        settings.cvclib.setOffset( rowIndex, newValue );
        mt::CV::getCVInterface().calibrateCV( rowIndex, newValue );
    }

    void CVClib::handleResetButtonClick( int rowIndex )
    {
        DBG( "Reset button clicked in row: " << rowIndex );
        auto* component = rawComponents[rowIndex];
        component->setOffsetValue( 0 );
        settings.cvclib.setOffset( rowIndex, 0 );
        mt::CV::getCVInterface().calibrateCV( rowIndex, 0 );
    }

    void CVClib::applyCalibrationValues()
    {
        std::array<uint16_t, 6> calib;
        calib[0] = 0;
        for( int i = 0; i < rawComponents.size(); ++i )
        {
            double offsetValue = rawComponents[i]->getOffsetValue();
            settings.cvclib.setOffset( i, offsetValue );
            calib[i + 1] = static_cast<uint16_t>( offsetValue ); // Assuming offset values are uint16_t-compatible
        }
        calib[5] = 4095;
        // Apply calibration values to hardware
        mt::CV::getCVInterface().applyCalibration( calib );
    }

}
