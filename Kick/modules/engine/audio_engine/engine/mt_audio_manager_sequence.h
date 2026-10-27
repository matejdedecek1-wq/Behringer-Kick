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
    class MTSequence;
    class MTTrack;
    class MTAudioManager;

    //
    class MTAudioSequenceManager
    {
    public:
        explicit MTAudioSequenceManager( MTSelected& Selected, MTAudioManager& Manager );
        virtual ~MTAudioSequenceManager();

        juce::String sseq{ "--" };
        juce::String strack{ "--" };

        std::function<void()>                                        seqSelectionChanged;
        std::function<void()>                                        trackSelectionChanged;
        std::function<void()>                                        seqListChanged;
        std::function<void( int Sequence )>                          trackListChanged;
        std::function<void( float BPM, int Bars, int Num, int Den )> seqTimeSignatureChanged;

        void close();
        void setup( juce::ValueTree VT, juce::UndoManager* UM );

        // Global playhead...
        inline MTPlayhead& getPlayhead() { return globalPlayhead; }
        inline MTPlayhead& getTrackPlayhead() { return trackPlayHead; }

        // State
        inline juce::UndoManager& getUndoManager() noexcept { return *undoManager; }
        inline juce::ValueTree&   getValueTree() noexcept { return vTree; }

        inline int getSelectedSequence() const { return selected.sequence; }
        inline int getSelectedTrack() const { return selected.track; }

        void setSelectedSequence( int num );
        void setSelectedSequence( const juce::String& Sequence );
        void setSelectedTrack( int num );
        void setSelectedTrack( const juce::String& Track );
        void setSelectedSequenceAndTrack( int Sequence, int Track );
        void setSelectedSequenceAndTrack( const juce::String& Sequence, const juce::String& Track );

        // Sequence APIs...
        int createSequence( const juce::String& Name );
        int deleteSequence( int Index );
        int deleteSequence( const juce::String& Name );

        int  copySequence( int Index );
        int  clearSequence( int Index );
        void forAllSequences( std::function<void( MTSequence* S )> func );

        inline MTSequence* getSequence() { return pSelectedSequence; };
        MTSequence*        getSequence( int index );
        MTSequence*        getSequenceByName( const juce::String& Name );
        MTSequence*        getSequenceByID( const juce::String& ID );

        int getSequenceIndex( const MTSequence* pSeq );
        int getSequenceIndexByName( const juce::String& Name );

        juce::ValueTree& getSequenceState( int Sequence );
        juce::ValueTree& getSequenceState( const juce::String& Name );
        juce::ValueTree  getSequenceTempoState( int Sequence, int TempoIndex );

        int setSequenceLength( const int Sequence, const int Bars );
        int setSequenceLength( const int Sequence, const float Multiplier );

        inline int                              getNumSequences() { return (int)sequences.size(); };
        int                                     getSequenceDataList( std::vector<MTISequenceData*>& List );
        inline const std::vector<juce::String>& getSequenceList() const noexcept { return sequenceList; };

        ///
        /// Track API's...
        ///
        int createTrack( int Sequence, const juce::String& Name );
        int deleteTrack( int Sequence, int Index );
        int deleteTrack( int Sequence, const juce::String& Name );
        int copyTrack( int Sequence, const juce::String from, const juce::String to );

        MTTrack*         getTrack();
        MTTrack*         getTrack( const MTSelected& Selection );
        MTTrack*         getTrack( int SequenceIndex, int TrackIndex );
        MTTrack*         getTrack( int SequenceIndex, const juce::String& TrackName );
        juce::ValueTree& getTrackState( int Sequence, int Track );
        juce::ValueTree& getTrackState( int Sequence, const juce::String& Name );

        void forAllTracksInSequence( int Sequence, std::function<void( MTTrack* T )> func );

        void processMidiMeters();

    protected:
        MTSequence* pSelectedSequence{ nullptr };
        MTPlayhead  globalPlayhead;
        MTPlayhead  trackPlayHead;

    private:
        void updateSequenceList() noexcept;
        int  createSequence( juce::ValueTree& VT );

    private:
        MTSelected&     selected;
        MTAudioManager& manager;

        juce::ValueTree    vTree{ IDs::SEQUENCES };
        juce::UndoManager* undoManager{ nullptr };

        std::vector<std::unique_ptr<MTSequence>> sequences;
        std::vector<juce::String>                sequenceList;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( MTAudioSequenceManager )
    };
} // namespace MT
