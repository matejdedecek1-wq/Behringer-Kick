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

namespace mt
{
    class MTProperties
    {
    public:
        explicit MTProperties( juce::Component* Parent )
            : T( Parent ), NVSet( Parent->getProperties() ) {}

        explicit MTProperties( juce::NamedValueSet& NVSET )
            : NVSet( NVSET ) {}

        int  getID() { return (int)NVSet.getWithDefault( "id", 0 ); }
        void setID( int ID ) { NVSet.set( "id", ID ); }

        float getCornerRadius() { return NVSet.getWithDefault( "cornerRadius", 5.f ); }
        void  setCornerRadius( float R )
        {
            NVSet.set( "cornerRadius", R );
            if( T ) T->repaint();
        }

        bool isShowingTitle() { return NVSet.getWithDefault( "title", false ); }
        void showTitle( bool S )
        {
            NVSet.set( "title", S );
            if( T ) T->repaint();
        }

        juce::Colour getTitleColour() { return juce::Colour( ( juce::uint32 )( (juce::int64)NVSet.getWithDefault( "titleColor", 0 ) ) ); }
        void         setTitleColour( const juce::Colour& C )
        {
            NVSet.set( "titleColor", (juce::int64)C.getARGB() );
            if( T ) T->repaint();
        }

        juce::Colour getTitleBackgroundColour() { return juce::Colour( ( juce::uint32 )( (juce::int64)NVSet.getWithDefault( "titleBackColor", 0 ) ) ); }
        void         setTitleBackgroundColour( const juce::Colour& C )
        {
            NVSet.set( "titleBackColor", (juce::int64)C.getARGB() );
            if( T ) T->repaint();
        }

        float getTitleFontHeight() { return NVSet.getWithDefault( "titleFontHeight", 14.f ); }
        void  setTitleFontHeight( float H )
        {
            NVSet.set( "titleFontHeight", H );
            if( T ) T->repaint();
        }

        juce::Justification getTitleJustification()
        {
            return juce::Justification( NVSet.getWithDefault( "titleJust", juce::Justification::centred ) );
        }
        void setTitleJustification( juce::Justification J )
        {
            NVSet.set( "titleJust", J.getFlags() );
            if( T ) T->repaint();
        }

        bool isShowingValue() { return NVSet.getWithDefault( "value", false ); }
        void showValue( bool S )
        {
            NVSet.set( "value", S );
            if( T ) T->repaint();
        }

        juce::Colour getValueColour() { return juce::Colour( ( juce::uint32 )( (juce::int64)NVSet.getWithDefault( "valueColor", 0 ) ) ); }
        void         setValueColour( const juce::Colour& C )
        {
            NVSet.set( "valueColor", (juce::int64)C.getARGB() );
            if( T ) T->repaint();
        }

        juce::Colour getValueBackgroundColour() { return juce::Colour( ( juce::uint32 )( (juce::int64)NVSet.getWithDefault( "valueBackColor", 0 ) ) ); }
        void         setValueBackgroundColour( const juce::Colour& C )
        {
            NVSet.set( "valueBackColor", (juce::int64)C.getARGB() );
            if( T ) T->repaint();
        }

        juce::Colour getHighlightColour() { return juce::Colour( ( juce::uint32 )( (juce::int64)NVSet.getWithDefault( "highlightColor", (juce::int64)ui::Theme::foregroundColour( 0 ).getARGB() ) ) ); }
        void         setHighlightColour( const juce::Colour& C )
        {
            NVSet.set( "highlightColor", (juce::int64)C.getARGB() );
            if( T ) T->repaint();
        }

        float getValueFontHeight() { return NVSet.getWithDefault( "valueFontHeight", 14.f ); }
        void  setValueFontHeight( float H )
        {
            NVSet.set( "valueFontHeight", H );
            if( T ) T->repaint();
        }

        juce::Justification getValueJustification() { return juce::Justification( NVSet.getWithDefault( "valueJust", juce::Justification::centred ) ); }
        void                setValueJustification( juce::Justification J )
        {
            NVSet.set( "valueJust", J.getFlags() );
            if( T ) T->repaint();
        }

