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
    //////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////    AppWindow
    //////////////////////////////////////////////////////////////////////////////////////////
    class AppWindow : public juce::Component
        , private juce::Timer
    {
    public:
        AppWindow();
        virtual ~AppWindow();

    private:
        void onFileLoaded( const juce::FileChooser& fc );

        virtual void timerCallback();
        void         paint( juce::Graphics& g ) override;
        void         resized() override;
        void         visibilityChanged() override;

    private:
        std::atomic_bool      isInitialized{ false };
        mt::MTAudioInterface& ae{ mt::getAudioInterface() };
        const mt::MTSelected& sel{ mt::getAudioInterface().getSelected() };

        ui::KickButton       loadFile{ "OPEN" }, playFile{ "PLAY" }, stopFile{ "STOP" };
        ui::KickToggleButton loopFile{ "Loop", "LOOP ON", "LOOP OFF" };
        ui::KickLevelMeter   meter;

        juce::FileChooser fc;
    };

}
