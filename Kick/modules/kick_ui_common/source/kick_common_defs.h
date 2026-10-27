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

#define GET_PARAM_OPTIONS 1
#define SET_PARAM_VALUE   2
#define GET_PARAM_VALUE   3

namespace mt::Kick
{
    struct ScopeTracker
    {};

    /// Constants.
    ///
    ///
    const static std::vector<juce::String> outMonoList = { "Out 1", "Out 2", "Out 3", "Out 4", "Out 5", "Out 6", "Out 7", "Out 8" };
    const static std::vector<juce::String> outputList  = { "Out 1,2", "Out 3,4", "Out 5,6", "Out 7,8" };
    const static std::vector<juce::String> filterTypes = { "Lowpass 1 Pole", "Lowpass 2 Pole", "Lowpass 4 Pole", "Lowpass 6 Pole", "Lowpass 8 Pole", "Highpass 1 Pole", "Highpass 2 Pole", "Highpass 4 Pole", "Highpass 6 Pole", "Highpass 8 Pole",
                                                           "Bandpass 2", "Bandpass 4", "Bandpass 6", "Bandpass 8", "Bandstop 2", "Bandstop 4", "Bandstop 6", "Bandstop 8", "Bandboost 2", "Bandboost 4", "Bandboost 6", "Bandboost 8", "Model 1", "Model 2", "Model 3", "Model 4", "Kick3000 LPF" };
    const static std::vector<juce::String> xyFxPresets = { "Beat Repeat LPF", "Beat Repeat BPF", "Beat Repeat HPF", "LPF Manual", "BPF Manual", "HPF Manual", "LPF", "BPF", "HPF", "Phaser Manual", "Phaser" };

    const static std::vector<std::vector<juce::String>> xyPresetsCoOrdinates = {
        { "Beat Division", "Cutoff" },
        { "Beat Division", "Cutoff" },
        { "Beat Division", "Cutoff" },
        { "Cutoff", "Resonance" },
        { "Cutoff", "Resonance" },
        { "Cutoff", "Resonance" },
        { "Beat Division", "Depth" },
        { "Beat Division", "Depth" },
        { "Beat Division", "Depth" },
        { "Cutoff", "Resonance" },
        { "Beat Division", "Resonance" }
    };

    const static std::vector<juce::String> transpose = { "OFF", "ON" };

    const static std::vector<juce::String> QuantizeStrings   = { "Off", "2 Bar", "1 Bar", "1/2", "1/4", "1/8", "1/16", "1/32", "1/64" };
    const static std::vector<float>        QuantizeMultipler = { 0.f, 2.f, 1.f, 0.5f, 0.25f, 0.125f, 0.0625f, 0.03125f, 0.015625f };

    const static juce::String              PadQuantizeDefault{ "4" };
    const static std::vector<juce::String> PadQuantizeStrings = { "1", "2", "2T", "4", "4T", "8", "8T", "16", "16T", "32", "32T" };
    const static std::vector<int>          PadQuantizeValues  = { 1, 2, 3, 4, 6, 8, 12, 16, 24, 32, 48 };

    const static juce::String              TimeDivisionDefault{ "16" };
    const static std::vector<juce::String> TimeDivisionStrings = { "1", "4", "4T", "8", "8T", "16", "16T", "32", "32T", "64", "64T" };
    const static std::vector<int>          TimeDivisionValues  = { 1, 4, 6, 8, 12, 16, 24, 32, 48, 64, 96 };

    const static std::vector<juce::String> CVPortNames   = { "NA", "CV-1", "CV-2", "CV-3", "CV-4", "CV-5", "CV-6", "CV-7", "CV-8" };
    const static std::vector<juce::String> CVNoteOptions = { "Last", "Highest", "Lowest" };

    // PAD Colour modes..
    // enum class PadColorMode { Off_Off, Fixed_Color, Off_Classic, Off_Velocity, Dim_Velocity, Bright_Velocity, };
    const static std::vector<juce::String> PadColorModes = {
        "Off - Off", "Off - Classic", "Fixed Colors", "Off - Velocity", "Dim - Velocity", "Bright - Velocity"
    };
    // Empty pad colour mode.
    const static std::vector<juce::String> EmptyPadModes = { "Off", "Dim", "Normal" };

    // Pad Perform.
    const static std::vector<juce::String> PadPerformType = {
        "Off", "Chromatic", "Notes", "Chords", "Progression", "Chromatic Chords"
    };
    const static std::vector<juce::String> PadPerformStartOn = { "Continuous", "Start on Root" };

    const static std::vector<juce::String> midiChannels = {
        "All", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16"
    };

    const static std::vector<juce::String> midiOutChannels = {
        "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16"
    };
    // Group Numbers.
    const static std::vector<juce::String> groupNumbers = {
        "NONE", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15"
    };

    // Pads.
    const static std::vector<juce::String> padBankNames = {
        "A", "B", "C", "D", "E", "F", "G", "H"
    };
    const static std::vector<juce::String> padNames = {
        "A1", "A2", "A3", "A4", "A5", "A6", "A7", "A8", "A9", "A10", "A11", "A12", "A13", "A14", "A15", "A16",
        "B1", "B2", "B3", "B4", "B5", "B6", "B7", "B8", "B9", "B10", "B11", "B12", "B13", "B14", "B15", "B16",
        "C1", "C2", "C3", "C4", "C5", "C6", "C7", "C8", "C9", "C10", "C11", "C12", "C13", "C14", "C15", "C16",
        "D1", "D2", "D3", "D4", "D5", "D6", "D7", "D8", "D9", "D10", "D11", "D12", "D13", "D14", "D15", "D16",
        "E1", "E2", "E3", "E4", "E5", "E6", "E7", "E8", "E9", "E10", "E11", "E12", "E13", "E14", "E15", "E16",
        "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12", "F13", "F14", "F15", "F16",
        "G1", "G2", "G3", "G4", "G5", "G6", "G7", "G8", "G9", "G10", "G11", "G12", "G13", "G14", "G15", "G16",
        "H1", "H2", "H3", "H4", "H5", "H6", "H7", "H8", "H9", "H10", "H11", "H12", "H13", "H14", "H15", "H16"
    };

    const static std::vector<juce::String> waveShapes = {
        "Sine", "Tri", "S&H", "Saw", "SawD", "Sqr", "Noise"
    };

    /// Type Definitions...

    const static std::vector<juce::String> eventType = { "Drum events", "Melodic events" };
    //const static inline std::vector<EventType> eventTypeEnum = { EventType::DrumEvents, EventType::MelodicEvents };

    const static std::vector<juce::String> replaceOptions = { "Replace all events", "Replace events in note range", "Add to existing range" };
    //const static inline std::vector<RandomEventType> replaceOptsEnum = { RandomEventType::ReplaceAll, RandomEventType::ReplaceInRange, RandomEventType::AppendExisting };

    /// Utility functions...
    static int findIndex( const juce::String& Element, const std::vector<juce::String>& ElementArray )
    {
        for( int i = 0; i < (int)ElementArray.size(); ++i )
        {
            if( Element.equalsIgnoreCase( ElementArray.at( i ) ) )
                return (int)i;
        }
        return -1;
    }

    static int getPadBankNumber( const juce::String& PadBank )
    {
        return findIndex( PadBank, padBankNames );
    }

    static const juce::String& getPadBankName( unsigned int Number )
    {
        return padBankNames.at( Number );
    }

    static int getPadNumber( const juce::String& Pad )
    {
        return findIndex( Pad, padNames );
    }

    static const juce::String& getPadName( unsigned int Number )
    {
        return padNames.at( Number );
    }

}
