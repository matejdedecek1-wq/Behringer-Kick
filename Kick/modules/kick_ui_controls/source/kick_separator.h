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

namespace ui
{
    class KickSeparator final : public juce::Component
    {
    public:
        enum Orientation
        {
            Vertical,
            Horizantal
        };

        explicit KickSeparator( juce::Colour Foreground = { ui::Theme::foregroundColour( 1 ) }, juce::Colour Background = { juce::Colours::transparentBlack }, Orientation O = Vertical );

        void setOrientation( Orientation O );
        void setColours( juce::Colour Foreground, juce::Colour Background = { ui::Theme::backgroundColour( 9 ) } );
        void setAlphaMultiplier( float a );
        void setThickness( int Thickness );

    private:
        Orientation  dir;
        juce::Colour foreground, background;
        int          thickNess{ 1 };
        float        alphaFactor = 0.75f;

        void paint( juce::Graphics& g ) override;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( KickSeparator )
    };
}
