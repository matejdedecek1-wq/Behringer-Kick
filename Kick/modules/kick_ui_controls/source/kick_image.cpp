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
    //////////////////////
    /// KICK Image
    //////////////////////

    KickImage::KickImage( const juce::String& Title, const juce::String& Image, bool ShowTitle, bool ShowBorder )
        : mt::MTProperties( this )
    {
        setName( Title );
        setTitle( Title );

        // Don't need any mouse intercepts...
        setInterceptsMouseClicks( false, false );

        // Defaults...
        showBorder( ShowBorder );
        showTitle( ShowTitle );

        setTitleColour( ui::Theme::textColour( 1 ) );
        setBackgroundColour( juce::Colours::transparentBlack );
        setBorderColour( ui::Theme::outlineColour( 1 ) );

        setImage( Image );
    }

    void KickImage::setImage( const juce::String& Image )
    {
        if( Image.isNotEmpty() )
            d_ = ui::Theme::getDrawable( Image );

        repaint();
    }

    void KickImage::setImage( juce::Image Image )
    {
        d_ = std::make_unique<juce::DrawableImage>( Image );
        repaint();
    }

    void KickImage::setColours( const juce::Colour& Text, const juce::Colour& Backgroud, const juce::Colour& Outline )
    {
        setTitleColour( Text );
        setBackgroundColour( Backgroud );
        setBorderColour( Outline );
    }

    void KickImage::paint( juce::Graphics& g )
    {
        auto b = getLocalBounds();
        if( b.isEmpty() )
            return;

        // Bacnground...
        mt::utils::fillRoundedRect( g, b, getBackgroundColour(), getCornerRadius() );

        // Draw Border.
        if( isShowingBorder() )
        {
            mt::utils::renderRoundedRect( g, b, getBorderColour(), getBorderThickness(), getCornerRadius() );
            b.reduce( getBorderThickness(), getBorderThickness() );
        }

        // Draw Title text.
        if( isShowingTitle() )
        {
            auto bTitle = b.removeFromTop( (int)( b.proportionOfHeight( 0.35 ) ) );
            mt::utils::renderText( g, getTitle(), bTitle, getTitleColour(), getTitleJustification(), getTitleFontHeight() );
        }

        // Draw Image...
        if( d_ )
        {
            d_->drawWithin( g, b.toFloat(), juce::RectanglePlacement::centred, 1.0f );
        }
    }

}
