namespace kick_ui
{
    FilterEnvEditKG::FilterEnvEditKG()
    {
        addComponent( filterLabel, 0, 0, 215 );
        addComponent( msLabel, 1, 0, 215 );
        addComponent( feOptions, 2, 0, 215 );
        addComponent( ampEnvOptions, 3, 0, 215 );

        addComponent( filterType, 0, 0, 130 );
        addComponent( filterCutoff, 0, 0, 130 );
        addComponent( filterReso, 0, 0, 130 );
        addComponent( filterEnv, 0, 0, 130 );

        addComponent( msVelStat, 1, 0, 130 );
        addComponent( msVelAtk, 1, 0, 130 );
        addComponent( msVelEnv, 1, 0, 130 );
        addComponent( msVelFlt, 1, 0, 130 );

        addComponent( feAttack, 2, 0, 130 );
        addComponent( feDecay, 2, 0, 130 );
        addComponent( feDecayFromOptions, 2, 0, 130 );
        addComponent( feAhdsrBtn, 2, 0, 130 );

        addComponent( ampAttack, 3, 0, 130 );
        addComponent( ampDecay, 3, 0, 130 );
        addComponent( ampDecayFromOptions, 3, 0, 130 );
        addComponent( ampAhdsrBtn, 3, 0, 130 );

        ampAhdsrBtn.addListener( this );
        feAhdsrBtn.addListener( this );

        feOptions.setOptions( { "Ad", "Ahds" } );
        ampEnvOptions.setOptions( { "Ad", "Ahds" } );
        feDecayFromOptions.setOptions( { "Start", "End" } );
        ampDecayFromOptions.setOptions( { "Start", "End" } );

        feOptions.optionChanged.connect( [this]( int index ) {
            if( index == 0 )
            {
                removeComponent( &feAttack );
                removeComponent( &feHold );
                removeComponent( &feDecay );
                removeComponent( &feSustain );
                removeComponent( &feAhdsrBtn );

                addComponent( feAttack, 2, 0, 130 );
                addComponent( feDecay, 2, 0, 130 );
                addComponent( feDecayFromOptions, 2, 0, 130 );
                addComponent( feAhdsrBtn, 2, 0, 130 );
            }
            else
            {
                removeComponent( &feAttack );
                removeComponent( &feDecay );
                removeComponent( &feAhdsrBtn );
                removeComponent( &feDecayFromOptions );

                addComponent( feAttack, 2, 0, 130 );
                addComponent( feHold, 2, 0, 130 );
                addComponent( feDecay, 2, 0, 130 );
                addComponent( feSustain, 2, 0, 130 );
                addComponent( feAhdsrBtn, 2, 0, 130 );
            }
            updateLayout();
        } );

        ampEnvOptions.optionChanged.connect( [this]( int index ) {
            if( index == 0 )
            {
                removeComponent( &ampAttack );
                removeComponent( &ampHold );
                removeComponent( &ampDecay );
                removeComponent( &ampSustain );
                removeComponent( &ampAhdsrBtn );

                addComponent( ampAttack, 3, 0, 130 );
                addComponent( ampDecay, 3, 0, 130 );
                addComponent( ampDecayFromOptions, 3, 0, 130 );
                addComponent( ampAhdsrBtn, 3, 0, 130 );
            }
            else
            {
                removeComponent( &ampAttack );
                removeComponent( &ampDecay );
                removeComponent( &ampDecayFromOptions );
                removeComponent( &ampAhdsrBtn );

                addComponent( ampAttack, 3, 0, 130 );
                addComponent( ampHold, 3, 0, 130 );
                addComponent( ampDecay, 3, 0, 130 );
                addComponent( ampSustain, 3, 0, 130 );
                addComponent( ampAhdsrBtn, 3, 0, 130 );
            }
            updateLayout();
        } );
    }

    FilterEnvEditKG::~FilterEnvEditKG()
    {
    }

    void FilterEnvEditKG::buttonClicked( juce::Button* )
    {
    }

}
