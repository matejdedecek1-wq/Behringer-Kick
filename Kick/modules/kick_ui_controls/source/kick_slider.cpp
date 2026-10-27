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

namespace ui
{
    /////////////////////////
    /// KICK Slider
    /////////////////////////

    KickSlider::KickSlider( const juce::String& Title, juce::Slider::SliderStyle Style, juce::Slider::TextEntryBoxPosition TextBoxPos )
        : mt::MTProperties( this )
    {
        setLookAndFeel( KickLooknFeel::getLnf() );

        setName( Title );
        setTitle( Title );
        setSliderStyle( Style );
        setValuePosition( TextBoxPos );

        setViewportIgnoreDragFlag( true );
        setTextBoxIsEditable( false );
        setScrollWheelEnabled( false );

        setNumDecimalPlacesToDisplay( 2 );
        setRange( 0.0, 1.0, 0.01 );

        onValueChange = [this]() {
            if( onValueChanged )
                onValueChanged( getValue() );
        };

        onDragEnd = [this]() {
            if( onDragStop )
                onDragStop( getValue() );
        };

        setTitleJustification( juce::Justification::centred );
        setValueJustification( juce::Justification::centred );

        setTitleColour( ui::Theme::textColour( 1 ) );
        setValueColour( ui::Theme::textColour( 2 ) );
    }

    void KickSlider::makeVisible( Component* Parent )
    {
        Parent->addAndMakeVisible( dynamic_cast<Component&>( *this ) );
    }

    void KickSlider::makeVisibleAndAddListner( Component* Parent )
    {
        makeVisible( Parent );
        addListener( dynamic_cast<juce::Slider::Listener*>( Parent ) );
    }

    void KickSlider::showTitle( const bool Show, juce::Justification Just )
    {
        mt::MTProperties::showTitle( Show );
        setTitleJustification( Just );
        repaint();
    }

    void KickSlider::showValue( const bool Show, juce::Justification Just, juce::Slider::TextEntryBoxPosition ValuePos )
    {
        mt::MTProperties::showValue( Show );
        setValueJustification( Just );
        setValuePosition( ValuePos );
    }

    void KickSlider::setValuePosition( juce::Slider::TextEntryBoxPosition ValuePos )
    {
        mt::MTProperties::showValue( juce::Slider::NoTextBox != ValuePos );
        setTextBoxStyle( ValuePos, true, 0, 0 );
        repaint();
    }

    void KickSlider::showTitleAndValue( const bool Show, juce::Slider::TextEntryBoxPosition ValuePos, juce::Justification justification )
    {
        showTitle( Show, justification );
        showValue( Show, justification, ValuePos );
    }

    void KickSlider::setDisplayProportions( const float Slider, const float Value, const float Title )
    {
        setTitleProportion( Title );
        setValueProportion( Value );
        repaint();
    }

    void KickSlider::setMaxFontSize( int Size )
    {
        setValueFontHeight( Size );
        repaint();
    }

    void KickSlider::sendPercentValueToHardware( float value )
    {
        int valueInPercent = (int)( 100.0 * getNormalized( value ) );
        contracts::DclSignals::sendCommandToDcl(
            contracts::DclContractsHelper::sendSliderValueInPercentMessage( valueInPercent,
                                                                            isCentered() ?
                                                                                contracts::DeviceControlContracts::SliderCmd::EN_SLIDER_LEVEL_LED :
                                                                                contracts::DeviceControlContracts::SliderCmd::EN_SLIDER_BAR_LED ) );
    }

    double KickSlider::getIncrement()
    {
        return getInterval();
    }

    void KickSlider::knobRotated( double deltaValue )
    {
        //DBG( deltaValue );
        double newval = getValue() + deltaValue;
        setValue( newval, juce::NotificationType::sendNotification );
        sendPercentValueToHardware( getValue() );
    }

    void KickSlider::sliderMoved( float percentValue )
    {
        //DBG( percentValue );
        setNormalized( ( percentValue ) );
        sendPercentValueToHardware( getValue() );
    }

    void KickSlider::knobClicked() {}

    /////////////////////////
    /// KICK Slider with Title
    /////////////////////////

    KickSliderWithTitle::KickSliderWithTitle( const juce::String& Title, juce::Slider::SliderStyle Style, juce::Slider::TextEntryBoxPosition TextBoxPos, bool ShowValue )
        : KickSlider( Title, Style, TextBoxPos )
    {
        showTitle( true );
        showValue( ShowValue, juce::Justification::centredLeft, TextBoxPos );
    }

    /////////////////////////
    /// KICK Rotary Slider
    /////////////////////////

    KickRotary::KickRotary( const juce::String& Title, juce::Slider::TextEntryBoxPosition TextBoxPos, bool ShowValue )
        : KickSlider( Title, juce::Slider::Rotary, TextBoxPos )
    {
        showTitle( true, juce::Justification::centred );
        showValue( ShowValue, juce::Justification::centred, TextBoxPos );
    }

    /////////////////////////
    /// KICK Rotary Slider
    /////////////////////////

    //
} // namespace ui
