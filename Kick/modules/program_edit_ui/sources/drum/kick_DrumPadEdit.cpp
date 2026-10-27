
namespace kick_ui
{
    DrumPadEdit::DrumPadEdit( std::vector<int>& Pads, mt::MTDrumProgram*& PDrum )
        : pads( Pads ), pDrum( PDrum ), KickGrid( 4, 1 )
    {
        addComponent( modeLabel, 0, 0, 214 );
        addComponent( padLabel, 1, 0, 214 );
        addComponent( simultaniousPlayLabel, 2, 0, 214 );
        addComponent( muteTarget, 3, 0, 214 );

        muteGroup.setListOptions( mt::Kick::groupNumbers );
        layerPlay.setOptions( { "CYC", "VEL", "RAN" } );
        samplePlay.setOptions( { "One Shot", "Note Off", "Note On" } );

        addComponent( muteGroup, 0, 0, 173 );
        addComponent( layerPlay, 0, 0, 173 );
        addComponent( samplePlay, 0, 0, 173 );
        //addComponent( padPolyphony, 0, 0, 173 );

        addComponent( padVolume, 1, 0, 173 );
        addComponent( padPan, 1, 0, 173 );

        for( auto i = 0; i < 4; ++i )
        {
            sPad[i].setTitle( "Pad" );
            mPad[i].setTitle( "Pad" );
            sPad[i].setNumColumns( 4 );
            mPad[i].setNumColumns( 4 );
            sPad[i].setListOptions( mt::Kick::padNames, 0 );
            mPad[i].setListOptions( mt::Kick::padNames, 0 );
            addComponent( sPad[i], 2, 0, 173 );
            addComponent( mPad[i], 3, 0, 173 );

            sPad[i].onSelectionChanged = [&, i]( int Sel, const juce::String& ) { // SET Simultaneous note play for Pad.
                sPlay[i] = Sel;
                if( pDrum && pads.size() )
                    for( auto it : pads )
                        pDrum->setSimultaneousNotePlay( it, sPlay );
            };

            mPad[i].onSelectionChanged = [&, i]( int Sel, const juce::String& ) { // SET Simultaneous mutes for Pad.
                sMute[i] = Sel;
                if( pDrum && pads.size() )
                    for( auto it : pads )
                        pDrum->setSimultaneousNoteMute( it, sMute );
            };
        }

        padPan.setRange( -1.0, 1.0, 0.05 );
        padVolume.setRange( 0.0, 1.0, 0.05 );
        padPan.setCentered( true );

        muteGroup.onSelectionChanged = [&]( int Sel, const juce::String& ) {
            if( pDrum && pads.size() )
                for( auto it : pads )
                    pDrum->setNoteMuteGroup( it, Sel );
        };

        layerPlay.onSelectionChanged = [&]( int Sel, const juce::String& ) {
            if( pDrum && pads.size() )
                for( auto it : pads )
                    pDrum->setNoteLayerPlay( it, (mt::LayerPlayType)Sel );
        };

        samplePlay.onSelectionChanged = [&]( int Sel, const juce::String& ) {
            if( pDrum && pads.size() )
                for( auto it : pads )
                    pDrum->setNoteSamplePlay( it, (mt::SamplePlayType)Sel );
        };

        padPan.onValueChanged = [&]( double value ) {
            if( pDrum && pads.size() )
                for( auto it : pads )
                    pDrum->setNotePan( it, value );
        };

        padVolume.onValueChanged = [&]( double value ) {
            if( pDrum && pads.size() )
                for( auto it : pads )
                    pDrum->setNoteGain( it, value );
        };
    }

    DrumPadEdit::~DrumPadEdit()
    {
    }

    void DrumPadEdit::refresh()
    {
        if( pDrum && pads.size() )
        {
            int note = pads[0];

            muteGroup.setSelected( pDrum->getNoteMuteGroup( note ), juce::dontSendNotification );
            layerPlay.setSelected( (int)pDrum->getNoteLayerPlay( note ), juce::dontSendNotification );
            samplePlay.setSelected( (int)pDrum->getNoteSamplePlay( note ), juce::dontSendNotification );

            padPan.setValue( pDrum->getNotePan( note ), juce::dontSendNotification );
            padVolume.setValue( (double)pDrum->getNoteGain( note ), juce::dontSendNotification );

            sPlay = pDrum->getSimultaneousNotePlay( note );
            sMute = pDrum->getSimultaneousNoteMute( note );

            for( auto i = 0; i < 4; ++i )
            {
                sPad[i].setSelected( sPlay[i], juce::dontSendNotification );
                mPad[i].setSelected( sMute[i], juce::dontSendNotification );
            }
        }
    }

    void DrumPadEdit::visibilityChanged()
    {
        for( auto& it : getChildren() )
            it->setVisible( isVisible() );

        if( isVisible() )
            refresh();
    }

}
