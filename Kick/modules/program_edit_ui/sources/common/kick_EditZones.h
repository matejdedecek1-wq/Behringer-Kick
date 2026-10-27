
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
    class EditZones : public ui::KickControlBase<juce::Component>
    {
    public:
        EditZones();

        juce::Array<int>& getEditZones();
        juce::String      getSelectedZonesString();

        juce::Array<int>& getSelected() { return selected; };

    private:
        ui::KickToggleButton current{ "Current" }, all{ "All" }, multiple{ "Multiple" };
        ui::KickSeparator    separator;
        ui::KickPadMiniMap   miniMap;

        juce::Array<int> selected{ 0 };
        int              lastPadPressed{ 0 };

        void clearQuick();

        void sliderMoved( float ) override{};
        void knobRotated( double ) override{};
        void knobClicked() override{};

        void bankPressed( int BankIndex ) override;
        void padPressed( int PadNo, int State, int Velocity, int Aftertouch ) override;

        void resized() override;
        void visibilityChanged() override;
    };
}
