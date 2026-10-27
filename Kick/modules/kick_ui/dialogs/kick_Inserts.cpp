
namespace kick_ui
{
#define insertGridHorSpacing 3
#define insertGridVerSpacing 30
#define insertTotalRows      2

    ///
    /// Inserts : Main screen interface class
    ///
    Inserts::Inserts()
        : kick_ui::IScreen( kick_ui::Screen::Inserts )
    {
        mt::utils::addAndMakeVisible( this, header ); // Visible
        mt::utils::addChildComponents( this, list, selector, options ); // Not visible by default.

        header.onBackButtonPressed = [this]( bool ) { closeScreen(); };
        header.showOptions( true, true );

        ui::themeChanged.connect( [&]() { header.setIcon( "Icon_Mixer" ); } );

        list.onInsertChanged = [&]( int i, const juce::String& name, bool value ) { onInsertChanged( i, name, value ); };
        list.onInsertDelete  = [&]( int i, const juce::String& name, bool value ) { onInsertDelete( i, name, value ); };
    };

    void Inserts::resized()
    {
        auto b = getLocalBounds();
        header.setBounds( b.removeFromTop( ui::layout::header.getHeight() ) );
        list.setBounds( b.removeFromLeft( b.proportionOfWidth( .3f ) ).reduced( 5, 10 ) );
        //auto bb = b.reduced( 5, 10 );
        options.setBounds( b.reduced( 5, 10 ) );
        selector.setBounds( options.getBounds() );
    }

    void Inserts::visibilityChanged()
    {
        auto visible = isVisible();

        if( !visible )
            kick_ui::signals::onScreenData( kick_ui::Screen::Inserts, nullptr );

        for( auto& it : getChildren() )
            it->setVisible( visible );
    }

    ///
    /// InsertsList
    ///
    InsertsList::InsertsList( InsertsSelector& Selector, InsertsOptions& InsertsOptions )
        : selector( Selector ), options( InsertsOptions )
    {
        mt::utils::addAndMakeVisible( this, allInserts, allDelete, allEnable );

        mt::utils::setValueFontHeight( 16, allInserts );
        mt::utils::setValueColour( ui::Theme::textColour( 2 ), allInserts );
        mt::utils::setValueJustification( juce::Justification::centredBottom, allInserts );

        allEnable.setToggleButtonText( "ENABLED", "BYPASSED" );
        //allDelete.setEnabled( false );
        //allEnable.setEnabled( false );

        // Connect signal.
        kick_ui::signals::onScreenData.connect( [&]( Screen Scr, void* PData ) {
            if( Screen::Inserts == Scr )
            {
                pEffects = (mt::MTIInsertsInterface*)PData;
            }
        } );

        for( auto i = 0; i < numRows; ++i )
        {
            add[i].setEnabled( false );
            enable[i].setEnabled( false );
            add[i].setToggleButtonText( "DELETE", "ADD" );
            enable[i].setToggleButtonText( "ENABLED", "BYPASSED" );
            inserts[i].setValueFontHeight( 18 );

            mt::utils::addAndMakeVisible( this, inserts[i], enable[i], add[i] );

            inserts[i].onClick = [&, i] {
                auto insName = inserts[i].getButtonText();
                if( insName.isEmpty() || insName.equalsIgnoreCase( "None" ) )
                {
                    options.setVisible( false );
                    selector.setVisible( true );
                    add[i].setEnabled( true );
                }
                else if( pEffects )
                {
                    options.setVisible( true );
                    options.setupControls( pEffects, i );
                }
                selected = i;
                repaint();
            };

            add[i].onClick = [&, i] {
                if( add[i].getToggleState() )
                {
                    selector.setVisible( false );
                    options.setVisible( true );
                    // Add insert...
                    auto s = selector.getSelected();
                    if( pEffects )
                        pEffects->addInsert( s, i );
                }
                else
                {
                    selector.setVisible( true );
                    options.setVisible( false );
                    // Remove insert...
                    if( pEffects )
                        pEffects->removeInsert( i );
                    inserts[i].triggerClick();
                }
                if( pEffects )
                {
                    refresh();
                    if( add[i].getToggleState() ) options.setupControls( pEffects, i );
                }
            };

            enable[i].onClick = [&, i] {
                if( pEffects )
                {
                    pEffects->setInsertBypass( i, !enable[i].getToggleState() );
                    refresh();
                    options.refresh();
                }
            };
        }

        options.onEnableClicked = [&]( int Position, bool State ) {
            enable[Position].setToggleState( State, juce::sendNotification );
        };

        options.onRotaryChanged = [&]( int Position, int Parameter, float Value ) {
            if( pEffects )
            {
                pEffects->setParameterValue( Position, Parameter, Value );
                refresh();
            }
        };

        allEnable.onClick = [&]() {
            auto bEnable = allEnable.getToggleState();
            for( auto i = 0; i < numRows; ++i )
            {
                if( enable[i].isEnabled() )
                    enable[i].setToggleState( bEnable, juce::sendNotification );
            }
        };

        allDelete.onClick = [&]() {
            if( false == allDelete.getToggleState() )
                for( auto i = 0; i < numRows; ++i )
                {
                    if( add[i].isEnabled() && pEffects )
                        pEffects->removeInsert( i );
                }
            refresh();
            inserts[selected != -1 ? selected : 0].triggerClick();
        };
    };

