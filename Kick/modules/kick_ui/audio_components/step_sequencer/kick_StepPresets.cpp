
using namespace kick_Constants;

namespace kick_ui
{
#define SPACING 8
    StepPresets::StepPresets( StepSequencerData& Data )
        : data( Data )
    {
        mt::utils::addAndMakeVisible( this, enablePreset, levelSlider, ramp, triangle, pulse, flip, mirror, inverse );
        mt::utils::makeHidden( ramp, triangle, pulse, flip, mirror, inverse );

        enablePreset.setValueColour( ui::Theme::textColour( 1 ) );
        enablePreset.setValueFontHeight( 17 );

        // Setup level slider...
        levelSlider.setCentered( true );
        levelSlider.showKnob( true );
        levelSlider.setRange( -64, 64, 1 );

        levelSlider.onValueChanged = [&]( double Value ) {
            offset = static_cast<int>( Value );
            if( sliderOffsetOnDragOn )
                sliderOffsetOnDragOn( offset );
        };

        levelSlider.onDragStop = [&]( double Value ) {
            if( sliderOffsetOnDragEnd )
                sliderOffsetOnDragEnd( offset );

            offset = static_cast<int>( 0 );
            levelSlider.setValue( 0, juce::dontSendNotification );
        };

        // Handle Preset onClick events...
        ramp.onClick     = [&]() { setActivePreset( StepPresetType::Ramp ); };
        triangle.onClick = [&]() { setActivePreset( StepPresetType::Triangle ); };
        pulse.onClick    = [&]() { setActivePreset( StepPresetType::Pulse ); };
        flip.onClick     = [&]() { setActivePreset( StepPresetType::Flip ); };
        mirror.onClick   = [&]() { setActivePreset( StepPresetType::Mirror ); };
        inverse.onClick  = [&]() { setActivePreset( StepPresetType::Inverse ); };

        // Setup enable preset button...
        enablePreset.onClick = [&]() {
            if( enablePreset.getToggleState() )
            {
                mt::utils::makeHidden( levelSlider );
                mt::utils::makeVisible( ramp, triangle, pulse, flip, mirror, inverse );
            }
            else
            {
                mt::utils::makeVisible( levelSlider );
                mt::utils::makeHidden( ramp, triangle, pulse, flip, mirror, inverse );
            }
            repaint();
        };

        // Default toggle state.
        enablePreset.setToggleState( false, juce::sendNotification );

        ui::themeChanged.connect( [&]() {
            enablePreset.setToggleButtonText( "Presets", "Presets" );
        } );
    }

    void StepPresets::paint( juce::Graphics& g )
    {
        g.setColour( ui::Theme::backgroundColour( 1 ) );
        g.fillRoundedRectangle( { 0.0f, 0.0f, 100.0f, 342.0f }, 5.0f );
        if( !isPresetEnabled() )
            drawMarks( g );
    }

    void StepPresets::drawMarks( juce::Graphics& g )
    {
        auto x1 = 25; // Start x-coordinate for left lines
        auto x2 = 50; // End x-coordinate for left lines

        auto y            = levelSlider.getY(); // Start y-coordinate for lines
        auto sliderBottom = levelSlider.getHeight();

        auto x3 = 50; // Start x-coordinate for right lines
        auto x4 = 75; // End x-coordinate for right lines

        g.setColour( ui::Theme::textColour( 5 ).contrasting( 0.5f ) );

        // Calculate the number of small lines
        int numSmallLines = 4;

        while( y < sliderBottom )
        {
            g.drawLine( x1, y, x2, y, 0.5f ); // Draw left main line
            g.drawLine( x3, y, x4, y, 0.5f ); // Draw right main line

            // Calculate spacing between small lines
            float bigLineSpacing   = 68; // Space between big lines
            float smallLineSpacing = bigLineSpacing / ( numSmallLines + 1 ); // Space between small lines

            // Draw smaller lines between the main lines
            for( int i = 1; i <= numSmallLines; ++i )
            {
                float smallY = y + i * smallLineSpacing;
                g.drawLine( x1 + 10, smallY, x2, smallY, 0.5f ); // Draw small line on the left side
                g.drawLine( x3, smallY, x4 - 10, smallY, 0.5f ); // Draw small line on the right side
            }

            y = (int)( y + bigLineSpacing ); // Increase y-coordinate for the next main line
        }
        g.drawLine( x1, y, x2, y, 0.5f ); // Draw left main line
        g.drawLine( x3, y, x4, y, 0.5f ); // Draw right main line
    }

