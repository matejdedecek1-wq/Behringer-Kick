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

#pragma once

namespace ui
{
    class KickTabbedComponent : public juce::TabbedComponent
        , public juce::Button::Listener
    {
    public:
        std::function<void( bool )>                                                                  onBackButtonPressed;
        std::function<void( int NewTabIndex, const juce::String& NewTabName )>                       onTabChanged;
        std::function<void( int NewTabIndex, int multiSelectIndex, const juce::String& NewTabName )> onMultiSelctTabChanged;

        explicit KickTabbedComponent( juce::TabbedButtonBar::Orientation orientation );

        void setBounds( juce::Rectangle<int> Normal, juce::Rectangle<int> FullScreen = layout::screen );
        void showFullSize();
        void showNormalSize();
        void setMultiselectTab( int tabIndex, int noOfSelect );

    private:
        juce::TabBarButton*  createTabButton( const juce::String& tabName, int tabIndex ) override;
        void                 resized() override;
        bool                 isFullSize{ false };
        juce::Rectangle<int> normalBounds{ layout::screenExcludingHeader }, fullScreenBounds{ layout::screen };
        KickImageButton      back{ "Back", "Mode Back Button - Round" };
        KickImageButton      fullscreen{ "FullScreen", "Mode Show Bar" };

        void         setupSize( bool isFull );
        virtual void currentTabChanged( int NewTabIndex, const juce::String& NewTabName ) override final;
        virtual void buttonClicked( juce::Button* Button ) override;

    private:
        std::map<int, int> multiSelectTab;
        int                currentTab = 0;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( KickTabbedComponent )
    };
}
