namespace kick_ui
{
    XyFxSetupScreen::XyFxSetupScreen()
    {
        attack.showTitleAndValue( true, juce::Slider::TextBoxBelow );
        release.showTitleAndValue( true, juce::Slider::TextBoxBelow );
        wetDry.showTitleAndValue( true, juce::Slider::TextBoxBelow );

        mt::utils::addAndMakeVisible( this, xAxisLabel, yAxisLabel, presets, attack, release, wetDry, xCoordinateValue, yCoordinateValue );
        mt::utils::setTitleColour( ui::Theme::textColour( 1 ), xAxisLabel, yAxisLabel );
        mt::utils::setValueJustification( juce::Justification::centred, xAxisLabel, yAxisLabel, xCoordinateValue, yCoordinateValue );
        mt::utils::setValueJustification( juce::Justification::centredBottom, attack, release, wetDry );
        mt::utils::setTitleJustification( juce::Justification::centredTop, attack, release, wetDry, xAxisLabel, yAxisLabel );
        mt::utils::setValueFontHeight( 18, attack, release, wetDry, xAxisLabel, yAxisLabel, xCoordinateValue, yCoordinateValue, xCoordinateValue, yCoordinateValue );
        mt::utils::setTitleFontHeight( 16, attack, release, wetDry, presets, xAxisLabel, yAxisLabel );
        mt::utils::setValueProportion( .35f, xAxisLabel, yAxisLabel );
        mt::utils::setValueBackgroundColour( ui::Theme::backgroundColour( 13 ), xAxisLabel, yAxisLabel, xCoordinateValue, yCoordinateValue );

        presets.setListOptions( mt::Kick::xyFxPresets );
        setXYCoordinateValues( 0, 0 );

        release.setTextValueSuffix( " %" );
        wetDry.setTextValueSuffix( " %" );

        attack.setRange( 0, 100, 1 );
        release.setRange( 0, 100, 1 );
        wetDry.setRange( 0, 100, 1 );

        presets.setJustificationType( juce::Justification::centred );
        presets.setTitleColour( ui::Theme::textColour( 1 ) );

        presets.onSelectionChanged = [&]( int Index, const juce::String& value ) {
            xAxisLabel.setText( mt::Kick::xyPresetsCoOrdinates.at( Index ).at( 0 ), juce::dontSendNotification );
            yAxisLabel.setText( mt::Kick::xyPresetsCoOrdinates.at( Index ).at( 1 ), juce::dontSendNotification );
            if( onPresetChange )
                onPresetChange();
        };
    }

    void XyFxSetupScreen::paint( juce::Graphics& g )
    {
        auto b = getLocalBounds();
        g.setColour( ui::Theme::backgroundColour() );
        g.fillRoundedRectangle( getLocalBounds().toFloat(), 3 );
        g.setColour( ui::Theme::outlineColour( 3 ) );
        g.drawRoundedRectangle( b.getX(), b.getY(), b.getWidth(), b.getHeight(), cornerSizeX, lineThickness );
    }

    void XyFxSetupScreen::resized()
    {
        presets.setBoundsRelative( .0584f, .0454f, .8830f, .1409f );
        attack.setBoundsRelative( .0584f, .209f, .8830f, .1409f );
        release.setBoundsRelative( .0584f, .3727f, .8830f, .1409f );
        wetDry.setBoundsRelative( .0584f, .5363f, .8830f, .1409f );

        xAxisLabel.setBoundsRelative( .0584f, .7f, .4269f, .1409f );
        yAxisLabel.setBoundsRelative( .5146f, .7f, .4269f, .1409f );
        xCoordinateValue.setBoundsRelative( .0584f, .8545f, .4269f, .1f );
        yCoordinateValue.setBoundsRelative( .5146f, .8545f, .4269f, .1f );
    }

    std::vector<juce::String> XyFxSetupScreen::getXYdata()
    {
        std::vector<juce::String> data;
        data.push_back( xAxisLabel.getText() );
        data.push_back( yAxisLabel.getText() );
        return data;
    }

    void XyFxSetupScreen::setXYCoordinateValues( const float xCoord, const float yCoord )
    {
        if( xAxisLabel.getText() == "Beat Division" )
            xCoordinateValue.setText( mt::Kick::QuantizeStrings.at( xCoord + 1 ), juce::dontSendNotification );
        else
            xCoordinateValue.setText( juce::String( xCoord, 3 ), juce::dontSendNotification );

        yCoordinateValue.setText( juce::String( yCoord, 3 ), juce::dontSendNotification );
    }

    void XyFxSetupScreen::knobCommand( int knob, int value )
    {
        switch( knob )
        {
            case contracts::DeviceControlContracts::KnobSequence::EN_KNOB_1:
                presets.knobRotated( value );
                break;
            case contracts::DeviceControlContracts::KnobSequence::EN_KNOB_2:
                attack.knobRotated( value );
                break;
            case contracts::DeviceControlContracts::KnobSequence::EN_KNOB_3:
                release.knobRotated( value );
                break;
            case contracts::DeviceControlContracts::KnobSequence::EN_KNOB_4:
                wetDry.knobRotated( value );
                break;
        }
    }