    void StepPresets::resized()
    {
        enablePreset.setBoundsRelative( .1f, .8421f, .8f, .1286f );
        levelSlider.setBoundsRelative( .35f, .0292f, .3f, .8070f );
        ramp.setBoundsRelative( .1f, .0292f, .8f, .1286f );
        triangle.setBoundsRelative( .1f, .1637f, .8f, .1286f );
        pulse.setBoundsRelative( .1f, .2982f, .8f, .1286f );
        flip.setBoundsRelative( .1f, .7017f, .8f, .1286f );
        mirror.setBoundsRelative( .1f, .4327f, .8f, .1286f );
        inverse.setBoundsRelative( .1f, .5672f, .8f, .1286f );
    }

    void StepPresets::setActivePreset( StepPresetType Type )
    {
        ramp.sendClick( StepPresetType::Ramp == Type );
        triangle.sendClick( StepPresetType::Triangle == Type );
        pulse.sendClick( StepPresetType::Pulse == Type );
        flip.sendClick( StepPresetType::Flip == Type );
        mirror.sendClick( StepPresetType::Mirror == Type );
        inverse.sendClick( StepPresetType::Inverse == Type );

        switch( Type )
        {
            case StepPresetType::Ramp:
            case StepPresetType::Triangle:
            case StepPresetType::Pulse:
            case StepPresetType::Inverse:
                if( modifyValues ) modifyValues( presetValues );
                break;
            case StepPresetType::Flip:
            case StepPresetType::Mirror:
                if( addValues ) addValues( presetValues );
                break;
        }
    }

    int StepPresets::setPresetValues( std::vector<int>& Values, int Min, int Max )
    {
        presetValues = Values;
        min          = Min;
        max          = Max;
        return 0;
    }

    ///////////////////////////////////////////////////////////////////////////
    ///////   IStepPreset...
    ///////////////////////////////////////////////////////////////////////////

    IStepPreset::IStepPreset( StepPresetType Type, StepSequencerData& Data, std::vector<int>& Values, int& Min, int& Max )
        : type( Type ), data( Data ), values( Values ), min( Min ), max( Max ) {}

    void IStepPreset::sendClick( bool Active )
    {
        if( isActive != Active )
        {
            isActive = Active;
            if( isActive )
                performCalculation();
        }
        else
        {
            if( isActive )
            {
                nextPresetValueType();
            }
        }
        triggerAsyncUpdate();
    }

    void IStepPreset::mouseUp( const juce::MouseEvent& )
    {
        if( onClick )
            onClick();
    }

    void IStepPreset::handleAsyncUpdate()
    {
        repaint();
    }

    void IStepPreset::nextPresetValueType()
    {
        int typeI = static_cast<int>( valueType );
        typeI++;
        if( typeI >= static_cast<int>( TypeMax ) )
            typeI = 1;

        valueType = static_cast<PresetValueType>( typeI );
        performCalculation();
    }

    bool IStepPreset::isForward()
    {
        return ( ( valueType == Single ) || ( valueType == Double ) || ( valueType == Quadruple ) );
    }

    void IStepPreset::performCalculation()
    {
        if( valueType == Single || valueType == ReverseSingle )
        {
            presetCount = 1;
        }
        else if( valueType == Double || valueType == ReverseDouble )
        {
            presetCount = 2;
        }
        else if( valueType == Quadruple || valueType == ReverseQuadruple )
        {
            presetCount = 4;
        }
        calculateValues();
    }

    ///////////////////////////////////////////////////////////////////////////
    ///////   StepRampPresets...
    ///////////////////////////////////////////////////////////////////////////

    StepRampPresets::StepRampPresets( StepSequencerData& Data, std::vector<int>& Values, int& Min, int& Max )
        : IStepPreset( StepPresetType::Ramp, Data, Values, Min, Max )
    {
        rampImages.push_back( ui::Theme::getDrawable( "seq_saw" ) );
        rampImages.push_back( ui::Theme::getDrawable( "seq_saw_active" ) );
        rampImages.push_back( ui::Theme::getDrawable( "seq_saw_rev" ) );
        rampImages.push_back( ui::Theme::getDrawable( "seq_saw_rev2" ) );

        rampImages.push_back( ui::Theme::getDrawable( "seq_saw1" ) );
        rampImages.push_back( ui::Theme::getDrawable( "seq_saw4" ) );
        rampImages.push_back( ui::Theme::getDrawable( "seq_saw5" ) );
    }

