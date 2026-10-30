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

namespace mt
{

    struct bubbleNotification : public juce::BubbleMessageComponent
    {
        bubbleNotification()
            : juce::BubbleMessageComponent()
        {
            // Positions are on the 1024x600 screen: show it on the app's window (ui::overlayRoot(),
            // which on the unit is the whole display), or on the desktop when none is registered.
            if( auto* root = ui::overlayRoot().getComponent() )
            {
                setAlwaysOnTop( true );
                root->addChildComponent( this );
            }
            else
                addToDesktop( 0 );
            setAllowedPlacement( BubbleMessageComponent::above );
        }

        void paint( juce::Graphics& g ) override
        {
            auto bb       = getLocalBounds();
            auto gradient = juce::ColourGradient::vertical( ui::Theme::backgroundColour( 1 ), ui::Theme::backgroundColour( 4 ), bb );

            g.setGradientFill( gradient );
            g.fillAll();

            g.setFont( 26 );
            paintContent( g, bb.getWidth(), bb.getHeight() );
        }

        void show( const juce::AttributedString& Str, juce::Rectangle<int> Bounds = ui::layout::statusBar, int TimeInMS = 1500 )
        {
            // On the window it is deleted once it has faded, rather than left there.
            showAt( Bounds, Str, TimeInMS, true, getParentComponent() != nullptr );
            setPosition( Bounds.getCentre(), -1 );
        }

        void getContentSize( int& w, int& h ) override
        {
            w = ui::layout::screen.getWidth();
            h = 60; //ui::layout::screen.getWidth();
        }
    };

} // namespace mt
