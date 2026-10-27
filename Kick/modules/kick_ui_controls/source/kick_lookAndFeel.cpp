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
    KickLooknFeel::KickLooknFeel()
    {
        auto svgSetter = [&]() {
            rotary_background = ui::Theme::getDrawable( "Rotary_background" );
            rotary_knob       = ui::Theme::getDrawable( "Rotary_Knob" );
            button_on         = ui::Theme::getDrawable( "button_on" );
            button_off        = ui::Theme::getDrawable( "button_off" );
            dropdown          = ui::Theme::getDrawable( "DropDown" );
        };

        // Load image assets...
        ui::themeChanged.connect( [svgSetter]() { svgSetter(); } );

        svgSetter();
    }

    void KickLooknFeel::drawLabel( juce::Graphics& g, juce::Label& label )
    {
        mt::MTProperties props( label.getProperties() );
        auto             bounds = label.getLocalBounds().reduced( 1 );

        auto cornerRadius = props.getCornerRadius();
        auto topLeft      = props.isTopLeftCurved();
        auto topRight     = props.isTopRightCurved();
        auto bottomLeft   = props.isBottomLeftCurved();
        auto bottomRight  = props.isBottomRightCurved();

        // Draw Border.
        if( props.isShowingBorder() )
        {
            auto bColor = props.getBorderColour();
            auto bSize  = props.getBorderThickness();
            mt::utils::renderRoundedRect( g, bounds, bColor, bSize, cornerRadius, topLeft, topRight, bottomLeft, bottomRight );
            bounds = bounds.reduced( bSize );
        }

        // Draw Title text.
        if( props.isShowingTitle() )
        {
            auto bBkColour = props.getTitleBackgroundColour();
            auto bTColour  = props.getTitleColour();
            auto tJust     = props.getTitleJustification();
            auto tHeight   = props.getTitleFontHeight();
            auto bTitle    = bounds.removeFromTop( (int)bounds.proportionOfHeight( props.getTitleProportion() ) );
            mt::utils::fillRoundedRect( g, bTitle, bBkColour, cornerRadius, topLeft, topRight, bottomLeft, bottomRight );
            mt::utils::renderText( g, label.getTitle().toUpperCase(), bTitle, bTColour, tJust, tHeight );
        }

        // Draw value background.
        {
            auto vBkColour = props.getValueBackgroundColour();
            if( !vBkColour.isTransparent() )
                mt::utils::fillRoundedRect( g, bounds, vBkColour, cornerRadius, topLeft, topRight, bottomLeft, bottomRight );
        }

        // Draw drop down icon.
        {
            if( props.isShowingDropdown() )
            {
                int dropWidth;
                if( dropdown )
                {
                    dropWidth  = dropdown->getWidth() + 10;
                    auto bDrop = bounds.removeFromRight( dropWidth ).reduced( 5, 0 );
                    // Draw dropdown arrow.
                    dropdown->drawWithin( g, bDrop.toFloat(), juce::RectanglePlacement::centred, 1.0f );
                }
                else
                {
                    dropWidth  = 16;
                    auto bDrop = bounds.removeFromRight( dropWidth ).reduced( 3, 0 );
                    auto cx    = bDrop.getCentreX();
                    auto cy    = bDrop.getCentreY();
                    // Draw manual render.
                    juce::Path triangle;
                    triangle.addTriangle( (float)( cx - 4 ), (float)( cy - 4 ), (float)( cx + 4 ), (float)( cy - 4 ), (float)cx, (float)( cy + 4 ) );
                    g.setColour( props.getHighlightColour() );
                    g.fillPath( triangle );
                }
            }
        }

        // Trim margin, if required.
        {
            auto vJust = props.getValueJustification();
            if( vJust.testFlags( juce::Justification::left | juce::Justification::topLeft | juce::Justification::bottomLeft | juce::Justification::centredLeft ) )
                bounds.removeFromLeft( (int)props.getPaddingLeft() );
            if( vJust.testFlags( juce::Justification::right | juce::Justification::topRight | juce::Justification::bottomRight | juce::Justification::centredRight ) )
                bounds.removeFromRight( (int)props.getPaddingRight() );

            drawLabelText( g, label, props, bounds );
        }
    }

    void KickLooknFeel::drawLabelText( juce::Graphics& g, juce::Label& label, mt::MTProperties& props, juce::Rectangle<int>& bounds )
    {
        auto vColour      = props.getValueColour();
        auto vJust        = props.getValueJustification();
        auto vHeight      = props.getValueFontHeight();
        auto vProgression = props.getScrollProgression();

        const auto labelTextArea = bounds;
        auto       labelText     = label.getText();

        // Check if there is any padding to be done...
        auto padLen = props.getPadLen();
        if( 0 < padLen )
        {
            auto pad     = props.getPadString();
            auto padText = juce::String::repeatedString( pad, padLen );
            labelText    = padText + label.getText();
        }

        g.setColour( vColour );
        g.setFont( vHeight );
        // Text Width is greater It starts scroll when mouse hover
        if( label.getFont().getStringWidth( labelText ) > labelTextArea.getWidth() && vProgression > 0.f )
        {
            const auto           optimumTextWidth = juce::Font( ( vHeight ) ).getStringWidth( labelText );
            juce::Rectangle<int> textBounds       = labelTextArea;
            const auto           xTranslation     = optimumTextWidth - labelTextArea.getWidth();
            textBounds                            = juce::Rectangle<int>{
                labelTextArea.getX() - (int)( xTranslation * vProgression ),
                bounds.getY(),
                optimumTextWidth,
                bounds.getHeight()
            };
            g.drawFittedText( labelText, textBounds, vJust, 1, 1.f );
        }
        else
            g.drawFittedText( labelText, bounds, vJust, 1, 1.f );
    }

    void KickLooknFeel::drawButtonBackground( juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
                                              bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown )
    {
        mt::MTProperties props( button.getProperties() );
        auto             bounds = button.getLocalBounds();

        if( bounds.getWidth() > appWidth * .2 )
            bounds.removeFromRight( 10 );

        // Draw title area
        if( props.isShowingTitle() )
        {
            auto bTitle = bounds.removeFromTop( bounds.proportionOfHeight( props.getTitleProportion() ) );
            mt::utils::fillRoundedRect( g, bTitle, props.getTitleBackgroundColour(), props.getCornerRadius() );
        }

        // Draw button text area
        auto outlineColour = ui::Theme::outlineColour( 1 );
        auto outlineWidth  = 2;
        auto alpha         = props.getValueBackgroundColour().getAlpha();

        if( shouldDrawButtonAsDown || ( button.isToggleable() && button.getToggleState() ) )
        {
            //button_on->drawWithin( g, bounds.toFloat(), juce::RectanglePlacement::stretchToFit, 1.0f );
            mt::utils::fillRoundedRect( g, bounds, ( ui::Theme::backgroundColour( 6 ).withMultipliedBrightness( .25f ).withAlpha( alpha ) ) );
            outlineColour = ui::Theme::outlineColour( 1 );
        }
        else
        {
            //button_off->drawWithin( g, bounds.toFloat(), juce::RectanglePlacement::stretchToFit, 1.0f );
            mt::utils::fillRoundedRect( g, bounds, props.getValueBackgroundColour() );
            outlineColour = ui::Theme::outlineColour( 2 );
        }

        // Draw Outline
        mt::utils::renderRoundedRect( g, bounds, outlineColour, outlineWidth, 5.f );

        // Draw marker, if button is highlighted
        if( shouldDrawButtonAsHighlighted )
            mt::utils::fillRoundedRect( g, bounds.reduced( 20, 0 ).withTrimmedTop( (int)bounds.proportionOfHeight( 1.0f - ( 4.f / bounds.getHeight() ) ) ), ui::Theme::foregroundColour( 1 ), 1.f );
    }

    void KickLooknFeel::drawButtonText( juce::Graphics& g, juce::TextButton& button, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown )
    {
        mt::MTProperties props( button.getProperties() );
        auto             bounds = button.getLocalBounds().reduced( 2 );

        // Draw button title.
        if( props.isShowingTitle() )
        {
            auto bTitle   = bounds.removeFromTop( bounds.proportionOfHeight( props.getTitleProportion() ) );
            auto bTColour = props.getTitleColour();
            auto tJust    = props.getTitleJustification();
            mt::utils::renderText( g, button.getTitle().toUpperCase(), bTitle.reduced( 5 ), bTColour, tJust );
        }

        // Draw button text
        drawButtonText( g, button, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown, props, bounds );
    }

    void KickLooknFeel::drawButtonText( juce::Graphics& g, juce::TextButton& button, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown, mt::MTProperties& props, juce::Rectangle<int>& bounds )
    {
        auto bValueColour = props.getValueColour();
        auto vColor       = button.isEnabled() ? bValueColour : ui::Theme::textColour( 3 );
        auto vJust        = props.getValueJustification();
        auto vFont        = juce::Font( ( props.getValueFontHeight() ) );
        auto vProgression = props.getScrollProgression();

        /*if( !( shouldDrawButtonAsDown || ( button.isToggleable() && button.getToggleState() ) ) )
            vColor = vColor.withMultipliedBrightness( 0.5f );*/
        if( !button.isEnabled() )
            vColor = vColor.withMultipliedBrightness( 0.8f );

        auto textBounds = bounds;
        if( vFont.getStringWidth( button.getButtonText() ) > bounds.getWidth() && vProgression > 0.f )
        {
            auto newX = bounds.getX() - (int)( ( vFont.getStringWidth( button.getButtonText() ) - bounds.getWidth() ) * vProgression );
            auto newW = vFont.getStringWidth( button.getButtonText() );
            textBounds.setX( newX );
            textBounds.setWidth( newW );
        }

        g.setColour( vColor );
        g.setFont( vFont );
        g.drawFittedText( button.getButtonText(), textBounds, vJust, 1, 1.f );
    }

    void KickLooknFeel::drawTabButton( juce::TabBarButton& button, juce::Graphics& g, bool /*isMouseOver*/, bool /*isMouseDown*/ )
    {
        const int  margin = 0, padding = 0;
        auto       activeArea = button.getActiveArea();
        const bool isFrontTab = button.isFrontTab();

        mt::MTProperties props( button.getProperties() );
        // Create shape for tab button...
        auto bounds = activeArea;
        if( isFrontTab )
        {
            switch( button.getTabbedButtonBar().getOrientation() )
            {
                break;
                case juce::TabbedButtonBar::TabsAtLeft:
                    bounds = activeArea.reduced( 0, margin );
                    bounds.removeFromLeft( margin );
                    break;
                case juce::TabbedButtonBar::TabsAtRight:
                    bounds = activeArea.reduced( 0, margin );
                    bounds.removeFromRight( margin );
                    break;
                case juce::TabbedButtonBar::TabsAtBottom:
                    bounds = activeArea.reduced( margin, 0 );
                    bounds.removeFromBottom( margin );
                    break;
                case juce::TabbedButtonBar::TabsAtTop: bounds = activeArea.reduced( margin, 0 ); bounds.removeFromTop( margin );
            }
        }
        else
        {
            activeArea.removeFromBottom( padding );
            bounds = activeArea.reduced( margin );
        }

        juce::Path p;
        p.addRectangle( bounds );

        // Fill background colour...
        g.setColour( button.findColour( juce::TextButton::buttonColourId ) );
        //g.fillPath( p );

        // Fill button outline...
        g.setColour( button.findColour( isFrontTab ? juce::TabbedButtonBar::frontOutlineColourId : juce::TabbedButtonBar::tabOutlineColourId ) );
        // g.strokePath( p, juce::PathStrokeType( isFrontTab ? 1.5f : 1.0f ) );

        // Write button text ...
        juce::AffineTransform t;
        switch( button.getTabbedButtonBar().getOrientation() )
        {
            break;
            case juce::TabbedButtonBar::TabsAtLeft:
                t = t.rotated( juce::MathConstants<float>::pi * -0.5f );
                break;
            case juce::TabbedButtonBar::TabsAtRight: t = t.rotated( juce::MathConstants<float>::pi * 0.5f );
        }
        g.addTransform( t );
        if( isFrontTab ) bounds.removeFromBottom( padding );
        auto text = button.getButtonText();
        if( ( text == "OFF" ) || ( text == "AUTO" ) || ( text == "MAN" ) )
        {
            if( text == "OFF" )
                loadImage = ui::Theme::getDrawable( "AuditionOff" );
            else if( text == "AUTO" )
                loadImage = ui::Theme::getDrawable( "AuditionAuto" );
            else if( text == "MAN" )
                loadImage = ui::Theme::getDrawable( "AuditionMan" );

            loadImage->drawWithin( g, bounds.toFloat(), juce::RectanglePlacement::fillDestination, 1.0f );
        }
        else
        {
            auto valueColour = juce::Colour( props.getValueColour() );
            g.setColour( button.findColour( juce::TextButton::buttonColourId ) );
            g.fillRoundedRectangle( bounds.toFloat(), 5 );
            g.setColour( ui::Theme::outlineColour( 2 ) );
            g.drawRoundedRectangle( bounds.toFloat(), 5, 2 );
            g.setColour( valueColour );
            g.setFont( 18 );
            g.drawFittedText( button.getButtonText().trim(), bounds, juce::Justification::centred, 1 );
        }

        KickMultiSelectTabBarButton* multiSelectButton = dynamic_cast<KickMultiSelectTabBarButton*>( &button );
        if( multiSelectButton )
        {
            auto                 textRect = button.getTextArea();
            juce::Rectangle<int> multiCountRect( 0, 0, textRect.getWidth(), 12 );
            float                drawStartx = multiCountRect.getCentreX() - ( ( multiSelectButton->getNumOfSelection() * 7 + ( multiSelectButton->getNumOfSelection() - 1 * 3.0f ) ) / 2.0f );
            for( int i = 0; i < multiSelectButton->getNumOfSelection(); i++ )
            {
                if( i == multiSelectButton->getCurrentSelection() )
                    g.setColour( ui::Theme::multiSelectTabActiveColour() );
                else
                    g.setColour( ui::Theme::multiSelectTabInActiveColour() );
                // g.fillEllipse( drawStartx, 8, 7, 7 );
                drawStartx += 7 + 3;
            }
        }
    }

    void KickLooknFeel::drawLinearSlider( juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float /*minSliderPos*/,
                                          float /* maxSliderPos*/, const juce::Slider::SliderStyle /*style*/, juce::Slider& slider )
    {
        mt::MTProperties props( slider.getProperties() );
        if( slider.isBar() )
        {
            // Slider area...
            bool                   bHoriz = slider.isHorizontal();
            juce::Rectangle<float> sliderRect{ (float)x, (float)y, (float)width, (float)height };

            juce::Rectangle<float> fillRect;
            juce::Rectangle<float> knobRect;
            juce::Rectangle<int>   tRect;
            juce::Rectangle<int>   vRect;
            juce::ColourGradient   fillColour;

            if( props.isShowingKnob() )
            {
                sliderRect = { x + 10.f, (float)y, width - 20.f, (float)height };
                if( bHoriz )
                {
                    knobRect = { (float)x, (float)y, 6.f, (float)height };
                    knobRect.setX( ( sliderPos - (float)x ) - 3.f );
                }
                else
                {
                    knobRect = { (float)x, (float)y, (float)width, 15.f };
                    knobRect.setY( sliderPos - (float)y - 3.f );
                }
            }

            if( props.isShowingTitle() )
            {
                auto w = ( props.getTitleProportion() * ( /*bHoriz ? height : width*/ height ) );
                tRect  = sliderRect.removeFromTop( w ).toNearestInt();
                g.setColour( props.getTitleColour() );
                g.setFont( props.getTitleFontHeight() );
                g.drawFittedText( slider.getTitle().toUpperCase(), tRect, props.getTitleJustification(), 1 );
            }

            if( props.isShowingValue() )
            {
                auto w = ( props.getValueProportion() * ( /*bHoriz ? height : width*/ height ) );
                vRect  = sliderRect.removeFromBottom( w ).toNearestInt();
                g.setColour( props.getValueColour() );
                g.setFont( props.getValueFontHeight() );
                g.drawFittedText( slider.getTextFromValue( slider.getValue() ), vRect, props.getValueJustification(), 1 );
            }

            if( props.isCentered() )
            {
                auto halfWidth       = sliderRect.proportionOfWidth( 0.5f );
                auto halfHeight      = sliderRect.proportionOfHeight( 0.5f );
                auto center          = bHoriz ? ( x + halfWidth ) : ( y + halfHeight );
                auto lessThanHallf   = center > sliderPos;
                auto isValueCentered = center == sliderPos;

                if( bHoriz )
                {
                    fillColour = juce::ColourGradient::horizontal( juce::Colour( 0xff, 0x5c, 0x00 ), juce::Colour( 0xff, 0x5c, 0x00 ), sliderRect );
                    // Recalculate fill area..
                    if( !isValueCentered )
                    {
                        fillRect = lessThanHallf ? sliderRect.withTrimmedRight( halfWidth ) : sliderRect.withTrimmedLeft( halfWidth );
                        fillRect = lessThanHallf ? fillRect.removeFromRight( ( center - sliderPos ) ) : fillRect.removeFromLeft( ( sliderPos - center ) );
                    }
                    else
                        fillRect = sliderRect.withSizeKeepingCentre( 3.f, sliderRect.getHeight() );
                }
                else
                {
                    fillColour = juce::ColourGradient::vertical( juce::Colour( 0xff, 0x5c, 0x00 ), juce::Colour( 0xff, 0x5c, 0x00 ), sliderRect );

                    // Recalculate fill area..
                    if( !isValueCentered )
                    {
                        fillRect = lessThanHallf ? sliderRect.withTrimmedBottom( halfHeight ) : sliderRect.withTrimmedTop( halfHeight );
                        fillRect = lessThanHallf ? fillRect.removeFromBottom( ( center - sliderPos ) ) : fillRect.removeFromTop( ( sliderPos - center ) );
                    }
                    else
                        fillRect = sliderRect.withSizeKeepingCentre( sliderRect.getWidth(), 3.f );
                }
                fillColour.addColour( 0.5f, juce::Colour( 0xff, 0xb6, 0x08 ) );
            }
            else
            {
                if( bHoriz )
                {
                    fillColour = juce::ColourGradient::horizontal( juce::Colour( 0xff, 0xb6, 0x08 ), juce::Colour( 0xff, 0x5c, 0x00 ), sliderRect );
                    fillRect   = sliderRect.withTrimmedRight( ( x + width - sliderPos ) );
                }
                else
                {
                    fillColour = juce::ColourGradient::vertical( juce::Colour( 0xff, 0xb6, 0x08 ), juce::Colour( 0xff, 0x5c, 0x00 ), sliderRect );
                    fillRect   = sliderRect.withTrimmedTop( ( sliderPos - y ) );
                }
            }

            g.saveState();
            juce::Path p;
            p.addRoundedRectangle( sliderRect, 5.f );
            g.reduceClipRegion( p );

            // Fill background...
            g.setColour( juce::Colours::black );
            g.fillPath( p );

            // Slider fill area.
            g.setGradientFill( fillColour );
            g.fillRect( fillRect );

            g.restoreState();

            // Slider Knob area.
            if( props.isShowingKnob() )
            {
                mt::utils::fillRoundedRect( g, knobRect.toNearestInt(), juce::Colours::aqua, 5.f );
            }
        }
    }

    void KickLooknFeel::drawRotarySlider( juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                                          const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider )
    {
        auto outline = slider.findColour( juce::Slider::rotarySliderOutlineColourId );
        auto fill    = slider.findColour( juce::Slider::rotarySliderFillColourId );

        // Slider area...
        mt::MTProperties       props( slider.getProperties() );
        juce::Rectangle<float> sliderRect{ (float)x, (float)y, (float)width, (float)height };
        float                  w = (int)( 0.22f * height );
        // Draw Title...
        if( props.isShowingTitle() )
        {
            auto tRect = sliderRect.removeFromTop( w ).toNearestInt();
            g.setColour( props.getTitleColour() );
            g.setFont( props.getTitleFontHeight() );
            g.drawFittedText( slider.getTitle().toUpperCase(), tRect, props.getTitleJustification(), 1 );
        }

        // Draw Value...
        if( props.isShowingValue() )
        {
            auto vRect = sliderRect.removeFromBottom( w ).toNearestInt();
            g.setColour( props.getValueColour() );
            g.setFont( props.getValueFontHeight() );
            g.drawFittedText( slider.getTextFromValue( slider.getValue() ), vRect, props.getValueJustification(), 1 );
        }

        if( slider.isEnabled() )
        {
            auto radius      = juce::jmin( sliderRect.getWidth(), sliderRect.getHeight() ) / 2.0f;
            auto lineW       = juce::jmin( 4.2f, radius * 0.5f );
            auto arcRadius   = radius - lineW * 0.5f;
            auto innerRadius = arcRadius * 2 - ( 3.5f * lineW );

            // Draw background arc...
            mt::utils::renderArc( g, sliderRect, arcRadius, rotaryStartAngle, rotaryEndAngle, outline, lineW );

            // Calculate angles for value arc
            float startAngle = 0, endAngle = 0;
            float centerAngle = ( ( rotaryEndAngle - rotaryStartAngle ) / 2 ) + rotaryStartAngle;
            if( !props.isCentered() )
            {
                startAngle = rotaryStartAngle;
                endAngle   = rotaryStartAngle + sliderPos * ( rotaryEndAngle - rotaryStartAngle );
            }
            else
            {
                startAngle = centerAngle;
                endAngle   = centerAngle + ( ( sliderPos - 0.5 ) * ( rotaryEndAngle - rotaryStartAngle ) );
            }

            // Draw filled arc...
            mt::utils::renderArc( g, sliderRect, arcRadius, startAngle, endAngle, fill, lineW );

            // Thumb...
            auto knob_rect         = juce::Rectangle<float>( innerRadius, innerRadius ).withCentre( sliderRect.getCentre() ).toFloat();
            auto knobRotationAngle = endAngle - centerAngle;

            // Knob Background
            mt::utils::renderSVGToFit( g, rotary_background.get(), knob_rect );

            // Knob Pointer
            mt::utils::renderSVGToFit( g, rotary_knob.get(), knob_rect, knobRotationAngle );
        }
    }

    void KickLooknFeel::drawToggleButton( juce::Graphics& g, juce::ToggleButton& button,
                                          bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown )
    {
        auto  fontSize       = juce::jmin( 15.0f, (float)button.getHeight() * 0.75f );
        float x              = (float)( button.getWidth() - button.getHeight() );
        float widthAndHeight = (float)button.getHeight();

        drawTickBox( g, button, x, 0,
                     widthAndHeight, widthAndHeight,
                     button.getToggleState(),
                     button.isEnabled(),
                     shouldDrawButtonAsHighlighted,
                     shouldDrawButtonAsDown );

        g.setColour( button.findColour( juce::ToggleButton::textColourId ) );
        g.setFont( fontSize );

        if( !button.isEnabled() )
            g.setOpacity( 0.5f );

        g.drawFittedText( button.getButtonText(),
                          button.getLocalBounds().withTrimmedRight( button.getWidth() - button.getHeight() - 10 ),
                          juce::Justification::centredRight, 10 );
    }

    void KickLooknFeel::drawScrollbar( juce::Graphics& g, juce::ScrollBar& scrollbar, int x, int y, int width, int height,
                                       bool isScrollbarVertical, int thumbStartPosition, int thumbSize, bool isMouseOver, [[maybe_unused]] bool isMouseDown )
    {
        juce::Rectangle<int> thumbBounds;

        if( isScrollbarVertical )
            thumbBounds = { x, thumbStartPosition, width, thumbSize };
        else
            thumbBounds = { thumbStartPosition, y, thumbSize, height };

        g.setColour( juce::ScrollBar::ColourIds::thumbColourId ? ui::Theme::backgroundColour( 6 ).brighter( 0.25f ) : ui::Theme::backgroundColour( 6 ) );
        g.fillRoundedRectangle( thumbBounds.reduced( 1 ).toFloat(), 4.0f );
    }

    void KickLooknFeel::fillTextEditorBackground( juce::Graphics& g, int width, int height, juce::TextEditor& textEditor )
    {
        mt::utils::fillRoundedRect( g, { 0, 0, width, height }, textEditor.findColour( juce::TextEditor::backgroundColourId ), 6.f );
    }

    std::unique_ptr<juce::FocusOutline> KickLooknFeel::createFocusOutlineForComponent( juce::Component& )
    {
        struct WindowProperties final : public juce::FocusOutline::OutlineWindowProperties
        {
            juce::Rectangle<int> getOutlineBounds( juce::Component& c ) override
            {
                return c.getScreenBounds();
            }

            void drawOutline( juce::Graphics& g, int width, int height ) override
            {
                g.setColour( ui::Theme::outlineColour( 1 ) );
                g.drawRoundedRectangle( { (float)width, (float)height }, 5.0f, 3.0f );
            }
        };

        return std::make_unique<juce::FocusOutline>( std::make_unique<WindowProperties>() );
    }
} // namespace :: ui
