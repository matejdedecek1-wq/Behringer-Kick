

namespace kick_ui
{

    PanVelocityEdit::PanVelocityEdit( std::vector<int>& Pads, mt::MTDrumProgram*& PDrum )
        : pads( Pads ), pDrum( PDrum ), KickGrid( 4, 2, true )
    {
        setColumnWidth( { 354, -1 } );
        //std::vector<juce::String> columTitle{ "Layers", "Pan", "Vel:Start", "Vel:End" };
        //setTitles( columTitle );

        for( auto i = 0; i < 4; i++ )
        {
            layerName[i].setTitle( "Layer " + juce::String( i + 1 ) );
            layerPan[i].setTitle( "Pan" );
            layerVeloStart[i].setTitle( "Vel:Start" );
            layerVeloEnd[i].setTitle( "Vel:End" );

            layerName[i].setDisplayFormat( ui::KickListBox::withTitleBar );
            layerPan[i].showTitleAndValue( true, juce::Slider::TextBoxLeft );
            layerVeloStart[i].showTitleAndValue( true, juce::Slider::TextBoxLeft );
            layerVeloEnd[i].showTitleAndValue( true, juce::Slider::TextBoxLeft );

            layerPan[i].setRange( -1.0, 1.0, 0.01 );
            layerVeloStart[i].setRange( 0.0, 127.0, 1.0 );
            layerVeloEnd[i].setRange( 0.0, 127.0, 1.0 );

            layerPan[i].setCentered( true );

            addComponent( layerName[i], i, 0, 334 );
            addComponent( layerPan[i], i, 1, 200 );
            addComponent( layerVeloStart[i], i, 1, 200 );
            addComponent( layerVeloEnd[i], i, 1, 200 );

            layerName[i].onSelectionChanged = [&, i]( int Sel, const juce::String& ) {
                auto s = mt::getAudioInterface().getAsset( mt::AssetTypes::Samples, Sel );
                if( pDrum )
                    for( auto it : pads )
                        pDrum->setLayerSource( it, i, s );
            };

            layerPan[i].onValueChanged = [&, i]( double Value ) {
                if( pDrum )
                    for( auto it : pads )
                        pDrum->setLayerPan( it, i, Value );
            };

            layerVeloStart[i].onValueChanged = [&, i]( double Value ) {
                if( pDrum )
                    for( auto it : pads )
                        pDrum->setLayerVelocityStart( it, i, (int)Value );
            };

            layerVeloEnd[i].onValueChanged = [&, i]( double Value ) {
                if( pDrum )
                    for( auto it : pads )
                        pDrum->setLayerVelocityEnd( it, i, (int)Value );
            };
        }
    }

    PanVelocityEdit::~PanVelocityEdit()
    {
    }

    void PanVelocityEdit::refresh()
    {
        if( pDrum && pads.size() )
        {
            int note = pads[0];

            layer = pDrum->getLayerSounds( note );
            for( int i = 0; i < 4; ++i )
            {
                auto sName = layer[i].length() ? juce::File( layer[i] ).getFileName() : "None";
                layerName[i].setSelected( sName, juce::dontSendNotification );

                //layerName[i].setValue( pDrum->getLayerPan( note, i ), dontSendNotification );
                layerPan[i].setValue( pDrum->getLayerPan( note, i ), juce::dontSendNotification );
                layerVeloStart[i].setValue( (double)pDrum->getLayerVelocityStart( note, i ), juce::dontSendNotification );
                layerVeloEnd[i].setValue( (double)pDrum->getLayerVelocityEnd( note, i ), juce::dontSendNotification );
            }
        }
    }

    void PanVelocityEdit::visibilityChanged()
    {
        for( auto it : getChildren() )
            it->setVisible( isVisible() );

        if( isVisible() )
        {
            assets = mt::getAudioInterface().getAssetNames( mt::AssetTypes::Samples );
            for( int i = 0; i < 4; ++i )
                layerName[i].setListOptions( assets, -1 );

            refresh();
        }
    }
}
