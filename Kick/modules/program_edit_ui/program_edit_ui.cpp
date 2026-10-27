

#ifdef PROGRAM_EDIT_UI_INCLUDE
    /* When you add this cpp file to your project, you mustn't include it in a file where you've
   already included any other headers - just put it inside a file on its own, possibly with your config
   flags preceding it, but don't include anything else. That also includes avoiding any automatic prefix
   header files that the compiler may be using.
*/
    #error "Incorrect use of JUCE cpp file"
#endif

// Module include files...
#include "program_edit_ui.h"

/*
#include "sources/common/kick_HeaderBase.h"
#include "sources/common/kick_EditZones.h"
#include "sources/common/kick_SampleEditorPv.h"
#include "sources/common/kick_SampleOffset.h"
#include "sources/common/kick_EffectsEdit.h"
*/
#include "sources/common/kick_AHDSView.h"
#include "sources/common/kick_FilterEnvEdit.h"

#include "sources/drum/kick_DrumProgramEdit.h"
#include "sources/keygroup/kick_KeyGroupProgramEdit.h"
#include "sources/clip/kick_ClipProgramEdit.h"
#include "sources/midi/kick_MidiProgramEdit.h"
#include "sources/cvgate/kick_CvGateProgramEdit.h"
#include "sources/audio/kick_AudioProgramEdit.h"

// Module source files...
#include "sources/kick_ProgramEdit.cpp"

/*
#include "sources/common/kick_HeaderBase.cpp"
#include "sources/common/kick_EditZones.cpp"
#include "sources/common/kick_SampleEditorPv.cpp"
#include "sources/common/kick_SampleOffset.cpp"
#include "sources/common/kick_EffectsEdit.cpp"
*/
#include "sources/common/kick_AHDSView.cpp"
#include "sources/common/kick_FilterEnvEdit.cpp"
