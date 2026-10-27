
namespace kick_ui
{
    KeyGroupProgramEdit::KeyGroupProgramEdit()
        : ProgEditInterface( mt::MTProgramType::Keygroup )
    {
        comps.reserve( KEY_GROUP_NAV_BUTTONS );
        comps.emplace_back( std::make_unique<KeyGroupMaster>( keys, pKeyGroup ) );
        comps.emplace_back( std::make_unique<KeyGroupSampleTab1>( keys, pKeyGroup ) );
        comps.emplace_back( std::make_unique<KeyGroupSampleTab2>( keys, pKeyGroup ) );
        comps.emplace_back( std::make_unique<KGLayerMix>( keys, pKeyGroup ) );
        comps.emplace_back( std::make_unique<KickFilterEnvelope>( keys, pKeyGroup ) );
        comps.emplace_back( std::make_unique<KeyGroupLFO>( keys, pKeyGroup ) );

        for( auto& it : comps )
            mt::utils::addChildComponents( this, it.get() );

        mt::utils::addAndMakeVisible( this, mainButton, samplesButton, layerMixButton, envelopesButton, lfoButton );
        mt::utils::addListener( this, mainButton, samplesButton, layerMixButton, envelopesButton, lfoButton );
        mt::utils::setValueColour( ui::Theme::textColour( 1 ), mainButton, samplesButton, layerMixButton, envelopesButton, lfoButton );
        mt::utils::setValueFontHeight( 18, mainButton, samplesButton, layerMixButton, envelopesButton, lfoButton );
        mt::utils::setRadioGroupID( (int)RadioGroupId::KeyGroupProgram, mainButton, samplesButton, layerMixButton, envelopesButton, lfoButton );

        // Keep default key
        keys.push_back( 0 );
        mainButton.triggerClick();

        dynamic_cast<KeyGroupSampleTab1*>( comps[1].get() )->onLayerSelected = [&]( const int SelectedLayer ) {
            dynamic_cast<KeyGroupSampleTab2*>( comps[2].get() )->selectLayer( SelectedLayer );
        };

        ui::themeChanged.connect( [&]() {
            mainButton.setToggleButtonText( "Main", "Main" );
            samplesButton.setToggleButtonText( "Samples", "Samples" );
            layerMixButton.setToggleButtonText( "Layer Mix", "Layer Mix" );
            envelopesButton.setToggleButtonText( "Envelopes", "Envelopes" );
            lfoButton.setToggleButtonText( "LFO", "LFO" );
        } );
    }

    void KeyGroupProgramEdit::refresh()
    {
        if( pKeyGroup )
            dynamic_cast<KeyGroupInterface*>( comps[tIndex].get() )->refresh();
    }

    void KeyGroupProgramEdit::handleCommand( CPktMgr::HWMsg_t& f_HWMsg )
    {
        if( pKeyGroup )
            dynamic_cast<KeyGroupInterface*>( comps[tIndex].get() )->handleCommand( f_HWMsg );
    }

    void KeyGroupProgramEdit::resized()
    {
        mainButton.setBoundsRelative( .0099f, .022f, .1896f, .096f );
        samplesButton.setBoundsRelative( .2078f, .022f, .1896f, .096f );
        layerMixButton.setBoundsRelative( .4051f, .022f, .1896f, .096f );
        envelopesButton.setBoundsRelative( .6028f, .022f, .1896f, .096f );
        lfoButton.setBoundsRelative( .8004f, .022f, .1896f, .096f );

        for( auto& it : comps )
            it->setBoundsRelative( .0097f, .139f, .982f, .842f );
    }

    void KeyGroupProgramEdit::buttonClicked( juce::Button* button )
    {
        isMain = isSample = false;

        if( button == &mainButton )
        {
            isDefault = false;
            isMain    = true;
            tIndex    = 0;
        }
        else if( button == &samplesButton )
        {
            isDefault = false;
            isSample  = true;
            sampleTab ^= 1;
            if( sampleTab == 0 )
                tIndex = 1;
            else
                tIndex = 2;
        }
        else if( button == &layerMixButton )
        {
            tIndex    = 3;
            isDefault = true;
        }
        else if( button == &envelopesButton )
        {
            tIndex    = 4;
            isDefault = true;
        }
        else if( button == &lfoButton )
        {
            tIndex    = 5;
            isDefault = true;
        }
        for( auto& comp : comps )
            comp->setVisible( false );
        comps[tIndex]->setVisible( true );
        // repaint();
    }

