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
    class PanVelocityEdit : public ui::KickGrid
    {
    public:
        PanVelocityEdit( std::vector<int>& Pads, mt::MTDrumProgram*& PDrum );
        ~PanVelocityEdit();

        void refresh();

    private:
        void visibilityChanged() override;

    protected:
        std::vector<int>&   pads;
        mt::MTDrumProgram*& pDrum;

        std::vector<juce::String> layer, assets;
        ui::KickListBox           layerName[4];
        ui::KickRotary            layerPan[4];
        ui::KickRotary            layerVeloStart[4];
        ui::KickRotary            layerVeloEnd[4];
    };

}
