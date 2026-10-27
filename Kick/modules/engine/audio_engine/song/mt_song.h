/*
 * Copyright (c) 2024 MUSIC Tribe
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
    ///
    /// SONG STEP
    ///

    struct MTSongStep
    {
        MTSongStep( juce::ValueTree& VT, juce::UndoManager* Undo );

        bool isValid();
        bool isInValid();

        void setRepeat( int Number );
        void setSequence( const juce::String& SequenceID );
        void setSequence( const int SequenceIndex );

        int          getRepeat();
        juce::String getSequence();

        MTSongStepInfo getInfo();

        juce::ValueTree    vTree;
        juce::UndoManager* undo{ nullptr };
    };

    ///
    /// SONG
    ///

    struct MTSong
    {
        MTSong( juce::ValueTree& VT, juce::UndoManager* Undo );

        bool isValid();
        bool isInValid();

        juce::String getID() const;
        juce::String getName() const;

        inline int             getNumSteps() const { return vTree.getNumChildren(); };
        inline juce::ValueTree getStep( int Index ) { return vTree.getChild( Index ); }

        void addStep( MTSongStepInfo& Step, int AtIndex = -1 );
        void editStep( MTSongStepInfo& Step, int AtIndex );
        void removeStep( int Index );
        void removeAllSteps();

        MTSongInfo getInfo() const;

        juce::ValueTree    vTree;
        juce::UndoManager* undo{ nullptr };
    };

    ///
    /// SONG MANAGER
    ///

    class MTSongManager
    {
    public:
        MTSongManager();

        void close();
        void init( juce::ValueTree VT, juce::UndoManager* Undo );

        inline juce::ValueTree getState() { return vTree; }
        inline juce::ValueTree getSong( int Index ) { return vTree.getChild( Index ); }

        juce::ValueTree getSong( const juce::String& SongName );

        std::vector<juce::String> getSongsList();

        void createSong( const juce::String& SongName );
        void deleteSong( const juce::String& SongName );
        void deleteSong( int Index );

        MTSongInfo getSongInfo( const juce::String& SongName );
        void       addSongStep( const juce::String& SongName, MTSongStepInfo& Step, int AtIndex = -1 );
        void       editSongStep( const juce::String& SongName, MTSongStepInfo& Step, int AtIndex = -1 );
        void       deleteSongStep( const juce::String& SongName, int AtIndex );
        void       deleteAllSongSteps( const juce::String& SongName );
        bool       isSongExists( const juce::String& SongName );

    private:
        juce::ValueTree    vTree;
        juce::UndoManager* undo{ nullptr };
    };
}
