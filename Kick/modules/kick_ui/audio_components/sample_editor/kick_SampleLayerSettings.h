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
namespace mt
{
    namespace ac
    {
        class SampleLayerSettings : public juce::Component
            , public juce::Button::Listener
        {
        public:
            SampleLayerSettings( SampleEditorData& data );
            ~SampleLayerSettings();
            void paint( juce::Graphics& g ) override;
            void resized() override;
            void buttonClicked( juce::Button* button ) override;
            void refresh();

        private:
            ui::KickLabel        linkSlices{ "Link Slices" };
            ui::KickToggleButton linkOn{ "On" };
            ui::KickToggleButton linkOff{ "Off" };

            ui::KickLabel        looplock{ "Loop Lock" };
            ui::KickToggleButton loopOn{ "On" };
            ui::KickToggleButton loopOff{ "Off" };

            ui::KickLabel        zeroSnap{ "Zero Snap" };
            ui::KickToggleButton zeroOn{ "On" };
            ui::KickToggleButton zeroOff{ "Off" };

            SampleEditorData& seData;
        };
    }
}
