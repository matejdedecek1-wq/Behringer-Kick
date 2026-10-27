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

namespace mt
{

    MTAudioSequenceManager::MTAudioSequenceManager( MTSelected& Selected, MTAudioManager& Manager )
        : selected( Selected ), manager( Manager )
    {
        // Reserve default storage size.
        sequences.reserve( 64 );

        // Connect callbacks...
        sequenceNameChanged.connect( [&]( MTSequence* ) { updateSequenceList(); } );
    }

    MTAudioSequenceManager::~MTAudioSequenceManager()
    {
        sequences.clear();
    }

    void MTAudioSequenceManager::close()
    {
        vTree       = juce::ValueTree{ IDs::SEQUENCES };
        undoManager = nullptr;

        pSelectedSequence = nullptr;
        sequences.clear();

        std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
        updateSequenceList();
        if( trackListChanged ) trackListChanged( -1 );
    }

    void MTAudioSequenceManager::setup( juce::ValueTree VT, juce::UndoManager* UM )
    {
        vTree       = VT;
        undoManager = UM;

        // Initialize Sequences....
        for( auto it : vTree )
        {
            if( it.hasType( IDs::SEQUENCE ) )
            {
                createSequence( it );
            }
        }
        std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );

        updateSequenceList();

        // Initialize Tracks....
        // Nothing to do here, as engine initializes this as part of sequences....
        if( trackListChanged ) trackListChanged( -1 );
    }

    ////////////////////////////
    //// Selection ...
    ////////////////////////////
    void MTAudioSequenceManager::setSelectedSequence( int num )
    {
        auto selection = AudIO::utils::limit<int>( -1, (int)sequences.size() - 1, num );
        auto prev      = selected.sequence;

        if( prev != selection )
        {
            auto isValid = juce::isPositiveAndBelow( selection, sequences.size() );

            pSelectedSequence = isValid ? sequences[selection].get() : nullptr;
            selected.sequence = isValid ? selection : -1;
            sseq              = pSelectedSequence ? pSelectedSequence->getName() : "--";

            if( pSelectedSequence )
                setSelectedTrack( pSelectedSequence->getSelectedTrack() );

            if( seqSelectionChanged )
                seqSelectionChanged();
        }
    }

    void MTAudioSequenceManager::setSelectedSequence( const juce::String& Sequence )
    {
        setSelectedSequence( getSequenceIndexByName( Sequence ) );
    }

    void MTAudioSequenceManager::setSelectedTrack( int num )
    {
        auto prev = selected.track;
        strack    = "--";

        if( auto s = getSequence( selected.sequence ) )
        {
            s->setSelectedTrack( num );
            selected.track = s->getSelectedTrack();
            if( auto trk = s->getTrack( selected.track ) )
                strack = trk->getName();
        }
        else
        {
            selected.track = num;
        }

        if( prev != selected.track )
        {
            if( trackSelectionChanged )
                trackSelectionChanged();
        }
    }

    void MTAudioSequenceManager::setSelectedTrack( const juce::String& Track )
    {
        if( auto s = getSequence( selected.sequence ) )
        {
            setSelectedTrack( s->getTrackIndex( Track ) );
        }
    }

    void MTAudioSequenceManager::setSelectedSequenceAndTrack( int Sequence, int Track )
    {
        setSelectedSequence( Sequence );
        setSelectedTrack( Track );
    }

    void MTAudioSequenceManager::setSelectedSequenceAndTrack( const juce::String& Sequence, const juce::String& Track )
    {
        setSelectedSequence( Sequence );
        setSelectedTrack( Track );
    }
    ////////////////////////////
    //// Timing ...
    ////////////////////////////

    int MTAudioSequenceManager::setSequenceLength( const int Sequence, const int Bars )
    {
        if( auto s = getSequence( Sequence ) )
        {
            s->setLength( Bars );
            return 0;
        }
        return -1;
    }

    int MTAudioSequenceManager::setSequenceLength( const int Sequence, const float Multiplier )
    {
        if( auto s = getSequence( Sequence ) )
        {
            s->setLength( Multiplier );
            return 0;
        }
        return -1;
    }

    ////////////////////////////
    //// Sequence
    ////////////////////////////

    int MTAudioSequenceManager::createSequence( const juce::String& Name )
    {
        if( getSequenceByName( Name ) || ( sequences.size() >= MAX_SEQUENCE ) )
            return -1;

        juce::ValueTree vt = createVTree( IDs::SEQUENCE, IDs::name, Name );
        vTree.appendChild( vt, undoManager );

        return createSequence( vt );
    }

    int MTAudioSequenceManager::createSequence( juce::ValueTree& VT )
    {
        static unsigned int _seqNo = 0;

        if( !VT.isValid() ) VT = juce::ValueTree( IDs::SEQUENCE );

        juce::String name = VT.getProperty( IDs::name );
        if( name.isEmpty() )
        {
            name = "Sequence" + juce::String( _seqNo + 1 );
            VT.setProperty( IDs::name, name, nullptr );
        }

        if( auto seqPtr = std::make_unique<MTSequence>( manager, VT, undoManager, globalPlayhead ) )
        {
            ++_seqNo;
            auto pSeq = seqPtr.get();
            sequences.push_back( std::move( seqPtr ) );
            // Register callbacks...
            pSeq->onTempoTimeSignatureChangeFn = [&]( float BPM, int Bars, int Num, int Den ) {
                if( seqTimeSignatureChanged )
                    seqTimeSignatureChanged( BPM, Bars, Num, Den );
            };

            if( 1 == sequences.size() )
                setSelectedSequence( 0 );

            // Update sequence lists...
            updateSequenceList();
            return _seqNo;
        }
        return -1;
    }

    int MTAudioSequenceManager::deleteSequence( int Index )
    {
        if( juce::isPositiveAndBelow( Index, sequences.size() ) )
        {
            auto& ptr = sequences[(size_t)Index];

            // Stop audio playback, if current sequence is playing...
            if( pSelectedSequence == ptr.get() )
            {
                manager.stop();
                pSelectedSequence = nullptr;
            }

            vTree.removeChild( ptr->getVT(), undoManager );
            sequences.erase( sequences.begin() + Index );

            setSelectedSequence( getSelectedSequence() );
            if( 0 == sequences.size() )
            {
                selected.sequence = -1;
                pSelectedSequence = nullptr;
            }

            // Update sequence list and call listeners...
            if( trackListChanged ) trackListChanged( selected.sequence );
            updateSequenceList();
            return 0;
        }
        return -1;
    }

    int MTAudioSequenceManager::deleteSequence( const juce::String& Name )
    {
        return deleteSequence( getSequenceIndexByName( Name ) );
    }
    int MTAudioSequenceManager::copySequence( int Index )
    {
        if( auto s = getSequence( Index ) )
        {
            auto vt = s->getState().createCopy();
            vt.removeProperty( IDs::id, nullptr );
            vt.setProperty( IDs::name, juce::String( s->getName() + "_copy" ), nullptr );
            vTree.appendChild( vt, undoManager );
            createSequence( vt );
        }
        return 0;
    }

    int MTAudioSequenceManager::clearSequence( int Index )
    {
        if( auto s = getSequence( Index ) )
            s->deleteAllTracks();
        return 0;
    }

    void MTAudioSequenceManager::forAllSequences( std::function<void( MTSequence* S )> func )
    {
        if( func )
        {
            for( const auto& it : sequences )
            {
                func( it.get() );
            }
        }
    }

    MTSequence* MTAudioSequenceManager::getSequence( int index )
    {
        if( juce::isPositiveAndBelow( index, sequences.size() ) )
            return (MTSequence*)sequences[(size_t)index].get();
        return nullptr;
    }

    MTSequence* MTAudioSequenceManager::getSequenceByName( const juce::String& Name )
    {
        for( auto& it : sequences )
            if( it->getName().equalsIgnoreCase( Name ) )
                return it.get();
        return nullptr;
    }

    MTSequence* MTAudioSequenceManager::getSequenceByID( const juce::String& ID )
    {
        for( auto& it : sequences )
            if( it->getID().equalsIgnoreCase( ID ) )
                return it.get();
        return nullptr;
    }

    int MTAudioSequenceManager::getSequenceIndex( const MTSequence* pSeq )
    {
        int index = 0;
        for( auto& it : sequences )
        {
            if( it.get() == pSeq )
                return index;
            ++index;
        }
        return -1;
    }

    int MTAudioSequenceManager::getSequenceIndexByName( const juce::String& Name )
    {
        int index = 0;
        for( auto& it : sequences )
        {
            if( Name.equalsIgnoreCase( it->getName() ) )
                return index;
            ++index;
        }
        return -1;
    }

    juce::ValueTree& MTAudioSequenceManager::getSequenceState( int Sequence )
    {
        if( juce::isPositiveAndBelow( Sequence, sequences.size() ) )
            return sequences[(size_t)Sequence]->getState();
        return (juce::ValueTree&)EMPTY::VTState;
    }

    juce::ValueTree& MTAudioSequenceManager::getSequenceState( const juce::String& Name )
    {
        return getSequenceState( getSequenceIndexByName( Name ) );
    }

    juce::ValueTree MTAudioSequenceManager::getSequenceTempoState( int Sequence, int TempoIndex )
    {
        if( juce::isPositiveAndBelow( Sequence, sequences.size() ) )
            return sequences[(size_t)Sequence]->getTempoState( TempoIndex );
        return EMPTY::VTState;
    }

    int MTAudioSequenceManager::getSequenceDataList( std::vector<MTISequenceData*>& List )
    {
        List.clear();
        forAllSequences( [&]( MTSequence* S ) {
            List.push_back( dynamic_cast<MTISequenceData*>( S ) );
        } );
        return 0;
    }

    void MTAudioSequenceManager::updateSequenceList() noexcept
    {
        sequenceList.clear();

        // Update sequence name list...
        forAllSequences( [&]( MTSequence* S ) {
            S->setIndex( (int)sequenceList.size() );
            sequenceList.push_back( S->getName() );
        } );

        // Trigger sequence list updated...
        if( seqListChanged ) seqListChanged();
    }

    ////////////////////////////
    //// Track
    ////////////////////////////

    int MTAudioSequenceManager::createTrack( int Sequence, const juce::String& Name )
    {
        if( auto seq = getSequence( Sequence ) )
        {
            auto trackNum = seq->createTrack( Name );
            if( -1 != trackNum )
            {
                if( 1 == seq->getNumTracks() )
                    setSelectedTrack( 0 );

                if( trackListChanged ) trackListChanged( Sequence );
                return trackNum;
            }
        }
        return -1;
    }

    int MTAudioSequenceManager::deleteTrack( int Sequence, int Index )
    {
        if( auto seq = getSequence( Sequence ) )
        {
            if( 0 == seq->deleteTrack( Index ) )
            {
                setSelectedTrack( getSelectedTrack() );
                if( trackListChanged ) trackListChanged( Sequence );
                return 0;
            }
        }
        return -1;
    }

    int MTAudioSequenceManager::deleteTrack( int Sequence, const juce::String& Name )
    {
        if( auto seq = getSequence( Sequence ) )
        {
            if( 0 == seq->deleteTrack( Name ) )
            {
                setSelectedTrack( getSelectedTrack() );
                if( trackListChanged ) trackListChanged( Sequence );
                return 0;
            }
        }
        return -1;
    }
    MTTrack* MTAudioSequenceManager::getTrack()
    {
        return this->getTrack( selected );
    }

    MTTrack* MTAudioSequenceManager::getTrack( const MTSelected& Selection )
    {
        return this->getTrack( Selection.sequence, Selection.track );
    }

    MTTrack* MTAudioSequenceManager::getTrack( int SequenceIndex, int TrackIndex )
    {
        if( auto s = getSequence( SequenceIndex ) )
            return (MTTrack*)s->getTrack( TrackIndex );
        return nullptr;
    }

    MTTrack* MTAudioSequenceManager::getTrack( int SequenceIndex, const juce::String& TrackName )
    {
        if( auto s = getSequence( SequenceIndex ) )
            return (MTTrack*)s->getTrack( TrackName );
        return nullptr;
    }

    juce::ValueTree& MTAudioSequenceManager::getTrackState( int Sequence, int Track )
    {
        if( auto t = getTrack( Sequence, Track ) )
            return t->getState();
        return (juce::ValueTree&)EMPTY::VTState;
    }

    juce::ValueTree& MTAudioSequenceManager::getTrackState( int Sequence, const juce::String& Name )
    {
        if( auto t = getTrack( Sequence, Name ) )
            return t->getState();
        return (juce::ValueTree&)EMPTY::VTState;
    }

    void MTAudioSequenceManager::forAllTracksInSequence( int Sequence, std::function<void( MTTrack* T )> func )
    {
        if( auto seq = getSequence( Sequence ) )
        {
            seq->forAllTracks( func );
        }
    }

    void MTAudioSequenceManager::processMidiMeters()
    {
        forAllTracksInSequence( getSelectedSequence(), []( MTTrack* T ) {
            T->processMidiMeter();
        } );
    }

    int MTAudioSequenceManager::copyTrack( int Sequence, const juce::String from, const juce::String to )
    {
        juce::Identifier excludeName( "name" ), excludeId( "id" ), pgm( "program" );

        auto fromState = getTrackState( Sequence, from );
        auto toState   = getTrackState( Sequence, to );
        auto fromNotes = fromState.getChildWithName( IDs::NOTELIST );
        auto toNotes   = toState.getChildWithName( IDs::NOTELIST );
        auto track     = getTrack( Sequence, to );

        for( int id = 0; id < fromState.getNumProperties(); ++id )
        {
            auto propertyName = fromState.getPropertyName( id );
            if( propertyName != excludeName && propertyName != excludeId )
                toState.setProperty( propertyName, fromState.getProperty( propertyName ), nullptr );
            juce::String pgmName = fromState.getProperty( pgm );
            auto         p       = getAudioInterface().getProgram( getAudioInterface().getProgramIndexByID( pgmName ) );
            track->setProgram( p );
        }
        toNotes.removeAllChildren( nullptr );
        for( const auto& child : fromNotes )
        {
            toNotes.appendChild( child.createCopy(), nullptr );
        }
        return 0;
    }

}
