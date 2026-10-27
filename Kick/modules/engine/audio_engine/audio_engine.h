/*******************************************************************************
 The block below describes the properties of this module, and is read by
 the Projucer to automatically generate project code that uses it.
 For details about the syntax and how to create or use a module, see the
 JUCE Module Format.txt file.

 BEGIN_JUCE_MODULE_DECLARATION

  ID:                 common
  vendor:             MUSIC Tribe, MY
  version:            0.1.0
  name:               MusicTribe Common definitions
  description:        Classes and interfaces containing common definitions
  website:
  license:            AGPLv3

  dependencies:       interface, audio_interface, audio_program, project, audio_midi

 END_JUCE_MODULE_DECLARATION

*******************************************************************************/

/*
 * Copyright (c) 2022 MUSIC Tribe
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
#ifndef AUDIO_ENGINE_H_INCLUDE
    #define AUDIO_ENGINE_H_INCLUDE

// Dependent module includes...
    #include <interface/interface.h>
    #include <audio_interface/audio_interface.h>
    #include <audio_program/audio_program.h>
    #include <project/project.h>
    #include <audio_midi/audio_midi.h>
    #include <audio_engine/audio_engine_song.h>

    #if defined( USE_VALUE_TREE_DEBUGGER ) && ( USE_VALUE_TREE_DEBUGGER > 0 )
        #include <jcf_debug/jcf_debug.h>
    #endif

// Module include files...
    #include "engine/mt_audio_metronome.h"
    #include "engine/mt_audio_manager.h"
    #include "engine/mt_audio_note.h"
    #include "engine/mt_audio_sequence.h"
    #include "engine/mt_audio_track.h"

#endif
