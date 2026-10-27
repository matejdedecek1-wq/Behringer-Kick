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
    class KickLabel
        : public juce::Label
        , public mt::MTProperties
        , protected juce::Timer
    {
    public:
        std::function<void( void )> onKeyBoardEnd;
        explicit KickLabel( const juce::String& Name = "", const juce::String& Text = "", bool ShowTitle = false, bool ShowBorder = false );

        void makeVisible( juce::Component* Parent );
        void makeVisibleAndAddListner( juce::Component* Parent );

        void showTitleAndBorder( bool Show );
        void showTitleBorderAndDropdown( bool Show );
        void setFontHeight( float FontHeight );
        void setTextColour( const juce::Colour& Title );

        void setColours( const juce::Colour& Text, const juce::Colour& Backgroud, const juce::Colour& Outline = juce::Colours::transparentBlack );
        void enableKeyboardEmptyText( bool );

    protected:
        void timerCallback() override;
        void mouseDown( const juce::MouseEvent& ) override;
        void mouseExit( const juce::MouseEvent& ) override;

    private:
        void editorShown( juce::TextEditor* Editor ) override;
        bool enableEmptyText = false;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( KickLabel )
    };

    class KickNameLabel : public KickLabel
    {
    public:
        std::function<void()> onClick;
        explicit KickNameLabel( const juce::String& Name, const juce::String& Text = "" );

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( KickNameLabel )
    };

    class KickTitleLabel : public KickLabel
    {
    public:
        explicit KickTitleLabel( const juce::String& Name, const juce::String& Text = "" );

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( KickTitleLabel )
    };

    class Kick2LineNameLabel : public KickNameLabel
    {
    public:
        explicit Kick2LineNameLabel( const juce::String& Name, const juce::String& Text = "" );

        void setLine2Text( const juce::String& Text );

    private:
        juce::String line2{ "" };

        void paint( juce::Graphics& g ) override;
        void timerCallback() override;
        void mouseDown( const juce::MouseEvent& ) override;
        void mouseExit( const juce::MouseEvent& ) override;

        juce::Rectangle<int> getScrollBounds( const juce::Rectangle<int>, const juce::String&, const juce::Font );

        juce::Rectangle<int> titleBounds;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( Kick2LineNameLabel )
    };

}
