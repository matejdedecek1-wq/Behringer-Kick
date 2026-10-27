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

  dependencies:       juce_core, juce_audio_formats, interface

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
#ifndef AUDIO_FILE_MANAGER_INCLUDE
    #define AUDIO_FILE_MANAGER_INCLUDE

    // Dependent module includes...
    #include <juce_core/juce_core.h>
    #include <juce_audio_formats/juce_audio_formats.h>
    #include <interface/interface.h>

    // Module include files...
    #include "source/interpolators.h"
    #include "source/audio_sound.h"
    #include "source/audio_sound_renderer.h"
    #include "source/audio_sound_player.h"
    #include "source/audiofile_manager.h"
    #include "source/mt_audio_asset_manager.h"

#endif // AUDIO_FILE_MANAGER_INCLUDE
