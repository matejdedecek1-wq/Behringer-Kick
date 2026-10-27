/*******************************************************************************
 The block below describes the properties of this module, and is read by
 the Projucer to automatically generate project code that uses it.
 For details about the syntax and how to create or use a module, see the
 JUCE Module Format.txt file.

 BEGIN_JUCE_MODULE_DECLARATION

  ID:                 program_edit_ui
  vendor:             MUSIC Tribe, MY
  version:            0.1.0
  name:               MusicTribe Common definitions
  description:        Classes and interfaces containing common definitions
  website:
  license:            AGPLv3

  dependencies:       kick_ui_header, audio_program, audio_file_manager, kick_ui_selectors, hardware, waveform

 END_JUCE_MODULE_DECLARATION

*******************************************************************************/
#pragma once
#ifndef PROGRAM_EDIT_UI_INCLUDE
    #define PROGRAM_EDIT_UI_INCLUDE

    // Dependent module includes.
    #include <kick_ui_header/kick_ui_header.h>
    #include <kick_ui_selectors/kick_ui_selectors.h>
    #include <audio_program/audio_program.h>
    #include <audio_file_manager/audio_file_manager.h>
    #include <hardware/hardware.h>
    #include <waveform/waveform.h>

    #include "sources/kick_ProgramEditInterface.h"
    #include "sources/kick_ProgramEdit.h"

#endif
