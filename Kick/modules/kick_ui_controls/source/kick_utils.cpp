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

#include "kick_utils_popup.cpp"
#include "kick_utils_bubble.cpp"

///
/// KickUtils...
///

namespace mt
{
    namespace utils
    {

        ///
        /// Pop-ups and dialogs...
        ///

        inline void showDialog_( bool IsOptions, const juce::String& Title, juce::Component* Comp, std::function<void( int )> Callback,
                                 juce::Rectangle<int> bounds, juce::Colour backgroundColour, juce::Colour titleTextColour, juce::Colour titleBackgroundColour, juce::Colour buttonBackgroundColour )
        {
            auto _dialogBox = getFullScreenModalPopUP<dialogDisplayer>();
            // Setup _dialogbox contents.
            _dialogBox->setTitleColours( titleTextColour, titleBackgroundColour );
            _dialogBox->setButtonBackgroundColour( buttonBackgroundColour );
            _dialogBox->setContentBackgroundColour( backgroundColour );
            _dialogBox->setTitle( Title );
            _dialogBox->setShowYesNoButtons( IsOptions );
            _dialogBox->setContent( Comp, bounds );
            _dialogBox->show( std::forward<std::function<void( int R )>>( Callback ) );
        }

        void exitDialog( juce::Component* Comp, int exitCode )
        {
            if( Comp )
            {
                if( Comp->isCurrentlyModal() )
                    Comp->exitModalState( exitCode );
                else if( auto dw = Comp->findParentComponentOfClass<dialogDisplayer>() )
                    dw->exitModalState( exitCode );
            }
        }

        void showDialog( const juce::String& Title, juce::Component* Comp, std::function<void( int )> Callback,
                         juce::Rectangle<int> bounds, juce::Colour backgroundColour, juce::Colour titleTextColour, juce::Colour titleBackgroundColour )
        {
            showDialog_( false, Title, Comp, std::forward<std::function<void( int )>>( Callback ), bounds, backgroundColour, titleTextColour, titleBackgroundColour, ui::Theme::backgroundColour( 1 ) );
        }

        void showOptions( const juce::String& Title, juce::Component* Comp, std::function<void( int )> Callback,
                          juce::Rectangle<int> bounds, juce::Colour backgroundColour, juce::Colour titleTextColour, juce::Colour titleBackgroundColour, juce::Colour buttonBackgroundColour )
        {
            showDialog_( true, Title, Comp, std::forward<std::function<void( int )>>( Callback ), bounds, backgroundColour, titleTextColour, titleBackgroundColour, buttonBackgroundColour );
        }

        fsPopup* showPopup( juce::Component* Comp, juce::Rectangle<int> Bounds, std::function<void( int )> Callback )
        {
            auto _dialogBox = getFullScreenModalPopUP<fsPopup>();
            // Setup _dialogbox contents.
            _dialogBox->setContent( Comp, Bounds );
            _dialogBox->show( std::forward<std::function<void( int R )>>( Callback ) );
            return _dialogBox;
        }

        void showNotification( const juce::AttributedString& Msg )
        {
            auto bmc = new bubbleNotification();
            bmc->show( Msg );
        }

        void showNotification( const juce::String& Msg )
        {
            juce::AttributedString m;
            m.append( Msg, { 26 }, ui::Theme::textColour() );
            m.setJustification( juce::Justification::centred );
            showNotification( m );
        }

        ///
        /// Bank...
        ///

        int getBankOffset( juce::String Bank )
        {
            auto idx        = ( (int)*Bank.getCharPointer() - (int)'A' );
            auto bankOffset = idx << 4;
            return bankOffset;
        }

        int getBankIndex( int Index ) { return ( Index >> 4 ); }
        int getPadIndexInBank( int ID ) { return ( ID & 0xF ); }

        juce::String getBank( int Index )
        {
            int i = getBankIndex( Index );
            return juce::String( ( 'A' + i ) );
        }

        juce::String getPad( int Index )
        {
            char bank[2] = { '\0' };
            bank[0]      = 'A' + getBankIndex( Index );
            return juce::String( bank ) + juce::String( 1 + getPadIndexInBank( Index ) ).paddedLeft( '0', 2 );
        }

        ///
        /// Graphic Utilities...
        ///

