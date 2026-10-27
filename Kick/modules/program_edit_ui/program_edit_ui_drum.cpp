

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

//
// Internal include files...
//
#include "sources/drum/kick_DrumProgramEdit.h"

#include "sources/drum/kick_MasterDrumProgramEdit.h"
#include "sources/drum/kick_DrumProgramSampleTab.h"
#include "sources/drum/kick_DrumProgramLfoModulation.h"
#include "sources/drum/kick_DrumProgramLayerMix.h"
#include "sources/drum/kick_DrumProgramEnvelopes.h"
#include "sources/drum/kick_DrumInserts.h"

//
// Module source files...
// Drum Program...
//
#include "sources/drum/kick_DrumProgramEdit.cpp"

#include "sources/drum/kick_MasterDrumProgramEdit.cpp"
#include "sources/drum/kick_DrumProgramSampleTab.cpp"
#include "sources/drum/kick_DrumProgramLfoModulation.cpp"
#include "sources/drum/kick_DrumProgramLayerMix.cpp"
#include "sources/drum/kick_DrumProgramEnvelopes.cpp"
#include "sources/drum/kick_DrumInserts.cpp"
