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

namespace ui
{
#define KICK_COMMON_TITLE_VALUE_SETTINGS()                    \
    showTitle( true );                                        \
    setTitleFontHeight( 16 );                                 \
    setValueFontHeight( 18 );                                 \
    setTitleJustification( juce::Justification::centredTop ); \
    setValueJustification( juce::Justification::centredBottom );

    struct KickLevelSlider : public KickSlider
    {
        explicit KickLevelSlider( const juce::String& Title = "LEVEL" )
            : KickSlider( Title, juce::Slider::LinearBar, juce::Slider::TextBoxBelow )
        {
            setRange( MIDI::MIN, MIDI::MAX, MIDI::STEP );
            setNumDecimalPlacesToDisplay( 0 );
            KICK_COMMON_TITLE_VALUE_SETTINGS();
        }

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( KickLevelSlider )
    };

    struct KickSemiSlider : public KickSlider
    {
        explicit KickSemiSlider( const juce::String& Title = "SEMI" )
            : KickSlider( Title, juce::Slider::LinearBar, juce::Slider::TextBoxBelow )
        {
            setRange( SEMI::MIN, SEMI::MAX, SEMI::STEP );
            setNumDecimalPlacesToDisplay( 0 );
            setCentered( true );
            KICK_COMMON_TITLE_VALUE_SETTINGS();
        }

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( KickSemiSlider )
    };

    struct KickFineSlider : public KickSlider
    {
        explicit KickFineSlider( const juce::String& Title = "FINE" )
            : KickSlider( Title, juce::Slider::LinearBar, juce::Slider::TextBoxBelow )
        {
            setRange( FINE::MIN, FINE::MAX, FINE::STEP );
            setNumDecimalPlacesToDisplay( 0 );
            setCentered( true );
            KICK_COMMON_TITLE_VALUE_SETTINGS();
        }

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( KickFineSlider )
    };

    struct KickPanRotary : public KickRotary
    {
        explicit KickPanRotary( const juce::String& Title = "PAN" )
            : KickRotary( Title, juce::Slider::TextBoxBelow, true )
        {
            setRange( PAN::MIN, PAN::MAX, PAN::STEP );
            setCentered( true );
            KICK_COMMON_TITLE_VALUE_SETTINGS();
        }

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( KickPanRotary )
    };

#undef KICK_COMMON_TITLE_VALUE_SETTINGS
}
