/*******************************************************************************
 The block below describes the properties of this module, and is read by
 the Projucer to automatically generate project code that uses it.
 For details about the syntax and how to create or use a module, see the
 JUCE Module Format.txt file.

 BEGIN_JUCE_MODULE_DECLARATION

  ID:                 kick_ui_controls
  vendor:             MUSIC Tribe, MY
  version:            0.1.0
  name:               MusicTribe Common definitions
  description:        Classes and interfaces containing common definitions
  website:
  license:            AGPLv3

  dependencies:       kick_ui_common, juce_gui_extra, juce_audio_utils, input_events

 END_JUCE_MODULE_DECLARATION

*******************************************************************************/

#pragma once
#ifndef KICK_UI_CONTROLS_INCLUDE
    #define KICK_UI_CONTROLS_INCLUDE

    // Dependent module includes.
    #include <kick_ui_common/kick_ui_common.h>
    #include <juce_gui_extra/juce_gui_extra.h>
    #include <juce_audio_utils/juce_audio_utils.h>
    #include "contracts/dcl/dcl_contracts.h"

    // Module include files.
    #include "source/kick_Theme.h"
    #include "source/kick_layout.h"

    #include "source/kick_utils_blur.h"
    #include "source/kick_utils_focusDebugger.h"
    #include "source/kick_utils_loadingAnimation.h"
    #include "source/kick_utils.h"

    #include "source/mt_properties.h"
    #include "source/kick_ControlBase.h"

    #include "source/kick_lookAndFeel.h"

    #include "source/kick_separator.h"
    #include "source/kick_viewport.h"
    #include "source/kick_slider.h"
    #include "source/kick_label.h"
    #include "source/kick_buttons.h"
    #include "source/kick_number.h"
    #include "source/kick_listbox.h"
    #include "source/kick_levelMeter.h"
    #include "source/kick_table.h"
    #include "source/kick_StackedComponent.h"
    #include "source/kick_GainSlider.h"
    #include "source/kick_MultiSelectTabBarButton.h"
    #include "source/kick_TimerControl.h"
    #include "source/kick_ToggleOptions.h"
    #include "source/kick_CyclicStateControl.h"
    #include "source/kick_OffsetControl.h"
    #include "source/kick_VirtualKeyboard.h"
    #include "source/kick_ProgressBar.h"
    #include "source/kick_AlertWindow.h"
    #include "source/kick_AnimatedText.h"
    #include "source/kick_image.h"
    #include "source/kick_serialNumber.h"
    #include "source/kick_Tempo.h"
    #include "source/kick_Grid.h"
    #include "source/kick_WaveRenderer.h"
    #include "source/kick_attachments.h"
    #include "source/kick_attachedControls.h"
    #include "source/kick_NoteSelector.h"
    #include "source/kick_IPv4Editor.h"

#endif
