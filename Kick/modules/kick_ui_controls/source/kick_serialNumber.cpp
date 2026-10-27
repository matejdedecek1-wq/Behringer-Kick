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
    /// KICK Serial Number Label
    //////////////////////
    KickSerialNumber::KickSerialNumber( const juce::String& Name, const juce::String& Image )
    {
        setName( Name );
        setTitle( Name );

        // Don't need any mouse intercepts.
        setInterceptsMouseClicks( false, false );
        setWantsKeyboardFocus( false );

        // Default colours...
        setColour( textColourId, ui::Theme::textColour( 1 ) );
        setColour( textBackgroundColourId, ui::Theme::backgroundColour( 6 ) );
        setColour( imageBackgroundColourId, juce::Colour( 0xFF422F05 ) );
        setColour( outlineColourId, juce::Colour( 0x0 ) );

        setText( Name );
        setImage( Image );
    }

    void KickSerialNumber::setText( const juce::String& Text )
    {
        text = Text;
        repaint();
    }
    void KickSerialNumber::setFontProperties( int FontHeight, juce::Justification Just )
    {
        fontHeight    = FontHeight;
        justification = Just;
        repaint();
    };

    void KickSerialNumber::setColours( const juce::Colour& Text, const juce::Colour& TextBackgroud, const juce::Colour& ImageBackgroud, const juce::Colour& Outline )
    {
        setColour( textColourId, Text );
        setColour( textBackgroundColourId, TextBackgroud );
        setColour( imageBackgroundColourId, ImageBackgroud );
        setColour( outlineColourId, Outline );
        repaint();
    }

    void KickSerialNumber::setImage( const juce::String& Image )
    {
        if( Image.isNotEmpty() )
            d_ = ui::Theme::getDrawable( Image );
    }

    void KickSerialNumber::setImage( juce::Image Image )
    {
        d_ = std::make_unique<juce::DrawableImage>( Image );
        repaint();
    }

    void KickSerialNumber::setImageSize( int W, int H )
    {
        imageW = W;
        imageH = H;
        repaint();
    }

    void KickSerialNumber::setCornerRadius( float Radius )
    {
        radius = Radius;
        repaint();
    };

    void KickSerialNumber::setYProportions( int TextAreaHeight, int ImageAreaHeight )
    {
        textAreaH  = TextAreaHeight;
        imageAreaH = ImageAreaHeight;
        repaint();
    };

    void KickSerialNumber::paint( juce::Graphics& g )
    {
        auto b = getLocalBounds();
        if( b.isEmpty() )
            return;

        // Draw outline.
        juce::Path p;
        auto       bf = b.toFloat();
        if( !findColour( outlineColourId ).isTransparent() )
        {
            p.addRoundedRectangle( bf.getX(), bf.getY(), bf.getWidth(), bf.getHeight(), radius, radius, true, false, true, false );
            g.setColour( findColour( outlineColourId ) );
            g.strokePath( p, juce::PathStrokeType( 1.f ) );
            bf.reduce( 1.f, 1.f );
        }

        // Draw text background.
        if( !findColour( textBackgroundColourId ).isTransparent() )
        {
            p.clear();
            p.addRoundedRectangle( bf.getX(), bf.getY(), bf.getWidth(), textAreaH, radius, radius, true, false, false, false );
            g.setColour( findColour( textBackgroundColourId ) );
            g.fillPath( p );
        }

        // Draw text.
        mt::utils::renderText( g, text, p.getBounds().toNearestInt(), findColour( textColourId ), justification, fontHeight );

        // Draw rest of the background.
        if( !findColour( imageBackgroundColourId ).isTransparent() )
        {
            p.clear();
            p.addRoundedRectangle( bf.getX(), bf.getY() + textAreaH, bf.getWidth(), bf.getHeight() - textAreaH, radius, radius, false, false, true, false );
            g.setColour( findColour( imageBackgroundColourId ) );
            g.fillPath( p );
        }

        // Draw image.
        if( d_ )
        {
            juce::Rectangle<float> imageBounds{ (float)imageW, (float)imageH };
            auto                   imgB = p.getBounds();
            imageBounds.setCentre( imgB.getCentre() );
            d_->drawWithin( g, imageBounds, juce::Justification::centred, 1.0f );
        }
    }

}
