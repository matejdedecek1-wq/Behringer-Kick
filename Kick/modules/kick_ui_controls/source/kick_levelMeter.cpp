
namespace ui
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ///////////////// KickLevelMeter ///////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    KickLevelMeter::KickLevelMeter( KickLevelMeter::Style Style )
    {
        setInterceptsMouseClicks( false, false );
        setRange( -100, 20 );
        setStyle( Style );
    }

    void KickLevelMeter::setStyle( KickLevelMeter::Style Style )
    {
        style = Style;
        resized();
    }

    void KickLevelMeter::setRange( double start, double end )
    {
        startValue = start;
        endValue   = end;

        norRange = juce::NormalisableRange<double>{ startValue, endValue, .0 };
        norRange.setSkewForCentre( -15 );
    }

    void KickLevelMeter::setSignalStrength( const std::vector<float>& sigStrength )
    {
        auto len = (int)sigStrength.size();
        if( 1 <= len ) lrms = sigStrength[0];
        if( 2 <= len ) rrms = sigStrength[1];
        if( 3 <= len ) lpeak = sigStrength[2];
        if( 4 <= len ) rpeak = sigStrength[3];
        repaint();
    }

    void KickLevelMeter::setSignalStrength( float lRms, float rRms, float lPeak, float rPeak )
    {
        lrms  = lRms;
        rrms  = rRms;
        lpeak = lPeak;
        rpeak = rPeak;
        repaint();
    }

    void KickLevelMeter::showTitle( const bool showTitle )
    {
        isShowTitle = showTitle;
    }

    void KickLevelMeter::paint( juce::Graphics& g )
    {
        g.setColour( ui::Theme::backgroundColour( 0 ).contrasting( 0.3f ) );

        if( style == Style::Vertical )
        {
            // Draw Marking Lines and Text...
            auto x1 = Marking_left.getX();
            auto x3 = Marking_right.getX();

            auto x2    = Marking_left.getX() + Marking_left.getWidth();
            auto x4    = Marking_right.getX() + Marking_right.getWidth();
            auto markY = 2;

            for( auto it : markings )
            {
                g.drawLine( x1, markY, x2, markY, 0.5f );
                g.drawLine( x3 + 5, markY, x4 + 10, markY, 0.5f );
                markY = markY + 27;
            }

            g.drawLine( x1, bLeft.getHeight(), x2, bLeft.getHeight(), 0.5f );
            g.drawLine( x3 + 5, bRight.getHeight(), x4 + 10, bRight.getHeight(), 0.5f );
        }

        if( style == Style::HorizantalWithMarks )
        {
            auto x1 = Marking_left.getX() + 2;

            auto y1 = Marking_left.getY() + 4;
            auto y2 = Marking_left.getHeight();
            auto y3 = Marking_right.getX() + 5;
            auto y4 = Marking_right.getHeight() + y3 - 4;

            auto h = Marking_left.getWidth() / ( markings.size() - 1 );
            for( auto it : markings )
            {
                g.drawLine( x1, y1, x1, y2, .5f );
                g.drawLine( x1, y3, x1, y4, .5f );
                x1 += h;
            }
        }

        if( style == Style::VerticalWithMarks )
        {
            auto x1 = Marking_left.getX();
            auto x2 = Marking_left.getWidth() + x1;
            auto x3 = Marking_right.getX();
            auto x4 = Marking_right.getWidth() + x3;

            auto markY = Marking_left.getY() + 1;

            auto space = Marking_left.getHeight() / ( markings.size() - 1 );
            for( auto it : markings )
            {
                g.drawLine( x1, markY, x2, markY, .5f );
                g.drawLine( x3, markY, x4, markY, .5f );
                markY += space;
            }
        }

        // fill level rects
        g.setColour( ui::Theme::backgroundColour( 5 ) );
        g.fillRoundedRectangle( bLeft.toFloat(), 1.f );
        g.fillRoundedRectangle( bRight.toFloat(), 1.f );

        // draw levels
        {
            auto leftFill  = bLeft;
            auto rightFill = bRight;

            if( Style::Vertical == style )
            {
                leftFill.removeFromTop( getPixelFromDb( lrms, true ) );
                rightFill.removeFromTop( getPixelFromDb( rrms, true ) );

                mt::utils::renderText( g, "L+R", bLabel, ui::Theme::textColour( 1 ), juce::Justification::centred );
            }
            else if( Style::Horizantal == style || style == Style::HorizantalWithMarks )
            {
                leftFill.removeFromRight( getPixelFromDb( lrms, true ) );
                rightFill.removeFromRight( getPixelFromDb( rrms, true ) );

                auto h = bLabel.getHeight() * 0.5f;
                auto l = bLabel.withTrimmedBottom( h );
                auto r = bLabel.withTrimmedTop( h );
                mt::utils::renderText( g, "L", l, ui::Theme::textColour( 1 ), juce::Justification::centred );
                mt::utils::renderText( g, "R", r, ui::Theme::textColour( 1 ), juce::Justification::centred );
            }
            else if( Style::VerticalWithoutMarks == style )
            {
                leftFill.removeFromTop( getPixelFromDb( lrms, true ) );
                rightFill.removeFromTop( getPixelFromDb( rrms, true ) );
            }
            else if( Style::VerticalWithMarks == style )
            {
                leftFill.removeFromTop( getPixelFromDb( lrms, true ) );
                rightFill.removeFromTop( getPixelFromDb( rrms, true ) );

                mt::utils::renderText( g, "L+R", bLabel, ui::Theme::textColour( 1 ), juce::Justification::centred );
            }

            g.setGradientFill( leftGradient );
            g.fillRect( leftFill );
            g.setGradientFill( rightGradient );
            g.fillRect( rightFill );
        }

        // draw max values
        {
            auto leftFill  = bLeft;
            auto rightFill = bRight;

            if( Style::Vertical == style || Style::VerticalWithoutMarks == style || Style::VerticalWithMarks == style )
            {
                leftFill.removeFromTop( getPixelFromDb( lpeak, true ) );
                leftFill.removeFromBottom( leftFill.getHeight() - 3 );
                rightFill.removeFromTop( getPixelFromDb( rpeak, true ) );
                rightFill.removeFromBottom( rightFill.getHeight() - 3 );
            }
            else if( Style::Horizantal == style || Style::HorizantalWithMarks == style )
            {
                leftFill.removeFromRight( getPixelFromDb( lpeak, true ) );
                leftFill.removeFromLeft( leftFill.getWidth() - 3 );
                rightFill.removeFromRight( getPixelFromDb( rpeak, true ) );
                rightFill.removeFromLeft( rightFill.getWidth() - 3 );
            }

            g.setColour( ui::Theme::meterColour( 2 ) );
            g.fillRect( leftFill );
            g.fillRect( rightFill );
        }
    }

    float KickLevelMeter::getPixelFromDb( float dbValue, bool vertical )
    {
        auto db = juce::jlimit<double>( startValue, endValue, (double)dbValue );
        return CommonUtils::getPixelFromDb( norRange, meterHeight, db, vertical );
    }

    void KickLevelMeter::resized()
    {
        if( getLocalBounds().isEmpty() )
            return;

        if( Style::Vertical == style )
        {
            auto b = getLocalBounds().reduced( levelMeterMargin, 0 );

            // Value Label...
            bLabel = b.removeFromBottom( labelHeight );
            b.removeFromBottom( gMargin );
            const int knobWidth = 7; // Adjust the width as needed

            bLeft.setBounds( b.getX() + 12, b.getY(), knobWidth, b.getHeight() );
            Marking_left = b.removeFromLeft( 15 );
            bRight.setBounds( b.getX() + 7, b.getY(), knobWidth, b.getHeight() );
            Marking_right = b.removeFromRight( 15 );

            leftGradient  = juce::ColourGradient::vertical( juce::Colours::red, juce::Colours::green, bLeft );
            rightGradient = juce::ColourGradient::vertical( juce::Colours::red, juce::Colours::green, bRight );

            meterHeight = static_cast<float>( bLeft.getHeight() );

            leftGradient  = juce::ColourGradient::vertical( juce::Colours::red, juce::Colours::green, bLeft );
            rightGradient = juce::ColourGradient::vertical( juce::Colours::red, juce::Colours::green, bRight );

            meterHeight = static_cast<float>( bLeft.getHeight() );

            markings.clear();
            for( float a : decibles )
            {
                markings.push_back( getPixelFromDb( a, true ) );
            }
        }
        else if( style == Style::Horizantal )
        {
            auto b = getLocalBounds().reduced( 0, levelMeterMargin );
            // Value Label...
            auto lWidth = juce::jmin( labelHeight, b.getHeight() / 2 );
            bLabel      = b.removeFromLeft( lWidth );
            b.removeFromLeft( gMargin );

            auto h        = ( b.getHeight() - 4 ) / 2;
            bLeft         = b.removeFromTop( h );
            bRight        = b.removeFromBottom( h );
            leftGradient  = juce::ColourGradient::horizontal( juce::Colours::green, juce::Colours::red, bLeft );
            rightGradient = juce::ColourGradient::horizontal( juce::Colours::green, juce::Colours::red, bRight );

            meterHeight = static_cast<float>( bLeft.getWidth() );
        }
        else if( style == Style::VerticalWithoutMarks )
        {
            auto b = getLocalBounds().reduced( ( getWidth() - ( levelMeterWidth * 2 + 2 ) ) / 2, 0 );
            bLeft  = b.removeFromLeft( levelMeterWidth );
            bRight = b.removeFromRight( levelMeterWidth );

            leftGradient  = juce::ColourGradient::vertical( juce::Colours::red, juce::Colours::green, bLeft );
            rightGradient = juce::ColourGradient::vertical( juce::Colours::red, juce::Colours::green, bRight );

            meterHeight = static_cast<float>( bLeft.getHeight() );
        }
        else if( style == Style::HorizantalWithMarks )
        {
            auto b = getLocalBounds();
            // Value Label...

            auto lWidth = juce::jmin( labelHeight, b.getHeight() / 2 );
            bLabel      = b.removeFromLeft( lWidth );
            b.removeFromLeft( gMargin );

            auto h        = ( b.getHeight() - 2 ) / 4;
            Marking_left  = b.removeFromTop( h );
            Marking_right = b.removeFromBottom( h );
            bLeft         = b.removeFromTop( h );
            bRight        = b.removeFromBottom( h );
            leftGradient  = juce::ColourGradient::horizontal( juce::Colours::green, juce::Colours::red, bLeft );
            rightGradient = juce::ColourGradient::horizontal( juce::Colours::green, juce::Colours::red, bRight );

            meterHeight = static_cast<float>( bLeft.getWidth() );

            markings.clear();
            for( float a : decibles )
            {
                markings.push_back( getPixelFromDb( a, true ) );
            }
        }
        else if( Style::VerticalWithMarks == style )
        {
            auto b = getLocalBounds();

            // Value Label...
            if( isShowTitle )
                bLabel = b.removeFromBottom( b.getWidth() / 2 );

            //auto w = b.getWidth();
            auto w        = ( b.getWidth() - 1 ) / 4;
            Marking_left  = b.removeFromLeft( w );
            Marking_right = b.removeFromRight( w );
            bLeft         = b.removeFromLeft( w );
            bRight        = b.removeFromRight( w );

            leftGradient  = juce::ColourGradient::vertical( juce::Colours::red, juce::Colours::green, bLeft );
            rightGradient = juce::ColourGradient::vertical( juce::Colours::red, juce::Colours::green, bRight );

            meterHeight = static_cast<float>( bLeft.getHeight() );

            leftGradient  = juce::ColourGradient::vertical( juce::Colours::red, juce::Colours::green, bLeft );
            rightGradient = juce::ColourGradient::vertical( juce::Colours::red, juce::Colours::green, bRight );

            meterHeight = static_cast<float>( bLeft.getHeight() );

            markings.clear();
            for( float a : decibles )
            {
                markings.push_back( getPixelFromDb( a, true ) );
            }
        }
    }
}
