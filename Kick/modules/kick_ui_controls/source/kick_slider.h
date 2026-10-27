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
#define DEFAULT_MAX_FONT_SIZE 24

    class KickSlider
        : public KickControlBase<juce::Slider>
        , public mt::MTProperties
    {
    public:
        std::function<void( double )> onValueChanged;
        std::function<void( double )> onDragStop;

        explicit KickSlider( const juce::String& Title = "", juce::Slider::SliderStyle Style = juce::Slider::LinearBar, juce::Slider::TextEntryBoxPosition TextBoxPos = juce::Slider::NoTextBox );

        void makeVisible( Component* Parent );
        void makeVisibleAndAddListner( Component* Parent );

        void showTitle( const bool Show, juce::Justification justification = juce::Justification::centredLeft );
        void showValue( const bool Show, juce::Justification justification = juce::Justification::centredLeft, juce::Slider::TextEntryBoxPosition ValuePos = juce::Slider::NoTextBox );
        void showTitleAndValue( const bool Show, juce::Slider::TextEntryBoxPosition ValuePos, juce::Justification justification = juce::Justification::centredLeft );
        void setValuePosition( juce::Slider::TextEntryBoxPosition ValuePos );

        void setDisplayProportions( const float Slider, const float Value, const float Title );
        void setMaxFontSize( int Size );

        void setNormalized( double Value, juce::NotificationType Notify = juce::dontSendNotification )
        {
            setValue(getNormalisableRange().convertFrom0to1( juce::jlimit( 0.0, 1.0, Value ) ), Notify );
        }
        double getNormalized() const noexcept { return getNormalisableRange().convertTo0to1( getValue() ); }
        double getNormalized( double Value ) const noexcept { return getNormalisableRange().convertTo0to1( Value ); }

        void sendPercentValueToHardware( float value );

        double getIncrement() override;
        void   knobRotated( double deltaValue ) override;
        void   knobClicked() override;
        void   sliderMoved( float ) override;
    };

    class KickSliderWithTitle : public KickSlider
    {
    public:
        explicit KickSliderWithTitle( const juce::String& Title, juce::Slider::SliderStyle Style = juce::Slider::LinearBar,
                                      juce::Slider::TextEntryBoxPosition TextBoxPos = juce::Slider::TextBoxAbove, bool ShowValue = false );

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( KickSliderWithTitle )
    };

    class KickRotary : public KickSlider
    {
    public:
        explicit KickRotary( const juce::String& Title = "", juce::Slider::TextEntryBoxPosition TextBoxPos = juce::Slider::TextBoxAbove, bool ShowValue = false );

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( KickRotary )
    };

    //
}
