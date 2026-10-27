#pragma once
// The embedded General MIDI SoundFont that used to live here (E-mu Systems, 1993) cannot be
// redistributed and was removed. SFSynth starts without a SoundFont; load your own .sf2
// with SFSynth::setSoundFont(), or embed one you have the rights to as a byte array here.
uint32_t soundfontsize = 0;
const unsigned char soundfont[1] = { 0 };
