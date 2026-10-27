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
    /// KICK Button
    //////////////////////

    KickButton::KickButton( const juce::String& Name, const juce::String& buttonText )
        : mt::MTProperties( this )
    {
        setLookAndFeel( KickLooknFeel::getLnf() );
        setViewportIgnoreDragFlag( false );

        setName( Name );
        setTitle( Name );
        setButtonText( buttonText.length() ? buttonText : Name );

        setTitleColour( ui::Theme::textColour( 1 ) );
        setValueColour( ui::Theme::textColour( 2 ) );

        setTitleBackgroundColour( ui::Theme::backgroundColour( 9 ) );
        setValueBackgroundColour( ui::Theme::backgroundColour( 3 ) );

        setBorderColour( ui::Theme::outlineColour() );
    }

    void KickButton::setColours( juce::Colour TextColour, juce::Colour OutlineColour, juce::Colour BackgroundColour, int OutlineThickness )
    {
        setValueColour( TextColour );
        setValueBackgroundColour( BackgroundColour );
        setBorderColour( OutlineColour );
        setBorderThickness( OutlineThickness );
    }

    void KickButton::setNormalized( float Value )
    {
        setToggleState( ( Value >= 0.5f ) ? true : false, juce::sendNotification );
    }

    float KickButton::getNormalized()
    {
        return getToggleState() ? 1.f : 0.f;
    }

    void KickButton::mouseDown( const juce::MouseEvent& e )
    {
        juce::Button::mouseDown( e );
        startTimer( getTimerSpeed() );
    }

    void KickButton::mouseExit( const juce::MouseEvent& e )
    {
        juce::Button::mouseExit( e );
        repaint();
    }

    void KickButton::timerCallback()
    {
        if( !isMouseButtonDown() || getScrollProgression() >= 1.f /*|| !isMouseOverOrDragging()*/ )
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

    //////////////////////
    /// KICK Toggle Button
    //////////////////////

    KickToggleButton::KickToggleButton( const juce::String& Name, const juce::String& OnText, const juce::String& OffText )
        : KickButton( Name )
    {
        setClickingTogglesState( true );
        setToggleButtonText( OnText, OffText );
    }

    void KickToggleButton::setToggleButtonText( const juce::String& OnText, const juce::String& OffText )
    {
        onText  = OnText;
        offText = OffText;
        buttonStateChanged();
        repaint();
    }

    void KickToggleButton::buttonStateChanged()
    {
        KickToggleButton::setButtonText( !isToggleable() ? getButtonText() : ( getToggleState() ? onText : offText ) );
    }

    //////////////////////
    /// KICK CheckBox Button
    //////////////////////
    KickCheckBoxButton::KickCheckBoxButton( const juce::String& buttonName, const juce::String& buttonText )
        : mt::MTProperties( this )
    {
        auto n = buttonText.length() ? buttonText : buttonName;
        setName( n );
        setTitle( n );
        setButtonText( n );
    }

    void KickCheckBoxButton::setNormalized( float Value )
    {
        setToggleState( ( Value >= 0.5f ) ? true : false, juce::sendNotification );
    }

    float KickCheckBoxButton::getNormalized()
    {
        return getToggleState() ? 1.f : 0.f;
    }

    //////////////////////
    /// KICK Image Button
    //////////////////////
    KickImageButton::KickImageButton( const juce::String& buttonName, const juce::String& NormalImage, const juce::String& DownImage, const juce::String& HoverImage, const juce::String& DisabledImage )
    {
        setName( buttonName );
        setButtonText( buttonName );
        setClickingTogglesState( true );
        setImages( NormalImage, DownImage, HoverImage, DisabledImage );
    }

    void KickImageButton::setImages( const juce::String& NormalImage, const juce::String& DownImage, const juce::String& HoverImage, const juce::String& DisabledImage )
    {
        if( NormalImage.isNotEmpty() )
        {
            imgNormal = ui::Theme::getDrawable( NormalImage );
            if( imgNormal ) imgNormal->setInterceptsMouseClicks( false, false );
        }

        if( DownImage.isNotEmpty() )
        {
            imgDown = ui::Theme::getDrawable( DownImage );
            if( imgDown ) imgDown->setInterceptsMouseClicks( false, false );
        }

        if( HoverImage.isNotEmpty() )
        {
            imgHover = ui::Theme::getDrawable( HoverImage );
            if( imgHover ) imgHover->setInterceptsMouseClicks( false, false );
        }

        if( DisabledImage.isNotEmpty() )
        {
            imgDisabled = ui::Theme::getDrawable( DisabledImage );
            if( imgDisabled ) imgDisabled->setInterceptsMouseClicks( false, false );
        }

        repaint();
    }

    void KickImageButton::setTemplateDemoImage( const juce::File& file )
    {
        if( file.existsAsFile() )
        {
            imgNormal = ui::Theme::getDrawable( file );
            if( imgNormal ) imgNormal->setInterceptsMouseClicks( false, false );
        }
    }

    void KickImageButton::setOverlayColours( const juce::Colour& Down, const juce::Colour& Normal, const juce::Colour& Hover )
    {
        downColour   = Down;
        normalColour = Normal;
        hoverColour  = Hover;
    }

    void KickImageButton::paintButton( juce::Graphics& g, bool asHighlighed, bool asDown )
    {
        auto bounds = getLocalBounds();

        if( isShowingBorder() && !getBorderColour().isTransparent() )
            mt::utils::renderRoundedRect( g, bounds, getBorderColour(), getBorderThickness(), getCornerRadius() );

        // Background.
        if( !getBackgroundColour().isTransparent() )
            mt::utils::fillRoundedRect( g, bounds, getBackgroundColour(), getCornerRadius() );

        // Render title
        if( isShowingTitle() )
        {
            auto bt = bounds.removeFromTop( bounds.proportionOfHeight( getTitleProportion() ) );
            mt::utils::fillRoundedRect( g, bt, getTitleBackgroundColour(), getCornerRadius() );
            mt::utils::renderText( g, getTitle().toUpperCase(), bt.reduced( 5 ), getTitleColour(), getTitleJustification(), getTitleFontHeight() );
        }

        // Draw image/
        auto         imgPtr        = ( imgNormal.get() );
        juce::Colour overlayColour = normalColour;

        if( asDown || ( isToggleable() && getToggleState() ) )
        {
            overlayColour = downColour;
            if( imgDown )
                imgPtr = ( imgDown.get() );
        }

        if( asHighlighed )
        {
            overlayColour = hoverColour;
            if( imgHover )
                imgPtr = ( imgHover.get() );
        }
        if( !isEnabled() )
        {
            overlayColour = dimColour;
            if( imgDisabled )
                imgPtr = imgDisabled.get();
        }

        if( imgPtr )
        {
            imgPtr->drawWithin( g, bounds.toFloat(), placementFlags, imgOpacity );
            if( !overlayColour.isTransparent() )
            {
                g.setColour( overlayColour );
                g.fillRoundedRectangle( bounds.toFloat(), getCornerRadius() );
            }
        }
        else
            KickButton::paintButton( g, asHighlighed, asDown );
    }

    void KickImageButton::mouseDoubleClick( const juce::MouseEvent& e )
    {
        if( onDoubleClick )
            onDoubleClick();
    }

    //////////////////////
    /// KICK Color Button
    //////////////////////
    KickColorButton::KickColorButton( const juce::Colour& onColor, const juce::Colour& offColor )
        : onCol( onColor )
        , offCol( offColor )
    {}

    void KickColorButton::setColor( const juce::Colour& onColor, const juce::Colour& offColor )
    {
        onCol  = onColor;
        offCol = offColor;
    }

    void KickColorButton::paint( juce::Graphics& g )
    {
        // Outline
        if( isShowingBorder() )
        {
            mt::utils::renderRoundedRect( g, getLocalBounds(), ui::Theme::backgroundColour( 6 ), 2.f, getCornerRadius() );
        }
        // Fill
        mt::utils::fillRoundedRect( g, getLocalBounds().reduced( 2 ), getToggleState() ? onCol : offCol, getCornerRadius() );
    }

    KickTextEditor::KickTextEditor( const juce::String& value )
        : mt::MTProperties( this )
    {
        setLookAndFeel( KickLooknFeel::getLnf() );
        lookAndFeelChanged();
        setName( value );
        setText( value );
    }

    KickRecordButton::KickRecordButton()
    {
        imgArm       = ui::Theme::getDrawable( "Arm_normal" );
        imgRecording = ui::Theme::getDrawable( "Arm_active" );
        imgOff       = ui::Theme::getDrawable( "sampler_Rec_stop" );
        currentIndex = 0;
    }

    void KickRecordButton::mouseDown( const juce::MouseEvent& e )
    {
        if( currentIndex < 2 )
            ++currentIndex;
        else
            currentIndex = 0;
        repaint();
    }

    void KickRecordButton::paintButton( juce::Graphics& g, bool asHighlighted, bool asDown )
    {
        auto imgPtr = imgArm.get();
        if( currentIndex == 0 )
            imgPtr = imgArm.get();
        if( currentIndex == 1 )
            imgPtr = imgRecording.get();
        if( currentIndex == 2 )
            imgPtr = imgOff.get();

        if( imgPtr )
        {
            imgPtr->drawWithin( g, getLocalBounds().toFloat(), juce::Justification::centred, 1.0f );
        }
        else
            KickToggleButton::paintButton( g, asHighlighted, asDown );
    }

}