        void renderRoundedRect( juce::Graphics& G, const juce::Rectangle<int>& Bounds, const juce::Colour& Color, int Thickness, float Corner, bool curveTopLeft, bool curveTopRight, bool curveBottomLeft, bool curveBottomRight )
        {
            auto       b = Bounds.toFloat();
            juce::Path p;
            G.setColour( Color );
            p.addRoundedRectangle( b.getX(), b.getY(), b.getWidth(), b.getHeight(), Corner, Corner, curveTopLeft, curveTopRight, curveBottomLeft, curveBottomRight );
            G.strokePath( p, juce::PathStrokeType( Thickness ) );
        }

        void fillRoundedRect( juce::Graphics& G, const juce::Rectangle<int>& Bounds, const juce::Colour& Color, float Corner, bool curveTopLeft, bool curveTopRight, bool curveBottomLeft, bool curveBottomRight )
        {
            auto       b = Bounds.toFloat();
            juce::Path p;
            G.setColour( Color );
            p.addRoundedRectangle( b.getX(), b.getY(), b.getWidth(), b.getHeight(), Corner, Corner, curveTopLeft, curveTopRight, curveBottomLeft, curveBottomRight );
            G.fillPath( p );
        }

        void fillGradientRoundedRect( juce::Graphics& G, const juce::Rectangle<int>& Bounds, const juce::ColourGradient& Color, float Corner )
        {
            G.setGradientFill( Color );
            G.fillRoundedRectangle( (float)Bounds.getX(), (float)Bounds.getY(), (float)Bounds.getWidth(), (float)Bounds.getHeight(), Corner );
        }

        void renderText( juce::Graphics& G, const juce::String& Text, const juce::Rectangle<int>& Bounds, const juce::Colour& Color, juce::Justification Just, float FontHeight )
        {
            G.setColour( Color );
            G.setFont( juce::jmin( FontHeight, ( 0.9f * Bounds.getHeight() ) ) );
            G.drawFittedText( Text, Bounds, Just, 1 );
        }

        void renderArc( juce::Graphics& G, const juce::Rectangle<float> TargetArea, float ArcRadius, float StartAngle, float EndAngle, const juce::Colour& Color, float Width )
        {
            juce::Path arcPath;
            arcPath.addCentredArc( TargetArea.getCentreX(), TargetArea.getCentreY(), ArcRadius, ArcRadius, 0.0f, StartAngle, EndAngle, true );
            G.setColour( Color );
            G.strokePath( arcPath, juce::PathStrokeType( Width, juce::PathStrokeType::curved, juce::PathStrokeType::rounded ) );
        }

        void renderTriangle( juce::Graphics& G, const juce::Rectangle<float>& Bounds, float CenterX, const juce::Colour& Color, float Width = 10, float Height = 16 )
        {
            juce::Path triangle;
            auto       w_2 = Width / 2.f;
            triangle.addTriangle( ( CenterX - w_2 ), Bounds.getY(), ( CenterX + w_2 ), Bounds.getY(), CenterX, Bounds.getBottom() );
            G.setColour( Color );
            G.fillPath( triangle );
        }

        void renderPlayhead( juce::Graphics& G, const juce::Rectangle<float>& Bounds, float CenterX, const juce::Colour& Color, float PHWidth, float PHHeight, float LineThickness )
        {
            juce::Path P;
            auto       height    = Bounds.getHeight();
            auto       thickness = LineThickness * 0.5f;
            auto       phWidth   = PHWidth * .5f;
            auto       phHeight  = juce::jmin( PHHeight, 0.8f * height );

            auto line = juce::Line{ CenterX, Bounds.getY(), CenterX, Bounds.getBottom() };
            P.startNewSubPath( line.getPointAlongLine( 0, phWidth ) );
            P.lineTo( line.getPointAlongLine( phHeight, thickness ) );
            P.lineTo( line.getPointAlongLine( height, thickness ) );
            P.lineTo( line.getPointAlongLine( height, -thickness ) );
            P.lineTo( line.getPointAlongLine( phHeight, -thickness ) );
            P.lineTo( line.getPointAlongLine( 0, -phWidth ) );
            P.closeSubPath();

            G.setColour( Color );
            G.fillPath( P );
        }

        void renderSVGToFit( juce::Graphics& G, const juce::Drawable* SVG, const juce::Rectangle<float> TargetArea, float Rotation )
        {
            if( auto* comp = dynamic_cast<const juce::DrawableComposite*>( SVG ) )
            {
                auto transform = juce::RectanglePlacement( juce::RectanglePlacement::fillDestination ).getTransformToFit( comp->getContentArea(), TargetArea ).rotated( Rotation, TargetArea.getCentreX(), TargetArea.getCentreY() );
                comp->draw( G, 1.0f, transform );
            }
        }

    } // namespace utils

} // namespace mt