    int InsertsList::getDefaultIndex()
    {
        if( pEffects )
        {
            for( auto i = 0; i < numRows; ++i )
            {
                auto insertInfo = pEffects ? pEffects->getInsertsInfo( i ) : mt::MTInsertsInfo{};
                if( insertInfo.name.isNotEmpty() )
                    return i;
            }
        }
        return 0;
    }

    void InsertsList::refresh()
    {
        auto infoSz = pEffects ? pEffects->getInserts()->size() : 0;

        auto isMasterEnable = false;
        for( auto i = 0; i < numRows; ++i )
        {
            auto insertInfo = pEffects ? pEffects->getInsertsInfo( i ) : mt::MTInsertsInfo{};

            auto name      = insertInfo.name;
            auto isAdded   = name.isNotEmpty();
            auto isEnabled = !insertInfo.bypass;

            inserts[i].setButtonText( name );

            add[i].setToggleState( isAdded, juce::dontSendNotification );
            enable[i].setToggleState( isEnabled, juce::dontSendNotification );

            enable[i].setEnabled( isAdded );
            add[i].setEnabled( isAdded );

            if( isAdded )
                isMasterEnable = true;
        }
        allEnable.setEnabled( isMasterEnable );
        allDelete.setEnabled( isMasterEnable );
        if( selected == -1 )
        {
            selected = isMasterEnable ? 0 : -1;
            repaint();
        }
    }

    void InsertsList::visibilityChanged()
    {
        auto visible = isVisible();
        if( visible )
        {
            refresh();
            auto defaultIndex = getDefaultIndex();
            options.setupControls( pEffects, defaultIndex ); //default
            inserts[defaultIndex].triggerClick();
        }
    }

    void InsertsList::paint( juce::Graphics& g )
    {
        mt::utils::fillRoundedRect( g, getLocalBounds(), ui::Theme::backgroundColour( 1 ) );
        if( selected != -1 )
            mt::utils::renderRoundedRect( g, { inserts[0].getX() - 5, inserts[selected].getY() - 5, getWidth() - 20, inserts[selected].getHeight() + 10 }, ui::Theme::outlineColour( 1 ) );
    }

    void InsertsList::resized()
    {
        auto b = getLocalBounds().reduced( 10 );
        b.removeFromTop( 10 );
        auto topRow = b.removeFromTop( 16 );

        const auto rowHeight   = b.proportionOfHeight( ( 1.f / ( numRows + 1 ) ) );
        const auto buttonWidth = b.proportionOfWidth( 0.25f );

        auto allRow = b.removeFromTop( rowHeight );
        // Positioning of all insert controls
        allInserts.setBounds( topRow.removeFromRight( 2 * buttonWidth ) );
        allDelete.setBounds( allRow.removeFromRight( buttonWidth ).reduced( 5, 10 ) );
        allEnable.setBounds( allRow.removeFromRight( buttonWidth ).reduced( 5, 10 ) );
        // Position insert rows.
        for( auto i = 0; i < numRows; ++i )
        {
            auto rowB = b.removeFromTop( rowHeight );
            add[i].setBounds( rowB.removeFromRight( buttonWidth ).reduced( 5, 10 ) );
            enable[i].setBounds( rowB.removeFromRight( buttonWidth ).reduced( 5, 10 ) );
            inserts[i].setBounds( rowB.reduced( 5 ) );
        }
    }

    ///
    /// InsertsSelector
    ///
    InsertsSelector::InsertsSelector()
    {
        mt::utils::addAndMakeVisible( this, insertCategories, insertList );

        insertCategories.onSelectionChanged = [&]( int index, const juce::String& selectedOption ) {
            auto list = mt::getAudioInterface().getAvailableInsertsListByCatogary( selectedOption );
            insertList.setContent( list, juce::dontSendNotification );
        };

        insertList.onSelectionChanged = [&]( int index, const juce::String& selectedOption ) {
            onClick( selectedOption );
        };

        juce::MessageManager::callAsync( [&]() {
            auto category = mt::getAudioInterface().getInsertCategory();
            insertCategories.setContent( category, juce::sendNotification );
        } );
    }

