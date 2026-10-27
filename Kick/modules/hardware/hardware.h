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
  dependencies:       juce_core

 END_JUCE_MODULE_DECLARATION

*******************************************************************************/

#pragma once
#ifndef HARDWARE_H_INCLUDE
    #define HARDWARE_H_INCLUDE

    #include <juce_core/juce_core.h>
    #include <juce_gui_basics/juce_gui_basics.h>
    #include "source/kick_I2c.h"
    #include "source/kick_Resorces.h"
    #include "source/kick_Hwpref.h"
    #include "source/kick_FirmwareUpdate.h"
    #include "source/kick_Hwcommon.h"
    #include "source/kick_UsbFunctionFS.h"
	#include "source/kick_WifiFirmware.h"

#endif
