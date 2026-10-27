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
    class fsPopup;

    // Thread Pool
    static juce::ThreadPool threadPool{ 5 };

    namespace utils
    {
        ///
        /// JUCE component helpers...
        ///

        template<typename... Args>
        void addChildComponents( juce::Component* parent, Args&&... args ) { ( parent->addChildComponent( args ), ... ); }
        template<typename... Args>
        void removeChildComponents( juce::Component* parent, Args&&... args ) { ( parent->removeChildComponent( args ), ... ); }

        template<typename... Args>
        void addAndMakeVisible( juce::Component* parent, Args&&... args ) { ( parent->addAndMakeVisible( args ), ... ); }

        template<typename... Args>
        void setRadioGroupID( int groupId, Args&&... args ) { ( args.setRadioGroupId( groupId ), ... ); }

        template<typename... Args>
        void makeVisible( Args&&... args ) { ( args.setVisible( true ), ... ); }
        template<typename... Args>
        void makeHidden( Args&&... args ) { ( args.setVisible( false ), ... ); }

        template<typename... Args>
        void makeOpaque( Args&&... args ) { ( args.setOpaque( true ), ... ); }

        template<typename... Args>
        void addListener( juce::Button::Listener* listener, Args&&... args ) { ( args.addListener( listener ), ... ); }
        template<typename... Args>
        void removeListener( juce::Button::Listener* listener, Args&&... args ) { ( args.removeListener( listener ), ... ); }

        template<typename... Args>
        void setEnabled( bool Enabled, Args&&... args ) { ( args.setEnabled( Enabled ), ... ); }

        template<typename... Args>
        void disableFocus( Args&&... args ) { ( args.setWantsKeyboardFocus( false ), ... ); }

        template<typename... Args>
        void disableMouseIntercepts( Args&&... args ) { ( args.setInterceptsMouseClicks( false, false ), ... ); }
        template<typename... Args>
        void enableMouseIntercepts( Args&&... args ) { ( args.setInterceptsMouseClicks( true, true ), ... ); }

        template<typename... Args>
        void disableMouseInterceptsAllowOnChildren( Args&&... args ) { ( args.setInterceptsMouseClicks( false, true ), ... ); }
        template<typename... Args>
        void enableMouseInterceptsDontAllowOnChildren( Args&&... args ) { ( args.setInterceptsMouseClicks( true, false ), ... ); }

        template<typename... Args>
        void disableMouseInterceptsAndFocus( Args&&... args )
        {
            ( args.setInterceptsMouseClicks( false, false ), ... );
            ( args.setWantsKeyboardFocus( false ), ... );
        }

        template<typename... Args>
        void setJustificationType( juce::Justification Just, Args&&... args ) { ( args.setJustificationType( Just ), ... ); }
        template<typename... Args>
        void setClickingTogglesState( bool State, Args&&... args ) { ( args.setClickingTogglesState( State ), ... ); }

        template<typename... Args>
        void setValueFontHeight( float FontHeight, Args&&... args ) { ( args.setValueFontHeight( FontHeight ), ... ); }
        template<typename... Args>
        void setTitleFontHeight( float FontHeight, Args&&... args ) { ( args.setTitleFontHeight( FontHeight ), ... ); }

        template<typename... Args>
        void setValueJustification( juce::Justification Just, Args&&... args ) { ( args.setValueJustification( Just ), ... ); }
        template<typename... Args>
        void setTitleJustification( juce::Justification Just, Args&&... args ) { ( args.setTitleJustification( Just ), ... ); }

        template<typename... Args>
        void setValueColour( juce::Colour Colour, Args&&... args ) { ( args.setValueColour( Colour ), ... ); }
        template<typename... Args>
        void setTitleColour( juce::Colour Colour, Args&&... args ) { ( args.setTitleColour( Colour ), ... ); }

        template<typename... Args>
        void setValueBackgroundColour( juce::Colour Colour, Args&&... args ) { ( args.setValueBackgroundColour( Colour ), ... ); }
        template<typename... Args>
        void setTitleBackgroundColour( juce::Colour Colour, Args&&... args ) { ( args.setTitleBackgroundColour( Colour ), ... ); }

        template<typename... Args>
        void setTitleProportion( float Proportion, Args&&... args ) { ( args.setTitleProportion( Proportion ), ... ); }
        template<typename... Args>
        void setValueProportion( float Proportion, Args&&... args ) { ( args.setTitleProportion( Proportion ), ... ); }

        template<typename... Args>
        void setPlacementFlag( int Flags, Args&&... args ) { ( args.setPlacementFlag( Flags ), ... ); }

        template<typename... Args>
        void setFontHeight( float FontHeight, Args&&... args ) { ( args.setFontHeight( FontHeight ), ... ); }

        template<typename... Args>
        void setShowTitle( bool Show, Args&&... args ) { ( args.showTitle( Show ), ... ); }

        ///
        /// Bank...
        ///

        int getBankOffset( juce::String Bank );
        int getBankIndex( int Index );
        int getPadIndexInBank( int ID );

        juce::String getBank( int Index );
        juce::String getPad( int Index );

        ///
        /// Pop-ups and dialogs...
        ///

        void showDialog( const juce::String& Title, juce::Component* Comp, std::function<void( int )> Callback,
                         juce::Rectangle<int> bounds   = ui::layout::dialog,
                         juce::Colour backgroundColour = ui::Theme::backgroundColour( 1 ), juce::Colour titleTextColour = ui::Theme::titleColour( 1 ), juce::Colour titleBackgroundColour = ui::Theme::backgroundColour( 2 ) );

        void showOptions( const juce::String& Title, juce::Component* Comp, std::function<void( int )> Callback,
                          juce::Rectangle<int> bounds   = ui::layout::dialog,
                          juce::Colour backgroundColour = ui::Theme::backgroundColour( 1 ), juce::Colour titleTextColour = ui::Theme::titleColour( 1 ), juce::Colour titleBackgroundColour = ui::Theme::backgroundColour( 2 ), juce::Colour buttonBackgroundColour = ui::Theme::backgroundColour( 1 ) );

        void showNotification( const juce::String& Msg );

        fsPopup* showPopup( juce::Component* Comp, juce::Rectangle<int> Bounds, std::function<void( int )> Callback );

        void exitDialog( juce::Component* Comp, int exitCode );

        ///
        /// Graphic Utilities...
        ///

        void renderRoundedRect( juce::Graphics& G, const juce::Rectangle<int>& Bounds, const juce::Colour& Color, int Thickness = 1, float Corner = 5.f, bool curveTopLeft = true, bool curveTopRight = true, bool curveBottomLeft = true, bool curveBottomRight = true );
        void fillRoundedRect( juce::Graphics& G, const juce::Rectangle<int>& Bounds, const juce::Colour& Color, float Corner = 5.f, bool curveTopLeft = true, bool curveTopRight = true, bool curveBottomLeft = true, bool curveBottomRight = true );
        void fillGradientRoundedRect( juce::Graphics& G, const juce::Rectangle<int>& Bounds, const juce::ColourGradient& Colour, float Corner = 5.f );
        void renderText( juce::Graphics& G, const juce::String& Text, const juce::Rectangle<int>& Bounds, const juce::Colour& Color, juce::Justification Just, float FontHeight = 26.f );
        void renderArc( juce::Graphics& G, const juce::Rectangle<float> TargetArea, float ArcRadius, float StartAngle, float EndAngle, const juce::Colour& Color, float Width );
        void renderPlayhead( juce::Graphics& G, const juce::Rectangle<float>& Bounds, float CenterX, const juce::Colour& Color, float PHWidth = 10, float PHHeight = 16, float LineThickness = 1 );
        void renderSVGToFit( juce::Graphics& G, const juce::Drawable* SVG, const juce::Rectangle<float> TargetArea, float Rotation = 0.0f );

    };

} // namespace ui
