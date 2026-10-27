namespace kick_ui
{
    DrumGroupInsert::DrumGroupInsert( std::vector<int>& Pads, mt::MTDrumProgram*& PDrum )
        : pDrum( PDrum ), pads( Pads )
    {
        mt::utils::addAndMakeVisible( this, insert1Label, insert2Label, insert3Label, insert4Label, type, edit1, edit2, edit3, edit4, del1, del2, del3, del4, rotary1, rotary2, rotary3, rotary4 );
        mt::utils::addAndMakeVisible( this, cutoff, resonance, feedBack, time1, time2, dryWet1, dryWet2, dryWet3, width1, width2, ratio, insert1, insert2, insert3, insert4 );
        mt::utils::setValueColour( ui::Theme::textColour( 1 ), insert1, insert2, insert3, insert4, del1, del2, del3, del4, edit1, edit2, edit3, edit4 );
        mt::utils::setValueColour( ui::Theme::textColour( 2 ), insert1Label, insert2Label, insert3Label, insert4Label );
        mt::utils::setValueBackgroundColour( ui::Theme::backgroundColour( 1 ), insert1, insert2, insert3, insert4 );
        mt::utils::setValueBackgroundColour( ui::Theme::backgroundColour( 3 ), insert1Label, insert2Label, insert3Label, insert4Label );
        mt::utils::setValueFontHeight( 16, insert1, insert2, insert3, insert4 );
        mt::utils::setValueFontHeight( 18, insert1Label, insert2Label, insert3Label, insert4Label, del1, del2, del3, del4, edit1, edit2, edit3, edit4 );
        mt::utils::setValueJustification( juce::Justification::centred, insert1Label, insert2Label, insert3Label, insert4Label );

        auto insertAdder = [&]() {
            kick_ui::signals::onScreenData( kick_ui::Screen::Inserts, (void*)( dynamic_cast<mt::MTIInsertsInterface*>( pDrum ) ) );
            kick_ui::signals::sigScreenOpen( kick_ui::Screen::Inserts );
        };

        edit1.onClick = insertAdder;
        edit1.onClick = insertAdder;
        edit1.onClick = insertAdder;
        edit1.onClick = insertAdder;

        del1.onClick = [&]() { deleteInsert( 0 ); };
        del2.onClick = [&]() { deleteInsert( 1 ); };
        del3.onClick = [&]() { deleteInsert( 2 ); };
        del4.onClick = [&]() { deleteInsert( 3 ); };

        insert1.setPadding( 10 );
        insert2.setPadding( 10 );
        insert3.setPadding( 10 );
        insert4.setPadding( 10 );

        type.setColours( ui::Theme::textColour( 2 ), ui::Theme::textColour( 1 ) );

        ui::themeChanged.connect( [&]() {

        } );
    }

    void DrumGroupInsert::deleteInsert( unsigned int Position )
    {
        if( pDrum )
        {
            pDrum->removeInsert( Position );
            refresh();
        }
    }

    void DrumGroupInsert::refresh()
    {
        if( pDrum )
        {
            insert1Label.setText( pDrum->getInsertName( 0 ), juce::dontSendNotification );
            insert2Label.setText( pDrum->getInsertName( 1 ), juce::dontSendNotification );
            insert3Label.setText( pDrum->getInsertName( 2 ), juce::dontSendNotification );
            insert4Label.setText( pDrum->getInsertName( 3 ), juce::dontSendNotification );
        }
    }

    void DrumGroupInsert::handleCommand( CPktMgr::HWMsg_t& f_HwMsg )
    {
        if( contracts::DclContractsHelper::isPadButtonMessage( f_HwMsg ) )
        {
            auto padNum = contracts::DclContractsHelper::getPadNumber( f_HwMsg );
            pads[0]     = padNum + mt::getAudioInterface().getBankOffset();
            refresh();
        }
    }

    void DrumGroupInsert::visibilityChanged()
    {
        if( isVisible() )
            refresh();
    }

    void DrumGroupInsert::paint( juce::Graphics& g )
    {
        mt::utils::fillRoundedRect( g, { 0, 0, 1004, 94 }, ui::Theme::backgroundColour( 1 ) );
        mt::utils::fillRoundedRect( g, { 0, 98, 1004, 94 }, ui::Theme::backgroundColour( 1 ) );
        mt::utils::fillRoundedRect( g, { 0, 196, 1004, 94 }, ui::Theme::backgroundColour( 1 ) );
        mt::utils::fillRoundedRect( g, { 0, 294, 1004, 94 }, ui::Theme::backgroundColour( 1 ) );
    }

    void DrumGroupInsert::resized()
    {
        insert1.setBoundsRelative( .019f, .05f, .16f, .14f );
        insert1Label.setBoundsRelative( .19f, .038f, .14f, .165f );
        edit1.setBoundsRelative( .342f, .05f, .044f, .14f );
        del1.setBoundsRelative( .39f, .05f, .044f, .14f );
        type.setBoundsRelative( .45f, .013f, .12f, .165f );
        cutoff.setBoundsRelative( .609f, .013f, .08f, .217f );
        resonance.setBoundsRelative( .746f, .013f, .08f, .217f );
        dryWet1.setBoundsRelative( .882f, .013f, .08f, .217f );

        insert2.setBoundsRelative( .019f, .304f, .16f, .14f );
        insert2Label.setBoundsRelative( .19f, .291f, .14f, .165f );
        edit2.setBoundsRelative( .342f, .304f, .044f, .14f );
        del2.setBoundsRelative( .39f, .304f, .044f, .14f );
        time1.setBoundsRelative( .472f, .266f, .08f, .217f );
        feedBack.setBoundsRelative( .609f, .266f, .08f, .217f );
        width1.setBoundsRelative( .746f, .266f, .08f, .217f );
        dryWet2.setBoundsRelative( .882f, .266f, .08f, .217f );

        insert3.setBoundsRelative( .019f, .558f, .16f, .14f );
        insert3Label.setBoundsRelative( .19f, .545f, .14f, .165f );
        edit3.setBoundsRelative( .342f, .558f, .044f, .14f );
        del3.setBoundsRelative( .39f, .558f, .044f, .14f );
        time2.setBoundsRelative( .472f, .52f, .08f, .217f );
        ratio.setBoundsRelative( .609f, .52f, .08f, .217f );
        width2.setBoundsRelative( .746f, .52f, .08f, .217f );
        dryWet3.setBoundsRelative( .882f, .52f, .08f, .217f );

        insert4.setBoundsRelative( .019f, .812f, .16f, .14f );
        insert4Label.setBoundsRelative( .19f, .799f, .14f, .165f );
        edit4.setBoundsRelative( .342f, .812f, .044f, .14f );
        del4.setBoundsRelative( .39f, .812f, .044f, .14f );
        rotary1.setBoundsRelative( .472f, .774f, .08f, .217f );
        rotary2.setBoundsRelative( .609f, .774f, .08f, .217f );
        rotary3.setBoundsRelative( .746f, .774f, .08f, .217f );
        rotary4.setBoundsRelative( .882f, .774f, .08f, .217f );
    }

    DrumGroup::DrumGroup()
    {
        mt::utils::addAndMakeVisible( this, type, type1, rotary1, rotary2 );
        type.setTitleColour( ui::Theme::textColour( 1 ) );
        type1.setTitleColour( ui::Theme::textColour( 1 ) );
    }

    void DrumGroup::paint( juce::Graphics& g )
    {
        mt::utils::fillRoundedRect( g, { 0, 0, 1004, 94 }, ui::Theme::backgroundColour( 1 ) );
        mt::utils::fillRoundedRect( g, { 0, 98, 1004, 94 }, ui::Theme::backgroundColour( 1 ) );
        mt::utils::fillRoundedRect( g, { 0, 196, 1004, 94 }, ui::Theme::backgroundColour( 1 ) );
        mt::utils::fillRoundedRect( g, { 0, 294, 1004, 94 }, ui::Theme::backgroundColour( 1 ) );
    }

    void DrumGroup::resized()
    {
        type.setBounds( 62, 5, 200, 64 );
        type1.setBounds( 555, 5, 200, 64 );
        rotary1.setBounds( 366, 5, 85, 84 );
        rotary2.setBounds( 859, 5, 85, 84 );
    }

    DrumGroupComponent::DrumGroupComponent( std::vector<int>& Pads, mt::MTDrumProgram*& PDrum )
        : pDrum( PDrum ), pads( Pads )
    {
        mt::utils::addAndMakeVisible( this, d1, d2, d3, d4 );
    }

    void DrumGroupComponent::paint( juce::Graphics& g )
    {
    }

    void DrumGroupComponent::resized()
    {
        auto b = getLocalBounds();
        d1.setBounds( 0, 0, 1004, 94 );
        d2.setBounds( 0, 98, 1004, 94 );
        d3.setBounds( 0, 196, 1004, 94 );
        d4.setBounds( 0, 294, 1004, 94 );
    }

    void DrumGroupComponent::handleCommand( CPktMgr::HWMsg_t& f_HwMsg )
    {
        if( contracts::DclContractsHelper::isPadButtonMessage( f_HwMsg ) )
        {
            auto padNum = contracts::DclContractsHelper::getPadNumber( f_HwMsg );
            pads[0]     = padNum + mt::getAudioInterface().getBankOffset();
            refresh();
        }
    }
};