    void KeyGroupProgramEdit::visibilityChanged()
    {
        auto visible = isVisible();
        if( visible )
        {
            pKeyGroup = pIProg ? dynamic_cast<mt::MTKeygroupProgram*>( pIProg ) : nullptr;
            refresh();
        }
        comps[tIndex]->setVisible( visible );
    }

    KeyGroupSampleTab1::KeyGroupSampleTab1( std::vector<int>& Keys, mt::MTKeygroupProgram*& pKeyGroup )
        : keys( Keys ), pKeygroup( pKeyGroup ), KickGrid( 4, 4 )
    {
        setRowTitles( { "WAVEFORM", "LAYER ASSIGN", "TIMING AND PITCH", "" } );
        addComponent( wav, 0, 0, 4 );

        for( auto i = 0; i < 4; ++i )
        {
            auto name = "Layer " + juce::String( i + 1 );
            layer[i].setName( name );
            layer[i].setTitle( name );
            layer[i].showTitle( true );
            layer[i].setID( i );
            layer[i].setValueFontHeight( 18.f );
            layer[i].setRadioGroupId( 563365 );
            layer[i].addListener( this );
            layer[i].setTitleColour( ui::Theme::textColour( 1 ) );

            addComponent( layer[i], 1, i );
        }

        start.setColours( ui::Theme::textColour( 2 ), ui::Theme::textColour( 1 ) );
        end.setColours( ui::Theme::textColour( 2 ), ui::Theme::textColour( 1 ) );
        semi.setColours( ui::Theme::textColour( 2 ), ui::Theme::textColour( 1 ) );
        fine.setColours( ui::Theme::textColour( 2 ), ui::Theme::textColour( 1 ) );

        addComponent( start, 2, 0 );
        addComponent( end, 2, 1 );
        addComponent( semi, 2, 2 );
        addComponent( fine, 2, 3 );

        semi.setRange( SEMI::MIN, SEMI::MAX, SEMI::STEP );
        fine.setRange( FINE::MIN, FINE::MAX, FINE::STEP );

        start.onValueChanged = [&]( juce::int64 Value ) {
            if( pKeygroup )
            {
                if( end.getValue() >= Value )
                    pKeygroup->setLayerLoopStart( keys[0], selectedLayer, Value );
                else
                    start.setValue( end.getValue(), juce::dontSendNotification );
            }
        };

        end.onValueChanged = [&]( juce::int64 Value ) {
            if( pKeygroup )
            {
                if( start.getValue() <= Value )
                    pKeygroup->setLayerLoopEnd( keys[0], selectedLayer, Value );
                else
                    end.setValue( start.getValue(), juce::dontSendNotification );
            }
        };

        semi.onValueChanged = [&]( juce::int64 Value ) {
            if( pKeygroup )
                pKeygroup->setLayerSemi( keys[0], selectedLayer, Value );
        };

        fine.onValueChanged = [&]( juce::int64 Value ) {
            if( pKeygroup )
                pKeygroup->setLayerFine( keys[0], selectedLayer, Value );
        };

        layer[0].setToggleState( true, juce::dontSendNotification );
    }

    void KeyGroupSampleTab1::buttonStateChanged( juce::Button* B )
    {
        if( B->getToggleState() )
        {
            auto selLayer = dynamic_cast<ui::KickToggleButton*>( B )->getID();
            if( selLayer != selectedLayer )
            {
                selectedLayer = selLayer;
                refresh();
            }
        }
    }

    void KeyGroupSampleTab1::refresh()
    {
        if( pKeygroup )
        {
            for( auto i = 0; i < 4; ++i )
            {
                auto path = pKeygroup->getLayerSource( keys[0], i );
                auto src  = juce::File( path ).getFileNameWithoutExtension();
                layer[i].setToggleButtonText( src, src );
            }
        }
        refreshKeys();
    }

