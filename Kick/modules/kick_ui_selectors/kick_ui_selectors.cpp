#ifdef KICK_UI_SELECTOR_H_INCLUDE
    /* When you add this cpp file to your project, you mustn't include it in a file where you've
   already included any other headers - just put it inside a file on its own, possibly with your config
   flags preceding it, but don't include anything else. That also includes avoiding any automatic prefix
   header files that the compiler may be using.
*/
    #error "Incorrect use of JUCE cpp file"
#endif
#include "kick_ui_selectors.h"

// Selectors...
#include "source/kick_headerBarComponents.cpp"
#include "source/kick_programSelector.cpp"
