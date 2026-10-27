namespace ui
{
    KickOffSetControl::KickOffSetControl( int r, const juce::String& name )
        : mt::MTProperties( this )
        , range( r )
        , ctrlName( name ) {}

    KickOffSetControl::~KickOffSetControl() {}

    void KickOffSetControl::setDisabled( bool disable )
    {
        disabled = disable;
        repaint();
    }

    void KickOffSetControl::knobRotated( double delta )
    {
        setValue( (int)( value + delta ) );
    }

    void KickOffSetControl::setValue( int newValue )
    {
        int tempVal = newValue;
        if( tempVal < -range || tempVal > range )
            return;

        value = tempVal;
        repaint();
    }

    void KickOffSetControl::knobClicked()
    {
        auto safeThis = new KickNumPad<int>( -range, range, value, 1 );
        safeThis->show( ctrlName, [this, safeThis]( int /*r*/ ) {
            knobRotated( safeThis->getValue() );
            if( safeThis ) delete safeThis;
            setFocus();
        } );
    }

    void KickOffSetControl::paint( juce::Graphics& g )
    {
        const juce::Rectangle<float> sliderRect = getSliderRect();
        if( disabled )
        {
            g.setColour( ui::Theme::backgroundColour( 3 ) );
            g.fillRect( sliderRect );
            return;
        }

        g.setColour( ui::Theme::backgroundColour( 2 ) );
        g.fillRect( sliderRect );

        //draw label
        g.setColour( juce::Colours::white );
        const juce::Rectangle<float>& textRect = getLabelRect();
        g.drawText( juce::String( value ), textRect, juce::Justification::centred );

        // draw title
        g.setColour( ui::Theme::titleColour() );
        g.drawText( ctrlName, getTitleRect(), juce::Justification::centred );

        //draw level
        g.setColour( juce::Colours::orange );
        if( value == 0 )
        {
            g.drawLine( sliderRect.getCentreX(), sliderRect.getY(), sliderRect.getCentreX(), (float)( sliderRect.getY() + sliderRect.getHeight() ), 1.0f );
        }
        else
        {
            float                  levelX = ( getWidth() / 2.0f ) * value / range;
            juce::Rectangle<float> levelFillRect;
            if( levelX < 0 )
                levelFillRect = juce::Rectangle<float>( sliderRect.getCentreX() + levelX, sliderRect.getY(), -levelX, sliderRect.getHeight() );
            else
                levelFillRect = juce::Rectangle<float>( sliderRect.getCentreX(), sliderRect.getY(), levelX, sliderRect.getHeight() );

            g.fillRect( levelFillRect );
        }

        KickControlBase<juce::Component>::paint( g );
    }

    void KickOffSetControl::mouseDrag( const juce::MouseEvent& event )
    {
        float mousePos = event.position.getX();
        if( mousePos < 0 )
            mousePos = 0.0f;
        if( mousePos > getWidth() )
            mousePos = (float)getWidth();

        const auto& sliderRect = getSliderRect();
        if( mousePos < sliderRect.getCentreX() )
        {
            value = (int)std::round( ( range * mousePos ) / ( sliderRect.getWidth() / 2.0f ) );
            value = -( range - value );
        }
        else
        {
            value = (int)std::round( ( range * ( sliderRect.getWidth() - mousePos ) ) / ( sliderRect.getWidth() / 2.0f ) );
            value = range - value;
        }

        repaint();
    }

    juce::Rectangle<float> KickOffSetControl::getSliderRect()
    {
        if( ctrlName.isEmpty() )
            return juce::Rectangle<float>( 0.0f, 0.0f, (float)getWidth(), getHeight() / 2 - 3.0f );
        else
            return juce::Rectangle<float>( 0.0f, getHeight() / 3.0f + 3.0f, (float)getWidth(), getHeight() / 3 - ( 2 * 3.0f ) );
    }

    juce::Rectangle<float> KickOffSetControl::getTitleRect()
    {
        if( ctrlName.isEmpty() )
            return juce::Rectangle<float>( 0.0f, 0.0f, 0.0f, 0.0f );
        else
            return juce::Rectangle<float>( 0.0f, 0.0f, (float)getWidth(), getHeight() / 3 - ( 2 * 3.0f ) );
    }

    juce::Rectangle<float> KickOffSetControl::getLabelRect()
    {
        if( ctrlName.isEmpty() )
            return juce::Rectangle<float>( 0.0f, getHeight() / 2 + 3.0f, (float)getWidth(), getHeight() / 2 - 3.0f );
        else
            return juce::Rectangle<float>( 0.0f, ( 2 * getHeight() / 3 ) + ( 2 * 3.0f ), (float)getWidth(), getHeight() / 3 - ( 2 * 3.0f ) );
    }

}