        float getValueProportion() { return NVSet.getWithDefault( "valueProp", 0.3125f ); }
        void  setValueProportion( float prop )
        {
            NVSet.set( "valueProp", prop );
            if( T ) T->resized();
        }

        bool isShowingDropdown() { return NVSet.getWithDefault( "drop", false ); }
        void showDropdown( bool S )
        {
            NVSet.set( "drop", S );
            if( T ) T->repaint();
        }

        /// Margin...
        int getMarginLeft() { return NVSet.getWithDefault( "MarginLeft", 0 ); }
        int getMarginRight() { return NVSet.getWithDefault( "MarginRight", 0 ); }
        int getMarginTop() { return NVSet.getWithDefault( "MarginTop", 0 ); }
        int getMarginBottom() { return NVSet.getWithDefault( "MarginBottom", 0 ); }

        void setMarginLeft( int M )
        {
            NVSet.set( "MarginLeft", M );
            if( T ) T->repaint();
        }
        void setMarginRight( int M )
        {
            NVSet.set( "MarginRight", M );
            if( T ) T->repaint();
        }
        void setMarginTop( int M )
        {
            NVSet.set( "MarginTop", M );
            if( T ) T->repaint();
        }
        void setMarginBottom( int M )
        {
            NVSet.set( "MarginBottom", M );
            if( T ) T->repaint();
        }
        void setMargin( int M )
        {
            NVSet.set( "MarginLeft", M );
            NVSet.set( "MarginRight", M );
            NVSet.set( "MarginTop", M );
            NVSet.set( "MarginBottom", M );
            if( T ) T->repaint();
        }
        void setMargin( int LeftRight, int TopBottom )
        {
            NVSet.set( "MarginLeft", LeftRight );
            NVSet.set( "MarginRight", LeftRight );
            NVSet.set( "MarginTop", TopBottom );
            NVSet.set( "MarginBottom", TopBottom );
            if( T ) T->repaint();
        }

        /// Padding...
        int getPaddingLeft() { return NVSet.getWithDefault( "PadLeft", 0 ); }
        int getPaddingRight() { return NVSet.getWithDefault( "PadRight", 0 ); }
        int getPaddingTop() { return NVSet.getWithDefault( "PadTop", 0 ); }
        int getPaddingBottom() { return NVSet.getWithDefault( "PadBottom", 0 ); }

        void setPaddingLeft( int M )
        {
            NVSet.set( "PadLeft", M );
            if( T ) T->repaint();
        }
        void setPaddingRight( int M )
        {
            NVSet.set( "PadRight", M );
            if( T ) T->repaint();
        }
        void setPaddingTop( int M )
        {
            NVSet.set( "PadTop", M );
            if( T ) T->repaint();
        }
        void setPaddingBottom( int M )
        {
            NVSet.set( "PadBottom", M );
            if( T ) T->repaint();
        }
        void setPadding( int M )
        {
            NVSet.set( "PadLeft", M );
            NVSet.set( "PadRight", M );
            NVSet.set( "PadTop", M );
            NVSet.set( "PadBottom", M );
            if( T ) T->repaint();
        }
        void setPadding( int LeftRight, int TopBottom )
        {
            NVSet.set( "PadLeft", LeftRight );
            NVSet.set( "PadRight", LeftRight );
            NVSet.set( "PadTop", TopBottom );
            NVSet.set( "PadBottom", TopBottom );
            if( T ) T->repaint();
        }

        /// Knob
        bool isShowingKnob() { return NVSet.getWithDefault( "knob", false ); }
        void showKnob( bool S )
        {
            NVSet.set( "knob", S );
            if( T ) T->repaint();
        }

        bool isCentered() { return NVSet.getWithDefault( "centered", false ); }
        void setCentered( bool S )
        {
            NVSet.set( "centered", S );
            if( T ) T->repaint();
        }

        bool isShowingBorder() { return NVSet.getWithDefault( "border", false ); }
        void showBorder( bool S )
        {
            NVSet.set( "border", S );
            if( T ) T->repaint();
        }
        int  getBorderThickness() { return NVSet.getWithDefault( "borderThickness", 1 ); }
        void setBorderThickness( int S )
        {
            NVSet.set( "borderThickness", S );
            if( T ) T->repaint();
        }

