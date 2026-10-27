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
    class KickSerialNumber : public juce::Component
    {
    public:
        explicit KickSerialNumber( const juce::String& Name, const juce::String& Image = "" );

        enum ColourIds
        {
            textColourId = 0xAA000001, /**< The colour for the text. */
            textBackgroundColourId, /**< The background colour to fill behind text label. */
            imageBackgroundColourId, /**< The background colour to fill behind image. */
            outlineColourId, /**< An optional colour to use to draw a border around the label. Leave this transparent to not have an outline. */
        };

        void setText( const juce::String& Text );
        void setFontProperties( int FontHeight, juce::Justification Just = juce::Justification::centred );

        void setImageSize( int W, int H );
        void setImage( const juce::String& Image );
        void setImage( juce::Image Image );

        void setColours( const juce::Colour& Text, const juce::Colour& TextBackgroud, const juce::Colour& ImageBackgroud, const juce::Colour& Outline = juce::Colours::transparentBlack );
        void setCornerRadius( float Radius );
        void setYProportions( int TextAreaHeight, int ImageAreaHeight );

    private:
        juce::String                    text;
        ui::KickImage                   image;
        std::unique_ptr<juce::Drawable> d_;
        juce::Justification             justification{ juce::Justification::centred };
        float                           radius{ 5.f };
        int                             fontHeight{ 26 };
        int                             textAreaH{ 54 };
        int                             imageW{ 25 };
        int                             imageH{ 25 };
        int                             imageAreaH{ 54 };

        void paint( juce::Graphics& g ) override;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( KickSerialNumber )
    };
}