    void KeyGroupSampleTab1::refreshKeys()
    {
        if( pKeygroup && keys.size() )
        {
            auto source = pKeygroup->getLayerSource( keys[0], selectedLayer );
            auto min{ 0 };
            auto max{ 1 };
            if( auto sPtr = mt::getAudioSoundCache().getOrLoad( source ) )
            {
                max = sPtr->getNumSamplesPerChannel();
                wav.setSource( sPtr->getData(), sPtr->getSampleRate() );

                start.setValueBounds( min, max, min );
                end.setValueBounds( min, max, max );
            }
            else
                wav.setSource( {}, 0 );

            mt::utils::setEnabled( source.isNotEmpty(), start, end, semi, fine );

            start.setValue( pKeygroup->getLayerLoopStart( keys[0], selectedLayer ), juce::dontSendNotification );
            auto endValue = pKeygroup->getLayerLoopEnd( keys[0], selectedLayer );
            end.setValue( endValue == -1 ? ( source.isNotEmpty() ? max : 0 ) : endValue, juce::dontSendNotification );
            semi.setValue( pKeygroup->getLayerSemi( keys[0], selectedLayer ), juce::dontSendNotification );
            fine.setValue( pKeygroup->getLayerFine( keys[0], selectedLayer ), juce::dontSendNotification );
        }
    }

    void KeyGroupSampleTab1::handleCommand( CPktMgr::HWMsg_t& f_HwMsg )
    {
        if( contracts::DclContractsHelper::isPadButtonMessage( f_HwMsg ) )
        {
            auto padNum = contracts::DclContractsHelper::getPadNumber( f_HwMsg );
            keys[0]     = padNum + mt::getAudioInterface().getBankOffset();
            refresh();
        }
    }

    void KeyGroupSampleTab1::visibilityChanged()
    {
        auto visible = isVisible();
        if( visible )
            refresh();
    }

    KeyGroupSampleTab2::KeyGroupSampleTab2( std::vector<int>& Keys, mt::MTKeygroupProgram*& pKeyGroup )
        : keys( Keys ), pKeygroup( pKeyGroup ), ui::KickGrid( 4, 4 )
    {
        setRowTitles( { "Waveform", "Slice And Loop", "Trigger Control", "Level And Pan" } );

        addComponent( wav, 0, 0, 4 );

        padLoop.showTitle( true );
        reverse.showTitle( true );
        slice.setColours( ui::Theme::textColour( 2 ), ui::Theme::textColour( 1 ) );
        loopStart.setColours( ui::Theme::textColour( 2 ), ui::Theme::textColour( 1 ) );
        loopEnd.setColours( ui::Theme::textColour( 2 ), ui::Theme::textColour( 1 ) );
        padLoop.setValueColour( ui::Theme::textColour( 1 ) );
        reverse.setValueColour( ui::Theme::textColour( 1 ) );
        offSet.setColours( ui::Theme::textColour( 2 ), ui::Theme::textColour( 1 ) );
        tailLength.setColours( ui::Theme::textColour( 2 ), ui::Theme::textColour( 1 ) );
        tailStart.setColours( ui::Theme::textColour( 2 ), ui::Theme::textColour( 1 ) );
        level.setColours( ui::Theme::textColour( 2 ), ui::Theme::textColour( 1 ) );
        layerPan.setColours( ui::Theme::textColour( 2 ), ui::Theme::textColour( 1 ) );

        addComponent( slice, 1, 0 );
        addComponent( loopStart, 1, 1 );
        addComponent( loopEnd, 1, 2 );
        addComponent( padLoop, 1, 3 );

        addComponent( offSet, 2, 0 );
        addComponent( tailLength, 2, 1 );
        addComponent( tailStart, 2, 2 );
        addComponent( reverse, 2, 3 );

        addComponent( level, 3, 0 );
        addComponent( layerPan, 3, 1 );

        tailLength.setRange( 0, 5000, 100 );
        tailStart.setRange( 0, 1000, 100 );
        layerPan.setRange( PAN::MIN, PAN::MAX, PAN::STEP );
        level.setRange( MIDI::MIN, MIDI::MAX, MIDI::STEP );

        loopStart.onValueChanged = [&]( juce::int64 Value ) {
            if( pKeygroup )
            {
                if( loopEnd.getValue() >= Value )
                    pKeygroup->setLayerLoopStart( keys[0], selectedLayer, Value );
                else
                    loopStart.setValue( loopEnd.getValue(), juce::dontSendNotification );
            }
        };

        loopEnd.onValueChanged = [&]( juce::int64 Value ) {
            if( pKeygroup )
            {
                if( loopStart.getValue() <= Value )
                    pKeygroup->setLayerLoopEnd( keys[0], selectedLayer, Value );
                else
                    loopEnd.setValue( loopStart.getValue(), juce::dontSendNotification );
            }
        };

        tailLength.onValueChanged = [&]( juce::int64 MilliSec ) {
            if( pKeygroup )
                pKeygroup->setLayerTailLength( keys[0], selectedLayer, MilliSec );
        };

        tailStart.onValueChanged = [&]( juce::int64 MilliSec ) {
            if( pKeygroup )
                pKeygroup->setLayerTailStart( keys[0], selectedLayer, MilliSec );
        };

        offSet.onValueChanged = [&]( juce::int64 Value ) {
            if( pKeygroup )
                pKeygroup->setLayerOffset( keys[0], selectedLayer, Value );
        };

        level.onValueChanged = [&]( juce::int64 Value ) {
            if( pKeygroup )
                pKeygroup->setLayerGain( keys[0], selectedLayer, CommonUtils::midiToGain( Value ) );
        };

        layerPan.onValueChanged = [&]( double Value ) {
            if( pKeygroup )
                pKeygroup->setLayerPan( keys[0], selectedLayer, Value );
        };

        padLoop.onClick = [&]() {
            if( pKeygroup )
                pKeygroup->setLayerLoop( keys[0], selectedLayer, padLoop.getToggleState() );
        };

        reverse.onClick = [&]() {
            if( pKeygroup )
                pKeygroup->setLayerReverse( keys[0], selectedLayer, reverse.getToggleState() );
        };
    }

