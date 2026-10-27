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
    class MTAudioProgramManager
    {
    public:
        explicit MTAudioProgramManager( MTSelected& Selected, AudIO::DawEngine& Engine );
        virtual ~MTAudioProgramManager();

        juce::String          sprog{ "--" };
        std::function<void()> progSelectionChanged;
        std::function<void()> progListChanged;

        // Selection API's...
        inline int getSelectedProgram() const { return selected.program; }
        void       setSelectedProgram( int num );
        void       setSelectedProgram( const juce::String& Program );

        void close();
        void setup( juce::ValueTree VT, juce::UndoManager* UM );

        // Program APIs...
        MTIProgram* createProgram( MTProgramType Type, const juce::String& Name );
        int         deleteProgram( int Index );
        int         deleteProgram( const juce::String& Name );
        int         duplicateProgram( int program );

        int          getProgramIndex( MTIProgram* program );
        int          getProgramIndexByName( const juce::String& Name );
        int          getProgramIndexById( const juce::String& ID );
        MTIProgram*  getProgram( int index );
        MTIProgram*  getProgramByName( const juce::String& Name );
        MTIProgram*  getProgramById( const juce::String& ID );
        juce::String getProgramName( const int Index );

        void forAllPrograms( std::function<void( MTIProgram* P )> func );

        inline const std::vector<std::unique_ptr<MTIProgram>>& getPrograms() const noexcept { return programs; }
        inline const std::vector<juce::String>&                getProgramList() const noexcept { return programList; };

        int setLevelForSelectedProgram( int Index, int Level );
        int setMuteForSelectedProgram( int Index, bool Mute );
        int setPanForSelectedProgram( int Index, double Pan );
        int setSemiForSelectedProgram( int Index, int Semi );
        int setFineForSelectedProgram( int Index, int Fine );
        int setPitchForSelectedProgram( int Index, int Semi, int Fine );

        void processMidiMeters();

    private:
        void        updateProgramList() noexcept;
        MTIProgram* createProgram( juce::ValueTree& vt, bool addToTree );

    private:
        MTSelected&        selected;
        AudIO::DawEngine&  engine;
        juce::ValueTree    vTree{ IDs::PROGRAMS };
        juce::UndoManager* undoManager{ nullptr };

        std::vector<std::unique_ptr<MTIProgram>> programs;
        std::vector<juce::String>                programList;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( MTAudioProgramManager )
    };
} // namespace MT
