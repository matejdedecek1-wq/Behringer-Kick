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
    /// KICK Label
    //////////////////////

    KickLabel::KickLabel( const juce::String& Name, const juce::String& Text, bool ShowTitle, bool ShowBorder )
        : mt::MTProperties( this )
    {
        setName( Name );
        setTitle( Name );

        if( Text.isEmpty() )
            setText( Name, juce::dontSendNotification );
        else
            setText( Text, juce::dontSendNotification );

        showTitle( ShowTitle );
        showBorder( ShowBorder );

        setTitleColour( ui::Theme::textColour( 3 ) );
        setValueColour( ui::Theme::textColour( 2 ) );

        setTitleBackgroundColour( ui::Theme::backgroundColour( 9 ) );
        setValueBackgroundColour( ui::Theme::backgroundColour( 9 ) );

        setBorderColour( ui::Theme::outlineColour() );

        setTitleJustification( juce::Justification::centredLeft );
        setValueJustification( juce::Justification::centredLeft );
        setEditable( false, false, false );
        setLookAndFeel( KickLooknFeel::getLnf() );

        addMouseListener( this, true );
    }

    void KickLabel::timerCallback()
    {
        if( !isMouseButtonDown() || getScrollProgression() >= 1.f )
        {
            setScrollProgression( 0.f );
            stopTimer();
            if( !isMouseButtonDown() )
                repaint();
            return;
        }
        setScrollProgression( getScrollProgression() + 0.05f );
        repaint();
    }

    void KickLabel::mouseDown( const juce::MouseEvent& e )
    {
        juce::Label::mouseDown( e );
        auto bounds = getBorderSize().subtractedFrom( getLocalBounds() );
        auto _tfont = juce::Font( ( getTitleFontHeight() ) );
        auto _vfont = juce::Font( ( getValueFontHeight() ) );
        if( _vfont.getStringWidth( getText() ) > bounds.getWidth() || _tfont.getStringWidth( getTitle() ) > bounds.getWidth() )
        {
            startTimer( getTimerSpeed() );
        }
    }

    void KickLabel::mouseExit( const juce::MouseEvent& e )
    {
        juce::Label::mouseExit( e );
        repaint();
    }

    void KickLabel::makeVisible( Component* Parent )
    {
        Parent->addAndMakeVisible( dynamic_cast<Component&>( *this ) );
    }

    void KickLabel::makeVisibleAndAddListner( Component* Parent )
    {
        makeVisible( Parent );
        addListener( dynamic_cast<Listener*>( Parent ) );
    }

    void KickLabel::showTitleAndBorder( bool Show )
    {
        showTitle( Show );
        showBorder( Show );
    }
    void KickLabel::showTitleBorderAndDropdown( bool Show )
    {
        showTitleAndBorder( Show );
        showDropdown( Show );
    }
    void KickLabel::setFontHeight( float FontHeight )
    {
        setValueFontHeight( FontHeight );
        resized();
    }

    void KickLabel::setTextColour( const juce::Colour& Title )
    {
        setValueColour( Title );
        repaint();
    }

    void KickLabel::setColours( const juce::Colour& Text, const juce::Colour& Backgroud, const juce::Colour& Outline )
    {
        setValueColour( Text );
        setValueBackgroundColour( Backgroud );
        setBorderColour( Outline );
    }

    void KickLabel::enableKeyboardEmptyText( bool enable )
    {
        enableEmptyText = enable;
    }
    void KickLabel::editorShown( juce::TextEditor* Editor )
    {
        if( onEditorShow )
        {
            onEditorShow();
            return;
        }
        if( isShowingTitle() )
            Editor->setBoundsRelative( 0, 0.3f, 1.0f, 0.7f );
        else
            Editor->setBounds( getLocalBounds() );

        auto& keyboard          = ui::KickVirtualKeyBoard::getWithText( enableEmptyText ? "" : Editor->getText() );
        keyboard.onEnterPressed = [&keyboard, this]( int v ) {
            mt::utils::exitDialog( keyboard.getCurrentlyFocusedComponent(), v );
        };

        keyboard.show(
            keyboard.getInputText(), ui::KickVirtualKeyBoard::Type::KEYS_LOWERCASE_ALPHABETIC, [&keyboard, this]( int r ) {
                if( r == UI::BUTTONS::APPLY )
                {
                    auto text = keyboard.getInputText();
                    if( text.containsNonWhitespaceChars() )
                    {
                        if( 0 < text.length() )
                            setText( text, juce::sendNotification );
                    }
                    if( onKeyBoardEnd )
                        onKeyBoardEnd();
                }
            },
            ui::layout::keyBoardPopup );
    }

    //////////////////////
    /// KICK Title Label
    //////////////////////

    KickTitleLabel::KickTitleLabel( const juce::String& Name, const juce::String& Text )
        : KickLabel( Name, Text )
    {
        setColours( ui::Theme::textColour( 2 ), ui::Theme::backgroundColour( 4 ), ui::Theme::outlineColour( 1 ) );
    }

    //////////////////////
    /// KICK Name Label
    //////////////////////
    KickNameLabel::KickNameLabel( const juce::String& Name, const juce::String& Text )
        : KickLabel( Name, Text )
    {
        setPadding( 15 );
        setEditable( false, true );
        showDropdown( false );
        setValueFontHeight( 26 );
        setColours( ui::Theme::textColour( 2 ), ui::Theme::backgroundColour( 4 ), ui::Theme::outlineColour( 1 ) );
    }

    /////////////////////////
    /// KICK Name Label
    //////////////////////

    Kick2LineNameLabel::Kick2LineNameLabel( const juce::String& Name, const juce::String& Text )
        : KickNameLabel( Name, Text ) {}

    void Kick2LineNameLabel::setLine2Text( const juce::String& Text )
    {
        line2 = Text;
        repaint();
    }

    void Kick2LineNameLabel::timerCallback()
    {
        if( !isMouseButtonDown() || getScrollProgression() >= 1.f )
        {
            setScrollProgression( 0.f );
            stopTimer();
            if( !isMouseButtonDown() )
                repaint();
            return;
        }
        setScrollProgression( getScrollProgression() + 0.05f );
        repaint();
    }

    void Kick2LineNameLabel::mouseExit( const juce::MouseEvent& e )
    {
        juce::Label::mouseExit( e );
        repaint();
    }

    void Kick2LineNameLabel::mouseDown( const juce::MouseEvent& e )
    {
        juce::Label::mouseDown( e );
        auto _tfont = juce::Font( ( getTitleFontHeight() ) );
        auto _vfont = juce::Font( ( getValueFontHeight() ) );

        if( _tfont.getStringWidth( getText() ) > titleBounds.getWidth() || _vfont.getStringWidth( line2 ) )
            startTimer( getTimerSpeed() );
    }

    void Kick2LineNameLabel::paint( juce::Graphics& g )
    {
        auto b = getLocalBounds().reduced( 1 );
        // Set background
        mt::utils::fillRoundedRect( g, b, getValueBackgroundColour() );

        // Reduce bounds for alignment..
        b.reduce( 20, 2 );

        g.setColour( getValueColour() );

        auto _vfont = juce::Font( ( getValueFontHeight() ) );
        g.setFont( _vfont );
        if( line2.isNotEmpty() )
        {
            titleBounds = b.removeFromTop( (int)( .60f * b.getHeight() ) );

            if( _vfont.getStringWidth( line2 ) > b.getWidth() && getScrollProgression() > 0.f )
            {
                auto textBounds = getScrollBounds( b, line2, _vfont );
                g.drawFittedText( line2, textBounds, getValueJustification(), 1, 1.f );
            }
            else
                g.drawFittedText( line2, b, getValueJustification(), 1, 1.f );
        }
        else if( line2.isEmpty() )
        {
            titleBounds = b;
        }

        auto _tfont = juce::Font( ( getTitleFontHeight() ) );
        g.setFont( _tfont );
        if( _tfont.getStringWidth( getText() ) > titleBounds.getWidth() && getScrollProgression() > 0.f )
        {
            auto textBounds = getScrollBounds( titleBounds, getText(), _tfont );
            g.drawFittedText( getText(), textBounds, getTitleJustification(), 1, 1.f );
        }
        else
            g.drawFittedText( getText(), titleBounds, getTitleJustification(), 1, 1.f );
    }

    juce::Rectangle<int> Kick2LineNameLabel::getScrollBounds( const juce::Rectangle<int> bounds, const juce::String& text, const juce::Font Font )
    {
        const auto           labelTextArea    = getBorderSize().subtractedFrom( getLocalBounds() );
        const auto           optimumTextWidth = Font.getStringWidth( text );
        const auto           xTranslation     = optimumTextWidth - labelTextArea.getWidth();
        juce::Rectangle<int> textBounds       = labelTextArea;
        textBounds                            = juce::Rectangle<int>{
            labelTextArea.getX() - (int)( xTranslation * getScrollProgression() ),
            bounds.getY(),
            optimumTextWidth,
            bounds.getHeight()
        };
        return textBounds;
    }
}
