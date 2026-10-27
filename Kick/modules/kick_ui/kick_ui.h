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

/*******************************************************************************
  The block below describes the properties of this module, and is read by
  the Projucer to automatically generate project code that uses it.
  For details about the syntax and how to create or use a module, see the
  JUCE Module Format.txt file.

  BEGIN_JUCE_MODULE_DECLARATION

   ID:                 kick_ui
   vendor:             MusicTribe
   version:            0.1.0
   name:               MusicTribe ui
   description:        Classes aand interfaces for DAW
   website:            http://www.musictribe.com/
   license:            AGPLv3

   dependencies:        kick_ui_header, kick_ui_settings, kick_ui_channel_mixer, kick_leds

  END_JUCE_MODULE_DECLARATION

 *******************************************************************************/

#pragma once
#ifndef KICK_UI_H_INCLUDE
    #define KICK_UI_H_INCLUDE

    #include <kick_ui_header/kick_ui_header.h>
    #include <kick_ui_settings/kick_ui_settings.h>
    #include <kick_ui_channel_mixer/kick_ui_channel_mixer.h>
    #include <kick_leds/kick_leds.h>
    #include <project/project.h>

//common
    #include "dialogs/kick_AppMenuOptions.h"
    #include "dialogs/kick_AppAssignSample.h"
    #include "dialogs/kick_Inserts.h"

    // Abstract Interfaces
    #include "row_view/kick_IRowView.h"

    // Program_view
    #include "program_view/source/kick_AppProgram.h"
    #include "program_view/source/kick_AppProgramComponent.h"
    #include "program_view/source/kick_AppProgramOptions.h"

    // Sequence_view
    #include "sequence_view/source/kick_AppSequence.h"
    #include "sequence_view/source/kick_AppSequenceComponent.h"
    #include "sequence_view/source/kick_AppSequenceOptions.h"

    // Track view
    #include "track_view/source/kick_AppTrack.h"
    #include "track_view/source/kick_AppTrackComponent.h"
    #include "track_view/source/kick_AppTrackOptions.h"

    // XYFX view
    #include "audio_components/xyfx/kick_XyFxView.h"

    // sample editor
    #include "audio_components/sample_editor/kick_SampleEditorInterface.h"
    #include "audio_components/sample_editor/kick_SampleEditorData.h"
    #include "audio_components/sample_editor/kick_SampleLayerSettings.h"
    #include "audio_components/sample_editor/kick_SampleEditorSettings.h"
    #include "audio_components/sample_editor/kick_SampleEditorCue.h"
    #include "audio_components/sample_editor/kick_SEScaleComponent.h"
    #include "audio_components/sample_editor/kick_SEViewComponent.h"
    #include "audio_components/sample_editor/kick_SampleEditorOptions.h"
    #include "audio_components/sample_editor/kick_SampleEditor.h"
    #include "audio_components/sample_editor/kick_SampleEditorLayer.h"
    #include "audio_components/sample_editor/kick_SampleEditorProcess.h"
    #include "audio_components/sample_editor/kick_SampleEditorAssign.h"

    // sampler
    #include "audio_components/sampler/kick_SaveSampleDialog.h"
    #include "audio_components/sampler/kick_SamplerRecorder.h"
    #include "audio_components/sampler/kick_KeepDiscardSampleComponent.h"
    #include "audio_components/sampler/kick_Sampler.h"

    //looper
    #include "audio_components/looper/kick_LoopRecorder.h"
    #include "audio_components/looper/kick_Looper.h"

    //dialogs
    #include "dialogs/kick_AppStatusbar.h"
    #include "dialogs/kick_AppTopbar.h"

    // preferences
    #include "swupdate/kick_SWUpdateScreen.h"
    #include "swupdate/kick_SWUpdateView.h"

    // modes
    #include "modes/kick_ListEdit.h"
    #include "modes/kick_PadColor.h"
    #include "modes/kick_trackMute.h"
    #include "modes/kick_padMute.h"
    #include "modes/kick_nextSequence.h"
    #include "modes/kick_song.h"
    #include "modes/mixers/kick_MixerTabView.h"
    #include "modes/mixers/kick_MixerHandlerInterface.h"
    #include "modes/mixers/kick_PadMixer.h"
    #include "modes/knobedit/kick_KnobGrid.h"
    #include "modes/knobedit/kick_KnobTabs.h"
    #include "modes/knobedit/kick_KnobEdit.h"
    #include "modes/midicontrol/kick_MidiControlTabs.h"
    #include "modes/midicontrol/kick_MidiControl.h"
    #include "modes/padperform/kick_PadPerform.h"
    #include "modes/filebrowser/kick_fileUtils.h"
    #include "modes/filebrowser/kick_fileBrowser.h"

    #include "dialogs/kick_AppHome.h"
    //new file
    #include "recent_project/kick_RecentProject.h"
    #include "recent_project/kick_RecentProject_Utils.h"

    //Erase
    #include "modes/kick_Erase.h"
    #include "modes/kick_Copy.h"
    #include "modes/kick_Delete.h"

#endif
