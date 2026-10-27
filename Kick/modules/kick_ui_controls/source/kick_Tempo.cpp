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
    //////////////////////
    /// KICK Tempo Editor
    //////////////////////
    KickTempoEditor::KickTempoEditor()
        : mt::MTProperties( this )
    {
        slider.setMaxFontSize( 100 );
        slider.showValue( true, juce::Justification::centred, juce::Slider::NoTextBox );

        inc.setConnectedEdges( (int)juce::Button::ConnectedEdgeFlags::ConnectedOnRight );
        dec.setConnectedEdges( (int)juce::Button::ConnectedEdgeFlags::ConnectedOnLeft );

        inputTextLabel.setFont( 24 );
        inputTextLabel.setColour( juce::TextEditor::backgroundColourId, ui::Theme::backgroundColour( 13 ) );
        inputTextLabel.setColour( juce::TextEditor::textColourId, ui::Theme::textColour( 2 ) );
        inputTextLabel.setCaretVisible( true );
        inputTextLabel.setFocusContainerType( juce::Component::FocusContainerType::focusContainer );
        inputTextLabel.setBorder( { 22, 2, 20, 2 } );
        inputTextLabel.setJustification( juce::Justification::centred );
        inputTextLabel.setText( slider.getTextFromValue( slider.getValue() ) );

        mt::utils::addAndMakeVisible( this, slider, num1, num2, num3, num4, num5, num6, num7, num8, num9, num0, inc, dec, del, clr, inputTextLabel, tap );
        mt::utils::setPlacementFlag( juce::Justification::centred, num1, num2, num3, num4, num5, num6, num7, num8, num9, num0, inc, dec, clr );

        num0.onClick = [&]() { addInput( 0 ); };
        num1.onClick = [&]() { addInput( 1 ); };
        num2.onClick = [&]() { addInput( 2 ); };
        num3.onClick = [&]() { addInput( 3 ); };
        num4.onClick = [&]() { addInput( 4 ); };
        num5.onClick = [&]() { addInput( 5 ); };
        num6.onClick = [&]() { addInput( 6 ); };
        num7.onClick = [&]() { addInput( 7 ); };
        num8.onClick = [&]() { addInput( 8 ); };
        num9.onClick = [&]() { addInput( 9 ); };

        auto getPresetStyle = []( int index ) -> juce::String {
            switch( index )
            {
                case 0: return "DUB";
                case 1: return "HIP-HOP";
                case 2: return "HOUSE";
                case 3: return "TECHNO";
                case 4: return "DUBSTEP";
                case 5: return "DRUM";
                default: return "DUB";
            }
        };

        auto getPresetValues = []( int index ) -> std::array<int, 5> {
            switch( index )
            {
                case 0: return { 60, 70, 75, 80, 90 };
                case 1: return { 60, 70, 80, 90, 100 };
                case 2: return { 110, 115, 120, 125, 130 };
                case 3: return { 120, 125, 130, 135, 140 };
                case 4: return { 135, 138, 140, 143, 145 };
                case 5: return { 160, 165, 170, 175, 180 };
                default: return { 60, 70, 75, 80, 90 };
            }
        };

        for( auto idx = 0; idx < tapStype.size(); ++idx )
        {
            mt::utils::addAndMakeVisible( this, tapStype[idx] );
            auto text = getPresetStyle( idx );

            tapStype[idx].setToggleButtonText( text, text );
            tapStype[idx].setRadioGroupId( 123456789 );
            tapStype[idx].setValueFontHeight( 14.f );
            tapStype[idx].setValueColour( ui::Theme::textColour( 1 ) );

            tapStype[idx].onClick = [&, idx]() {
                if( tapStype[idx].getToggleState() )
                {
                    auto values = getPresetValues( idx );
                    for( auto i = 0; i < tapValue.size(); ++i )
                    {
                        tapValue[i].setButtonText( juce::String{ values[i] } );
                        tapValue[i].setID( values[i] );
                        tapValue[i].setValueColour( ui::Theme::textColour( 1 ) );
                    }
                }
            };
        }

        for( auto idx = 0; idx < tapValue.size(); ++idx )
        {
            mt::utils::addAndMakeVisible( this, tapValue[idx] );

            tapValue[idx].setValueFontHeight( 26 );
            tapValue[idx].onClick = [&, idx]() {
                auto bpm = tapValue[idx].getID();
                slider.setValue( bpm );
            };
        }

        inc.onClick = [&]() {
            slider.setValue( slider.getValue() + interVal, juce::sendNotification );
        };

        dec.onClick = [&]() {
            slider.setValue( slider.getValue() - interVal, juce::sendNotification );
        };

        clr.onClick = [&]() {
            slider.setValue( ( (int)slider.getValue() ) / 10, juce::dontSendNotification );
            intermidentValue = intermidentValue / 10;
            inputTextLabel.setText( juce::String( intermidentValue ) );
        };

        slider.onValueChanged = [&]( [[maybe_unused]] double Val ) {
            intermidentValue = Val;
            inputTextLabel.setText( juce::String( Val ) );
        };

        tap.onClick = [&]() {
            auto curTime = juce::Time::getMillisecondCounterHiRes();

            if( 0 == tapCount )
                startMs = curTime;

            ++tapCount;
            if( numTapsForTempo <= tapCount )
            {
                const auto timeDiff       = curTime - startMs;
                const auto secondsElapsed = ( timeDiff ) / 1000;
                double     bpm            = ( ( 60.0 * tapCount ) / secondsElapsed );
                slider.setValue( bpm );
                tapCount = 0;
            }
            tap.setToggleState( false, juce::dontSendNotification );
        };

        tapStype[0].triggerClick();
        tap.setValueColour( ui::Theme::textColour( 1 ) );
        tap.setValueFontHeight( 24 );
    }

    void KickTempoEditor::setRange( int Min, int Max, int Default, int Step, int Value )
    {
        def              = Default;
        interVal         = Step;
        minValue         = Min;
        maxValue         = Max;
        intermidentValue = 0;
        slider.setRange( Min, Max, Step );
        slider.setValue( Value );
    }

    void KickTempoEditor::setValue( int Val )
    {
        slider.setValue( Val, juce::sendNotification );
    }

    void KickTempoEditor::reset()
    {
        setValue( def );
        interVal = 0;
    };

    void KickTempoEditor::clear()
    {
        setValue( slider.getMinimum() );
        interVal = 0;
    };

    void KickTempoEditor::addInput( int Value )
    {
        auto now = std::chrono::steady_clock::now();
        if( now - lastInputTime > timeoutDuration )
        {
            intermidentValue = 0; // Reset current value if timeout
        }
        lastInputTime = now;

        intermidentValue = intermidentValue * 10 + Value;
        if( intermidentValue > maxValue )
        {
            intermidentValue = maxValue; // Ensure value does not exceed max
        }
        refresh();
    }

    void KickTempoEditor::refresh()
    {
        auto val = juce::jlimit( minValue, maxValue, intermidentValue );
        slider.setValue( (double)val, juce::dontSendNotification );
        inputTextLabel.setText( juce::String( intermidentValue ) );
    }

    void KickTempoEditor::show( const juce::String& Title, std::function<void( int )> CallbackOnExit )
    {
        //slider.setFocus( true );
        inputTextLabel.setText( slider.getTextFromValue( slider.getValue() ) );
        mt::utils::showOptions( Title, this, CallbackOnExit, { 0, 0, tempoWidth, numpadHeight } );
    }

    void KickTempoEditor::paint( juce::Graphics& g )
    {
        mt::utils::renderText( g, "----------------------------------------------------     PRESET     ----------------------------------------------------", textBounds, ui::Theme::textColour( 2 ), juce::Justification::centred, 16 );
        auto bounds = tap.getBounds();
        bounds.removeFromRight( 9 );
        mt::utils::renderRoundedRect( g, bounds.expanded( 1 ), ui::Theme::outlineColour( 1 ), 2 );
    }

    void KickTempoEditor::resized()
    {
        auto b = getLocalBounds();

        // text label
        b.removeFromTop( buttonSpace1 );
        inputTextLabel.setBounds( b.removeFromTop( inputTextHeight ).reduced( spaceAround1, 0 ) );

        //// Row 1 -? inc, dec, slider.
        b.removeFromTop( buttonSpace1 );
        textBounds.setBounds( 288, 164, 470, 22 );
        auto row1 = b.removeFromTop( buttonHeight2 ).reduced( spaceAround1, 0 );
        dec.setBounds( row1.removeFromLeft( buttonWidth3 ) );
        row1.removeFromRight( 3 );
        inc.setBounds( row1.removeFromRight( buttonWidth3 ) );
        slider.setBounds( row1.reduced( buttonSpace2, 0 ) );

        // Divide remaining area into left and right panes.
        tap.setBoundsRelative( .3587f, .7223, .59f, .2702f );

        num7.setBoundsRelative( .05f, .4275f, .075f, .1228f );
        num8.setBoundsRelative( .1375f, .4275f, .075f, .1228f );
        num9.setBoundsRelative( .225f, .4275f, .075f, .1228f );

        num4.setBoundsRelative( .05f, .5749f, .075f, .1228f );
        num5.setBoundsRelative( .1375f, .5749f, .075f, .1228f );
        num6.setBoundsRelative( .225f, .5749f, .075f, .1228f );

        num1.setBoundsRelative( .05f, .7223f, .075f, .1228f );
        num2.setBoundsRelative( .1375f, .7223f, .075f, .1228f );
        num3.setBoundsRelative( .225f, .7223f, .075f, .1228f );

        num0.setBoundsRelative( .05f, .8697f, .075f, .1228f );
        clr.setBoundsRelative( .1375f, .8697f, .1625f, .1228f );

        auto x0 = .3587f;
        for( auto& it : tapStype )
        {
            it.setBoundsRelative( x0, .4668f, .0875, .0835f );
            x0 += .1f;
        }
        auto x1 = .3787f;
        for( auto& it : tapValue )
        {
            it.setBoundsRelative( x1, .5749f, .1f, .1228f );
            x1 += .1125f;
        }
    }

    //////////////////////
    /// KICK Tempo
    //////////////////////
    KickTempo::KickTempo( const juce::String& Name, int Min, int Max, int Default, int StepSize, bool ShowTitle, bool ShowBorder )
        : mt::MTProperties( this )
        , value( Default )
    {
        setName( Name );
        setTitle( Name );
        setRange( Min, Max, StepSize );

        setTitleColour( ui::Theme::textColour() );
        setValueColour( ui::Theme::textColour( 2 ) );
        setTitleBackgroundColour( ui::Theme::backgroundColour( 9 ) );
        setValueBackgroundColour( ui::Theme::backgroundColour( 3 ) );
        setBorderColour( ui::Theme::outlineColour() );
        setValueFontHeight( 26.f );
        setTitleJustification( juce::Justification::centredTop );
        showTitle( ShowTitle );
        showBorder( ShowBorder );
    }

    void KickTempo::setRange( int Min, int Max, int StepSize )
    {
        min  = Min;
        max  = Max;
        step = StepSize;
        norm = juce::NormalisableRange<double>( min, max, step );
        setValue( value, juce::dontSendNotification );
    }

    void KickTempo::setMin( int Min )
    {
        min  = juce::jmin( Min, max );
        norm = juce::NormalisableRange<double>( min, max, step );
        setValue( value, juce::dontSendNotification );
    }

    void KickTempo::setMax( int Max )
    {
        max  = juce::jmax( min, Max );
        norm = juce::NormalisableRange<double>( min, max, step );
        setValue( value, juce::dontSendNotification );
    }

    void KickTempo::setValue( int Value, juce::NotificationType Notify )
    {
        auto nValue = norm.snapToLegalValue( Value );

        if( value != nValue )
        {
            value = nValue;
            sendPercentValueToHardware( value );

            if( Notify != juce::dontSendNotification )
            {
                listeners.callChecked( Component::BailOutChecker( this ), [&]( KickTempo::Listener& l ) { l.tempoChanged( this ); } );
                if( onValueChanged ) onValueChanged( value );
            }
            triggerAsyncUpdate();
        }
    }

    void KickTempo::sliderMoved( float percentValue )
    {
        double tempValue = norm.convertFrom0to1( ( (double)percentValue ) );
        setValue( tempValue );
    }

    void KickTempo::sendPercentValueToHardware( int Value )
    {
        auto valueInPercent = (int)( 100.0 * norm.convertTo0to1( Value ) );
        contracts::DclSignals::sendCommandToDcl( contracts::DclContractsHelper::sendSliderValueInPercentMessage( valueInPercent ) );
    }

    void KickTempo::showEditor()
    {
        if( !isEditing && isEnabled() )
        {
            auto& editor = KickTempoEditor::Instance(); // Static instance... (reuse this again).

            isEditing = true;
            editor.setRange( min, max, def, step, value );
            editor.show( "BPM SELECTOR", [&]( int R ) {
                isEditing = false;
                if( R == UI::BUTTONS::APPLY )
                {
                    setValue( editor.getValue() );
                }
                else
                {
                    sendPercentValueToHardware( getValue() );
                }
                // Regain focus back...
                setFocus( true );
            } );
        }
    }

    void KickTempo::paint( juce::Graphics& g )
    {
        auto b = getLocalBounds();
        if( b.isEmpty() )
            return;

        // Draw outline.
        if( isShowingBorder() )
        {
            mt::utils::renderRoundedRect( g, b, getBorderColour(), getBorderThickness(), getCornerRadius() );
            b = b.reduced( getBorderThickness() );
        }

        // Draw title.
        if( isShowingTitle() )
        {
            auto height = b.proportionOfHeight( getTitleProportion() );
            auto bTitle = b.removeFromTop( height );

            mt::utils::fillRoundedRect( g, b, getTitleBackgroundColour(), getCornerRadius() );
            mt::utils::renderText( g, getTitle().toUpperCase(), bTitle, getTitleColour(), getTitleJustification(), getTitleFontHeight() );
        }

        // Draw Value
        {
            juce::String str( value );
            if( getPadLen() )
                str = str.paddedLeft( getPadChar(), getPadLen() );

            mt::utils::fillRoundedRect( g, b, getValueBackgroundColour(), getCornerRadius() );
            mt::utils::renderText( g, str + getPostFix(), b, getValueColour(), getValueJustification(), getValueFontHeight() );
        }
    }

    void KickTempo::handleAsyncUpdate()
    {
        if( !isUpdatePending() )
            repaint();
    }
}
