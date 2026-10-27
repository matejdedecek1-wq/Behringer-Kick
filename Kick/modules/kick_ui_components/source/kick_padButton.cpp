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
    //////////////////////
    /// KICK Pad Button
    //////////////////////
    KickPadButton::KickPadButton( const juce::String& buttonName, PadButtonType Type )
        : type( Type )
    {
        setLookAndFeel( KickLooknFeel::getLnf() );
        setBackgroundColour( ui::Theme::backgroundColour() );

        setClickingTogglesState( true );
        addMouseListener( this, true );

        group.setValueJustification( juce::Justification::centred );
        name.setValueJustification( juce::Justification::topLeft );
        name.setFontHeight( 14.0f );
        group.setFontHeight( 18.0f );

        name.setColours( ui::Theme::textColour( 1 ), ui::Theme::backgroundColour( 9 ) );
        group.setColours( ui::Theme::textColour( 2 ), ui::Theme::backgroundColour( 9 ) );
        value.setColour( juce::Label::textColourId, ui::Theme::textColour( 5 ) );
        value.setJustificationType( juce::Justification::centredLeft );

        rec.setOverlayColours( ui::Theme::highlightColour( 1 ) );
        slider.setColour( juce::Slider::backgroundColourId, ui::Theme::backgroundColour( 0 ) );
        panSlider.setColour( juce::Slider::backgroundColourId, ui::Theme::backgroundColour( 0 ) );
        m.setColor( ui::Theme::backgroundColour( 6 ), ui::Theme::backgroundColour( 0 ) );
        lrow1.setValueBackgroundColour( ui::Theme::backgroundColour( 9 ) );
        lrow2.setValueBackgroundColour( ui::Theme::backgroundColour( 9 ) );
        lrow3.setValueBackgroundColour( ui::Theme::backgroundColour( 9 ) );

        slider.setNumDecimalPlacesToDisplay( 0 );
        panSlider.setNumDecimalPlacesToDisplay( 2 );
        slider.setRange( 0, 127 );
        panSlider.setRange( -1, 1 );
        panSlider.setCentered( true );

        setHeaderText( buttonName );

        slider.showValue( true );
        panSlider.showValue( true );

        mt::utils::addChildComponents( this, name, group, value, lrow1, lrow2,
                                       lrow3, solo, mute, rec, m, slider, panSlider, levelMeter );
        mt::utils::disableMouseIntercepts( name, group, value, lrow2, lrow3,
                                           solo, mute, rec, levelMeter );

        setInterceptsMouseClicks( true, true );

        auto displayMute = [&, button = this]( bool state ) {
            m.setToggleState( state, juce::dontSendNotification );
            mute.setToggleState( state, juce::dontSendNotification );

            if( type == PadButtonType::muteMode || type == PadButtonType::muteGroup )
                value.setText( state ? "Muted" : "UnMute", juce::dontSendNotification );

            if( onMuteChanged ) onMuteChanged( button, state );
        };

        m.onStateChange       = [=]() { displayMute( m.getToggleState() ); };
        mute.onStateChange    = [=]() { displayMute( mute.getToggleState() ); };
        slider.onValueChanged = [&, button = this]( double Val ) {
            if( slider.isVisible() )
                value.setText( juce::String( Val ), juce::dontSendNotification );
            if( onSliderChanged )
                onSliderChanged( button, Val );
        };
        panSlider.onValueChanged = [&, button = this]( double Val ) {
            if( panSlider.isVisible() )
                value.setText( juce::String( Val ), juce::dontSendNotification );
            if( onPanChanged )
                onPanChanged( button, Val );
        };

        themeChanged.connect( [&]() {
            solo.setImages( "solo_mini", "solo_mini_active" );
            mute.setImages( "mute_mini", "mute_mini_active" );
            rec.setImages( "Arm_normal" );
        } );
    }

    // Type...
    void KickPadButton::setType( PadButtonType Type )
    {
        type = Type;
        resized();
        repaint();
    }

    // Text components...
    void KickPadButton::setHeaderText( const juce::String& Text )
    {
        //if( isEnabled )
        {
            // name.setTextColour( ui::Theme::textColour( 1 ) );
            name.setTextColour( headerColour );
            name.setText( Text, juce::dontSendNotification );
            setName( Text );
            if( onHeaderChanged )
                onHeaderChanged( this, Text );
        }
    }

    void KickPadButton::setGroupText( const juce::String& Text )
    {
        if( isEnabled )
        {
            group.setColours( groupColour, Text.length() != 0 ? Theme ::backgroundColour( 5 ) : Theme ::backgroundColour( 9 ) );
            group.setText( Text.length() != 0 ? Text.paddedLeft( '0', 2 ) : Text, juce::dontSendNotification );
            if( onGroupChanged )
                onGroupChanged( this, Text );
        }
    }

    void KickPadButton::setRow1Text( const juce::String& Text, juce::Justification Just )
    {
        if( isEnabled )
        {
            lrow1.setValueFontHeight( 20 );
            lrow1.setTextColour( row1Colour );
            lrow1.setJustificationType( Just );
            lrow1.setText( Text, juce::sendNotification );
            if( onRow1Changed )
                onRow1Changed( this, Text );
        }
    }

    void KickPadButton::setRow2Text( const juce::String& Text, juce::Justification Just )
    {
        if( isEnabled )
        {
            lrow2.setTextColour( row2Colour );
            lrow2.setJustificationType( Just );
            lrow2.setText( Text, juce::dontSendNotification );
            if( onRow2Changed )
                onRow2Changed( this, Text );
        }
    }

    void KickPadButton::setRow3Text( const juce::String& Text, juce::Justification Just )
    {
        if( isEnabled )
        {
            lrow3.setTextColour( row3Colour );
            lrow3.setJustificationType( Just );
            lrow3.setText( Text, juce::dontSendNotification );
            if( onRow3Changed )
                onRow3Changed( this, Text );
        }
    }

    void KickPadButton::setHeaderColour( juce::Colour HeaderColour )
    {
        headerColour = HeaderColour;
    }

    void KickPadButton::setGroupColour( juce::Colour GroupColour )
    {
        groupColour = GroupColour;
    }

    void KickPadButton::setRow1Colour( juce::Colour Row1Colour )
    {
        row1Colour = Row1Colour;
    }

    void KickPadButton::setRow2Colour( juce::Colour Row2Colour )
    {
        row2Colour = Row2Colour;
    }

    void KickPadButton::setRow3Colour( juce::Colour Row3Colour )
    {
        row3Colour = Row3Colour;
    }

    // Slider component...
    void KickPadButton::setSliderRange( double Min, double Max, double StepSize )
    {
        if( isEnabled )
        {
            slider.setRange( Min, Max, StepSize );
        }
    }

    void KickPadButton::setSliderValue( double Value )
    {
        if( isEnabled && ( ( type == PadButtonType::levelMode ) || ( type == PadButtonType::insertMode ) ) )
        {
            value.setText( juce::String( Value ), juce::dontSendNotification );
            slider.setValue( Value, juce::dontSendNotification );
        }
    }

    void KickPadButton::setPanSliderValue( double Value )
    {
        if( isEnabled && type == PadButtonType::panMode )
        {
            value.setText( juce::String( Value ), juce::dontSendNotification );
            panSlider.setValue( Value, juce::sendNotification );
        }
    }

    double KickPadButton::getSliderValue()
    {
        return slider.getValue();
    }

    double KickPadButton::getPanSliderValue()
    {
        return panSlider.getValue();
    }

    void KickPadButton::updateLevelMeter( float lRms, float rRms, float lPeak, float rPeak )
    {
        levelMeter.setSignalStrength( lRms, rRms, lPeak, rPeak );
    }

    void KickPadButton::setMute( bool Value, juce::NotificationType Notification )
    {
        if( isEnabled )
        {
            m.setToggleState( Value, Notification );
            mute.setToggleState( Value, Notification );
            if( type == PadButtonType::muteMode || type == PadButtonType::muteGroup )
                value.setText( Value ? "Muted" : "UnMute", juce::dontSendNotification );
        }
    }

    void KickPadButton::setSolo( bool Value, juce::NotificationType Notification )
    {
        if( isEnabled )
        {
            solo.setToggleState( Value, Notification );
            if( onSoloChanged )
                onSoloChanged( this, solo.getToggleState() );
        }
    }

    void KickPadButton::resetBorderColor()
    {
        setBorderColor( DEFAULT_BORDER_COLOR, DEFAULT_BORDER_THICKNESS );
        repaint();
    }

    void KickPadButton::setBorderColor( const juce::Colour& Col, float Thickness )
    {
        if( isEnabled )
        {
            border          = Col;
            borderThickness = Thickness;
            repaint();
            if( onColorChanged )
                onColorChanged( this, Col, Thickness );
        }
    }

    void KickPadButton::setActive( const bool IsActive )
    {
        value.setColour( juce::Label::textColourId, IsActive ? ui::Theme::textColour( 1 ) : ui::Theme::textColour( 5 ) );
        slider.setEnabled( IsActive );
        panSlider.setEnabled( IsActive );
        m.setEnabled( IsActive );
        isEnabled = IsActive;
        repaint();
    }

    void KickPadButton::setSelected( const bool Select )
    {
        if( selected != Select )
        {
            selected = Select;
            repaint();
        }
    }

    void KickPadButton::setHighlight( const bool Highlight, juce::Colour HighlightedColour )
    {
        if( isEnabled )
        {
            isHighlight       = Highlight;
            highlightedColour = HighlightedColour;
            repaint();
        }
    }

    void KickPadButton::setElementsEnable( const bool Enable )
    {
        if( isEnabled )
        {
            isVisible = Enable;
            resized();
        }
    }

    void KickPadButton::setFocusOutline( bool state )
    {
        setHasFocusOutline( state );
    }

    void KickPadButton::setTextColour( const juce::Colour& Text )
    {
        name.setTextColour( Text );
    }

    void KickPadButton::paint( juce::Graphics& g )
    {
        auto br = getLocalBounds();

        float corner      = 6.0f;
        float stateCorner = 5.0f;

        // Background...
        g.setColour( isEnabled ? ui::Theme::backgroundColour( 1 ) : ui::Theme::backgroundColour( 3 ) );
        g.fillRoundedRectangle( br.toFloat(), corner );

        // Button Outline...
        g.setColour( ui::Theme::outlineColour( 3 ) );
        g.drawRoundedRectangle( br.toFloat(), corner, borderThickness );

        // Colour border...
        if( isEnabled )
        {
            auto bounds = br.toFloat().reduced( 2 );
            g.setColour( border );
            g.drawRoundedRectangle( bounds, stateCorner, borderThickness );
            if( isHighlight )
            {
                g.setColour( highlightedColour );
                g.drawRoundedRectangle( bounds, stateCorner, highlightBorderThickness );
            }
        }
    }

    void KickPadButton::resized()
    {
        for( auto it : getChildren() )
            it->setVisible( false );

        auto meters = [&]() { // Meters...
            mt::utils::makeVisible( levelMeter );
            levelMeter.setBoundsRelative( .038f, .22f, .92f, .1f );
        };

        auto hdr1 = [&]() { // Top Row...
            mt::utils::makeVisible( name, solo, rec, mute );
            name.setBoundsRelative( .05f, .05f, .483f, .18f );
            solo.setBoundsRelative( .651f, .05f, .090f, .18f );
            rec.setBoundsRelative( .767f, .05f, .090f, .18f );
            mute.setBoundsRelative( .883f, .05f, .090f, .18f );
        };

        auto hdr2 = [&]() {
            // Top Row...
            mt::utils::makeVisible( name );
            name.setBoundsRelative( .06f, .05f, .7f, .18f );
        };

        auto hdr3 = [&]() {
            name.setVisible( true );
            name.setColours( ui::Theme::textColour( 1 ), ui::Theme::backgroundColour( 9 ) );
            name.setBoundsRelative( .0625f, .08f, .40f, .18f );
        };

        auto hdr4 = [&]() { // Top Row...
            mt::utils::makeVisible( name, group );
            name.setBoundsRelative( .06f, .06f, .34f, .32f );
            group.setBoundsRelative( .78f, .06f, .17f, .32f );
        };

        auto hdr5 = [&]() {
            mt::utils::makeVisible( name, solo, mute );
            name.setBoundsRelative( .06f, .08f, .57f, .15f );
        };

        switch( type )
        {
            default:
            case PadButtonType::with1Row:
            {
                hdr2();
                lrow1.setVisible( true );
                lrow1.setJustificationType( juce::Justification::centred );
                lrow1.setBoundsRelative( .05f, .643f, .9f, .4f );
            }
            break;
            case PadButtonType::with3Rows:
            {
                hdr2();
                mt::utils::makeVisible( lrow1, lrow2, lrow3 );
                lrow1.setBoundsRelative( .058f, .25f, .92f, .285f );
                lrow2.setBoundsRelative( .06f, .528f, .92f, .18f );
                lrow3.setBoundsRelative( .06f, .70f, .92f, .18f );
            }
            break;
            case PadButtonType::colorMode:
            {
                hdr2();
                lrow1.setVisible( true );
                lrow1.setBoundsRelative( .0625f, .68f, 0.875f, .2f );
            }
            break;
            case PadButtonType::levelMode:
            {
                hdr1();
                meters();
                mt::utils::makeVisible( lrow1, slider, value );
                lrow1.setBoundsRelative( .05f, .25f, .9f, .2f );
                slider.setBoundsRelative( .05f, .5f, .9f, .2f );
                value.setBoundsRelative( .05f, .7f, .9f, .24f );
            }
            break;
            case PadButtonType::panMode:
            {
                hdr1();
                meters();
                mt::utils::makeVisible( lrow1, panSlider, value );
                lrow1.setBoundsRelative( .05f, .25f, .9f, .2f );
                panSlider.setBoundsRelative( .05f, .50f, .9f, .2f );
                value.setBoundsRelative( .05f, .7f, .9f, .24f );
            }
            break;
            case PadButtonType::muteMode:
            {
                hdr1();
                meters();
                mt::utils::makeVisible( lrow1, m, value );
                lrow1.setBoundsRelative( .05f, .25f, .9f, .2f );
                m.setBoundsRelative( .05f, .45f, .9f, .25f );
                value.setBoundsRelative( .05f, .7f, .9f, .24f );
            }
            break;
            case PadButtonType::insertMode:
            {
                hdr1();
                meters();
                mt::utils::makeVisible( slider, value );
                slider.setBoundsRelative( .038f, .5f, .92f, .18f );
                value.setBoundsRelative( .038f, .7f, .92f, .24f );
            }
            break;
            case PadButtonType::assignSample:
            {
                hdr3();
                lrow1.setVisible( true );
                lrow1.setBoundsRelative( .0625f, .68f, 0.875f, .2f );
            }
            break;
            case PadButtonType::muteGroup:
            {
                hdr4();
                lrow1.setVisible( true );
                lrow1.setBoundsRelative( .05f, .60f, 0.93f, .30f );
            }
            break;
            case PadButtonType::padMixer:
            {
                hdr5();
                if( isVisible )
                    mt::utils::makeVisible( lrow1, lrow2, solo, mute, levelMeter );
                else
                    mt::utils::makeHidden( lrow1, lrow2, solo, mute, levelMeter );

                lrow1.setBoundsRelative( .06f, .25f, .57f, .15f );
                lrow2.setBoundsRelative( .06f, .42f, .57f, .15f );
                solo.setBoundsRelative( .06f, .64f, .28f, .26f );
                mute.setBoundsRelative( .39f, .64f, .28f, .26f );
                levelMeter.setBoundsRelative( .734f, .08f, .204f, .82f );
            }
            break;
        }
    }

    void KickPadButton::clear()
    {
        isHighlight = selected = false;
        setGroupText( "" );
        setRow1Text( "", juce::Justification::centredLeft );
        setRow2Text( "", juce::Justification::centredLeft );
        setRow3Text( "", juce::Justification::centredLeft );
        setMute( false, juce::dontSendNotification );
        setSliderValue( 0 );
        setPanSliderValue( 0 );
        resetBorderColor();
    }

    void KickPadButton::knobClicked()
    {
        if( onKnobClickedCallback )
            onKnobClickedCallback();
    }

    void KickPadButton::mouseDoubleClick( const juce::MouseEvent& /*event*/ )
    {
        if( onDoubleClicked )
            onDoubleClicked( this, getID() );
#if 0
        if( !isMaximized )
        {
            isMaximized = true;
            auto b = getLocalBounds();
            auto p = getParentComponent();
            KickUtils::showDialog( getName(), this, [&, b, p]( int )
                {
                    isMaximized = false;
            //setBounds( b );
            //p->addAndMakeVisible( this );
            p->repaint();
} );
        }
#endif
    }

    //..
} // namespace ui