    void StepRampPresets::paint( juce::Graphics& g )
    {
        auto img = rampImages[isActive ? static_cast<int>( valueType ) : 0].get();
        img->drawWithin( g, getLocalBounds().toFloat(), juce::RectanglePlacement::centred, 1.0f );
    }

    void StepRampPresets::calculateValues()
    {
        values.clear();
        values.resize( data.numSteps );

        float imgX = 160;
        float xMax = imgX / presetCount;
        int   nl   = data.numSteps / presetCount;
        float dx   = xMax / nl;
        float m    = max / xMax;
        float x    = dx;

        std::list<int> valuesTmp;
        for( int i = 0; i < nl; i++ )
        {
            int paramV = static_cast<int>( std::ceil( m * x ) );

            if( isForward() )
                valuesTmp.push_back( paramV );
            else
                valuesTmp.push_front( paramV );

            x += dx;
        }

        if( !valuesTmp.empty() )
        {
            for( int i = 0; i < data.numSteps; )
            {
                for( int val : valuesTmp )
                {
                    values[(size_t)i] = val;
                    i++;
                }
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    ///////   StepPulsePresets...
    ///////////////////////////////////////////////////////////////////////////

    StepPulsePresets::StepPulsePresets( StepSequencerData& Data, std::vector<int>& Values, int& Min, int& Max )
        : IStepPreset( StepPresetType::Pulse, Data, Values, Min, Max )
    {
        pulseImages.push_back( ui::Theme::getDrawable( "seq_squ" ) );
        pulseImages.push_back( ui::Theme::getDrawable( "seq_ssqu_active" ) );
        pulseImages.push_back( ui::Theme::getDrawable( "seq_squ4" ) );
        pulseImages.push_back( ui::Theme::getDrawable( "seq_squ5" ) );

        pulseImages.push_back( ui::Theme::getDrawable( "seq_squ_rev" ) );
        pulseImages.push_back( ui::Theme::getDrawable( "seq_squ_rev2" ) );
        pulseImages.push_back( ui::Theme::getDrawable( "seq_squ_rev3" ) );
    }

    void StepPulsePresets::paint( juce::Graphics& g )
    {
        auto img = pulseImages[isActive ? static_cast<int>( valueType ) : 0].get();
        img->drawWithin( g, getLocalBounds().toFloat(), juce::RectanglePlacement::centred, 1.0f );
    }

    void StepPulsePresets::calculateValues()
    {
        values.clear();
        values.resize( (size_t)data.numSteps );

        unsigned int   nl = data.numSteps / presetCount;
        std::list<int> valuesTmp;
        for( unsigned int i = 0; i < nl; i++ )
        {
            int paramV = max;
            if( i >= ( nl / 2 ) )
                paramV = 0;

            if( isForward() )
                valuesTmp.push_back( paramV );
            else
                valuesTmp.push_front( paramV );
        }
        if( !valuesTmp.empty() )
        {
            for( int i = 0; i < data.numSteps; )
            {
                for( int val : valuesTmp )
                {
                    values[(size_t)i] = val;
                    i++;
                }
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    ///////   StepTrianglePresets...
    ///////////////////////////////////////////////////////////////////////////

    StepTrianglePresets::StepTrianglePresets( StepSequencerData& Data, std::vector<int>& Values, int& Min, int& Max )
        : IStepPreset( StepPresetType::Triangle, Data, Values, Min, Max )
    {
        triangleImages.push_back( ui::Theme::getDrawable( "seq_Tri" ) );
        triangleImages.push_back( ui::Theme::getDrawable( "seq_Tri_active" ) );
        triangleImages.push_back( ui::Theme::getDrawable( "seq_Tri4" ) );
        triangleImages.push_back( ui::Theme::getDrawable( "seq_Tri5" ) );

        triangleImages.push_back( ui::Theme::getDrawable( "seq_Tri_rev" ) );
        triangleImages.push_back( ui::Theme::getDrawable( "seq_Tri_rev2" ) );
        triangleImages.push_back( ui::Theme::getDrawable( "seq_Tri_rev3" ) );
    }

    void StepTrianglePresets::paint( juce::Graphics& g )
    {
        auto img = triangleImages[isActive ? static_cast<int>( valueType ) : 0].get();
        img->drawWithin( g, getLocalBounds().toFloat(), juce::RectanglePlacement::centred, 1.0f );
    }

    void StepTrianglePresets::calculateValues()
    {
        values.clear();
        values.resize( data.numSteps );

        float        imgX = 160;
        float        xMax = imgX / presetCount;
        unsigned int nl   = data.numSteps / presetCount;
        float        dx   = xMax / nl;
        float        m1, m2 = 0;
        float        x = 0;
        int          c1, c2 = 0;

        if( isForward() )
        {
            m1 = max / ( xMax / 2 );
            m2 = -m1;
            c1 = 0;
            c2 = 2 * max;
            x  = dx;
        }
        else
        {
            m1 = -( max / ( xMax / 2 ) );
            m2 = -m1;
            c1 = max;
            c2 = -max;
        }
        std::list<int> valuesTmp;
        int            paramV = max;
        for( unsigned int i = 0; i < nl; i++ )
        {
            if( i < nl / 2 )
            {
                paramV = static_cast<int>( std::ceil( c1 + ( m1 * x ) ) );
                x += dx;
            }
            else if( i == ( nl / 2 ) )
            {
                //last calculated value
                //paramV = paramV;
                // special case for reverse triangle
                if( x != ( xMax / 2 ) + dx )
                    x = ( xMax / 2 ) + ( 2 * dx );
            }
            else
            {
                paramV = static_cast<int>( std::ceil( ( m2 * x ) + c2 ) );
                x += dx;
            }

            valuesTmp.push_back( paramV );
        }

        if( !valuesTmp.empty() )
        {
            for( int i = 0; i < data.numSteps; )
            {
                for( int val : valuesTmp )
                {
                    values[(size_t)i] = val;
                    i++;
                }
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    ///////   StepPresetFlip
    ///////////////////////////////////////////////////////////////////////////

    StepPresetFlip::StepPresetFlip( StepSequencerData& Data, std::vector<int>& Values, int& Min, int& Max )
        : IStepPreset( StepPresetType::Flip, Data, Values, Min, Max )
    {
        mt::utils::addAndMakeVisible( this, flipPreset );
        flipPreset.onClick = [&]() {
            if( onClick )
                onClick();
            flipPreset.setToggleState( false, juce::dontSendNotification );
        };
    }

    void StepPresetFlip::calculateValues()
    {
        for( auto i = 0; i < values.size(); i++ )
        {
            values[i] = ( values[i] ) ? 0 : max;
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    ///////   StepPresetMirror...
    ///////////////////////////////////////////////////////////////////////////

    StepPresetMirror::StepPresetMirror( StepSequencerData& Data, std::vector<int>& Values, int& Min, int& Max )
        : IStepPreset( StepPresetType::Mirror, Data, Values, Min, Max )
    {
        mt::utils::addAndMakeVisible( this, mirrorPreset );
        mirrorPreset.onClick = [&]() {
            if( onClick )
                onClick();
            mirrorPreset.setToggleState( false, juce::dontSendNotification );
        };
    }

    void StepPresetMirror::calculateValues()
    {
        std::reverse( values.begin(), values.end() );
    }

    ///////////////////////////////////////////////////////////////////////////
    ///////   StepPresetInverse...
    ///////////////////////////////////////////////////////////////////////////

    StepPresetInverse::StepPresetInverse( StepSequencerData& Data, std::vector<int>& Values, int& Min, int& Max )
        : IStepPreset( StepPresetType::Inverse, Data, Values, Min, Max )
    {
        mt::utils::addAndMakeVisible( this, inversePreset );
        inversePreset.onClick = [&]() {
            if( onClick )
                onClick();
            inversePreset.setToggleState( false, juce::dontSendNotification );
        };
    }

    void StepPresetInverse::calculateValues()
    {
        auto size = values.size();
        for( auto i = 0; i < size; i++ )
        {
            values[i] = juce::jlimit( min, max, ( max - values[(size_t)i] ) );
        }
    }

}
