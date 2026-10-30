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
    /// Covers a screen, or part of one, whose job is to drive a device feature this system does
    /// not have (mt::hw::isAvailable, modules/hardware/source/kick_HardwareProbe.h): Wi-Fi,
    /// Ethernet, CV / gate, firmware update, the resource meters.
    ///
    /// A screen creates it only when its feature is unavailable, so on a unit it never exists.
    /// It covers `Area` of its parent (the whole parent when empty), swallows touches so the
    /// controls underneath cannot be used, reads "<label>: not available on this system" and logs
    /// "FeatureUnavailable: <feature> shown" / "hidden" as it appears and goes.
    class FeatureUnavailable : public juce::Component
        , private juce::Timer
    {
    public:
        /// Feature: the probe's name ("wifi"), used in the log. Label: what the screen shows ("Wi-Fi").
        FeatureUnavailable( juce::String Feature, juce::String Label, juce::String Reason, juce::Rectangle<int> Area = {} )
            : feature( std::move( Feature ) ), label( std::move( Label ) ), reason( std::move( Reason ) ), area( Area )
        {
            setTitle( label + ": not available on this system" );
            setInterceptsMouseClicks( true, false );
            setAlwaysOnTop( true );
            startTimer( 250 ); // a parent screen being shown does not notify its children
        }

        ~FeatureUnavailable() override
        {
            if( wasShowing )
                juce::Logger::writeToLog( "FeatureUnavailable: " + feature + " hidden" );
        }

        /// Creates the panel in Slot and adds it to Parent when Available is false; removes it when
        /// true. Screens call it whenever they are laid out or shown.
        static void update( std::unique_ptr<FeatureUnavailable>& Slot, juce::Component& Parent, bool Available,
                            const juce::String& Feature, const juce::String& Label, const juce::String& Reason, juce::Rectangle<int> Area = {} )
        {
            if( Available )
            {
                Slot = nullptr;
                return;
            }
            if( Slot == nullptr )
                Slot = std::make_unique<FeatureUnavailable>( Feature, Label, Reason, Area );
            if( Slot->getParentComponent() != &Parent )
                Parent.addAndMakeVisible( *Slot );
            Slot->toFront( false );
            Slot->fit();
        }

        void paint( juce::Graphics& g ) override
        {
            g.fillAll( ui::Theme::backgroundColour( 4 ).withAlpha( 0.94f ) );

            auto box = getLocalBounds().toFloat().withSizeKeepingCentre( juce::jmin( 640.f, getWidth() - 40.f ), juce::jmin( 190.f, getHeight() - 20.f ) );
            g.setColour( ui::Theme::backgroundColour( 1 ) );
            g.fillRoundedRectangle( box, 8.f );
            g.setColour( ui::Theme::titleColour( 1 ) );
            g.drawRoundedRectangle( box, 8.f, 1.5f );

            const float k    = juce::jlimit( 0.55f, 1.f, box.getHeight() / 190.f ); // smaller areas, smaller type
            auto        text = box.reduced( 24.f * k, 18.f * k );
            g.setFont( juce::Font( 26.f * k, juce::Font::bold ) );
            g.drawFittedText( label, text.removeFromTop( 34.f * k ).toNearestInt(), juce::Justification::centred, 1 );
            g.setColour( ui::Theme::textColour( 1 ) );
            g.setFont( juce::Font( 20.f * k ) );
            g.drawFittedText( "Not available on this system", text.removeFromTop( 30.f * k ).toNearestInt(), juce::Justification::centred, 1 );
            g.setColour( ui::Theme::textColour( 1 ).withAlpha( 0.75f ) );
            g.setFont( juce::Font( 16.f * k ) );
            g.drawFittedText( reason, text.toNearestInt(), juce::Justification::centred, 5, 0.9f );
        }

        void parentHierarchyChanged() override { fit(); }
        void parentSizeChanged() override { fit(); }

        // Swallow touches: nothing under this panel may reach the code that drives the feature.
        void mouseDown( const juce::MouseEvent& ) override {}

    private:
        void fit()
        {
            if( auto* p = getParentComponent() )
                setBounds( area.isEmpty() ? p->getLocalBounds() : area );
        }

        void timerCallback() override
        {
            const bool showing = isShowing();
            if( showing != wasShowing )
                juce::Logger::writeToLog( "FeatureUnavailable: " + feature + ( showing ? " shown" : " hidden" ) );
            wasShowing = showing;
        }

        juce::String         feature, label, reason;
        juce::Rectangle<int> area;
        bool                 wasShowing{ false };
    };
}
