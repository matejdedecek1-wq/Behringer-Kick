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
namespace kick_ui
{
    class KnobGrid : public juce::Component
    {
    public:
        KnobGrid();
        ~KnobGrid();

        std::function<void( int )>              onknobSelection;
        std::function<int( juce::String, int )> parameterSelection;
        std::function<void( bool )>             isPad;

        void resized() override;
        void paint( juce::Graphics& g ) override;
        void setInfoText2( juce::String txt );
        void labelBounds( bool isScreenClicked );
        int  getState( int knobNum );
        void knobCommand( int knob, int value );
        void labelInfo( int knobnum, juce::String name );
        void setValues( int knobnum, int value );
        void setPadState( bool State );
        void setPadIndex( int bankIndex, int padIndex );
        void setIndex( int index );

    private:
        std::vector<std::unique_ptr<ui::KickRotary>> qlinkKnobs;
        mt::KickKnobControl&                         knob{ mt::getSettings().knobControl };

        bool          initialized   = false;
        bool          screenClicked = true;
        int           selectedRow, selectedColumn{ -1 };
        int           state{ 0 };
        bool          isPadState{ false };
        int           bankNum{ -1 }, padNum{ -1 }, indexNum{ -1 };
        juce::String  infoText;
        ui::KickLabel label;
        ui::KickLabel knobdata;
        ui::KickLabel knobLabel;
    };
}
