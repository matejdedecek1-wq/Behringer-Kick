# Registers AudIO's JUCE modules with the Kick build (Kick/3rdParty.cmake includes this file).
#
# The original file was not part of the 0700-AAM release (see MusicTribeOpenSource/0700-AAM
# issue #9). This version and the AudIO_constants / AudIO_juce modules it registers are
# reconstructions taken from the community macOS port (github.com/Eight4aWish/0700-AAM,
# branch macos-build) and reused for the Windows build. They are not Music Tribe's original
# code; where a value is a guess the header says so.
juce_add_modules(
    ${CMAKE_CURRENT_LIST_DIR}/AudIO
    ${CMAKE_CURRENT_LIST_DIR}/AudIO_constants
    ${CMAKE_CURRENT_LIST_DIR}/AudIO_juce
)
