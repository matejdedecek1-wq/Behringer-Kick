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
    ///
    ///

    struct MenuItem
    {
        juce::String Name;
        juce::String Icon;
        bool         enabled = true;
        //String Help;
        MenuItem() {}
        MenuItem( const juce::String& N, const juce::String& I )
            : Name( N ), Icon( I ){};
    };

    class KickMenuComponent : public KickControlBase<juce::Component>
    {
    public:
        std::function<void( const juce::String& Option, int index, const juce::String& Group )> onMenuOptionClicked;

        explicit KickMenuComponent( const juce::String& Name, const int NumColumns = 4,
                                    const juce::Rectangle<int>& Hdr = ui::layout::title );
        ~KickMenuComponent();

        void setColumns( int NumColumns );

        void addOption( const MenuItem& Item, const juce::String& Group );
        void addOptions( const std::vector<MenuItem>& Items, const juce::String& Group = "" );

        void hide();
        void show( juce::Rectangle<int> bounds );

        void visibilityChanged();

    private:
        struct MenuGroup
        {
            KickLabel                         name;
            juce::OwnedArray<KickImageButton> items;
        };

        int                         noOfColumns{ 4 }, navSelect{ 0 }, navGroup{ 0 };
        juce::OwnedArray<MenuGroup> model;
        juce::Rectangle<int>        headerSize;

        void AddOptionInternal( const std::vector<MenuItem>& Items, const juce::String& Group = "" );
        void resized() override;

        void knobRotated( double ) override;
        void knobClicked() override;
        void padPressed( int PadNo, int State, int Velocity, int Aftertouch ) override;
        void sliderMoved( float ) override;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( KickMenuComponent )
    };

}
