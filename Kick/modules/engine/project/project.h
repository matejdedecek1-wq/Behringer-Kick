/*******************************************************************************
 The block below describes the properties of this module, and is read by
 the Projucer to automatically generate project code that uses it.
 For details about the syntax and how to create or use a module, see the
 JUCE Module Format.txt file.

 BEGIN_JUCE_MODULE_DECLARATION

  ID:                 projectHelpers
  vendor:             MUSIC Tribe, MY
  version:            0.1.0
  name:               MusicTribe Audio Engine
  description:        Utilities for managing separate functionalities of audio engine
  website:
  license:            AGPLv3

  dependencies:       kick_ui_common, interface

 END_JUCE_MODULE_DECLARATION

*******************************************************************************/

#pragma once
#ifndef PROJECT_HELPERS_H_INCLUDE
    #define PROJECT_HELPERS_H_INCLUDE

    #include <kick_ui_common/kick_ui_common.h>
    #include <interface/interface.h>

    #include "kick_xml_manager.h"
    #include "kick_settings.h"

static const char* const __projectFileExtension = ".kickPrj";
static const char* const __projectFileVersion   = "0.1";
static const char* const __projectVersion       = "1.0.0";

namespace mt
{
    class projectManager : public KickXML
    {
    public:
        projectManager();
        ~projectManager() = default;

        void scanDirectory( const juce::File Directory );
        void rename( const juce::String& NewName );

        const juce::String& getProjectDir();
        juce::ValueTree     loadOrCreate( const juce::File& XMLFIle );

    private:
        juce::OwnedArray<juce::File> scannedProjects;
    };

}

#endif
