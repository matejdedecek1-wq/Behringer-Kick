

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
#include "sources/midi/kick_MidiProgramEdit.h"
#include "sources/cvgate/kick_CvGateProgramEdit.h"

// Module source files...

// MIDI Program...
#include "sources/midi/kick_MidiProgramEdit.cpp"

// CV Program...
#include "sources/cvgate/kick_CvGateProgramEdit.cpp"
