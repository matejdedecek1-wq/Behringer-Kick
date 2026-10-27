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
    class CvProgramEdit : public ProgEditInterface
        , private juce::Timer
    {
    public:
        CvProgramEdit();
        ~CvProgramEdit();

        void refresh() override;
        void setupHeader( kick_ui::KickModeHeader& Hdr ) override{};
        void handleCommand( CPktMgr::HWMsg_t& ) override;

    private:
        void timerCallback() override;
        void paint( juce::Graphics& g ) override;
        void resized() override;
        void visibilityChanged() override;

    private:
        ui::KickSlider     cv[mt::CV::MaxCVPorts];
        mt::CV&            cvInterface;
        std::vector<float> cvValues;
    };
}
