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

inline static constexpr int groupPadSpacer = 10;
inline static constexpr int leftSpacer     = 10;
inline static constexpr int topSpacer      = 10;
inline static constexpr int leftRightSpace = 20;
inline static constexpr int topBottomSpace = 22;

inline static constexpr int padSpacer     = 2;
inline static constexpr int miniPadWidth  = 20;
inline static constexpr int padHeight     = 14;
inline static constexpr int removeFromTop = 12;
inline static constexpr int topTrim_      = 21;
inline static constexpr int bottomTrim_   = 21;
inline static constexpr int leftTrim_     = 1;
inline static constexpr int rightTrim_    = 1;
inline static constexpr int labelWidth_   = 44;
inline static constexpr int leftPadAlign  = 55;
inline static constexpr int rightPadAlign = 15;
inline static constexpr int cornerSize    = 4;

namespace ui
{
#define NUM_COLS_IN_BANK    ( 4 )
#define NUM_ROWS_IN_BANK    ( 4 )
#define NUM_PADS_IN_BANK    ( NUM_COLS_IN_BANK * NUM_ROWS_IN_BANK )
#define NUM_BANKS           ( 8 )

#define NUM_BANKS_IN_ROW    ( 2 )
#define NUM_BANKS_IN_COLUMN ( 4 )

    class KickMiniPad : public juce::Component
    {
    public:
        KickMiniPad( const juce::Colour Normal      = ui::Theme::outlineColour( 5 ),
                     const juce::Colour Highlighted = ui::Theme::outlineColour( 1 ), const juce::Colour DisabledColor = ui::Theme::outlineColour( 4 ) );
        ~KickMiniPad();

        std::function<void( int )> onClicked;

        void      setId( int ID ) { id = ID; }
        const int getId() { return id; }

        bool isActive() { return active; }
        bool isHighlight() { return highlight; }
        void setActive( bool Active );
        void setHighlight( bool Highlight );
        void triggerClick();

        void makeVisible( juce::Component* Parent );
        void setColours( const juce::Colour Normal, const juce::Colour Highlighted, const juce::Colour DisabledColor = ui::Theme::outlineColour( 4 ), const juce::Colour SoloColor = ui::Theme::padButtonColour( 13 ), bool IsSolo = false );

    private:
        bool         highlight{ false }, active{ false }, solo{ false };
        int          id{ 0 };
        juce::Colour disabledColor, normalColor, highlightColor, soloColor;

        virtual void paint( juce::Graphics& g ) override;
        virtual void resized() override;
        virtual void mouseDown( const juce::MouseEvent& event ) override;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( KickMiniPad )
    };

    class KickMiniPadGroup : public juce::Component
    {
    public:
        KickMiniPadGroup( juce::String Bank, juce::Justification::Flags Justification );
        virtual ~KickMiniPadGroup();

        std::function<void( juce::String, int )> onClicked;

        void select( bool Select );

        void setJustification( juce::Justification::Flags Justification );

        void setColours( int Index, const juce::Colour Normal, const juce::Colour Highlighted, const juce::Colour DisabledColor = ui::Theme::outlineColour( 4 ), const juce::Colour SoloColor = ui::Theme::padButtonColour( 13 ), bool IsSolo = false );
        void setHighlight( int Index, bool Highlight ); // Sets 'Color' to pad specified by 'Index'
        void setActive( int Index, bool Active ); // Sets pad active/deactive
        void clearAll(); // Removes colour from all pads.

        bool isHighlight( int Index );
        void triggerClick( int Index );

    private:
        juce::Justification::Flags                pos_{ juce::Justification::left };
        KickLabel                                 bank_;
        std::vector<std::unique_ptr<KickMiniPad>> pads_;
        bool                                      isSelected_{ false };
        juce::Rectangle<int>                      padBounds_;

        virtual void paint( juce::Graphics& g ) override;
        virtual void resized() override;
        void         mouseDown( const juce::MouseEvent& e ) override;
    };

    class KickPadMiniMap : public juce::Component
    {
    public:
        enum Layout
        {
            Vertical,
            Horizantal,
        };

        explicit KickPadMiniMap( const juce::Colour& Normal      = ui::Theme::outlineColour( 5 ),
                                 const juce::Colour& Highlighted = ui::Theme::outlineColour( 1 ),
                                 const juce::Colour& Disabled = ui::Theme::outlineColour( 5 ), Layout Lay = Vertical );

        std::function<void( juce::String, int )> onClicked;

        void select( const juce::String& Bank, bool Selection );
        void select( int BankIndex, bool Selection );

        void setColours( int Index, const juce::Colour Normal, const juce::Colour Highlighted, const juce::Colour DisabledColor = ui::Theme::outlineColour( 4 ), const juce::Colour SoloColor = ui::Theme::padButtonColour( 13 ), bool IsSolo = false );
        void setActive( int Index, bool Active );
        void setHighlight( int Index, bool Highlight );
        void highlightAll( bool Highlight );
        void clearAll();

        bool isHighlight( int Index );
        void triggerClick( int Index );

        void setLayout( Layout layout );

        void makeVisible( juce::Component* Parent );

    private:
        Layout                                         layout_;
        std::vector<std::unique_ptr<KickMiniPadGroup>> banks_;

        virtual void resized();

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( KickPadMiniMap )
    };
}
