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
#include <functional>

namespace kick_ui
{
    enum class Screen
    {
        Invalid     = -9999,
        HomeOptions = 1000,
        HomeListView,
        HomeGridView,
        HomeSequencerView,
        HomeXYView,
        HomePreferencesView,
        HomePreferences,
        //
        MenuSampleEdit,
        MenuProgramEdit,
        MenuSampler,
        MenuLooper,
        MenuPadMute,
        MenuTrackMute,
        MenuPadMixer,
        MenuChannelMixer,
        MenuKnobEdit,
        MenuListEdit,
        MenuPadPerform,
        MenuMidiControl,
        MenuFileBrowser,
        MenuNextSequence,
        MenuSong,
        MenuColorEdit,
        MenuAssignSample,
        RecentProject,
        EditProgram,
        //
        SWUpdateView,
        SWUpdateWindow,
        ProcessEdit,
        CVClib,
        //
        SystemResources,
        Inserts,
        Temp
    };

    struct signals
    {
        static sigslot::signal<Screen> sigScreenClose;
        static sigslot::signal<Screen> sigScreenOpen;
        static sigslot::signal<Screen> onScreenLoaded;
        static sigslot::signal<Screen> onScreenClosed;

        static sigslot::signal<Screen, void*> onScreenData;
    };

    // Base class for all UI screens...
    class IScreen
    {
    public:
        std::function<void()> onClose = []() {};
        std::function<void()> onOpen  = []() {};

        explicit IScreen( Screen Scr )
            : scr_( Scr ) {}
        virtual ~IScreen() = default;

        inline Screen getType() const { return scr_; }

        void closeScreen( const Screen NewSCR = Screen::Invalid )
        {
            onClose();
            signals::sigScreenClose( NewSCR );
        };

        virtual bool canClose() { return false; }
        virtual bool requiresControlBar() { return false; };
        virtual bool requiresStatusBar() { return true; };
        virtual bool requiresHandleCommand() { return false; }

        virtual void knobCommand( int, int ){};
        virtual void handleCommand( CPktMgr::HWMsg_t& ){};

    private:
        const Screen scr_;
    };

}
