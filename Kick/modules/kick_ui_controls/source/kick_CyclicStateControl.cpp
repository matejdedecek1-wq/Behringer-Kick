namespace ui
{

    KickCyclicStateControl::KickCyclicStateControl()
    {
        mt::utils::addAndMakeVisible( this, leftNav, rightNav );
        mt::utils::addListener( this, leftNav, rightNav );
        addAndMakeVisible( stateButton );

        themeChanged.connect( [&]() {
            stateButton.setImages( states[currentState] );
        } );
    }

    void KickCyclicStateControl::resized()
    {
        int buttonWidth = getWidth() - ( 2 * 40 ) - ( 2 * 2 );
        leftNav.setBounds( 0, 0, 40, getHeight() );
        stateButton.setBounds( leftNav.getRight() + 2, 0, buttonWidth, getHeight() );
        rightNav.setBounds( stateButton.getRight() + 2, 0, 40, getHeight() );
    }

    void KickCyclicStateControl::setCurrentState( unsigned int cs )
    {
        currentState = cs;
        stateButton.setImages( states[cs] );
    }

    void KickCyclicStateControl::setNormalized( float Value )
    {
        auto state = (int)( Value * states.size() );
        setCurrentState( state );
    }

    float KickCyclicStateControl::getNormalized()
    {
        return ( (float)currentState / (float)states.size() );
    }

    void KickCyclicStateControl::buttonClicked( juce::Button* button )
    {
        if( states.size() == 0 )
            return;

        if( button == &leftNav )
        {
            if( currentState == 0 )
                currentState = (unsigned int)states.size() - 1;
            else
                currentState--;
        }
        else
        {
            if( currentState == (unsigned int)states.size() - 1 )
                currentState = 0;
            else
                currentState++;
        }

        stateChanged( (int)currentState );
        setCurrentState( currentState );
    }

}
