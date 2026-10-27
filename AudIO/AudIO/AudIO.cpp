/*
 * Copyright (c) 2022 MUSIC Tribe
 *
 * This file is part of AudIO.
 *
 * AudIO is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Affero General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * AudIO is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with AudIO. If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

// Module include files...
#include "AudIO.h"

// Module source files...
#include "Core/Design.cpp"
#include "Core/DesignImpl.cpp"
#include "Core/Chain.cpp"
#include "Core/Bus.cpp"
//#include "Core/Console.cpp"
#include "Core/MeterEngine.cpp"

#include "Dsp/lfo.cpp"

#if !defined( AUDIO_ONLY_CORE )

    #if defined( AUDIO_DAW_CORE ) && ( AUDIO_DAW_CORE > 0 )
        #include "Daw/DawBus.cpp"
        #include "Daw/DawRouting.cpp"
        #include "Daw/DawEngine.cpp"
        //#include "Daw/DawNote.cpp"
        #include "Daw/DawInserts.cpp"
        #include "Daw/DawTime.cpp"
        #include "Daw/DawTypes.cpp"
        #if defined( AUDIO_DAW_EXTENSIONS ) && ( AUDIO_DAW_EXTENSIONS > 0 )
            #include "Daw/DawSequence.cpp"
            #include "Daw/DawTrack.cpp"
        #endif
    #endif // defined( AUDIO_DAW_CORE )

//#include "Devices/StereoOut.cpp"

    #include "Objects/ADSR.cpp"
    #include "Objects/LevelMeter.cpp"
    #include "Objects/Mixer.cpp"
    #include "Objects/SineWave.cpp"
    #include "Objects/VolumeAndPan.cpp"

    #if defined( AUDIO_OBJECT_AUDIO_READER_WRITER ) && ( AUDIO_OBJECT_AUDIO_READER_WRITER > 0 )
        #include "Objects/AudioReaderWriter.cpp"
    #endif

    #if defined( AUDIO_OBJECT_AUDIO_FILE_PLAYBACK ) && ( AUDIO_OBJECT_AUDIO_FILE_PLAYBACK > 0 )
        #include "Objects/PlayBack.cpp"
    #endif

    #if defined( AUDIO_OBJECT_AUDIO_FILE_RECORDER ) && ( AUDIO_OBJECT_AUDIO_FILE_RECORDER > 0 )
        #include "Objects/Recorder.cpp"
    #endif

    #if defined( AUDIO_OBJECT_SF_SYNTH ) && ( AUDIO_OBJECT_SF_SYNTH > 0 )
        #include "Objects/SFSynth.cpp"
        #include "Objects/SoundFontSynthesis/SoundFonts.c"
    #endif

#endif // #if !defined( AUDIO_ONLY_CORE )
