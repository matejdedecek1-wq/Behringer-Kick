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
    /// KICK Separator
    //////////////////////

    KickSeparator::KickSeparator( juce::Colour Foreground, juce::Colour Background, Orientation O )
        : dir( O ), foreground( Foreground ), background( Background )
    {
        this->setInterceptsMouseClicks( false, false );
    }

    void KickSeparator::setOrientation( Orientation O )
    {
        dir = O;
        this->repaint();
    }

    void KickSeparator::setColours( juce::Colour Foreground, juce::Colour Background )
    {
        this->foreground = Foreground;
        this->background = Background;
        this->repaint();
    }

    void KickSeparator::setAlphaMultiplier( float a )
    {
        this->alphaFactor = a;
        this->repaint();
    }

    void KickSeparator::setThickness( int Thickness )
    {
        this->thickNess = Thickness;
        this->repaint();
    }

    void KickSeparator::paint( juce::Graphics& g )
    {
        // Background...
        g.fillAll( background.withAlpha( this->alphaFactor ) );

        juce::Rectangle<int> r;
        auto                 b = getLocalBounds();
        if( Horizantal == dir )
            r.setBounds( 0, 0, b.getWidth(), thickNess );
        else
            r.setBounds( 0, 0, thickNess, b.getHeight() );

        r.setCentre( b.getCentre() );

        // Foreground...
        g.setColour( foreground.withAlpha( this->alphaFactor ) );
        g.fillRect( r );
    }
}
