/*
 * Copyright (c) 2021 MUSIC Tribe
 *
 * This file is part of Kick.
 *
 * Kick is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Affero General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * Kick is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with Kick. If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#pragma once

namespace mt
{
    static std::atomic<bool> _preferFlatNames{ false };

    namespace Note
    {
        // Note Colours..
        static const std::vector<int> whites  = { 0, 2, 4, 5, 7, 9, 11 };
        static const std::vector<int> blacks  = { 1, 3, 6, 8, 10 };
        static const std::vector<int> colours = { 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0 }; // 0 - White Note, 1 - Black Note.

        // Root Notes.
        static const std::vector<juce::String> sharpNames = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
        static const std::vector<juce::String> flatNames  = { "C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab", "A", "Bb", "B" };

        static const std::vector<juce::String>& rootNames() { return _preferFlatNames.load() ? flatNames : sharpNames; }

        // Note Lengths
        static const std::vector<juce::String> lengthNames = { "Bar", "1/2", "1/2T", "1/4", "1/4T", "1/8", "1/8T", "1/16", "1/16T", "1/32", "1/32T", "1/64", "1/64T" };

        // Utility functions...
        static int getRootNumber( const juce::String& Root )
        {
            auto index = Kick::findIndex( Root, sharpNames );
            if( 0 <= index ) return index;
            index = Kick::findIndex( Root, flatNames );
            if( 0 <= index ) return index;
            return -1;
        }

        static juce::String& getRootName( int Note )
        {
            auto names = _preferFlatNames ? Note::flatNames : Note::sharpNames;
            return names[Note % names.size()];
        }

        static double getLength( const juce::String& Length )
        {
            if( Length.equalsIgnoreCase( "Bar" ) ) { return 1.0; }
            if( Length.equalsIgnoreCase( "1/2" ) ) { return 0.5; }
            if( Length.equalsIgnoreCase( "1/2T" ) ) { return 0.75; }
            if( Length.equalsIgnoreCase( "1/4" ) ) { return 0.25; }
            if( Length.equalsIgnoreCase( "1/4T" ) ) { return 0.375; }
            if( Length.equalsIgnoreCase( "1/8" ) ) { return 0.125; }
            if( Length.equalsIgnoreCase( "1/8T" ) ) { return 0.1875; }
            if( Length.equalsIgnoreCase( "1/16" ) ) { return 0.0625; }
            if( Length.equalsIgnoreCase( "1/16T" ) ) { return 0.09375; }
            if( Length.equalsIgnoreCase( "1/32" ) ) { return 0.03125; }
            if( Length.equalsIgnoreCase( "1/32T" ) ) { return 0.046875; }
            if( Length.equalsIgnoreCase( "1/64" ) ) { return 0.015625; }
            if( Length.equalsIgnoreCase( "1/64T" ) ) { return 0.0234375; }
            return 0.0f;
        }
    }

    namespace ScaleAndNotes
    {
        constexpr int maxPianoKeys{ 127 };
        constexpr int maxNotes{ 128 };
        constexpr int maxMidiNote{ maxPianoKeys };

        // Octave.
        const static inline std::vector<juce::String> octave = {
            "-2", "-1", "0", "1", "2", "3", "4", "5", "6", "7", "8"
        };

        // Midi Notes.
        const std::vector<juce::String> midiNoteNames = {
            "C-2", "C#-2", "D-2", "D#-2", "E-2", "F-2", "F#-2", "G-2", "G#-2", "A-2", "A#-2", "B-2",
            "C-1", "C#-1", "D-1", "D#-1", "E-1", "F-1", "F#-1", "G-1", "G#-1", "A-1", "A#-1", "B-1",
            "C0", "C#0", "D0", "D#0", "E0", "F0", "F#0", "G0", "G#0", "A0", "A#0", "B0",
            "C1", "C#1", "D1", "D#1", "E1", "F1", "F#1", "G1", "G#1", "A1", "A#1", "B1",
            "C2", "C#2", "D2", "D#2", "E2", "F2", "F#2", "G2", "G#2", "A2", "A#2", "B2",
            "C3", "C#3", "D3", "D#3", "E3", "F3", "F#3", "G3", "G#3", "A3", "A#3", "B3",
            "C4", "C#4", "D4", "D#4", "E4", "F4", "F#4", "G4", "G#4", "A4", "A#4", "B4",
            "C5", "C#5", "D5", "D#5", "E5", "F5", "F#5", "G5", "G#5", "A5", "A#5", "B5",
            "C6", "C#6", "D6", "D#6", "E6", "F6", "F#6", "G6", "G#6", "A6", "A#6", "B6",
            "C7", "C#7", "D7", "D#7", "E7", "F7", "F#7", "G7", "G#7", "A7", "A#7", "B7",
            "C8", "C#8", "D8", "D#8", "E8", "F8", "F#8", "G8"
        };

        const std::vector<juce::String> midiNoteNamesFlat = {
            "C-2", "Db-2", "D-2", "Eb-2", "E-2", "F-2", "Gb-2", "G-2", "Ab-2", "A-2", "Bb-2", "B-2",
            "C-1", "Db-1", "D-1", "Eb-1", "E-1", "F-1", "Gb-1", "G-1", "Ab-1", "A-1", "Bb-1", "B-1",
            "C0", "Db0", "D0", "Eb0", "E0", "F0", "Gb0", "G0", "Ab0", "A0", "Bb0", "B0",
            "C1", "Db1", "D1", "Eb1", "E1", "F1", "Gb1", "G1", "Ab1", "A1", "Bb1", "B1",
            "C2", "Db2", "D2", "Eb2", "E2", "F2", "Gb2", "G2", "Ab2", "A2", "Bb2", "B2",
            "C3", "Db3", "D3", "Eb3", "E3", "F3", "Gb3", "G3", "Ab3", "A3", "Bb3", "B3",
            "C4", "Db4", "D4", "Eb4", "E4", "F4", "Gb4", "G4", "Ab4", "A4", "Bb4", "B4",
            "C5", "Db5", "D5", "Eb5", "E5", "F5", "Gb5", "G5", "Ab5", "A5", "Bb5", "B5",
            "C6", "Db6", "D6", "Eb6", "E6", "F6", "Gb6", "G6", "Ab6", "A6", "Bb6", "B6",
            "C7", "Db7", "D7", "Eb7", "E7", "F7", "Gb7", "G7", "Ab7", "A7", "Bb7", "B7",
            "C8", "Db8", "D8", "Eb8", "E8", "F8", "Gb8", "G8"
        };

        static const std::vector<juce::String>& noteNames() { return _preferFlatNames.load() ? midiNoteNamesFlat : midiNoteNames; }

        static const std::vector<juce::String> Chord = {
            "1-3-5", "1-4-5", "1-2-5", "1-3-5-7", "1-3-5-7b"
        };

        static const std::vector<juce::String> ChromaticChord = {
            "Major", "Minor", "Sus2", "Sus4", "Major7", "Minor7", "Major9", "Minor9", "Augumented", "Diminished"
        };

        // W,  H,  W,  h, W,  W,   h,  W,  h,  W,  h,  W
        // 1, b2,  2, b3, 3,  4,  b5,  5, #5,  6, b7,  7
        // 0,  1,  2,  3, 4,  5,   6,  7,  8,  9, 10,  11
        static const std::map<juce::String, const std::vector<int>> chordFormulaMap = {
            { "1-3-5", { 4, 7 } },
            { "1-4-5", { 5, 7 } },
            { "1-2-5", { 2, 7 } },
            { "1-3-5-7", { 5, 7, 11 } },
            { "1-3-5-7b", { 5, 7, 10 } },
        };

        static const std::vector<juce::String> PadPerform = {

        };
        // Scales and utilities..
        enum scale_type
        {
            scale_Major,
            scale_Natural_Minor,
            scale_Melodic_Minor,
            scale_Harmonic_Minor,
            scale_Pentatonic_Major,
            scale_Pentatonic_Minor,
            scale_Blues_Major,
            scale_Blues_Minor,
            scale_Augmented,
            scale_Diminished,

            scale_Aeolian,
            scale_Locrian,
            scale_Ionian,
            scale_Dorian,
            scale_Phrygian,
            scale_Lydian,
            scale_Mixolydian,

            scale_Oriental,
            scale_Byzantine,
            scale_Prometheus,

            scale_Algerian,
            scale_Arabic,
            scale_Balinese,
            scale_Chinese,
            scale_AncientChinese,
            scale_Egyptian,
            scale_Hawaiian,
            scale_Hungarian1,
            scale_Hungarian2,
            scale_Indian1,
            scale_Indian2,
            scale_Mongolian,
            scale_Neapolitan_Major,
            scale_Neapolitan_Minor,
            scale_Persian,
            scale_Romanian_Major,
            scale_Romanian_Minor,
            scale_Spanish_8_Tone,
            scale_Ukrainian_Dorian,
            scale_Japanese1,
            scale_Japanese2,
            scale_Japanese3,
            scale_Japanese4,

            scale_invalid
        };

        static const std::vector<juce::String> scaleNames = {
            "Major",
            "Natural Minor",
            "Melodic Minor",
            "Harmonic Minor",
            "Pentatonic Major",
            "Pentatonic Minor",
            "Blues Major",
            "Blues Minor",
            "Augmented",
            "Diminished",

            "Aeolian",
            "Locrian",
            "Ionian",
            "Dorian",
            "Phrygian",
            "Lydian",
            "Mixolydian",

            "Oriental",
            "Byzantine",
            "Prometheus",

            "Algerian",
            "Arabic",
            "Balinese",
            "Chinese",
            "AncientChinese",
            "Egyptian",
            "Hawaiian",
            "Hungarian1",
            "Hungarian2",
            "Indian1",
            "Indian2",
            "Mongolian",
            "Neapolitan Major",
            "Neapolitan Minor",
            "Persian",
            "Romanian Major",
            "Romanian Major",
            "Spanish 8 Tone",
            "Ukrainian Dorian",
            "Japanese1",
            "Japanese2",
            "Japanese3",
            "Japanese4",
        };

        static const std::unordered_map<scale_type, const std::vector<int>> scaleFormulaMap = {
            { scale_Major, { 2, 2, 1, 2, 2, 2, 1 } },
            { scale_Natural_Minor, { 2, 1, 2, 2, 1, 2, 2 } },
            { scale_Melodic_Minor, { 2, 1, 2, 2, 2, 2, 1 } },
            { scale_Harmonic_Minor, { 2, 1, 2, 2, 1, 3, 1 } },
            { scale_Pentatonic_Major, { 2, 2, 3, 2, 3 } },
            { scale_Pentatonic_Minor, { 3, 2, 2, 3, 2 } },
            { scale_Blues_Major, { 2, 1, 1, 3, 2, 3 } },
            { scale_Blues_Minor, { 3, 2, 1, 1, 3, 2 } },
            { scale_Augmented, { 3, 1, 3, 1, 3, 1 } },
            { scale_Diminished, { 2, 1, 2, 1, 2, 1, 2, 1 } },

            { scale_Aeolian, { 2, 1, 2, 2, 1, 2, 2 } },
            { scale_Locrian, { 1, 2, 2, 1, 2, 2, 2 } },
            { scale_Ionian, { 2, 2, 1, 2, 2, 2, 1 } },
            { scale_Dorian, { 2, 1, 2, 2, 2, 1, 2 } },
            { scale_Phrygian, { 1, 2, 2, 2, 1, 2, 2 } },
            { scale_Lydian, { 2, 2, 2, 1, 2, 2, 1 } },
            { scale_Mixolydian, { 2, 2, 1, 2, 2, 1, 2 } },

            { scale_Oriental, { 1, 3, 1, 1, 3, 1, 2 } },
            { scale_Byzantine, { 1, 3, 1, 2, 1, 3, 1 } },
            { scale_Prometheus, { 2, 2, 2, 3, 1, 2 } },

            { scale_Algerian, { 2, 1, 2, 1, 2, 1, 2, 1 } },
            { scale_Arabic, { 2, 2, 1, 1, 2, 2, 2 } },
            { scale_Balinese, { 1, 2, 4, 1, 4 } },
            { scale_Chinese, { 4, 2, 1, 4, 1 } },
            { scale_AncientChinese, { 2, 2, 2, 1, 2 } },
            { scale_Egyptian, { 2, 3, 2, 3, 2 } },
            { scale_Hawaiian, { 2, 1, 2, 2, 2, 2, 1 } },
            { scale_Hungarian1, { 2, 1, 3, 1, 1, 3, 1 } },
            { scale_Hungarian2, { 3, 1, 2, 1, 2, 1, 2 } },
            { scale_Indian1, { 1, 2, 2, 3, 2 } },
            { scale_Indian2, { 2, 2, 1, 2, 1, 2, 2 } },
            { scale_Mongolian, { 2, 2, 3, 2, 3 } },
            { scale_Neapolitan_Major, { 1, 2, 2, 2, 2, 2, 1 } },
            { scale_Neapolitan_Minor, { 1, 2, 2, 2, 1, 3, 1 } },
            { scale_Persian, { 1, 3, 1, 2, 1, 3 } },
            { scale_Romanian_Major, { 1, 3, 2, 1, 2, 1, 2 } },
            { scale_Romanian_Minor, { 2, 1, 3, 1, 2, 1, 2 } },
            { scale_Spanish_8_Tone, { 1, 2, 1, 1, 1, 2, 2, 2 } },
            { scale_Ukrainian_Dorian, { 2, 1, 3, 1, 2, 1, 2 } },
            { scale_Japanese1, { 1, 4, 2, 3, 2 } },
            { scale_Japanese2, { 2, 3, 2, 2, 3 } },
            { scale_Japanese3, { 1, 4, 1, 4, 2 } },
            { scale_Japanese4, { 2, 1, 4, 1, 4 } },
        };

        /*
        std::vector<int> Jazz_altered_dominant{ 1, 2, 1, 2, 2, 2, 2 };
        std::vector<int> Jazz_bebop_dominant{ 2, 2, 1, 2, 2, 1, 1, 1 };
        std::vector<int> Jazz_bebop_dorian_minor{ 2, 1, 1, 1, 2, 2, 1, 2 };
        std::vector<int> Jazz_bebop_harmonic_minor{ 2, 1, 2, 2, 1, 2, 1, 1 };
        std::vector<int> Jazz_bebop_major{ 2, 2, 1, 2, 1, 1, 2, 1 };
        std::vector<int> Jazz_bebop_melodic_minor{ 2, 1, 2, 2, 1, 1, 2, 1 };
        std::vector<int> Jazz_blues_major{ 2, 1, 1, 3, 2, 3 };
        std::vector<int> Jazz_blues_minor{ 3, 2, 1, 1, 3, 2 };
        std::vector<int> Jazz_half_whole{ 1, 2, 1, 2, 1, 2, 1, 2 };
        std::vector<int> Jazz_whole_half{ 2, 1, 2, 1, 2, 1, 2, 1 };
        */

        //// Utility functions...

        static int                 getNoteNumber( const juce::String& Note ) { return Kick::findIndex( Note, midiNoteNames ); }
        static const juce::String& getNoteName( int Note ) { return midiNoteNames.at( ( Note % midiNoteNames.size() ) ); }
        static int                 getOctave( int Note ) { return ( Note / 12 ); }
        static int                 getRootNameIndex( const juce::String& Note )
        {
            auto index = Kick::findIndex( Note, midiNoteNamesFlat );
            if( 0 <= index )
                return index % 12;
            index = Kick::findIndex( Note, midiNoteNames );
            if( 0 <= index ) return index % 12;
            return -1;
        }

        static const scale_type getScaleType( const juce::String& ScaleName )
        {
            int scaleNum = Kick::findIndex( ScaleName, scaleNames );
            if( juce ::isPositiveAndBelow<int>( scaleNum, scaleNames.size() ) )
                return (scale_type)scaleNum;
            return scale_invalid;
        }

        static std::vector<int> getNotesInScale( const int RootNote, const scale_type Scale, const int Start = 0, const int End = 127 )
        {
            auto scale = scaleFormulaMap.find( Scale );
            if( scale != scaleFormulaMap.end() )
            {
                int              scaleIterator = 0;
                std::vector<int> notes;
                auto             scaleFormula = scale->second;

                for( int i = RootNote; i <= End; )
                {
                    if( Start <= i && End >= i )
                        notes.push_back( i );

                    i += scaleFormula[(size_t)scaleIterator++];
                    if( scaleIterator >= (int)scaleFormula.size() )
                        scaleIterator = 0;
                }
                return notes;
            }
            return {};
        }

        static std::vector<int> getNotesInScale( const juce::String& Root, const juce::String& Scale, const juce::String& StartNote, const juce::String& EndNote )
        {
            auto rootNum   = Note::getRootNumber( Root );
            auto startNum  = getNoteNumber( StartNote );
            auto endNum    = getNoteNumber( EndNote );
            auto scaleType = getScaleType( Scale );
            return getNotesInScale( rootNum, scaleType, startNum, endNum );
        }
    }
}
