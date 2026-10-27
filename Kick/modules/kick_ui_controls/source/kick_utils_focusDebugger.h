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

namespace mt
{
    struct FocusDebugger
        : public juce::Component
        , public juce::Timer
        , public juce::DeletedAtShutdown
    {
        juce::WeakReference<juce::Component> currently_focused_component;
        juce::Rectangle<int>                 outline_r;

        FocusDebugger()
        {
            startTimer( 50 );
            setInterceptsMouseClicks( false, false );
            setOpaque( false );
            setAlwaysOnTop( true );
        }

        void updateMe()
        {
            if( currently_focused_component )
            {
                auto r    = currently_focused_component->getScreenBounds().expanded( 2 );
                outline_r = r.withZeroOrigin();
                if( r.getWidth() < 300 ) r = r.withWidth( 300 );
                r = r.withHeight( r.getHeight() + 35 );
                setBounds( r );
                if( !isVisible() )
                {
                    if( !isOnDesktop() )
                    {
                        addToDesktop( juce::ComponentPeer::windowIsTemporary | juce::ComponentPeer::windowIgnoresMouseClicks | juce::ComponentPeer::windowIgnoresKeyPresses );
                    }
                    setVisible( true );
                }
                repaint();
            }
            else
            {
                setVisible( false );
            }
        }

        void paint( juce::Graphics& g ) override
        {
            juce::Colour outline_colour( 0xffff0000 );
            g.setColour( outline_colour );
            g.drawRect( outline_r );
            g.setFont( juce::Font( 14.f ) );
            juce::String     txt = "null";
            juce::Component* c   = currently_focused_component;
            if( c )
            {
                std::string classname = typeid( *c ).name(); //cxxDemangle(typeid(*c).name());
                classname             = classname.substr( 0, 30 );
                txt                   = "";
                txt << "class=" << classname << "; name=\"" << c->getName() << "\"; ID=\"" << c->getComponentID() << "\"";
                txt << "; focOrder=" << c->getExplicitFocusOrder();
            }
            g.setColour( juce::Colours::darkblue.withAlpha( 0.6f ) );
            g.fillRect( 0, outline_r.getBottom(), getWidth(), getHeight() - outline_r.getBottom() );
            g.setColour( juce::Colours::white );
            g.drawFittedText( txt, getLocalBounds(), juce::Justification::bottomLeft, 5 );
        }

        void updateCurrentFocusedComponent()
        {
            auto* c = juce::Component::getCurrentlyFocusedComponent();
            if( c != currently_focused_component ) { currently_focused_component = c; }
        }

        void timerCallback() override
        {
            updateCurrentFocusedComponent();
            updateMe();
        }
    };
} // namespace ui
