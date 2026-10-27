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
    //
    //
    inline static constexpr int padding      = 10;
    inline static constexpr int itemSpacingW = 8;
    inline static constexpr int itemSpacingH = 9;

    KickMenuComponent::KickMenuComponent( const juce::String& Name, const int NumColumns, const juce::Rectangle<int>& Hdr )
        : noOfColumns( NumColumns )
        , headerSize( Hdr )
    {
        setName( Name );
    }

    KickMenuComponent::~KickMenuComponent() {}

    void KickMenuComponent::setColumns( int NumColumns )
    {
        noOfColumns = juce::jmax( 1, NumColumns );
        resized();
    }

    void KickMenuComponent::addOption( const MenuItem& Item, const juce::String& Group )
    {
        std::vector<MenuItem> Items{ Item };
        addOptions( Items, Group );
    }

    void KickMenuComponent::addOptions( const std::vector<MenuItem>& Items, const juce::String& Group )
    {
        AddOptionInternal( Items, Group );
        resized();
    }

    void KickMenuComponent::show( juce::Rectangle<int> bounds )
    {
        navGroup = navSelect = 0;
        setBounds( bounds );
        setFocus();
        setVisible( true );
        toFront( true );
    }

    void KickMenuComponent::hide()
    {
        setVisible( false );
    }

    void KickMenuComponent::AddOptionInternal( const std::vector<MenuItem>& Items, const juce::String& Group )
    {
        MenuGroup* entry{ nullptr };
        for( auto it : model )
        {
            if( it->name.getText().equalsIgnoreCase( Group ) )
            {
                entry = it;
                break;
            }
        }
        if( !entry )
        {
            model.add( new MenuGroup{ KickLabel{ Group }, {} } );
            entry = model.getLast();
            entry->name.makeVisible( this );
            entry->name.setInterceptsMouseClicks( false, false );
            entry->name.setColour( juce::Label::textColourId, ui::Theme::foregroundColour( 1 ) );
        }

        for( auto menuItem : Items )
        {
            entry->items.add( new KickImageButton{ menuItem.Name, menuItem.Icon } );
            auto item = entry->items.getLast();
            item->setEnabled( menuItem.enabled );
            auto index = entry->items.size() - 1;

            mt::utils::addAndMakeVisible( this, item );
            item->onClick = [this, item, index, entry]() {
                if( onMenuOptionClicked )
                    onMenuOptionClicked( item->getName(), index, entry->name.getText() );
                //hide();
            };
        }
    }

    void KickMenuComponent::resized()
    {
        auto b = getLocalBounds().reduced( padding );
        if( b.isEmpty() )
            return;

        if( model.size() )
        {
            int numHeaders = 0, numRows = 0;
            for( auto group : model )
            {
                numHeaders += group->name.getText().length() ? 1 : 0;
                numRows += (int)std::ceil( (float)group->items.size() / (float)noOfColumns );
            }

            auto totalPaddingH = ( itemSpacingH * ( ( numHeaders + numRows ) - 1 ) );
            auto totalPaddingW = ( itemSpacingW * ( noOfColumns - 1 ) );

            // Header height/width...
            auto hdrW = juce::jmin( b.getWidth(), headerSize.getWidth() );
            auto hdrH = juce::jmin( b.getHeight(), headerSize.getHeight() );

            // Divide remaining space for displaying items...
            auto itemW       = ( b.getWidth() - totalPaddingW ) / noOfColumns;
            auto availHeight = ( b.getHeight() - totalPaddingH ) - numHeaders * hdrH;
            auto itemH       = availHeight / numRows;

            for( auto group : model )
            {
                // Add group header row...
                if( group->name.getText().length() )
                {
                    auto bHdr = b.removeFromTop( hdrH );
                    group->name.setBounds( bHdr );
                    b.removeFromTop( padding );
                }

                // Add items/options...
                auto idx = 0;
                while( idx < group->items.size() )
                {
                    auto bRow = b.removeFromTop( itemH );
                    for( auto i = 0; i < noOfColumns; i++ )
                    {
                        if( idx < group->items.size() )
                        {
                            auto bItem = bRow.removeFromLeft( itemW );
                            group->items[idx++]->setBounds( bItem );
                            bRow.removeFromLeft( itemSpacingW );
                        }
                    }
                    b.removeFromTop( itemSpacingH );
                }
            }
        }
    }

    void KickMenuComponent::visibilityChanged()
    {
        if( isVisible() || isShowing() )
        {
            for( int i = 0; i < model[navGroup]->items.size(); i++ )
            {
                model[navGroup]->items[i]->setState( i == navSelect ? juce::Button::buttonOver : juce::Button::buttonNormal );
            }
        }
    }

    void KickMenuComponent::knobRotated( double delta )
    {
        if( 0 < model.size() && 0 < model[0]->items.size() )
        {
            navSelect = juce::jlimit( 0, model[navGroup]->items.size() - 1, (int)( navSelect + delta ) );
            for( int i = 0; i < model[navGroup]->items.size(); i++ )
            {
                model[navGroup]->items[i]->setState( i == navSelect ? juce::Button::buttonOver : juce::Button::buttonNormal );
            }
        }
    }

    void KickMenuComponent::knobClicked()
    {
        if( 0 < model.size() && 0 < model[navGroup]->items.size() )
        {
            model[navGroup]->items[navSelect]->triggerClick();
        }
    }

    void KickMenuComponent::padPressed( int PadNo, int State, int Velocity, int AfterTouch )
    {
        auto idx = PadNo % 16;
        idx      = 15 - idx;
        if( contracts::DeviceControlContracts::BtnState::BTN_PRESSED == State )
            model[navGroup]->items[idx]->triggerClick();
    }

    void KickMenuComponent::sliderMoved( float )
    {
    }

};
