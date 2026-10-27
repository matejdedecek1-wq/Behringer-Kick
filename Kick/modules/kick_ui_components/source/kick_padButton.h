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
#define DEFAULT_BORDER_COLOR     ui::Theme::outlineColour( 1 )
#define DEFAULT_BORDER_THICKNESS 1.0f

    class KickPadButton : public KickControlBase<KickButton>
    {
    public:
        std::function<void( void )> onKnobClickedCallback;
        enum class PadButtonType
        {
            with1Row,
            with3Rows,
            colorMode,
            levelMode,
            panMode,
            muteMode,
            insertMode,
            assignSample,
            muteGroup,
            padMixer
        };

        std::function<void( KickPadButton*, int Id )> onDoubleClicked;

        std::function<void( KickPadButton*, const juce::Colour&, float )> onColorChanged;
        std::function<void( KickPadButton*, const juce::String& )>        onHeaderChanged;
        std::function<void( KickPadButton*, const juce::String& )>        onGroupChanged;
        std::function<void( KickPadButton*, const juce::String& )>        onRow1Changed;
        std::function<void( KickPadButton*, const juce::String& )>        onRow2Changed;
        std::function<void( KickPadButton*, const juce::String& )>        onRow3Changed;
        std::function<void( KickPadButton*, bool )>                       onMuteChanged;
        std::function<void( KickPadButton*, bool )>                       onSoloChanged;
        std::function<void( KickPadButton*, double )>                     onSliderChanged;
        std::function<void( KickPadButton*, double )>                     onPanChanged;

        KickPadButton( const juce::String& buttonName = "", PadButtonType Type = PadButtonType::with1Row );

        // Type...
        PadButtonType getType() { return type; };
        void          setType( PadButtonType Type );
        void          sliderMoved( float ) override {}
        void          knobClicked() override;
        void          knobRotated( double ) override {}
        // Text components...
        void setHeaderText( const juce::String& Text );
        void setGroupText( const juce::String& Text );
        void setRow1Text( const juce::String& Text, juce::Justification Just );
        void setRow2Text( const juce::String& Text, juce::Justification Just );
        void setRow3Text( const juce::String& Text, juce::Justification Just );

        // Colours
        void setHeaderColour( juce::Colour HeaderColour = ui::Theme::textColour( 1 ) );
        void setGroupColour( juce::Colour GroupColour = ui::Theme::textColour( 2 ) );
        void setRow1Colour( juce::Colour Row1Colour = ui::Theme::textColour( 2 ) );
        void setRow2Colour( juce::Colour Row2Colour = ui::Theme::textColour( 1 ) );
        void setRow3Colour( juce::Colour Row3Colour = ui::Theme::textColour( 1 ) );

        // Slider component...
        void   setSliderRange( double Min, double Max, double StepSize = 0.0 );
        void   setSliderValue( double Value );
        void   setPanSliderValue( double Value );
        double getSliderValue();
        double getPanSliderValue();
        void   updateLevelMeter( float lRms, float rRms, float lPeak, float rPeak );

        void setMute( bool Value, juce::NotificationType Notification = juce::sendNotification );
        void setSolo( bool Value, juce::NotificationType Notification = juce::sendNotification );

        void resetBorderColor();
        void setBorderColor( const juce::Colour& Col, float Thickness = 1.0f );
        void setActive( const bool IsActive );
        void setSelected( const bool Select );
        void setHighlight( const bool Highlight, juce::Colour highlightedColour = ui::Theme::selectionColour( 2 ) );

        bool                getMute() { return mute.getToggleState(); };
        bool                getSolo() { return solo.getToggleState(); };
        juce::String        getHeaderText() { return name.getText(); }
        juce::String        getGroupText() { return group.getText(); }
        juce::String        getRow1Text() { return lrow1.getText(); }
        juce::String        getRow2Text() { return lrow2.getText(); }
        juce::String        getRow3Text() { return lrow3.getText(); }
        const juce::Colour& getBorderColor() { return border; }
        float               getBorderThickness() { return borderThickness; }

        bool isHighlighted() { return isHighlight; }
        bool isActive() { return isEnabled; }
        bool isSelected() { return selected; }
        void setTextColour( const juce::Colour& Text );
        void setElementsEnable( const bool state );
        void setFocusOutline( bool state );
        void clear();

    private:
        float         borderThickness{ DEFAULT_BORDER_THICKNESS };
        float         highlightBorderThickness{ DEFAULT_BORDER_THICKNESS * 2 };
        PadButtonType type;
        KickLabel     name{ "" }, group{ "" }, value{ "" };
        KickLabel     lrow1{ "" }, lrow2{ "" }, lrow3{ "" };

        KickImageButton            solo{ "Solo", "solo_mini", "solo_mini_active" };
        KickImageButton            mute{ "Mute", "mute_mini", "mute_mini_active" };
        KickImageButton            rec{ "Rec", "Arm_normal" };
        KickMeterVerticalWithMarks levelMeter;

        KickColorButton m{ ui::Theme::highlightColour( 1 ), ui::Theme::highlightColour( 0 ) };

        KickSlider slider{ "", juce::Slider::LinearBar, juce::Slider::NoTextBox };
        KickSlider panSlider{ "", juce::Slider::LinearBar, juce::Slider::NoTextBox };
        KickSlider meter[2];

        juce::Colour border{ DEFAULT_BORDER_COLOR };
        juce::Colour highlightedColour{ ui::Theme::selectionColour( 2 ) };
        juce::Colour headerColour{ ui::Theme::textColour( 1 ) };
        juce::Colour groupColour{ ui::Theme::textColour( 2 ) };
        juce::Colour row1Colour{ ui::Theme::textColour( 2 ) };
        juce::Colour row2Colour{ ui::Theme::textColour( 1 ) };
        juce::Colour row3Colour{ ui::Theme::textColour( 1 ) };

        bool isMaximized{ false }, isHighlight{ false }, selected{ false }, isEnabled{ false }, isVisible{ false };

        void paint( juce::Graphics& g ) override;
        void resized();

        void mouseDoubleClick( const juce::MouseEvent& event ) override;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( KickPadButton )
    };
}
