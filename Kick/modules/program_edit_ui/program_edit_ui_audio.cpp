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
#include "sources/audio/kick_AudioProgramEdit.h"
#include "sources/audio/kick_AudioProgramEdit.cpp"
#include "sources/audio/kick_AudioProgramEditGrid.cpp"
#include "sources/audio/kick_AudioSelectTool.cpp"
#include "sources/audio/kick_AudioEraseTool.cpp"
#include "sources/audio/kick_AudioDragTool.cpp"
#include "sources/audio/kick_AudioCutTool.cpp"
#include "sources/audio/kick_AudioPencilTool.cpp"
#include "sources/audio/kick_AudioMagnifyTool.cpp"
