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

namespace kick_ui
{
    class KickEffectsEdit : public ui::KickGrid
        , public juce::Button::Listener
    {
    public:
        KickEffectsEdit( std::vector<int>& Pads );
        ~KickEffectsEdit();
        void buttonClicked( juce::Button* button );

        void refresh();

    private:
        enum class LayerType
        {
            L1,
            L2,
            L3,
            L4
        };
        void addLayerOptions( LayerType type );

    private:
        void visibilityChanged() override
        {
            for( auto it : getChildren() )
                it->setVisible( isVisible() );

            if( isVisible() )
                refresh();
        }

    private:
        std::vector<int>&     pads;
        mt::MTAudioInterface& ae{ mt::getAudioInterface() };

        ui::KickListBox layerOne{ "L1", 1 };
        ui::KickListBox layerTwo{ "L2", 1 };
        ui::KickListBox layerThree{ "L3", 1 };
        ui::KickListBox layerFour{ "L4", 1 };

        juce::Component      dummy1;
        ui::KickImageButton  editL1{ "Edit", "effect1" };
        ui::KickImageButton  deleteL1{ "Delete1", "effect2" };
        ui::KickToggleButton onL1{ "On" };
        ui::KickRotary       sendL1{ "Send 1" };

        juce::Component      dummy2;
        ui::KickImageButton  editL2{ "Edit2", "effect1" };
        ui::KickImageButton  deleteL2{ "Delete2", "effect2" };
        ui::KickToggleButton onL2{ "On" };
        ui::KickRotary       sendL2{ "Send 2" };

        juce::Component      dummy3;
        ui::KickImageButton  editL3{ "Edit3", "effect1" };
        ui::KickImageButton  deleteL3{ "Delete3", "effect2" };
        ui::KickToggleButton onL3{ "On" };
        ui::KickRotary       sendL3{ "Send 3" };

        juce::Component      dummy4;
        ui::KickImageButton  editL4{ "Edit4", "effect1" };
        ui::KickImageButton  deleteL4{ "Delete4", "effect2" };
        ui::KickToggleButton onL4{ "On" };
        ui::KickRotary       sendL4{ "Send 4" };

        ui::KickButton onAll{ "On" };
        bool           l2added = false;
    };
}
