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
    //41, 43
    class FlattenPads : public juce::Component
        , public juce::Button::Listener
    {
    public:
        FlattenPads( std::vector<int>& Pads );
        ~FlattenPads();

        void paint( juce::Graphics& g );
        void resized();
        void buttonClicked( juce::Button* button ) override;

        void refresh();

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

        ui::KickListBox   pad{ "Pad", 4 };
        ui::KickNumber    audioTail{ "Audio Tail", 0, 99, 1, 1, true };
        ui::KickLabel     sampleName{ "New Sample Name" };
        ui::KickNameLabel sampleNameEdit{ "Sample 1" };
        ui::KickButton    apply{ "Apply" };
        ui::KickButton    cancel{ "Cancel" };
    };

}
