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

    class SettingsCV : public SettingsInterface
    {
    public:
        SettingsCV();

        // Virtual functions
        void refresh() override;
        void setupHeader( kick_ui::KickModeHeader& Hdr ) override;
        void handleCommand( CPktMgr::HWMsg_t& ) override;

    private:
    private:
        mt::KickSettings& settings{ mt::getSettings() };
        ui::KickListBox   cv0{ ui::KickListBox::withTitleBar, "CV-1 Start Note", mt::ScaleAndNotes::noteNames(), 0, 8 };
        ui::KickListBox   cv1{ ui::KickListBox::withTitleBar, "CV-2 Start Note", mt::ScaleAndNotes::noteNames(), 0, 8 };
        ui::KickListBox   cv2{ ui::KickListBox::withTitleBar, "CV-3 Start Note", mt::ScaleAndNotes::noteNames(), 0, 8 };
        ui::KickListBox   cv3{ ui::KickListBox::withTitleBar, "CV-4 Start Note", mt::ScaleAndNotes::noteNames(), 0, 8 };
        ui::KickListBox   cv4{ ui::KickListBox::withTitleBar, "CV-5 Start Note", mt::ScaleAndNotes::noteNames(), 0, 8 };
        ui::KickListBox   cv5{ ui::KickListBox::withTitleBar, "CV-6 Start Note", mt::ScaleAndNotes::noteNames(), 0, 8 };
        ui::KickListBox   cv6{ ui::KickListBox::withTitleBar, "CV-7 Start Note", mt::ScaleAndNotes::noteNames(), 0, 8 };
        ui::KickListBox   cv7{ ui::KickListBox::withTitleBar, "CV-8 Start Note", mt::ScaleAndNotes::noteNames(), 0, 8 };

        ui::KickButton calibrate{ "CV CALIBRATION" };
        ui::KickLabel  calMsg{ "CalMsg" };
    };

    class CVClibRawComponent : public juce::Component
    {
    public:
        CVClibRawComponent( int index );
        using ButtonClickCallback = std::function<void( int )>;

        void   setMinusButtonClickCallback( ButtonClickCallback callback ) { onMinusClicked = callback; }
        void   setPlusButtonClickCallback( ButtonClickCallback callback ) { onPlusClicked = callback; }
        void   setResetButtonClickCallback( ButtonClickCallback callback ) { onResetClicked = callback; }
        double getOffsetValue() const { return Clib; }
        void   setOffsetValue( int value )
        {
            Clib = value;
            offset.setText( juce::String( Clib ), juce::dontSendNotification );
        }

    private:
        void           paint( juce::Graphics& g ) override;
        void           resized() override;
        int            rowIndex;
        ui::KickLabel  expectedVoltage{ "" };
        ui::KickButton adjustmentMinus{ "-" };
        ui::KickButton adjustmentPlus{ "+" };
        ui::KickButton resetBtn{ "Reset" };
        ui::KickLabel  offset{ "" };
        ui::KickLabel  step{ "STEP" };
        ui::KickLabel  expVelLabel{ "EXPECTED VOLTAGE" };
        ui::KickLabel  adjLabel{ "ADJUSTMENT" };

        ButtonClickCallback onMinusClicked;
        ButtonClickCallback onPlusClicked;
        ButtonClickCallback onResetClicked;
        int                 Clib = 0;

    public:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( CVClibRawComponent )
    };

    class CVClib : public kick_ui::IScreen
        , public juce::Component
    {
    public:
        CVClib();
        ~CVClib()
        {}

        void paint( juce::Graphics& g ) override
        {
            mt::utils::fillRoundedRect( g, centreBounds, ui::Theme::backgroundColour( 1 ), 6.0f, false, false, true, true );

            //Draw Border
            g.setColour( ui::Theme::outlineColour( 3 ) );
            g.drawRoundedRectangle( 10, 85, 1002, 390, 6.0f, 1.0f );
        }

        void resized() override;
        void handleCommand( CPktMgr::HWMsg_t& );
        void handleMinusButtonClick( int rowIndex );
        void handlePlusButtonClick( int rowIndex );
        void handleResetButtonClick( int rowIndex );

    private:
        void                                 applyCalibrationValues();
        mt::KickSettings&                    settings{ mt::getSettings() };
        juce::OwnedArray<CVClibRawComponent> rawComponents;
        kick_ui::KickModeHeader              header{ "", "Icon_SettingsScreen" };
        juce::Rectangle<int>                 centreBounds = { 10, 85, 1002, 390 };
        ui::KickButton                       apply{ "APPLY" };
        ui::KickButton                       goback{ "GO BACK" };

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( CVClib )
    };
}
