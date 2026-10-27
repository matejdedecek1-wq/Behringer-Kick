/*******************************************************************************
 The block below describes the properties of this module, and is read by
 the Projucer to automatically generate project code that uses it.
 For details about the syntax and how to create or use a module, see the
 JUCE Module Format.txt file.

 BEGIN_JUCE_MODULE_DECLARATION

  ID:                 common_defs
  vendor:             MUSIC Tribe, MY
  version:            0.1.0
  name:               MusicTribe Common definitions
  description:        Classes and interfaces containing common definitions
  website:
  license:            AGPLv3

  dependencies:       kick_ui_controls, juce_audio_utils

 END_JUCE_MODULE_DECLARATION

*******************************************************************************/


#pragma once
#ifndef KICK_UI_COMPONENTS_INCLUDE
    #define KICK_UI_COMPONENTS_INCLUDE

    // Dependent module includes.
    #include <juce_audio_utils/juce_audio_utils.h>
    #include <kick_ui_controls/kick_ui_controls.h>

    // Module include files.
    #include "source/kick_common_controls.h"
    #include "source/kick_KeyboardComponent.h"
    #include "source/kick_tabbedComponent.h"
    #include "source/kick_padButtonGroup.h"
    #include "source/kick_padMiniMap.h"
    #include "source/kick_menuComponent.h"
    #include "source/kick_IOComponent.h"
    #include "source/kick_XYFXGrid.h"
    #include "source/kick_GridSettings.h"
    #include "source/kick_WarningComponent.h"

#endif
