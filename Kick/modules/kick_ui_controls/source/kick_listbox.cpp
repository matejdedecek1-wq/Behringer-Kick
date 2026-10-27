/*
 * Copyright (c) 2021 MUSIC Tribe
 *
 * This file is part of Kick.
 *
 * Kick is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Affero General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * Kick is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with Kick. If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

namespace ui
{
    //////////////////////
    /// KICK Button
    //////////////////////
#define LISTBOX_RADIO_GROUP_ID       ( 0x5673245 )
#define IMAGE_LISTBOX_RADIO_GROUP_ID ( 0x5673246 )

#define D_spaceW                     ( 10 )
#define D_spaceH                     ( 9 )
#define D_spaceSurround              ( 10 )
#define D_noneBtnWidth               ( 322 )
#define D_noneBtnHeight              ( 34 )

    KickList::KickList( const juce::String& Title, const std::vector<juce::String>& Options, int Selected, int NumColumns )
        : mt::MTProperties( this )
        , noOfColumns( NumColumns )
    {
        setTitle( Title );
        setName( Title );
        setOpaque( true );

        setLookAndFeel( KickLooknFeel::getLnf() );

        setWantsKeyboardFocus( false );
        setScrollOnDragMode( juce::Viewport::ScrollOnDragMode::all );
        listComp.setWantsKeyboardFocus( false );

        setViewedComponent( &listComp, false );

        if( Options.size() )
            setContent( Options, Selected );
    }

    KickList::~KickList()
    {
        options.clear();
    }

    void KickList::addOption( const juce::String& Opt, int Index )
    {
        auto sz  = options.size();
        auto idx = Index + 1;

        if( sz < idx )
        {
            for( auto i = sz; i < idx; ++i )
            {
                auto& b = options.emplace_back( std::make_unique<KickToggleButton>( Opt ) );

                b->setRadioGroupId( LISTBOX_RADIO_GROUP_ID, juce::NotificationType::dontSendNotification );
                b->setID( i );
                b->showBorder( false );
                b->setValueColour( ui::Theme::textColour( 1 ) );
                b->setValueFontHeight( 16 );

                b->onClick = [&, b = b.get()]() {
                    if( b->getToggleState() )
                    {
                        if( !wasMouseDragging )
                        {
                            selection = b->getID();
                            onSelectionChanged( selection, b->getButtonText() );

                            if( isDialogOpen )
                            {
                                mt::utils::exitDialog( this, selection );
                            }
                        }
                    }
                };
            }
        }

        auto& b = options[Index];
        b->setToggleButtonText( Opt, Opt );
        b->setWantsKeyboardFocus( false );
        mt::utils::addAndMakeVisible( &listComp, b.get() );
    }

    void KickList::setContent( const std::vector<juce::String>& Data, int Selected, juce::NotificationType Notification )
    {
        // Cleanup.
        listComp.removeAllChildren();

        // Add options...
        numOptions = 0;
        for( auto& opt : Data )
            addOption( opt, numOptions++ );

        // Housekeeping...
        setHighlight( Selected, true );
        setSelected( Selected, Notification );
        resized();
    }

    const juce::String& KickList::getSelected()
    {
        static juce::String empty{ "" };
        if( juce::isPositiveAndBelow( selection, numOptions ) )
            return options[selection]->getButtonText();
        return empty;
    }

    std::vector<juce::String> KickList::getOptions()
    {
        std::vector<juce::String> list;
        for( auto& i : options )
        {
            list.push_back( i->getButtonText() );
        }
        return list;
    }

    void KickList::setColumns( int Cols )
    {
        noOfColumns = juce::jmax<int>( 1, Cols );
        resized();
    }

    void KickList::setSelected( int O, juce::NotificationType notification )
    {
        if( 0 > O )
        {
            selection = -1;
            for( auto& it : options )
                it->setToggleState( false, juce::dontSendNotification );
        }
        else if( 0 < numOptions )
        {
            selection = juce::jlimit( 0, ( numOptions - 1 ), O );
            options[selection]->setToggleState( true, juce::NotificationType::dontSendNotification );
        }
        if( notification != juce::NotificationType::dontSendNotification && onSelectionChanged )
        {
            if( 0 > selection || 0 == numOptions )
                onSelectionChanged( -1, "" );
            else
                onSelectionChanged( selection, options[selection]->getButtonText() );
        }
    }

    void KickList::setHighlight( int O, bool Highlight )
    {
        if( 0 < numOptions )
        {
            highlight = juce::jlimit( 0, ( numOptions - 1 ), O );
            for( int i = 0; i < numOptions; ++i )
                options[i]->setState( i == highlight ? juce::Button::buttonOver : juce::Button::buttonNormal );
        }
    }

    void KickList::setSelected( const juce::String& O, juce::NotificationType notification )
    {
        setSelected( getIndex( O ), notification );
    }

    int KickList::getIndex( const juce::String& O )
    {
        for( auto i = 0; i < numOptions; i++ )
            if( !O.compare( options[i]->getButtonText() ) )
                return i;
        return -1;
    }

    void KickList::setNormalized( float Value )
    {
        auto v = (int)( Value * numOptions );
        setSelected( v, juce::sendNotification );
    }

    float KickList::getNormalized()
    {
        return ( (float)selection / (float)numOptions );
    }

    void KickList::makeVisible( Component* Parent )
    {
        Parent->addAndMakeVisible( dynamic_cast<Component&>( *this ) );
    }

    void KickList::setSpacing( int H, int W, int S )
    {
        spaceH        = H;
        spaceW        = W;
        spaceSurround = S;
    }

    void KickList::setButtonHeight( int H )
    {
        rowHeight = H;
    }

    void KickList::drawCurvesAndOutline( const bool curves, const bool outline )
    {
        bCurves      = curves;
        bDrawOutline = outline;
        repaint();
    }

    void KickList::paint( juce::Graphics& g )
    {
        if( bCurves )
            mt::utils::fillRoundedRect( g, getLocalBounds(), backgroundColour, 6.f );
        else
            mt::utils::fillRoundedRect( g, getLocalBounds(), backgroundColour, 1.f, false, false, false, false );
        if( bDrawOutline )
            mt::utils::renderRoundedRect( g, getLocalBounds(), ui::Theme::backgroundColour( 0 ), 1, 6.f );
    }

    void KickList::resized()
    {
        if( getLocalBounds().isEmpty() || 0 == numOptions )
            return;

        //auto b = getLocalBounds().reduced( spaceSurround );
        auto b = getLocalBounds().reduced( 0, spaceSurround );
        b.removeFromLeft( spaceSurround );
        auto compWidth = b.getWidth();
        auto h         = rowHeight;
        auto w         = ( ( b.getWidth() - ( spaceW * ( noOfColumns - 1 ) ) ) / (noOfColumns)-spaceSurround / 2 );

        int column = 0, optionX = b.getX(), optionY = b.getY();
        for( auto i = 0; i < numOptions; ++i )
        {
            auto& opt = options[i];
            opt->setBounds( optionX, optionY, w, h );
            optionX += ( w + spaceW );
            if( ++column >= noOfColumns )
            {
                optionX = b.getX();
                optionY += ( h + spaceH );
                column = 0;
            }
        }

        listComp.setBounds( b.getX(), b.getY(), compWidth, ( optionY + h + spaceH ) );
    }

    void KickList::show( const juce::String& Title, std::function<void( int )> CallbackOnExit, juce::Rectangle<int> Bounds )
    {
        if( !isDialogOpen )
        {
            isDialogOpen = true;
            setHighlight( getSelectedIndex(), true );
            mt::utils::showOptions(
                Title.length() ? Title : getTitle(), this, [this, CallbackOnExit]( int Selected ) {
                    if( CallbackOnExit ) CallbackOnExit( Selected );
                    isDialogOpen = false;
                    // Regain focus
                    setFocus( true );
                },
                Bounds );
            refreshViewPosition();
        }
    }

    void KickList::sliderMoved( float Percent )
    {
        setViewPositionProportionately( 0.0, (double)Percent );
    };

    void KickList::knobRotated( double delta )
    {
        setHighlight( (int)( highlight + delta ), true );
        setViewPosition( getViewPositionX(), options[highlight].get()->getY() );
    }

    void KickList::knobClicked()
    {
        if( options.size() && highlight != -1 && highlight < options.size() )
            options[highlight]->triggerClick();
    }

    void KickList::setOptionEnable( int Index, bool State )
    {
        if( options.size() && Index != -1 && Index < options.size() )
            options[Index]->setEnabled( State );
    }

    void KickList::refreshViewPosition()
    {
        if( options.size() && highlight != -1 && highlight < options.size() )
            setViewPosition( getViewPositionX(), options[highlight].get()->getY() );
    }

    //////////////////////
    /// KICK List With Image
    //////////////////////

    KickListWithImage::KickListWithImage( const juce::String& Title, const std::vector<juce::String>& defaultOptions, const std::vector<juce::String>& activeOptions, int Selected, int NumColumns )
        : mt::MTProperties( this )
        , noOfColumns( NumColumns )
    {
        setTitle( Title );
        setName( Title );

        //setScrollOnDragMode( juce::Viewport::ScrollOnDragMode::all );
        listComp.setWantsKeyboardFocus( false );

        setViewedComponent( &listComp, false );

        if( defaultOptions.size() )
            setContent( defaultOptions, activeOptions, Selected );
    }

    KickListWithImage::~KickListWithImage()
    {
        options.clear();
    }

    void KickListWithImage::setContent( const std::vector<juce::String>& defaultOptions, const std::vector<juce::String>& activeOptions, int Selected, juce::NotificationType Notification )
    {
        // Cleanup.
        listComp.removeAllChildren();

        // Add options...
        numOptions = 0;
        for( auto i = 0; i < defaultOptions.size(); i++ )
        {
            addOption( defaultOptions[i], activeOptions[i], numOptions++ );
        }

        // Housekeeping...
        setSelected( Selected, Notification );
        resized();
    }

    void KickListWithImage::addOption( const juce::String& Opt1, const juce::String& Opt2, int Index )
    {
        if( options.size() < Index + 1 )
            options.resize( Index + 1 );

        auto& b = options[Index];
        if( !b.get() )
        {
            options[Index] = std::move( std::make_unique<KickImageButton>( "", Opt1, Opt2 ) );

            b->setID( Index );
            b->setRadioGroupId( IMAGE_LISTBOX_RADIO_GROUP_ID, juce::NotificationType::dontSendNotification );

            b->onClick = [it = b.get(), this]() {
                if( it->getToggleState() )
                {
                    selection = it->getID();
                    if( onSelectionChanged )
                        onSelectionChanged( selection, options[selection]->getButtonText() );
                }
            };
            mt::utils::addAndMakeVisible( &listComp, b.get() );
        }
    }

    void KickListWithImage::setSelected( int O, juce::NotificationType notification )
    {
        if( 0 > O )
        {
            selection = -1;
            for( auto& it : options )
                it->setToggleState( false, juce::dontSendNotification );
        }
        else if( 0 < numOptions )
        {
            selection = juce::jlimit( 0, ( numOptions - 1 ), O );
            options[selection]->setToggleState( true, juce::NotificationType::dontSendNotification );
        }
        if( notification != juce::NotificationType::dontSendNotification && onSelectionChanged )
        {
            if( 0 > selection || 0 == numOptions )
                onSelectionChanged( -1, "" );
            else
                onSelectionChanged( selection, options[selection]->getButtonText() );
        }
    }

    void KickListWithImage::paint( juce::Graphics& g )
    {
        auto b = getLocalBounds();
        mt::utils::fillRoundedRect( g, { b.getX(), b.getY(), b.getWidth(), b.getHeight() }, backgroundColour, 5.0f );
        mt::utils::renderRoundedRect( g, { b.getX(), b.getY(), b.getWidth(), b.getHeight() }, ui::Theme::outlineColour( 3 ), 2, 6.0f );
    }

    void KickListWithImage::resized()
    {
        if( getLocalBounds().isEmpty() || 0 == numOptions )
            return;

        auto b         = getLocalBounds().reduced( D_spaceSurround );
        auto compWidth = b.getWidth();

        auto h = rowHeight;
        auto w = ( b.getWidth() - ( D_spaceW * ( noOfColumns - 1 ) ) ) / ( noOfColumns );

        int column = 0, optionX = b.getX(), optionY = b.getY();
        for( auto i = 0; i < numOptions; ++i )
        {
            auto& opt = options[i];
            if( i == 0 )
            {
                opt->setBounds( optionX, optionY, D_noneBtnWidth, D_noneBtnHeight );
            }
            else
            {
                opt->setBounds( optionX, optionY, w, h );
            }

            optionX += ( w + D_spaceW );
            if( ( ++column >= noOfColumns ) || ( i == 0 ) )
            {
                optionX = b.getX();
                optionY += ( h + D_spaceH );
                column = 0;
            }
        }
        auto b1 = getLocalBounds();
        listComp.setBounds( b1.getX(), b1.getY(), b1.getWidth(), b1.getHeight() );
    }

    //////////////////////
    /// KICK List Box
    //////////////////////

    KickListBox::KickListBox( const juce::String& Title, int NumColumns )
        : KickListBox( Title, {}, 0, NumColumns )
    {}

    KickListBox::KickListBox( const juce::String& Title, const std::vector<juce::String>& Options, int Selected, int NumColumns )
        : KickListBox( BoxType::withTitleBar, Title, Options, Selected, NumColumns )
    {}

    KickListBox::KickListBox( BoxType Type, const juce::String& Title, const std::vector<juce::String>& Options, int Selected, int NumColumns )
        : mt::MTProperties( this )
        , type( Type )
        , list( Title, Options, Selected, NumColumns )
    {
        setTitle( Title );

        title.setPadding( 2 );
        value.setPadding( 8 );

        title.setColours( ui::Theme::textColour( 1 ), ui::Theme::backgroundColour( 9 ), ui::Theme::outlineColour( 0 ) );
        value.setColours( ui::Theme::textColour( 2 ), ui::Theme::backgroundColour( 3 ), ui::Theme::outlineColour( 1 ) );

        title.setValueJustification( juce::Justification::centredTop );
        value.showDropdown( true );
        value.setValueFontHeight( 18.f );

        setDisplayFormat( type );
        mt::utils::addAndMakeVisible( this, title, value, plus, minus );

        this->setPaintingIsUnclipped( true );
        this->setInterceptsMouseClicks( true, false );

        // Setup defaults...
        title.setText( Title, juce::dontSendNotification );
        value.setText( list.getSelected(), juce::dontSendNotification );

        addMouseListener( this, true );

        list.onSelectionChanged = [this]( int Index, const juce::String& Option ) {
            value.setText( Option, juce::dontSendNotification );

            if( onValueChanged )
                onValueChanged( Option );

            if( onSelectionChanged )
                onSelectionChanged( Index, Option );
        };

        minus.onClick = [&]() { setSelected( getSelectedIndex() - 1 ); };
        plus.onClick  = [&]() { setSelected( getSelectedIndex() + 1 ); };

        for( auto it : getChildren() )
        {
            it->setWantsKeyboardFocus( false );
        }
    }

    void KickListBox::setTitle( const juce::String& Title )
    {
        setName( Title );
        title.setText( Title, juce::dontSendNotification );
        list.setTitle( Title );
        value.setTitle( Title );
    }

    void KickListBox::setNumColumns( int NumColumns )
    {
        list.setColumns( NumColumns );
    }

    void KickListBox::setListOptions( const std::vector<juce::String>& Options, int SelectedIndex )
    {
        list.setContent( Options, SelectedIndex, juce::sendNotificationAsync );
    }

    void KickListBox::setDisplayFormat( BoxType Format )
    {
        type = Format;
        resized();
    }

    void KickListBox::setJustificationType( juce::Justification newJustification )
    {
        title.setValueJustification( newJustification );
        value.setValueJustification( newJustification );
    }

    void KickListBox::setJustificationType( juce::Justification TitleJustification, juce::Justification ValueJustification )
    {
        title.setValueJustification( TitleJustification );
        value.setValueJustification( ValueJustification );
    }

    juce::String KickListBox::getSelectedValue( void )
    {
        return value.getText();
    }

    const juce::String& KickListBox::getSelected()
    {
        return list.getSelected();
    }

    int KickListBox::getSelectedIndex()
    {
        return list.getSelectedIndex();
    }

    void KickListBox::setSelected( const juce::String& O, juce::NotificationType notification )
    {
        juce::MessageManager::callAsync( [this, O, notification] {
            list.setSelected( O, notification );
            value.setText( list.getSelected(), juce::dontSendNotification );
        } );
    }

    void KickListBox::setSelected( int O, juce::NotificationType notification )
    {
        juce::MessageManager::callAsync( [this, O, notification] {
            list.setSelected( O, notification );
            value.setText( list.getSelected(), juce::dontSendNotification );
        } );
    }

    std::vector<juce::String> KickListBox::getListOptions()
    {
        return list.getOptions();
    }

    void KickListBox::makeVisible( Component* Parent )
    {
        Parent->addAndMakeVisible( dynamic_cast<Component&>( *this ) );
    }

    void KickListBox::knobRotated( double deltaValue )
    {
        if( isEnabled() )
        {
            int val = getSelectedIndex() + deltaValue;
            setSelected( val < 0 ? 0 : val );
        }
    }

    void KickListBox::knobClicked()
    {
        if( isEnabled() )
            list.show( getName(), [&]( int ) {
                // Regain focus...
                setFocus( true );
            } );
    }

    void KickListBox::sliderMoved( float percentValue )
    {
        if( isEnabled() )
        {
            int index = static_cast<int>( percentValue * list.getSize() );
            setSelected( index );
        }
    }

    void KickListBox::mouseDoubleClick( const juce::MouseEvent& e )
    {
        knobClicked();
    }

    void KickListBox::mouseDown( const juce::MouseEvent& event )
    {
        Component::BailOutChecker checker( this );
        if( checker.shouldBailOut() )
            return;

        auto currentTime = juce::Time::getCurrentTime();

        if( ( currentTime - lastClickTime ).inMilliseconds() <= doubleClickTimeoutMs && currentTime != lastClickTime )
            mouseDoubleClick( event );
        else
            lastClickTime = currentTime;
    }

    void KickListBox::resized()
    {
        auto b = getLocalBounds();
        if( b.isEmpty() )
            return;

        for( auto i : getChildren() )
            i->setVisible( false );

        if( type == BoxType::withoutTitleBarWithNavigationButtons || type == BoxType::withTitleBarWithNavigationButtons )
        {
            auto w = b.getWidth();
            auto m = juce::jmax<int>( 38, (int)( w * .1f ) );
            mt::utils::makeVisible( minus, plus );
            minus.setBounds( b.removeFromLeft( m ) );
            plus.setBounds( b.removeFromRight( m ) );
        }

        if( type == BoxType::withTitleBar || type == BoxType::withTitleBarWithNavigationButtons )
        {
            mt::utils::makeVisible( title );
            title.setBounds( b.removeFromTop( b.proportionOfHeight( 0.35f ) ) );
        }

        mt::utils::makeVisible( value );
        value.setBounds( b );
    }

#undef D_spaceW
#undef D_spaceH
#undef D_spaceSurround
#undef D_noneBtnWidth
#undef D_noneBtnHeight

}
