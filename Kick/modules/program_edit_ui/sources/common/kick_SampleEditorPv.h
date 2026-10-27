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
    class SampleEditorPv : public ui::KickGrid
    {
    public:
        SampleEditorPv();

    private:
        ui::KickListBox layerOne{ "L1", 1 };
        ui::KickListBox layerTwo{ "L2", 1 };
        ui::KickListBox layerThree{ "L3", 1 };
        ui::KickListBox layerFour{ "L4", 1 };

        ui::KickRotary layerOnePan{ "Semi" };
        ui::KickRotary layerOneVelocityStart{ "Volume" };
        ui::KickRotary layerOneVelocityEnd{ "Volume" };

        ui::KickRotary layerTwoPan{ "Semi" };
        ui::KickRotary layerTwoVelocityStart{ "Volume" };
        ui::KickRotary layerTwoVelocityEnd{ "Volume" };

        ui::KickRotary layerThreePan{ "Semi" };
        ui::KickRotary layerThreeVelocityStart{ "Volume" };
        ui::KickRotary layerThreeVelocityEnd{ "Volume" };

        ui::KickRotary layerFourPan{ "Semi" };
        ui::KickRotary layerFourVelocityStart{ "Volume" };
        ui::KickRotary layerFourVelocityEnd{ "Volume" };
    };

}