    XyFxView::XyFxView()
        : kick_ui::IScreen( kick_ui::Screen::HomeXYView )
    {
        mt::utils::addAndMakeVisible( this, setupScreen, latch, setup, xyFx, header, on_off, output, hdrComp );
        mt::utils::addListener( this, latch, setup );
        xyFx.setEnabled( false );

        hdrComp.setValueBackgroundColor( ui::Theme::backgroundColour( 3 ) );
        header.showOptions( false, true );
        header.onBackButtonPressed = [this]( bool ) { closeScreen( Screen::HomeListView ); };
        output.setJustificationType( juce::Justification::centredLeft );
        output.setTitleColour( ui::Theme::textColour( 1 ) );
        output.setListOptions( { "Output 1", "Output 2", "Output 1, 2" } );

        mt::utils::setValueColour( ui::Theme::textColour( 1 ), latch, setup, on_off );
        mt::utils::setValueFontHeight( 18, latch, setup, on_off );
        output.setTitleFontHeight( 16 );

        xyFx.setXYCoordinates( setupScreen.getXYdata() );

        on_off.onClick = [&]() {
            xyFx.setEnabled( on_off.getToggleState() );
            xyFx.repaint();
        };

        setupScreen.onPresetChange = [&]() {
            xyFx.setXYCoordinates( setupScreen.getXYdata() );
            setupScreen.setXYCoordinateValues( xyFx.getXCoordinate(), xyFx.getYCoordinate() );
        };

        xyFx.onValueChanged = [&]( const float xCoord, const float yCoord ) {
            setupScreen.setXYCoordinateValues( xCoord, yCoord );
        };

        ui::themeChanged.connect( [&]() {
            latch.setToggleButtonText( "Latch", "Latch" );
            setup.setToggleButtonText( "Setup", "Setup" );
            on_off.setToggleButtonText( "Off", "On" );
            header.setIcon( "Icon_XY" );
        } );
    }

    void XyFxView::resized()
    {
        auto b = getLocalBounds();
        header.setBounds( b.removeFromTop( ui::layout::header.getHeight() ) );
        on_off.setBoundsRelative( .8082f, .0299f, .0880f, .0823f );
        latch.setBoundsRelative( .0097f, .9213f, .1174f, .0636f );
        setup.setBoundsRelative( .8747f, .9213f, .1174f, .0636f );
        xyFx.setBoundsRelative( .0097f, .1610f, .9823f, .7391f );
        output.setBoundsRelative( .2250f, .0299f, .1956f, .0823f );
        hdrComp.setBoundsRelative( .4305f, .0299f, .1956f, .0823f );
        setupScreen.setBoundsRelative( .0f, .0f, .0f, .0f );
    }

    void XyFxView::visibilityChanged()
    {
    }

    void XyFxView::buttonClicked( juce::Button* button )
    {
        if( button == &setup )
        {
            if( button->getToggleState() )
            {
                xyFx.setBoundsRelative( .0097f, .1610f, .6330f, .7391f );
                setupScreen.setBoundsRelative( .6575f, .1610f, .3346f, .8243f );
                setup.setBoundsRelative( .5273f, .9213f, .1174f, .0636f );
            }
            else
            {
                resized();
            }
        }
        else if( button == &latch )
        {
            xyFx.setLatch( latch.getToggleState() );
        }
    }

    void XyFxView::handleCommand( CPktMgr::HWMsg_t& f_HwMsg )
    {
        if( contracts::DclContractsHelper::isKnobMessage( f_HwMsg ) )
        {
            auto knobID    = contracts::DclContractsHelper::getKnobCode( f_HwMsg );
            auto knobValue = contracts::DclContractsHelper::getKnobValue( f_HwMsg );
            if( knobID != contracts::DeviceControlContracts::EN_KNOB_CENTER )
                setupScreen.knobCommand( knobID, knobValue );
        }
        else if( contracts::DclContractsHelper::isButtonMessage( f_HwMsg ) && ( contracts::DclContractsHelper::getButtonState( f_HwMsg ) == contracts::DeviceControlContracts::BTN_PRESSED ) )
        {
            auto  buttonCode = contracts::DclContractsHelper::getButtonCode( f_HwMsg );
            auto& ae         = mt::getAudioInterface();
            auto& aeSel      = ae.getSelected();
            switch( buttonCode )
            {
                case contracts::DeviceControlContracts::EN_PLAY: ae.setSequencePlay( aeSel.sequence, false, true ); return;
                case contracts::DeviceControlContracts::EN_PLAYSTART: ae.setSequencePlay( aeSel.sequence, true, true ); return;
                case contracts::DeviceControlContracts::EN_STOP: ae.setSequencePause(); return;
            }
        }
    }

}
