

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

// Internal include files...
#include "sources/common/kick_HeaderBase.h"
#include "sources/common/kick_EditZones.h"
#include "sources/common/kick_AHDSView.h"
#include "sources/common/kick_SampleEditorPv.h"
#include "sources/common/kick_SampleOffset.h"

#include "sources/common/kick_FilterEnvEdit.h"
#include "sources/common/kick_EffectsEdit.h"

#include "sources/keygroup/kick_KeyGroupProgramEdit.h"

// Module source files...

// Keygroup Program...
#include "sources/keygroup/kick_LfoModulationKG.cpp"
#include "sources/keygroup/kick_PanVelocityKG.cpp"
#include "sources/keygroup/kick_KeyGroupMaster.cpp"
#include "sources/keygroup/kick_KeyGroupProgramEdit.cpp"
