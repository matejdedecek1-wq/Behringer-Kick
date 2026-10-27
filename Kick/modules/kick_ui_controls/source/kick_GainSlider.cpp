///////////////////////////////////////////////////////////////////////////////////////////////////////
/// ///////////////// GainMeter ///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ui
{
    // gainmeter
#define scaleTopMargin       5 // markingTextHeight/2
#define scaleBottomMargin    30
#define scaleMarkingSpace    2
#define levelsSpacing        3
#define sliderHalfWidth      10
#define markingTextHeight    10
#define sliderWidth          2

#define gMargin              6
#define sliderTrackHalfWidth 8
#define labelHeight          30
#define labelHeight1         14
#define Spacing              27
#define KnobWidth            60
#define KnobHeight           44
#define levelMeterWidth      8

    KickGainSlider::KickGainSlider()
        : mt::MTProperties( this )
    {
        setRange( -100.0, 20.0 );
    }

    void KickGainSlider::setRangeAndDisplayValues( const juce::Point<float>& range, const std::vector<float>& displayValues )
    {
        decibles = displayValues;
        setRange( range.getX(), range.getY() );
    }

    void KickGainSlider::setRange( double s, double e )
    {
        min = (float)juce::jmin( s, e );
        max = (float)juce::jmax( s, e );

        normalized = juce::NormalisableRange<double>{ min, max, .0 };
        normalized.setSkewForCentre( -15 );

        setValue( min, juce::dontSendNotification );
    }

    void KickGainSlider::setValue( float valueDb, juce::NotificationType Notify )
    {
        currentDbValue = juce::jlimit( min, max, valueDb );
        setSliderPos( getPixelFromDb( currentDbValue ), Notify );
    }

    void KickGainSlider::setNormalized( float Value )
    {
        setValue( (float)normalized.convertFrom0to1( Value ), juce::sendNotification );
    }

    float KickGainSlider::getNormalized()
    {
        return (float)normalized.convertTo0to1( currentDbValue );
    }

    void KickGainSlider::paint( juce::Graphics& g )
    {
        //    // Draw Label.
        const juce::String& displayValue = currentDbValue < 0 ? juce::String().formatted( "%.2fdB", CommonUtils::convertToPrecision<float>( currentDbValue, 2 ) ) : juce::String().formatted( "+%.2fdB", CommonUtils::convertToPrecision<float>( currentDbValue, 2 ) );
        mt::utils::renderText( g, displayValue, bLabel, ui::Theme::backgroundColour( 6 ), juce::Justification::centred );

        // Draw Marking Lines and Text...
        auto x1 = bMarkingLines.getX() + 20;
        auto x2 = bMarkingLines.getX() + bMarkingLines.getWidth() - 25;
        auto t1 = bMarkingText.getX();
        auto t2 = bMarkingText.getWidth();
        int  i  = 0;
        g.setColour( juce::Colours::white );

        for( auto it : markings )
        {
            auto y = bMarkingLines.getY() + it;
            g.drawLine( x1, y, x2, y, 0.5f );

            juce::Rectangle<int> bText{ x2 + 2, (int)y - 8, t2, 16 };
            mt::utils::renderText( g, juce::String( decibles[i++] ), bText, juce::Colours::white, juce::Justification::centredLeft );
        }

        // Draw Slider track...
        g.setColour( ui::Theme::backgroundColour( 5 ) );
        g.fillRect( bSliderTrack );
        // Draw Slider Knob ...
        auto y0   = bSliderTrack.getY();
        auto y1   = y0 + bSliderTrack.getHeight();
        auto sPos = juce::jlimit( y0, y1, (int)getPixelFromDb( currentDbValue ) );

        auto kx1 = bSliderTrack.getX() - 8;
        auto kw  = bSliderTrack.getWidth() + 16;
        bSliderKnob.setBounds( 8, (int)sPos - 10, KnobWidth, KnobHeight );

        _knobImg->drawAt( g, 8, (int)bSliderKnob.getY() - 10, 1.0f );
    }

    void KickGainSlider::resized()
    {
        // Complete area...
        auto b = getLocalBounds();

        // Value Label...
        bLabel = b.removeFromBottom( labelHeight );
        b.removeFromBottom( 20 );

        // Marking area...
        auto bMarking = b;
        bMarkingLines = bMarking;
        auto w        = ( b.getWidth() >> 1 ) + sliderTrackHalfWidth + 10;
        bMarking.removeFromLeft( w );
        bMarkingText = bMarking.withTrimmedLeft( 3 );

        // Slider Track...
        auto sliderTrackTrimX = ( bMarkingLines.getWidth() >> 1 ) - 2;
        bSliderTrack          = bMarkingLines.reduced( sliderTrackTrimX, 0 );

        // Calculate marking positions with equal spacing...
        markings.clear();
        for( float val : decibles )
            markings.push_back( getPixelFromDb( val ) );

        // Calculate slider knob position...
        setSliderPos( getPixelFromDb( currentDbValue ), juce::dontSendNotification );
        setSliderPos( getPixelFromDb( 60.0f ), juce::dontSendNotification );
    }

    void KickGainSlider::mouseDrag( const juce::MouseEvent& event )
    {
        if( !isSliderDragged )
        {
            if( bSliderKnob.contains( event.mouseDownPosition.toInt() ) )
                isSliderDragged = true;
        }
        else if( event.position.getY() >= 0 )
        {
            setSliderPos( event.position.getY(), juce::sendNotificationAsync );
        }
    }

    void KickGainSlider::mouseUp( const juce::MouseEvent& event )
    {
        isSliderDragged = false;
    }

    void KickGainSlider::mouseDoubleClick( const juce::MouseEvent& event )
    {
        if( !event.mouseWasDraggedSinceMouseDown() && event.position.getY() >= 0 )
            setSliderPos( event.position.getY(), juce::sendNotificationAsync );
    }

    void KickGainSlider::knobRotated( double delta )
    {
        auto sliderPos = bSliderKnob.getCentreY();
        setSliderPos( sliderPos + (float)delta, juce::sendNotificationAsync );
    }

    void KickGainSlider::sliderMoved( float value )
    {
        // Set Value...
        auto sliderPos = bSliderTrack.getHeight();
        setSliderPos( sliderPos * value, juce::sendNotificationAsync );
    }

    void KickGainSlider::knobClicked()
    {
        KickGainSlider* meter = new KickGainSlider();
        meter->setSize( 150, 580 );
        meter->setValue( getValue(), juce::dontSendNotification );
        mt::utils::showDialog( "", meter, [meter, this]( int /*r*/ ) {
            setValue( meter->getValue(), juce::sendNotificationAsync );
            delete meter;
        },
                               { 150, 580 } );
    }

    void KickGainSlider::setSliderPos( float newPos, juce::NotificationType Notify )
    {
        auto y0 = bSliderTrack.getY();
        auto y1 = y0 + bSliderTrack.getHeight();

        auto sPos      = juce::jlimit( y0, y1, (int)newPos );
        currentDbValue = getDbFromPixel( (float)sPos );

        repaint();

        if( Notify != juce::dontSendNotification )
            gainChanged( currentDbValue );
    }

    float KickGainSlider::getDbFromPixel( float pixel )
    {
        auto h   = bSliderTrack.getHeight();
        auto y0  = bSliderTrack.getY();
        auto y1  = y0 + h;
        auto pix = juce::jlimit( y0, y1, (int)pixel );

        if( ( 0 == h ) || ( 0 == pix ) ) return ( 20.f );
        return CommonUtils::getDbFromPixel( normalized, (float)h, (float)pix, true );
    }

    float KickGainSlider::getPixelFromDb( float dbValue )
    {
        auto h  = bSliderTrack.getHeight();
        auto db = juce::jlimit( min, max, dbValue );
        return CommonUtils::getPixelFromDb( normalized, (float)h, db, true );
    }

    void KickGainSlider::setKnobImage( const juce::String& image )
    {
        _knobImg = ui::Theme::getDrawable( image );
    }
}
