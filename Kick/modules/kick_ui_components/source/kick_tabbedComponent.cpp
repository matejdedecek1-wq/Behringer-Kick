#include "kick_tabbedComponent.h"
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
    KickTabbedComponent::KickTabbedComponent( juce::TabbedButtonBar::Orientation orientation )
        : juce::TabbedComponent( orientation )
    {
        setLookAndFeel( KickLooknFeel::getLnf() );
        addChildComponent( back );
        addChildComponent( fullscreen );
        back.addListener( this );
        fullscreen.addListener( this );
        setOutline( 0 );
        setTabBarDepth( layout::tabBar.getHeight() );

        themeChanged.connect( [&]() {
            back.setImages( "Mode Back Button - Round" );
            fullscreen.setImages( "Mode Show Bar" );
        } );
    }

    void KickTabbedComponent::setBounds( juce::Rectangle<int> Normal, juce::Rectangle<int> FullScreen )
    {
        normalBounds     = Normal;
        fullScreenBounds = FullScreen;
        setupSize( isFullSize );
    }

    void KickTabbedComponent::showFullSize()
    {
        setupSize( true );
    }

    void KickTabbedComponent::showNormalSize()
    {
        setupSize( false );
    }

    void KickTabbedComponent::setMultiselectTab( int tabIndex, int noOfSelect )
    {
        multiSelectTab.insert( { tabIndex, noOfSelect } );
    }

    void KickTabbedComponent::setupSize( bool isFull )
    {
        isFullSize = isFull;
        back.setVisible( isFull );
        fullscreen.setVisible( isFull );

        TabbedComponent::setBounds( isFullSize ? fullScreenBounds : normalBounds );
    }

    void KickTabbedComponent::currentTabChanged( int newTabIndex, const juce::String& NewTabName )
    {
        //DBG( "Tab: " << NewTabIndex << ", Name: " << NewTabName );
        if( newTabIndex != currentTab )
        {
            auto multiIt = multiSelectTab.find( currentTab );
            if( multiIt != multiSelectTab.end() )
            {
                KickMultiSelectTabBarButton* tabButton = static_cast<KickMultiSelectTabBarButton*>( getTabbedButtonBar().getTabButton( currentTab ) );
                tabButton->clearSelection();
            }
        }
        currentTab = newTabIndex;
        if( onTabChanged )
            onTabChanged( newTabIndex, NewTabName );
    }

    void KickTabbedComponent::buttonClicked( juce::Button* Button )
    {
        if( Button == &back )
        {
            if( onBackButtonPressed )
                onBackButtonPressed( Button->getToggleState() );
        }
        else if( Button == &fullscreen )
        {
            setupSize( !isFullSize );
        }
        else
        {
        }
    }

    void KickTabbedComponent::resized()
    {
        juce::TabbedComponent::resized();

        auto&                bar = getTabbedButtonBar();
        juce::Rectangle<int> b   = bar.getBounds();

        if( isFullSize )
        {
            auto depth = getTabBarDepth();
            back.setBounds( b.getX(), b.getY(), depth, depth );
            fullscreen.setBounds( b.getTopRight().getX() - depth, b.getY(), depth, depth );
            b = bar.getBounds().withTrimmedLeft( depth ).withTrimmedRight( depth );
        }
        getTabbedButtonBar().setBounds( b );
    }

    juce::TabBarButton* KickTabbedComponent::createTabButton( const juce::String& tabName, int tabIndex )
    {
        auto multiIt = multiSelectTab.find( tabIndex );
        if( multiIt != multiSelectTab.end() )
        {
            auto* multiBtn    = new KickMultiSelectTabBarButton( multiIt->second, getTabbedButtonBar(), tabName );
            multiBtn->onClick = [tabName, tabIndex, this, multiBtn]() {
                multiBtn->incrementSelection();
                if( onMultiSelctTabChanged )
                    onMultiSelctTabChanged( tabIndex, multiBtn->getCurrentSelection(), tabName );
            };

            return multiBtn;
        }
        else
        {
            return TabbedComponent::createTabButton( tabName, tabIndex );
        }
    }
}
