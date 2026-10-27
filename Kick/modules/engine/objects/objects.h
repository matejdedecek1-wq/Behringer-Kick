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

  dependencies:       juce_audio_devices, juce_audio_utils, juce_audio_formats, juce_dsp, audio_file_manager, audio_program

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
#ifndef PROCESSING_OBJECTS_H_INCLUDE
    #define PROCESSING_OBJECTS_H_INCLUDE

// Dependent module includes...

    #include <juce_audio_devices/juce_audio_devices.h>
    #include <juce_audio_utils/juce_audio_utils.h>
    #include <juce_audio_formats/juce_audio_formats.h>
    #include <juce_dsp/juce_dsp.h>
    #include <audio_file_manager/audio_file_manager.h>
    #include <audio_program/audio_program.h>

    // Module include files...
    #include "sampler/samplerVoice.h"
    #include "sampler/ISampler.h"
    #include "sampler/sampler.h"
    #include "sampler/pitchShifter.h"
    #include "sampler/DrumSampler.h"
    #include "sampler/ClipSampler.h"
    #include "sampler/KeygroupSampler.h"

#endif // PROCESSING_OBJECTS_H_INCLUDE
