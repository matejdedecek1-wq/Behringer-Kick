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

#ifndef KICK_LAYOUT_H
    #define KICK_LAYOUT_H

namespace ui
{
    // Application Width/Height...
    enum
    {
        appWidth  = 1024,
        appHeight = 600
    };

    // Screen Resolution...
    enum
    {
        screenWidth  = 1024,
        screenHeight = 600
    };

    // Scale Factor (Screen to Application ration)
    template<typename T>
    constexpr T _sX( const float v ) noexcept
    {
        return (T)( (float)v * (float)screenWidth / (float)appWidth );
    }

    template<typename T>
    constexpr T _sY( const float v ) noexcept
    {
        return (T)( (float)v * (float)screenHeight / (float)appHeight );
    }

    template<typename T>
    constexpr T _sX( const int v ) noexcept
    {
        return (T)( (float)v * (float)screenWidth / (float)appWidth );
    }

    template<typename T>
    constexpr T _sY( const int v ) noexcept
    {
        return (T)( (float)v * (float)screenHeight / (float)appHeight );
    }

    // Int/Float scale X and Y routines.
    #define _scaledX( v )  _sX<int>( v )
    #define _scaledY( v )  _sY<int>( v )
    #define _scaledXF( v ) _sX<float>( v )
    #define _scaledYF( v ) _sY<float>( v )

    typedef struct padding
    {
        int top{ 10 }, bottom{ 10 }, left{ 10 }, right{ 10 };
    } Padding;

    namespace layout
    {
        constexpr int getAppWidth() { return appWidth; }
        constexpr int getAppHeight() { return appHeight; }

        constexpr int scrollBarThickness{ _scaledX( 4 ) };
        constexpr int tabRowWidth{ _scaledX( 920 ) };
        constexpr int tabRowHeight{ _scaledY( 112 ) };
        constexpr int tabRowSpacing{ _scaledX( 4 ) };

        // Dialog window size...
        const juce::Rectangle<int> dialog{ ( 0 ), ( 0 ), _scaledX( appWidth * 0.635f ), _scaledY( appHeight * 0.867f ) };
        // Numeric component size...
        const juce::Rectangle<int> numpad{ ( 0 ), ( 0 ), _scaledX( 300.0F ), _scaledY( 200.0f ) };
        // Warning window size...
        const juce::Rectangle<int> warningPopup{ ( 0 ), ( 0 ), _scaledX( 600.0F ), _scaledY( 360.0f ) };
        // Keyboard window size...
        const juce::Rectangle<int> keyBoardPopup{ ( 0 ), ( 0 ), _scaledX( 800.0F ), _scaledY( 520.0f ) };

        // Complete Screen
        const juce::Rectangle<int> screen{ ( 0 ), ( 0 ), _scaledX( appWidth ), _scaledY( appHeight ) };
        const juce::Rectangle<int> screenExcludingHeader{ _scaledX( 0 ), _scaledY( 80 ), _scaledX( appWidth ), _scaledY( appHeight ) - _scaledY( 80 ) };
        const juce::Rectangle<int> header{ _scaledX( 0 ), _scaledY( 0 ), _scaledX( appWidth ), _scaledY( 76 ) };

        // Tabbed
        const juce::Rectangle<int> tabWindow{ _scaledX( 0 ), _scaledY( 80 ), _scaledX( appWidth ), _scaledY( ( appHeight - 140 ) ) };
        const juce::Rectangle<int> tabBar{ _scaledX( 0 ), _scaledY( 80 ), _scaledX( appWidth ), _scaledY( 54 ) };
        const juce::Rectangle<int> tabArea{ _scaledX( 0 ), _scaledY( 134 ), _scaledX( appWidth ), _scaledY( ( appHeight - 174 ) ) };
        const juce::Rectangle<int> tabRow{ _scaledX( 0 ), _scaledY( 0 ), _scaledX( appWidth - 24 ), _scaledY( 110 ) };

        const juce::Rectangle<int> gridCtrlbar{ _scaledX( 0 ), _scaledY( 80 ), _scaledX( 970 ), _scaledY( 30 ) };
        const juce::Rectangle<int> transportbar{ _scaledX( 450 ), _scaledY( 85 ), _scaledX( appWidth - 450 ), _scaledY( 34 ) };

        // Status Bar
        const juce::Rectangle<int> statusBar{ _scaledX( 0 ), _scaledY( ( appHeight - 64 ) ), _scaledX( appWidth ), _scaledY( 64 ) };
        // Title
        const juce::Rectangle<int> title{ _scaledX( 0 ), _scaledY( 0 ), _scaledX( appWidth ), _scaledY( 20 ) };

        // Margins...
        // Left, Top, Right, Bottom
        const juce::Rectangle<int> tabMargin{ _scaledX( 10 ), _scaledY( 6 ), _scaledX( 10 ), _scaledY( 10 ) };

    }
}
#endif // KICK_LAYOUT_H
