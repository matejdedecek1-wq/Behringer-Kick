/*
 * Copyright (c) 2024 MUSIC Tribe
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
    namespace StepSequencerConstants
    {
        constexpr int stepMinWidth = 50; // Includes space between 2 steps
        constexpr int presetsWidth = 70;

        constexpr int controlBarHeight = 34;
        constexpr int stepButtonHeight = 40;

        constexpr int space     = 10;
        constexpr int stepSpace = 10;
    };

    struct StepSequencerData
    {
        int numSteps{ 0 };
        int numStepsPerPage{ 0 };
        int timediv{ 16 };
        int ticksPerQl{ 0 };
        int stepWidth{ 0 };

        std::vector<bool> selected{ false };
        std::vector<int>  indexValues{ 0 };
        std::vector<int>  indexProbValues{ 0 };
        std::vector<int>  indexRatchetValues{ 0 };
    };

}