        juce::Colour getBorderColour() { return juce::Colour( ( juce::uint32 )( (juce::int64)NVSet.getWithDefault( "borderColor", 0 ) ) ); }
        void         setBorderColour( const juce::Colour& C )
        {
            NVSet.set( "borderColor", (juce::int64)C.getARGB() );
            if( T ) T->repaint();
        }

        juce::Colour getBackgroundColour() { return juce::Colour( ( juce::uint32 )( (juce::int64)NVSet.getWithDefault( "backColor", 0 ) ) ); }
        void         setBackgroundColour( const juce::Colour& C )
        {
            NVSet.set( "backColor", (juce::int64)C.getARGB() );
            if( T ) T->repaint();
        }

        void setColours( const juce::Colour& V, const juce::Colour& C )
        {
            NVSet.set( "titleColor", (juce::int64)C.getARGB() );
            NVSet.set( "valueColor", (juce::int64)V.getARGB() );
            if( T ) T->repaint();
        }

        bool isTopLeftCurved() { return NVSet.getWithDefault( "topLeft", true ); }
        bool isTopRightCurved() { return NVSet.getWithDefault( "topRight", true ); }
        bool isBottomLeftCurved() { return NVSet.getWithDefault( "bottomLeft", true ); }
        bool isBottomRightCurved() { return NVSet.getWithDefault( "bottomRight", true ); }
        void setBorderCurves( bool topLeft, bool topRight, bool bottomLeft, bool bottomRight )
        {
            NVSet.set( "topLeft", topLeft );
            NVSet.set( "bottomLeft", bottomLeft );
            NVSet.set( "topRight", topRight );
            NVSet.set( "bottomRight", bottomRight );
            if( T ) T->repaint();
        }

        int  getFontHeight() { return NVSet.getWithDefault( "fontHeight", 14.f ); }
        void setFontHeight( float H )
        {
            NVSet.set( "fontHeight", H );
            if( T ) T->repaint();
        }

        float getTitleProportion() { return NVSet.getWithDefault( "titleProp", 0.3125f ); }
        void  setTitleProportion( float prop )
        {
            NVSet.set( "titleProp", prop );
            if( T ) T->resized();
        }

        int  getTimerSpeed() { return NVSet.getWithDefault( "timerSpeed", 100 ); }
        void setTimerSpeed( const int milliSeconds )
        {
            NVSet.set( "timerSpeed", milliSeconds );
        }

        float getScrollProgression() { return NVSet.getWithDefault( "scrollProgression", 0.f ); }
        void  setScrollProgression( const float progression )
        {
            NVSet.set( "scrollProgression", progression );
        }

        juce::String getPreFix() { return NVSet.getWithDefault( "prefix", "" ); }
        void         setPreFix( const juce::String& PreFix )
        {
            NVSet.set( "prefix", PreFix );
            if( T ) T->repaint();
        }

        juce::String getPostFix() { return NVSet.getWithDefault( "postfix", "" ); }
        void         setPostFix( const juce::String& PostFix )
        {
            NVSet.set( "postfix", PostFix );
            if( T ) T->repaint();
        }

        juce::String getPadString() { return NVSet.getWithDefault( "padChar", " " ).toString(); }
        char         getPadChar() { return getPadString().getLastCharacter(); }
        void         setPadChar( const juce::String& C )
        {
            NVSet.set( "padChar", C );
            if( T ) T->repaint();
        }

        int  getPadLen() { return NVSet.getWithDefault( "padLen", 0 ); }
        void setPadLen( int Len )
        {
            NVSet.set( "padLen", Len );
            if( T ) T->repaint();
        }

        int  getOffset() { return NVSet.getWithDefault( "offset", 0 ); }
        void setOffset( int Len )
        {
            NVSet.set( "offset", Len );
            if( T ) T->repaint();
        }

    private:
        juce::Component*     T{ nullptr };
        juce::NamedValueSet& NVSet;
    };

}