    void InsertsSelector::paint( juce::Graphics& g )
    {
        mt::utils::fillRoundedRect( g, insertCategories.getBounds(), ui::Theme::backgroundColour( 2 ) );
        mt::utils::fillRoundedRect( g, insertList.getBounds(), ui::Theme::backgroundColour( 3 ) );
    }

    void InsertsSelector::resized()
    {
        auto b = getLocalBounds();
        insertCategories.setBounds( b.removeFromLeft( b.proportionOfWidth( .35f ) ) );
        b.removeFromLeft( 5 );
        insertList.setBounds( b );
    }

    ///
    /// InsertsOptions
    ///
    InsertsOptions::InsertsOptions()
    {
        mt::utils::setValueFontHeight( 20, insertName );
        mt::utils::setValueColour( ui::Theme::textColour( 2 ), insertName );
        mt::utils::setValueJustification( juce::Justification::centred, insertName );

        enable.setToggleButtonText( "ENABLED", "BYPASSED" );

        enable.onClick = [&]() { onEnableClicked( pos, enable.getToggleState() ); };
    }

    void InsertsOptions::setupControls( mt::MTIInsertsInterface* pEffect, int Position )
    {
        pInsert = pEffect;
        pos     = Position;

        // Resize controls.
        info = pInsert ? pInsert->getInsertsInfo( pos ) : mt::MTInsertsInfo{};
        setupControls( info.params.size() );

        // Update values.
        refresh();
    }

    void InsertsOptions::refresh()
    {
        info = pInsert ? pInsert->getInsertsInfo( pos ) : mt::MTInsertsInfo{};

        insertName.setText( info.name, juce ::dontSendNotification );
        enable.setToggleState( !info.bypass, juce::dontSendNotification );
        for( auto i = 0; i < info.params.size(); ++i )
        {
            rotary[i]->setTitle( info.params[i].name );
            rotary[i]->setValue( info.params[i].value, juce::dontSendNotification );
        }
    }

    void InsertsOptions::setupControls( int NumControls )
    {
        // Resize; if required.
        if( NumControls > rotary.size() )
        {
            auto i = (int)rotary.size();
            for( ; i < NumControls; ++i )
            {
                auto ptr = std::make_unique<ui::KickRotary>( "" );
                // Setup rotary
                ptr->showTitleAndValue( true, juce::Slider::TextBoxBelow, juce::Justification::centred );
                ptr->setDisplayProportions( 0.6f, 0.2f, 0.2f );
                ptr->setRange( 0.f, 1.f, 0.001f );
                ptr->onValueChanged = [&, i]( double Value ) { onRotaryChanged( pos, i, (float)Value ); };

                // Insert this control to vector.
                rotary.emplace_back( std::move( ptr ) );
            }
        }

        // Clear all rotaties...
        removeAllChildren();
        for( auto& it : rotary )
        {
            it->setTitle( "" );
            it->setValue( 0.0, juce::dontSendNotification );
        }

        // Enable required number of controls...
        addAndMakeVisible( insertName );
        addAndMakeVisible( enable );
        for( auto i = 0; i < NumControls; ++i )
            addAndMakeVisible( rotary[i].get() );

        resized();
    }

    void InsertsOptions::visibilityChanged()
    {
        if( !isVisible() )
            setupControls( 0 );
    }

    void InsertsOptions::paint( juce::Graphics& g )
    {
        mt::utils::fillRoundedRect( g, getLocalBounds(), ui::Theme::backgroundColour( 3 ) );
    }

    void InsertsOptions::resized()
    {
        auto b = getLocalBounds().reduced( 20 );
        // Name
        auto topBar = b.removeFromTop( 30 ).reduced( 20, 1 );
        enable.setBounds( topBar.removeFromRight( 75 ) );
        topBar.removeFromRight( 10 );
        insertName.setBounds( topBar );
        // Controls
        auto w = b.proportionOfWidth( 0.25f );
        auto h = b.proportionOfHeight( 0.25f );

        auto numControls = (int)info.params.size();
        auto idx         = 0;
        while( idx < numControls )
        {
            auto rowB = b.removeFromTop( h ).reduced( 5 );
            for( int j = 0; j < 4; ++j, ++idx )
            {
                auto colB = rowB.removeFromLeft( w ).reduced( 5 );
                if( idx < numControls )
                {
                    rotary[idx]->setBounds( colB );
                }
                else
                    break;
            }
        }
    }

#undef insertGridHorSpacing
#undef insertGridVerSpacing
#undef insertTotalRows
}
