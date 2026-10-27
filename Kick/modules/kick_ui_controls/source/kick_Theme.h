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

#ifndef THEME_H
    #define THEME_H

namespace ui
{
    static sigslot::signal<> themeChanged;

    enum FONT
    {
        XS = 100,
        S,
        M,
        L,
        XL,
        MAX
    };

    struct Theme
    {
        // Theme control API's.
        static juce::String getCurrentTheme() noexcept;
        static void         loadTheme( const juce::String& ThemeDir, bool TriggerNotification = true );

        // Font API's
        int font( FONT Font );

        // Colour API's
        static juce::Colour padButtonColour( int ID );

        static juce::Colour titleColour( int ID = 1 );
        static juce::Colour textColour( int ID = 1 );

        static juce::Colour backgroundColour( int ID = 1 );
        static juce::Colour foregroundColour( int ID = 1 );
        static juce::Colour selectionColour( int ID = 1 );
        static juce::Colour outlineColour( int ID = 1 );
        static juce::Colour highlightColour( int ID = 1 );

        static juce::Colour meterColour( int ID = 1 );
        static juce::Colour waveformColour( int ID = 1 );
        static juce::Colour noteColour( int velocity );
        static juce::Colour gridColour( int ID = 1 );

        static juce::Colour multiSelectTabActiveColour();
        static juce::Colour multiSelectTabInActiveColour();

        static juce::Colour virtualKeyboardTextColour();
        static juce::Colour virtualKeyboardBkColour();

        static juce::Colour XYFXCellColour();
        static juce::Colour XYFXCellSelectColour( int alpha );
        static juce::Colour XYFXAxisColour();

        // Asset API's
        static juce::File getAssetsDirectory();

        static juce::File  getAsset( const juce::String& File );
        static juce::Image getImage( const juce::String& Image );

        static juce::File                      getThemeImage( const juce::String& IMGName );
        static std::unique_ptr<juce::Drawable> getDrawable( const juce::String& IMGName );
        static std::unique_ptr<juce::Drawable> getDrawable( juce::File IMGFile );
    };

}
#endif // THEME_H
