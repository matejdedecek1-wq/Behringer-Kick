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
    class AppMainmode
        : public juce::Component
        , public kick_ui::IScreen
        , private juce::AsyncUpdater
    {
    public:
        AppMainmode();
        ~AppMainmode();

        bool requiresControlBar() override { return true; }
        bool requiresStatusBar() override { return true; }
        bool requiresHandleCommand() override { return true; }
        enum class TabType
        {
            Sequence,
            Track,
            Program
        };
        enum class UndoRedo
        {
            Undo,
            Redo,
        };

        void handleCommand( CPktMgr::HWMsg_t& ) override;
        void refreshTab( int Index );
        void refreshDeleteButton();
        void handleUndoRedo( TabType tabType, UndoRedo undoRedoOption );

    private:
        std::unique_ptr<AppProgram>  pProg;
        std::unique_ptr<AppTrack>    pTrack;
        std::unique_ptr<AppSequence> pSeq;
        AppTrackOptions              trackoptions;
        AppSequenceOptions           seqoptions;
        AppProgramOptions            prgOptions;

        mt::MTAudioInterface& ae{ mt::getAudioInterface() };
        const mt::MTSelected& sel{ mt::getAudioInterface().getSelected() };

        int                                 tabSelected{ 1 };
        std::array<ui::KickToggleButton, 3> tabButtons;

        struct toolbar : public juce::Component
        {
            ui::KickImageButton add{ "Add", "Add_normal", "Add_normal", "Add_active" };
            ui::KickImageButton del{ "Del", "Trash", "Trash_Active", "Trash", "Trash Disabled" };
            ui::KickButton      undo{ TRANS( "Undo" ) };
            ui::KickButton      redo{ TRANS( "Redo" ) };
            juce::UndoManager&  um{ mt::getAudioInterface().getUndoManager() };
            toolbar();
            void paint( juce::Graphics& g ) override;
            void resized() override;
        } tBar;

        void resized() override;
        void visibilityChanged() override;
        void handleAsyncUpdate() override;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( AppMainmode )
    };
}