    void KeyGroupSampleTab2::selectLayer( int SelectedLayer )
    {
        selectedLayer = SelectedLayer;
        refresh();
    }

    void KeyGroupSampleTab2::refresh()
    {
        if( pKeygroup && keys.size() )
        {
            auto source = pKeygroup->getLayerSource( keys[0], selectedLayer );
            auto min{ 0 };
            auto max{ 1 };
            if( auto sPtr = mt::getAudioSoundCache().getOrLoad( source ) )
            {
                max = sPtr->getNumSamplesPerChannel();
                wav.setSource( sPtr->getData(), sPtr->getSampleRate() );

                loopStart.setValueBounds( min, max, min );
                loopEnd.setValueBounds( min, max, max );
            }
            else
                wav.setSource( {}, 0 );

            mt::utils::setEnabled( source.isNotEmpty(), loopStart, loopEnd, tailStart, tailLength, layerPan, level, offSet, padLoop, slice, reverse );

            loopStart.setValue( pKeygroup->getLayerLoopStart( keys[0], selectedLayer ), juce::dontSendNotification );
            auto loopEndValue = pKeygroup->getLayerLoopEnd( keys[0], selectedLayer );
            loopEnd.setValue( loopEndValue == -1 ? ( source.isNotEmpty() ? max : 0 ) : loopEndValue, juce::dontSendNotification );
            tailLength.setValue( pKeygroup->getLayerTailLength( keys[0], selectedLayer ), juce::dontSendNotification );
            tailStart.setValue( pKeygroup->getLayerTailStart( keys[0], selectedLayer ), juce::dontSendNotification );
            layerPan.setValue( pKeygroup->getLayerPan( keys[0], selectedLayer ), juce::dontSendNotification );
            level.setValue( CommonUtils::gainToMidi( pKeygroup->getLayerGain( keys[0], selectedLayer ) ), juce::dontSendNotification );
            offSet.setValue( pKeygroup->getLayerOffset( keys[0], selectedLayer ), juce::dontSendNotification );
            padLoop.setToggleState( pKeygroup->isLayerLooping( keys[0], selectedLayer ), juce::dontSendNotification );
            reverse.setToggleState( pKeygroup->isLayerReversed( keys[0], selectedLayer ), juce::dontSendNotification );
            slice.setValue( pKeygroup->getLayerSlice( keys[0], selectedLayer ), juce::dontSendNotification );
        }
    }

    void KeyGroupSampleTab2::visibilityChanged()
    {
        if( isVisible() )
            refresh();
    }

    void KeyGroupSampleTab2::handleCommand( CPktMgr::HWMsg_t& f_HwMsg )
    {
        if( contracts::DclContractsHelper::isPadButtonMessage( f_HwMsg ) )
        {
            auto padNum = contracts::DclContractsHelper::getPadNumber( f_HwMsg );
            keys[0]     = padNum + mt::getAudioInterface().getBankOffset();
            refresh();
        }
    }
}
