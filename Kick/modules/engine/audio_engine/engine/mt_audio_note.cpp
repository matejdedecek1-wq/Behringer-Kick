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
    ///
    /// Struct MTNoteUtil.
    ///
    struct MTNoteUtil
    {
        static juce::ValueTree createVT( const MTNote& N, int& noteId )
        {
            auto vt = createVTree( IDs::NOTE,
                                   IDs::id, ++noteId,
                                   IDs::no, N.number.get(),
                                   IDs::velocity, N.velocity.get(),
                                   IDs::start, N.start.get(),
                                   IDs::length, N.length.get(),
                                   IDs::probability, N.probability.get(),
                                   IDs::ratchet, N.ratchet.get() );
            return vt;
        }

        static juce::ValueTree createVT( const juce::ValueTree& VT, int& noteId )
        {
            juce::ValueTree vTree = VT.createCopy();
            vTree.setProperty( IDs::id, ++noteId, nullptr );
            return vTree;
        }
    };

    ///
    /// Class MTNoteList.
    ///

    struct NoteSorter
    {
        int compareElements( const juce::ValueTree& A, const juce::ValueTree& B )
        {
            auto apos = static_cast<unsigned int>( (int)A.getProperty( IDs::start, 0 ) );
            auto bpos = static_cast<unsigned int>( (int)B.getProperty( IDs::start, 0 ) );
            return ( apos < bpos ) ? -1 : ( ( bpos < apos ) ? 1 : 0 );
        }
    };
    static NoteSorter _noteSorter;

    MTNoteList::MTNoteList( const juce::ValueTree& VT, juce::UndoManager* UM, MTSequence& Seq )
        : parent( VT )
        , um( UM )
        , seq( Seq )
    {
        vTree = parent.getOrCreateChildWithName( IDs::NOTELIST, um );

        // Find maximum unique id...
        uniqueNoteId = 0;
        for( auto it : vTree )
        {
            if( uniqueNoteId < (int)it[IDs::id] )
                uniqueNoteId = it[IDs::id];

            int start = it.getProperty( IDs::start );
            int id    = it.getProperty( IDs::id );
            if( !notes.contains( start ) )
            {
                notes.set( start, new juce::HashMap<int, MTNote*>() );
            }
            notes[start]->set( id, new MTNote( it ) );
        }

        deSelectAllNotes();
    }

    void MTNoteList::sort()
    {
        vTree.sort( _noteSorter, um, false );

        if( pMidiSeq )
        {
            pMidiSeq->updateMatchedPairs();
            pMidiSeq->sort();
        }
        triggerAsyncUpdate();
    }

    void MTNoteList::forAllNotes( std::function<void( juce::ValueTree& Note )> Func )
    {
        if( Func )
            for( auto it : vTree )
                if( it.hasType( IDs::NOTE ) )
                    Func( it );
    }

    void MTNoteList::forSelectedNotes( std::function<void( juce::ValueTree& Note )> Func )
    {
        if( Func )
            for( auto it : selected )
                Func( it );
    }

    void MTNoteList::forNotesInRange( const NotesInRange& Range, std::function<void( juce::ValueTree& Note )> Func )
    {
        // Invalid Note Range.
        if( !( 0 > Range.noteStart || 0 > Range.noteEnd || 128 < Range.noteStart || 128 < Range.noteEnd ) )
        {
            if( Func )
            {
                auto noteMin = juce::jmin<int>( Range.noteStart, Range.noteEnd );
                auto noteMax = juce::jmax<int>( Range.noteStart, Range.noteEnd );
                auto timeMin = juce::jmin<int>( Range.startTime, Range.endTime );
                auto timeMax = juce::jmax<int>( Range.startTime, Range.endTime );

                for( auto it : vTree )
                {
                    MTNote n( it );
                    bool   add = false;

                    // For Note within range...
                    if( noteMin <= n.number.get() && n.number.get() <= noteMax )
                    {
                        // Valid start time provided.
                        if( 0 <= timeMin )
                        {
                            auto end = n.start + n.length;
                            if( timeMin <= n.start || timeMin < end ) //Check Valid range of note
                            {
                                if( 0 <= timeMax ) // Valid End-Time provided.
                                { // Add notes whose start time is less than endtime
                                    if( n.start.get() < timeMax )
                                        add = true;
                                }
                                else
                                { // No valid end time; add all notes that are after start-time till the end.
                                    add = true;
                                }
                            }
                        }
                        else
                        { // No valid start time; add notes in note-range throughout timeline.
                            add = true;
                        }
                    }

                    if( add )
                        Func( it );
                }
            }
        }
    }

    void MTNoteList::forNotesOutsideRange( const NotesInRange& Range, std::function<void( juce::ValueTree& Note )> Func )
    {
        // Invalid Note Range.
        if( !( 0 > Range.noteStart || 0 > Range.noteEnd || 128 < Range.noteStart || 128 < Range.noteEnd ) )
        {
            if( Func )
            {
                auto noteMin = juce::jmin<int>( Range.noteStart, Range.noteEnd );
                auto noteMax = juce::jmax<int>( Range.noteStart, Range.noteEnd );
                auto timeMin = juce::jmin<int>( Range.startTime, Range.endTime );
                auto timeMax = juce::jmax<int>( Range.startTime, Range.endTime );

                for( auto it : vTree )
                {
                    MTNote n( it );
                    bool   add = false;

                    // For Note within range...
                    if( n.number.get() < noteMin || noteMax < n.number.get() )
                    {
                        // Valid start time provided.
                        if( 0 <= timeMin )
                        {
                            if( n.start < timeMin )
                            {
                                add = true;
                            }
                            else
                            {
                                if( 0 <= timeMax ) // Valid End-Time provided.
                                { // Add notes whose start time is less than endtime
                                    if( n.start > timeMax )
                                        add = true;
                                }
                            }
                        }
                        else
                        { // No valid start time; add notes throughout timeline.
                            add = true;
                        }
                    }

                    if( add )
                        Func( it );
                }
            }
        }
    }

    MTNote MTNoteList::getNoteData( int NoteId )
    {
        auto i = find( NoteId );
        return MTNote( i, nullptr );
    }

    MTNote MTNoteList::getNoteData( int Note, int Start, int Length )
    {
        juce::ValueTree n = find( Note, Start, Length );
        return MTNote( n );
    }

    std::pair<juce::ValueTree, bool> MTNoteList::getNoteVT( int Note, int Start, int Length )
    {
        std::pair<juce::ValueTree, bool> retVal;
        juce::ValueTree                  n = find( Note, Start, Length );
        if( n.isValid() )
        {
            retVal.first  = n;
            retVal.second = true;
        }
        else
        {
            retVal.first  = {};
            retVal.second = false;
        }
        return retVal;
    }

    int MTNoteList::getNoteID( int Note, int Start, int Length )
    {
        juce::ValueTree nVT = find( Note, Start, Length );
        return MTNote( nVT ).id;
    }

    int MTNoteList::getSelectedNoteID( int Note, int Start, int Length )
    {
        if( juce::isPositiveAndBelow( Note, 128 ) )
            for( auto it : selected )
            {
                MTNote n( it, um );

                // Find if note is available...
                if( Note == n.number.get() )
                {
                    if( 0 < Length )
                    {
                        if( ( n.start == Start ) && ( Length == n.length ) )
                            return n.id;
                    }
                    else
                    {
                        if( ( n.start <= Start ) && ( ( n.start + n.length ) > Start ) )
                            return n.id;
                    }
                }
            }
        return -1;
    }

    bool MTNoteList::isValidSelectedNote( int Note, int Start, int Length )
    {
        juce::ValueTree n = find( Note, Start, Length );
        return selected.contains( n );
    }

    bool MTNoteList::isValidNote( int Note, int Start, int Length )
    {
        juce::ValueTree n = find( Note, Start, Length );
        return MTNote( n ).isValid();
    }

    bool MTNoteList::isValidNote( int Note, int Start )
    {
        juce::ValueTree n = find( Note, Start );
        return MTNote( n ).isValid();
    }

    bool MTNoteList::isValidNoteInRange( const NotesInRange& Range )
    {
        if( vTree.isValid() )
        {
            auto timeStart = Range.startTime;
            auto timeEnd   = Range.endTime;
            for( auto it : vTree )
            {
                MTNote note( it );

                auto noteStartTick = note.start.get();
                if( noteStartTick >= timeStart && noteStartTick < timeEnd )
                    return true;
            }
        }
        return false;
    }

    juce::ValueTree MTNoteList::find( const int NoteID )
    {
        return vTree.getChildWithProperty( IDs::id, NoteID );
    }

    juce::ValueTree MTNoteList::find( juce::ValueTree& Note )
    {
        MTNote note( Note );
        for( auto it : vTree )
        {
            MTNote n( it );

            if( note.start < n.start )
                break;

            if( ( n.number.get() == note.number.get() ) && ( n.start.get() == note.start.get() ) )
                return it;
        }
        return juce::ValueTree{ IDs::NOTE };
    }

    juce::ValueTree MTNoteList::find( int Note, int Start, int Length )
    {
        if( juce::isPositiveAndBelow( Note, 128 ) )
        {
            // Find if note is available...
            for( auto it : vTree )
            {
                MTNote n( it );

                if( Start < n.start )
                    break;

                if( Note == n.number.get() )
                {
                    if( 0 < Length )
                    {
                        if( ( n.start == Start ) && ( Length == n.length ) )
                            return it;
                    }
                    else
                    {
                        if( ( n.start <= Start ) && ( ( n.start + n.length ) > Start ) )
                            return it;
                    }
                }
            }
        }
        return juce::ValueTree{ IDs::NOTE };
    }

    juce::ValueTree MTNoteList::find( int Note, int Start )
    {
        if( juce::isPositiveAndBelow( Note, 128 ) )
        {
            // Find if note is available...
            for( auto it : vTree )
            {
                MTNote n( it );
                if( Start < 0 )
                    break;
                if( Note == n.number.get() )
                {
                    auto noteStart = n.start.get();
                    auto noteEnd   = noteStart + n.length.get();
                    if( Start >= noteStart && Start < noteEnd )
                        return it;
                }
            }
        }
        return juce::ValueTree{ IDs::NOTE };
    }

    int MTNoteList::getNotesInRange( std::vector<MTNote>& Notes, const NotesInRange& Range )
    {
        Notes.clear();
        forNotesInRange( Range, [&]( juce::ValueTree& Note ) {
            Notes.emplace_back( MTNote( Note ) );
        } );
        return 0;
    }

    int MTNoteList::setNoteVelocityInRange( const NotesInRange& Range, int Velocity )
    {
        bool changed = false;
        forNotesInRange( Range, [&, Velocity]( juce::ValueTree& Note ) {
            MTNote n( Note, um );
            n.velocity = Velocity;
            changed    = true;
        } );
        if( changed ) triggerAsyncUpdate();
        return 0;
    }

    int MTNoteList::setNoteProbabilityInRange( const NotesInRange& Range, int Probability )
    {
        bool changed = false;
        forNotesInRange( Range, [&, Probability]( juce::ValueTree& Note ) {
            MTNote n( Note, um );
            n.probability = Probability;
            changed       = true;
        } );
        if( changed ) triggerAsyncUpdate();
        return 0;
    }

    int MTNoteList::setNoteRatchetInRange( const NotesInRange& Range, int Ratchet )
    {
        bool changed = false;
        forNotesInRange( Range, [&, Ratchet]( juce::ValueTree& Note ) {
            MTNote n( Note, um );
            n.ratchet = Ratchet;
            changed   = true;
        } );
        if( changed ) triggerAsyncUpdate();
        return 0;
    }

    int MTNoteList::addNote( juce::ValueTree NoteVT, bool SortNoteList )
    {
        if( NoteVT.isValid() )
        {
            vTree.addChild( NoteVT, -1, um );
            MTNote* n     = new MTNote( NoteVT );
            int     start = n->start;
            int     id    = n->id;
            //noteCount++;
            /*juce::Logger::writeToLog( "Note Added, " + juce::String( n->number.get() ) +
                ", Note Count, " + juce::String( vTree.getNumChildren() ) + ", Count, " + juce::String( noteCount) );*/

            if( !notes.contains( start ) )
            {
                notes.set( start, new juce::HashMap<int, MTNote*>() );
            }
            notes[start]->set( id, n );
            // MIDi file...
            if( pMidiSeq )
            {
                juce::MidiMessage msgOn  = juce::MidiMessage::noteOn( 1, n->number.get(), (juce::uint8)n->velocity.get() );
                juce::MidiMessage msgOff = juce::MidiMessage::noteOff( 1, n->number.get(), (juce::uint8)n->velocity.get() );
                pMidiSeq->addEvent( msgOn, n->start.get() );
                pMidiSeq->addEvent( msgOff, n->start.get() + n->length.get() );
                pMidiSeq->updateMatchedPairs();
            }

            if( SortNoteList )
                sort();

            return 0;
        }
        return -1;
    }

    int MTNoteList::addNote( const MTNote& Note, bool SortNoteList )
    {
        auto note = MTNoteUtil::createVT( Note, uniqueNoteId );
        return addNote( note, SortNoteList );
    }

    int MTNoteList::addNote( int Note, int Velocity, int Probabality, int Ratchet, int Start, int Length, bool SortNoteList )
    {
        MTNote note;
        note.number      = Note;
        note.velocity    = Velocity;
        note.start       = Start;
        note.length      = Length;
        note.probability = Probabality;
        note.ratchet     = Ratchet;
        return addNote( note, SortNoteList );
    }

    int MTNoteList::modifyNote( const int NoteID, int Velocity, int Start, int Length, bool SortNoteList )
    { // Search if we already have note with this ID..
        auto it   = find( NoteID );
        auto note = MTNote( it, um );
        if( note.isValid() )
        {
            note.velocity = Velocity;
            note.length   = Length;
            if( note.start != Start )
            {
                updateNote( note, Start );
                if( SortNoteList )
                    sort();
            }
            return 0;
        }
        return -1;
    }

    int MTNoteList::addOrModifyNote( int Note, int Start, int Length, int Velocity, int Probability, int Ratchet )
    {
        int retVal = -1;
        // Invalid Note Range.
        if( juce::isPositiveAndBelow( Note, 128 ) )
        {
            auto it   = find( Note, Start, Length );
            auto note = MTNote( it, um );
            if( note.isValid() )
            {
                note.length      = Length;
                note.velocity    = Velocity;
                note.probability = Probability;
                note.ratchet     = Ratchet;
                if( note.start != Start )
                {
                    updateNote( note, Start );
                    sort();
                }
                retVal = 0;
            }
            else
                retVal = addNote( Note, Velocity, Probability, Ratchet, Start, Length, true );
        }
        return retVal;
    }

    int MTNoteList::eraseNote( juce::ValueTree NoteVT )
    {
        if( NoteVT.getNumProperties() )
        {
            int     start  = NoteVT.getProperty( IDs::start );
            int     id     = NoteVT.getProperty( IDs::id );
            MTNote* p      = ( *notes[start] )[id];
            int     number = p->number.get();
            notes[start]->remove( id );
            delete p;
            if( notes[start]->size() == 0 )
            {
                juce::HashMap<int, MTNote*>* map = notes[start];
                notes.remove( start );
                delete map;
            }
            selected.removeAllInstancesOf( NoteVT );
            vTree.removeChild( NoteVT, um );
            triggerAsyncUpdate();
        }
        return 1;
    }

    int MTNoteList::eraseNote( int NoteID )
    {
        return eraseNote( find( NoteID ) );
    }

    int MTNoteList::eraseNote( int Note, int Start, int Length )
    {
        return eraseNote( find( Note, Start, Length ) );
    }

    bool MTNoteList::isInvalidNoteExists()
    {
        bool retVal = false;
        forSelectedNotes( [&]( juce::ValueTree& Note ) {
            if( ( 0 > (int)Note[IDs::no] ) || ( 0 > (int)Note[IDs::start] ) )
                retVal = true;
            ;
        } );

        return retVal;
    }

    int MTNoteList::eraseInvalidNotes()
    {
        juce::ScopedLock sl{ lock };
        for( auto it : vTree )
        {
            MTNote n( it, nullptr );
            if( !n.isValid() )
            {
                temp.addIfNotAlreadyThere( it );
            }
        }
        clearNotesToDelete();
        return 0;
    }

    int MTNoteList::selectNote( juce::ValueTree Note, bool Select )
    {
        if( Note.isValid() && Note.hasType( IDs::NOTE ) )
        {
            if( Select )
                selected.addIfNotAlreadyThere( Note );
            else
                selected.removeAllInstancesOf( Note );
            return 0;
        }
        return -1;
    }

    int MTNoteList::selectAllNotes( bool Select )
    {
        if( !Select )
            selected.clearQuick();
        else
            for( auto it : vTree )
                selected.addIfNotAlreadyThere( it );
        return 0;
    }

    int MTNoteList::selectAllInNote( const int NoteNumber, bool Select )
    {
        selected.clearQuick();
        if( Select )
        {
            for( auto it : vTree )
            {
                auto noteNum = (int)it.getProperty( IDs::no );
                if( noteNum == NoteNumber )
                    selected.addIfNotAlreadyThere( it );
            }
            return 0;
        }
        return -1;
    }

    int MTNoteList::selectNote( const int NoteId, bool Select )
    {
        return selectNote( find( NoteId ), Select );
    }

    int MTNoteList::selectNote( const std::vector<int>& NoteIds, bool Select )
    {
        int retVal = 0;
        for( auto& it : NoteIds )
            retVal += selectNote( it, Select );
        return retVal;
    }

    int MTNoteList::selectNote( const int Note, const int Start, const int Length, bool Select )
    {
        return selectNote( find( Note, Start, Length ), Select );
    }

    int MTNoteList::selectNote( const int Note, const int Start, bool Select )
    {
        return selectNote( find( Note, Start ), Select );
    }

    int MTNoteList::selectNotesInRange( const NotesInRange& Range, bool Select )
    {
        forNotesInRange( Range, [&, Select]( juce::ValueTree& Note ) {
            selectNote( Note, Select );
        } );
        return 0;
    }

    bool MTNoteList::isSelectedNoteInRange( const NotesInRange& Range )
    {
        bool ret{ false };
        forNotesInRange( Range, [&]( juce::ValueTree& Note ) {
            if( selected.contains( Note ) )
                ret = true;
        } );
        return ret;
    }

    int MTNoteList::deleteAllNotes()
    {
        juce::ScopedLock sl( lock );
        selected.clearQuick();
        vTree.removeAllChildren( um );
        notes.clear();
        triggerAsyncUpdate();
        return 0;
    }

    int MTNoteList::deleteSelectedNotes()
    {
        juce::ScopedLock sl( lock );
        for( auto it : selected )
        {
            int     start  = it.getProperty( IDs::start );
            int     id     = it.getProperty( IDs::id );
            MTNote* p      = ( *notes[start] )[id];
            int     number = p->number.get();
            notes[start]->remove( id );
            delete p;
            if( notes[start]->size() == 0 )
            {
                juce::HashMap<int, MTNote*>* map = notes[start];
                notes.remove( start );
                delete map;
            }
            vTree.removeChild( it, um );
        }
        selected.clearQuick();
        triggerAsyncUpdate();
        return 0;
    }

    void MTNoteList::clearNotesToDelete()
    {
        juce::ScopedLock sl{ lock };
        for( auto it : temp )
            eraseNote( it );
        temp.clearQuick();
        triggerAsyncUpdate();
    }

    int MTNoteList::deleteNotesInRange( const NotesInRange& Range )
    {
        juce::ScopedLock sl{ lock };
        forNotesInRange( Range, [&]( juce::ValueTree& Note ) {
            temp.addIfNotAlreadyThere( Note );
        } );
        clearNotesToDelete();
        return 0;
    }

    int MTNoteList::deleteNotesOutsideRange( const NotesInRange& Range )
    {
        juce::ScopedLock sl{ lock };
        forNotesOutsideRange( Range, [&]( juce::ValueTree& Note ) {
            temp.addIfNotAlreadyThere( Note );
        } );
        clearNotesToDelete();
        return 0;
    }

    int MTNoteList::deleteNotesInNoteRange( int Start, int End )
    {
        NotesInRange range( Start, End );
        return deleteNotesInRange( range );
    }

    int MTNoteList::deleteNotesOutsideNoteRange( int Start, int End )
    {
        NotesInRange range( Start, End );
        return deleteNotesOutsideRange( range );
    }

    int MTNoteList::deleteNoteInTimeRange( int StartTime, int EndTime )
    {
        NotesInRange range( MIDI::MIN, MIDI::MAX, StartTime, EndTime );
        return deleteNotesInRange( range );
    }

    int MTNoteList::deleteNotesOutsideTimeRange( int StartTime, int EndTime )
    {
        NotesInRange range( MIDI::MIN, MIDI::MAX, StartTime, EndTime );
        return deleteNotesOutsideRange( range );
    }

    int MTNoteList::editNote( juce::ValueTree& VT, double DeltaTimeInTicks, int DeltaNoteNumber, double DeltaLength, bool SortNoteList )
    {
        if( VT.isValid() )
        {
            MTNote note( VT, um );
            bool   changed = false;

            if( 0 != (int)DeltaNoteNumber )
            {
                note.number = note.number.get() + DeltaNoteNumber;
                changed     = true;
            }
            if( 0 != (int)DeltaLength )
            {
                note.length = (int)( note.length + DeltaLength );
                if( 0 >= note.length.get() ) note.length = 1;
                changed = true;
            }
            if( 0 != (int)DeltaTimeInTicks )
            {
                int start = (int)( note.start + DeltaTimeInTicks );
                if( start < 0 )
                    return eraseNote( VT );
                else
                {
                    updateNote( note, start );
                    changed = true;

                    if( SortNoteList )
                        sort();
                }
            }
            if( changed ) triggerAsyncUpdate();
            return 0;
        }
        return -1;
    }

    int MTNoteList::nudgeNote( juce::ValueTree& VT, double DeltaTimeInTicks, bool SortNoteList )
    {
        return editNote( VT, DeltaTimeInTicks, 0, 0, SortNoteList );
    }

    int MTNoteList::transposeNote( juce::ValueTree& VT, int DeltaNoteNumber, bool SortNoteList )
    {
        return editNote( VT, 0, DeltaNoteNumber, 0, SortNoteList );
    }

    int MTNoteList::editNoteStart( juce::ValueTree& VT, int DeltaNoteStart )
    {
        if( ( DeltaNoteStart != 0 ) && VT.isValid() )
        {
            MTNote note( VT, um );

            auto noteStart = note.start.get();
            auto noteEnd   = noteStart + note.length;

            auto newStart = noteStart + DeltaNoteStart;
            if( 0 > newStart ) newStart = 0;
            if( newStart >= noteEnd ) return 0;
            updateNote( note, newStart, noteEnd - newStart );
            triggerAsyncUpdate();
            return 0;
        }
        return -1;
    }

    int MTNoteList::editNoteEnd( juce::ValueTree& VT, int DeltaNoteEnd )
    {
        if( DeltaNoteEnd != 0 && VT.isValid() )
        {
            MTNote note( VT, um );

            auto noteStart = note.start.get();
            auto noteEnd   = noteStart + note.length;

            auto newEnd = noteEnd + DeltaNoteEnd;
            if( newEnd > noteStart )
            {
                note.length = ( newEnd - noteStart );
            }
            return 0;
        }
        return -1;
    }

    int MTNoteList::editMuteSelectedNotes( const bool MuteStatus )
    {
        int retVal = 0;
        for( auto it : selected )
        {
            MTNote n( it, um );
            n.mute = MuteStatus;
        }
        triggerAsyncUpdate();
        return retVal;
    }

    int MTNoteList::editSelectedNotes( double DeltaTimeInTicks, int DeltaNoteNumber, double DeltaLength )
    {
        int retVal = 0;
        for( auto it : selected )
        {
            retVal = editNote( it, DeltaTimeInTicks, DeltaNoteNumber, DeltaLength, false );
        }

        // Sort note list.
        if( AudIO::utils::isNotEqual( DeltaTimeInTicks, 0.0 ) )
            sort();
        else
            triggerAsyncUpdate();
        return retVal;
    }

    int MTNoteList::pasteNote( juce::ValueTree& VT, double DeltaTimeInTicks, int DeltaNoteNumber, double DeltaLength, bool SortNoteList )
    {
        MTNote n( VT, um );
        if( n.isValid() )
        {
            auto   pnVt = MTNoteUtil::createVT( VT, uniqueNoteId );
            MTNote pn( pnVt, nullptr );

            pn.start  = (int)( pn.start.get() + DeltaTimeInTicks );
            pn.number = (int)( pn.number.get() + DeltaNoteNumber );
            pn.length = (int)( pn.length.get() + DeltaLength );

            return addNote( pnVt, SortNoteList );
        }
        return -1;
    }

    int MTNoteList::pasteNote( juce::ValueTree& VT, double DeltaTimeInTicks, bool SortNoteList )
    {
        MTNote n( VT, um );
        if( n.isValid() )
        {
            auto   pnVt = MTNoteUtil::createVT( VT, uniqueNoteId );
            MTNote pn( pnVt, nullptr );
            pn.start = DeltaTimeInTicks;

            return addNote( pnVt, SortNoteList );
        }
        return -1;
    }

    int MTNoteList::pasteSelectNote( juce::ValueTree& VT, double DeltaTimeInTicks, int DeltaNoteNumber, double DeltaLength, bool SortNoteList )
    {
        auto retVal = pasteNote( VT, DeltaTimeInTicks, DeltaNoteNumber, DeltaLength, SortNoteList );
        if( 0 == retVal )
            selectNote( VT, false );

        return retVal;
    }

    int MTNoteList::pasteNote( const int NoteID, double DeltaTimeInTicks, int DeltaNoteNumber, double DeltaLength )
    {
        auto vt = find( NoteID );
        return pasteNote( vt, DeltaTimeInTicks, DeltaNoteNumber, DeltaLength, true );
    }

    int MTNoteList::pasteSelectedNotes( double DeltaTimeInTicks, int DeltaNoteNumber, double DeltaLength )
    {
        int retVal = 0;
        for( auto it : selected )
        {
            retVal += pasteNote( it, DeltaTimeInTicks, DeltaNoteNumber, DeltaLength, false ); // Don't sort at every step...
        }

        // Sort note list.
        if( AudIO::utils::isNotEqual( DeltaTimeInTicks, 0.0 ) )
            sort();
        else
            triggerAsyncUpdate();
        return retVal;
    }

    int MTNoteList::pasteNotes( juce::Array<juce::ValueTree>& SelectedNotes, double DeltaTimeInTicks )
    {
        MTNote n( SelectedNotes[0] );
        int    retVal    = 0;
        int    noteStart = n.start.get();
        for( auto it : SelectedNotes )
        {
            MTNote note( it );
            int    start      = note.start.get();
            auto   deltaValue = DeltaTimeInTicks + start - noteStart;
            retVal += pasteNote( it, deltaValue, false ); // Don't sort at every step...
        }

        // Sort note list.
        if( AudIO::utils::isNotEqual( DeltaTimeInTicks, 0.0 ) )
            sort();
        else
            triggerAsyncUpdate();
        return retVal;
    }

    int MTNoteList::pasteSelectedNotesDefault()
    {
        int retVal       = 0;
        int minStartTime = selected[0][IDs::start];
        int maxEndTime   = (int)selected[0][IDs::start] + (int)selected[0][IDs::length];

        forSelectedNotes( [&]( juce::ValueTree& Note ) {
            auto noteEndTime = (int)Note[IDs::start] + (int)Note[IDs::length];
            if( (int)Note[IDs::start] < minStartTime ) minStartTime = Note[IDs::start];
            if( noteEndTime > maxEndTime ) maxEndTime = noteEndTime;
        } );

        double deltaTimeInTicks = maxEndTime - minStartTime;
        for( auto it : selected )
        {
            retVal += pasteSelectNote( it, deltaTimeInTicks, 0, 0, false ); // Don't sort at every step...
        }

        // Sort note list.
        if( AudIO::utils::isNotEqual( deltaTimeInTicks, 0.0 ) )
            sort();
        return retVal;
    }

    int MTNoteList::editNote( const int NoteId, double DeltaTimeInTicks, int DeltaNoteNumber, double DeltaLength, bool SortNoteList )
    {
        auto tmp = find( NoteId );
        return editNote( tmp, DeltaTimeInTicks, DeltaNoteNumber, DeltaLength, SortNoteList );
    }

    int MTNoteList::editNote( const int NoteId, int NoteStart )
    {
        auto   tmp = find( NoteId );
        MTNote note( tmp, um );
        if( note.isValid() )
        {
            note.start = NoteStart;
            return 0;
        }
        return -1;
    }

    int MTNoteList::editNoteLength( const int NoteId, double DeltaLength, bool SortNoteList, bool isLeftEdge )
    {
        auto tmp = find( NoteId );
        if( isLeftEdge )
            return editNote( tmp, DeltaLength, 0, -DeltaLength, SortNoteList );
        else
            return editNote( tmp, 0, 0, DeltaLength, SortNoteList );
    }

    int MTNoteList::editNote( const std::vector<int>& NoteIds, double DeltaTimeInTicks, int DeltaNoteNumber, double DeltaLength )
    {
        for( auto it : NoteIds )
            editNote( it, DeltaTimeInTicks, DeltaNoteNumber, DeltaLength, false ); // Don't sort at every step...

        if( AudIO::utils::isNotEqual( DeltaTimeInTicks, 0.0 ) )
            sort();
        return 0;
    }

    int MTNoteList::editNote( const NotesInRange& Range, double DeltaTimeInTicks, int DeltaNoteNumber, double DeltaLength )
    {
        auto retVal = 0;
        forNotesInRange( Range, [&]( juce::ValueTree& Note ) {
            retVal = editNote( Note, DeltaTimeInTicks, DeltaNoteNumber, DeltaLength, false );
        } );
        // Sort note list.
        if( AudIO::utils::isNotEqual( DeltaTimeInTicks, 0.0 ) )
            sort();
        return 0;
    }

    int MTNoteList::nudgeNotes( int noteNumber, double DeltaTimeInTicks )
    {
        juce::ScopedLock sl{ lock };
        auto             retVal = 0;

        if( AudIO::utils::isNotEqual( DeltaTimeInTicks, 0.0 ) )
        {
            bool changed = false;
            for( auto i = 0; i < vTree.getNumChildren(); ++i )
            {
                auto   Note = vTree.getChild( i );
                MTNote n( Note );
                if( noteNumber == n.number.get() )
                {
                    retVal = nudgeNote( Note, DeltaTimeInTicks, false );
                    if( retVal == 1 )
                        i--;
                    changed = true;
                }
            }
            if( changed ) sort();
        }
        return retVal;
    }

    int MTNoteList::nudgeSelectedNotes( double DeltaTimeInTicks, bool Snap, double TotalTicks )
    {
        juce::ScopedLock sl{ lock };

        if( AudIO::utils::isNotEqual( DeltaTimeInTicks, 0.0 ) )
        {
            if( isValidNudge( DeltaTimeInTicks, TotalTicks, Snap ) )
            {
                for( auto Note : selected )
                {
                    nudgeNote( Note, DeltaTimeInTicks, false );
                }
                if( 0 != selected.size() ) sort();
            }
        }
        return 0;
    }

    int MTNoteList::transposeSelectedNotes( int DeltaNoteNumber )
    {
        juce::ScopedLock sl{ lock };
        if( AudIO::utils::isNotEqual( DeltaNoteNumber, 0 ) )
        {
            if( !isSelectedNotesWithinRange( DeltaNoteNumber ) )
            {
                for( auto Note : selected )
                {
                    MTNote n( Note );
                    n.number = ( n.number.get() + DeltaNoteNumber );
                }
            }
        }
        return 0;
    }

    int MTNoteList::editSelectedNotesStart( int DeltaNoteStart, bool Snap )
    {
        juce::ScopedLock sl{ lock };
        if( AudIO::utils::isNotEqual( DeltaNoteStart, 0 ) )
        {
            if( !isSelectedNoteStartZero( DeltaNoteStart, Snap ) || DeltaNoteStart > 0 )
            {
                for( auto Note : selected )
                {
                    editNoteStart( Note, DeltaNoteStart );
                }
                if( 0 != selected.size() ) sort();
            }
        }
        return 0;
    }

    bool MTNoteList::isSelectedNoteStartZero( double Length, bool Snap )
    {
        if( !selected.isEmpty() )
        {
            for( auto Note : selected )
            {
                MTNote n( Note );
                auto   start = n.start.get();
                if( Length < 0 )
                {
                    if( Snap )
                    {
                        if( start == 0 )
                            return true;
                    }
                    else
                    {
                        if( start + Length < 0 )
                            return true;
                    }
                }
            }
        }
        return false;
    }

    bool MTNoteList::isSelectedNotesWithinRange( int DeltaNoteNum )
    {
        if( !selected.isEmpty() )
        {
            for( auto Note : selected )
            {
                MTNote n( Note );
                auto   noteNum = n.number.get();
                if( DeltaNoteNum > 0 )
                {
                    if( 127 == noteNum )
                        return true;
                }
                else if( DeltaNoteNum < 0 )
                {
                    if( 0 == noteNum )
                        return true;
                }
            }
        }
        return false;
    }

    bool MTNoteList::isValidNudge( double Length, double TotalTicks, bool Snap )
    {
        if( !selected.isEmpty() )
        {
            for( auto Note : selected )
            {
                MTNote n( Note );
                auto   start = n.start.get();
                if( Length < 0 )
                {
                    if( Snap )
                    {
                        if( start == 0 )
                            return false;
                    }
                    else
                    {
                        if( start + Length < 0 )
                            return false;
                    }
                }
                else
                {
                    auto validStartValue = start + n.length.get() + Length;
                    if( validStartValue > TotalTicks )
                        return false;
                }
            }
        }
        return true;
    }

    int MTNoteList::editSelectedNotesEnd( int DeltaNoteEnd )
    {
        juce::ScopedLock sl{ lock };
        if( AudIO::utils::isNotEqual( DeltaNoteEnd, 0 ) )
        {
            for( auto Note : selected )
            {
                editNoteEnd( Note, DeltaNoteEnd );
            }
        }
        return 0;
    }

    int MTNoteList::editNoteLength( juce::ValueTree noteOnEdit, const int deltaLength, bool isLeftEdge )
    {
        if( noteOnEdit.isValid() )
            if( !isLeftEdge )
                return editNote( noteOnEdit, 0, 0, deltaLength, false );
            else
                return editNote( noteOnEdit, deltaLength, 0, ( -1 ) * deltaLength );

        return 0;
    }

    int MTNoteList::editSelectedNote( const int DeltaTicks, bool IsLeftEdge )
    {
        for( auto index = 0; index < selected.size(); ++index )
        {
            MTNote note( selected[index] );
            if( note.isValid() )
            {
                auto newStart = note.start.get() + ( IsLeftEdge ? DeltaTicks : 0 );
                if( newStart < 0 )
                {
                    eraseNote( selected[index] );
                    index--;
                }
                else
                {
                    auto length = note.length.get() + ( IsLeftEdge ? ( -1 ) * DeltaTicks : DeltaTicks );
                    updateNote( note, newStart, length );
                }
            }
        }
        return -1;
    }

    int MTNoteList::updateNote( MTNote& Note, int NewStart, int Length )
    {
        if( Length != 0 )
        {
            Note.length = Length;
            if( 0 >= Note.length.get() ) Note.length = 1;
        }

        auto prevStart = Note.start.get();
        Note.start     = NewStart;
        if( auto* innerMap = notes[prevStart] )
        {
            if( auto* keyNote = ( *innerMap )[Note.id.get()] )
            {
                innerMap->remove( Note.id.get() );
                if( innerMap->size() == 0 )
                {
                    notes.remove( prevStart );
                    delete innerMap;
                }

                if( !notes.contains( Note.start.get() ) )
                    notes.set( Note.start.get(), new juce::HashMap<int, MTNote*>() );

                notes[Note.start.get()]->set( Note.id.get(), keyNote );

                // MIDi file...
                if( pMidiSeq )
                {
                    juce::MidiMessage msgOn  = juce::MidiMessage::noteOn( 1, keyNote->number.get(), (juce::uint8)keyNote->velocity.get() );
                    juce::MidiMessage msgOff = juce::MidiMessage::noteOff( 1, keyNote->number.get(), (juce::uint8)keyNote->velocity.get() );
                    pMidiSeq->addEvent( msgOn, keyNote->start.get() );
                    pMidiSeq->addEvent( msgOff, keyNote->start.get() + keyNote->length.get() );
                    pMidiSeq->updateMatchedPairs();
                }
            }
        }
        return 0;
    }

    int MTNoteList::editSelectedNotesLength( const int deltaLength, bool isLeftEdge )
    {
        forSelectedNotes( [&]( juce::ValueTree& Note ) {
            editNoteLength( Note, deltaLength, isLeftEdge );
        } );
        return 0;
    }

    int MTNoteList::editSelectedNotesVelocity( const int Velocity )
    {
        forSelectedNotes( [&]( juce::ValueTree& Note ) {
            editNoteVelocity( Note, Velocity );
        } );
        return 0;
    }

    int MTNoteList::editSelectedNotesProbability( const int Probability )
    {
        forSelectedNotes( [&]( juce::ValueTree& Note ) {
            editNoteProbability( Note, Probability );
        } );
        return 0;
    }

    int MTNoteList::editSelectedNotesRatchet( const int Ratchet )
    {
        forSelectedNotes( [&]( juce::ValueTree& Note ) {
            editNoteRatchet( Note, Ratchet );
        } );
        return 0;
    }

    int MTNoteList::editNoteVelocity( juce::ValueTree& VT, const int Velocity )
    {
        if( VT.isValid() && VT.hasType( IDs::NOTE ) )
        {
            MTNote n( VT, um );
            n.velocity = Velocity;
            triggerAsyncUpdate();
            return 0;
        }
        return -1;
    }

    int MTNoteList::editNoteVelocity( const int NoteID, const int Velocity )
    {
        auto note = find( NoteID );
        return editNoteVelocity( note, Velocity );
    }

    int MTNoteList::editNoteVelocity( const std::vector<int>& NoteIDs, const std::vector<int>& Velocities )
    {
        int i = 0;
        for( auto it : NoteIDs )
        {
            editNoteVelocity( it, Velocities[i++] );
        }
        return 0;
    }

    int MTNoteList::editNoteVelocity( const NotesInRange& Range, int Velocity )
    {
        forNotesInRange( Range, [&]( juce::ValueTree& Note ) {
            editNoteVelocity( Note, Velocity );
        } );
        return 0;
    }

    int MTNoteList::editNoteProbability( juce::ValueTree& VT, const int Probability )
    {
        if( VT.isValid() && VT.hasType( IDs::NOTE ) )
        {
            MTNote n( VT, um );
            n.probability = Probability;
            triggerAsyncUpdate();
            return 0;
        }
        return -1;
    }

    int MTNoteList::editNoteProbability( const int NoteID, const int Probability )
    {
        auto note = find( NoteID );
        return editNoteProbability( note, Probability );
    }

    int MTNoteList::editNoteProbability( const std::vector<int>& NoteIDs, const std::vector<int>& Probabilities )
    {
        int i = 0;
        for( auto it : NoteIDs )
        {
            editNoteProbability( it, Probabilities[i++] );
        }
        return 0;
    }

    int MTNoteList::editNoteProbability( const NotesInRange& Range, int Probability )
    {
        forNotesInRange( Range, [&]( juce::ValueTree& Note ) {
            editNoteProbability( Note, Probability );
        } );
        return 0;
    }

    int MTNoteList::editNoteRatchet( juce::ValueTree& VT, const int Ratchet )
    {
        if( VT.isValid() && VT.hasType( IDs::NOTE ) )
        {
            MTNote n( VT, um );
            n.ratchet = Ratchet;
            triggerAsyncUpdate();
            return 0;
        }
        return -1;
    }

    int MTNoteList::editNoteRatchet( const int NoteID, const int Ratchet )
    {
        auto note = find( NoteID );
        return editNoteRatchet( note, Ratchet );
    }

    int MTNoteList::editNoteRatchet( const std::vector<int>& NoteIDs, const std::vector<int>& Ratchets )
    {
        int i = 0;
        for( auto it : NoteIDs )
        {
            editNoteRatchet( it, Ratchets[i++] );
        }
        return 0;
    }

    int MTNoteList::editNoteRatchet( const NotesInRange& Range, int Ratchet )
    {
        forNotesInRange( Range, [&]( juce::ValueTree& Note ) {
            editNoteRatchet( Note, Ratchet );
        } );
        return 0;
    }

    int MTNoteList::scaleNote( juce::ValueTree& VT, float factor, bool SortNoteList )
    {
        if( VT.isValid() && VT.hasType( IDs::NOTE ) )
        {
            MTNote n( VT, um );
            n.start  = (int)( n.start * factor );
            n.length = (int)( n.length * factor );
            if( SortNoteList ) sort();
            return 0;
        }
        return -1;
    }

    int MTNoteList::scaleSelectedNotes( float factor )
    {
        int retVal = 0;
        for( auto it : vTree )
            retVal += scaleNote( it, factor, false );

        sort();
        return retVal;
    }

    int MTNoteList::scaleAllNotes( float factor )
    {
        int retVal = 0;
        forEachChildInVTree( vTree, [factor, this, &retVal]( juce::ValueTree& v ) {
            retVal += scaleNote( v, factor, false );
        } );
        // No need to sort, as all notes' start time is altered by same amount.
        return retVal;
    }

    void MTNoteList::setMidiList( juce::MidiMessageSequence* MidiList )
    {
        pMidiSeq = MidiList;
        sort();
    }

    int MTNoteList::splitNotes( const track::splitInfo& Info )
    {
        if( 1 < Info.parts )
        {
            juce::Array<juce::ValueTree> splitNoteArray;
            for( auto Note : vTree )
            {
                MTNote n( Note, nullptr );
                auto   st  = n.start.get();
                auto   len = n.length.get() / Info.parts;
                if( len == 0 )
                    continue;
                n.length = len;
                for( int i = 1; i < Info.parts; ++i )
                {
                    auto   nVt = MTNoteUtil::createVT( Note, uniqueNoteId );
                    MTNote pn( nVt, nullptr );

                    pn.start  = st + ( i * len );
                    pn.length = len;
                    splitNoteArray.add( nVt );
                }
            }

            for( auto it : splitNoteArray )
                vTree.appendChild( it, um );

            // Sort note list.
            sort();
        }
        return 0;
    }

    int MTNoteList::splitSelectedNotes( const track::splitInfo& Info )
    {
        if( Info.parts > 1 && selected.size() > 0 )
        {
            juce::Array<juce::ValueTree> splitNotesArray;
            for( auto it : selected )
            {
                MTNote n( it );
                auto   start = n.start.get();
                auto   len   = n.length.get() / Info.parts;
                if( len == 0 )
                    continue;
                n.length = len;
                for( auto id = 1; id < Info.parts; ++id )
                {
                    auto   nVt = MTNoteUtil::createVT( it, uniqueNoteId );
                    MTNote nNote( nVt );

                    nNote.start  = start + ( id * len );
                    nNote.length = len;
                    splitNotesArray.add( nVt );
                }
            }

            for( auto it : splitNotesArray )
            {
                vTree.addChild( it, -1, um );
                selected.add( it );
            }
            sort();
        }
        return 0;
    }

    int MTNoteList::humanizeNotes( const track::humanizeInfo& Info )
    {
        bool         sortRequired{ false };
        juce::Random r;

        if( Info.onlySelected )
        {
            for( auto Note : selected )
                humanizeNotes( Info, Note, r, sortRequired );
        }
        else
        {
            for( auto Note : vTree )
                humanizeNotes( Info, Note, r, sortRequired );
        }

        if( sortRequired )
            sort();
        return 0;
    }

    int MTNoteList::humanizeNotes( const track::humanizeInfo& Info, juce::ValueTree& Note, juce::Random& r, bool& sortRequired )
    {
        auto deltaDiff = [&r]( int Value, bool negative = false ) -> double {
            double delta = ( ( 100 - ( Value * r.nextFloat() ) ) / 100 );
            if( negative )
                delta *= ( 0 < r.nextInt() ? 1 : -1 );
            return delta;
        };

        MTNote n( Note, um );
        if( Info.isTimeEnabled && Info.time )
        {
            auto newStart = (int)( n.start * ( deltaDiff( Info.time, true ) / 16 ) ); // Max 16th note.
            updateNote( n, newStart );
            sortRequired = true;
        }

        if( Info.isNoteLengthEnabled && Info.noteLength )
        {
            n.length = (int)( n.length * deltaDiff( Info.noteLength ) );
        }

        if( Info.isVelocityEnabled && Info.velocity )
        {
            n.velocity = (int)( n.velocity.get() * deltaDiff( Info.velocity ) );
        }
        return 0;
    }

    int MTNoteList::randomNotes( const track::randomEventsInfo& Info )
    {
        juce::Random r;

        int  p1            = Kick::getPadNumber( Info.pads.start );
        int  p2            = Kick::getPadNumber( Info.pads.end );
        auto pitchStart    = juce::jmin<int>( p1, p2 );
        auto pitchEnd      = juce::jmax<int>( p1, p2 );
        auto noteLen       = seq.toTicks( Info.noteLength );
        auto durationTicks = Info.patternSize * seq.getTicksPerBar( 0 );
        auto maxNotes      = ( ( pitchEnd - pitchStart ) * ( durationTicks / noteLen ) );
        auto density       = juce::jlimit( 20, 100, Info.densityValue );
        auto rhythm        = juce::jlimit( 20, 100, Info.rhythmVariationValue );
        auto notesToAdd    = maxNotes * ( r.nextFloat() * ( density / 100.0f ) );

        notesToAdd *= ( 1 + ( r.nextFloat() * ( rhythm / 100.0f ) ) );
        durationTicks -= noteLen;

        if( track::RandomEventType::AppendExisting == Info.replace )
            notesToAdd = (int)( juce::jlimit<double>( 0.0, notesToAdd, notesToAdd - getNumNotes() ) );

        auto randomVTree = juce::ValueTree( IDs::NOTELIST );

        switch( Info.replace )
        {
            case track::RandomEventType::AppendExisting:
                break;
            case track::RandomEventType::ReplaceInRange:
                deleteNotesInNoteRange( pitchStart, pitchEnd );
                break;
            case track::RandomEventType::ReplaceAll:
                deleteAllNotes();
                break;
        }

        // Add Notes...
        for( int i = 0; i < (int)notesToAdd; ++i )
        {
            MTNote n;
            n.length      = noteLen;
            n.number      = r.nextInt( { (int)pitchStart, (int)pitchEnd } );
            n.start       = (int)( r.nextFloat() * durationTicks );
            n.velocity    = (int)( r.nextFloat() * MIDI::MAX );
            n.id          = ++uniqueNoteId;
            n.probability = (int)( r.nextFloat() * PROBABILITY::MAX );
            n.ratchet     = (int)( r.nextFloat() * RATCHET::MAX );

            randomVTree.addChild( n.getState(), -1, nullptr );
        }

        randomVTree.sort( _noteSorter, nullptr, false );
        for( auto index = 0; index < randomVTree.getNumChildren(); ++index )
        {
            auto child = randomVTree.getChild( index );
            randomVTree.removeChild( child, nullptr );
            addNote( child );
            --index;
        }
        return 0;
    }

    int MTNoteList::pitchQuantizeNotes( const track::pitchQuantizeInfo& Info )
    {
        if( Info.onlySelected )
        {
            for( auto Note : selected )
                pitchQuantizeNotes( Info, Note );
        }
        else
        {
            for( auto Note : vTree )
                pitchQuantizeNotes( Info, Note );
        }
        return 0;
    }

    int MTNoteList::pitchQuantizeNotes( const track::pitchQuantizeInfo& Info, juce::ValueTree& Note )
    {
        auto   startNote = ScaleAndNotes::getNoteNumber( Info.range.start );
        auto   endNote   = ScaleAndNotes::getNoteNumber( Info.range.end );
        auto   scale     = ScaleAndNotes::getNotesInScale( Info.rootNoteValue, Info.scaleValue, Info.range.start, Info.range.end );
        MTNote n( Note, um );

        int noteNumber = n.number.get();
        // Move within the desired octave...
        while( startNote > noteNumber ) { noteNumber += 12; } // Move Octave up
        while( endNote < noteNumber ) { noteNumber -= 12; } // Move Octave down
        // Move to nearest boundary.
        if( startNote > noteNumber ) { noteNumber = startNote; }
        if( endNote < noteNumber ) { noteNumber = endNote; }
        // Move to nearest scale note.
        for( unsigned int i = 0; i < scale.size(); ++i )
        {
            if( scale[i] == noteNumber || scale[i + 1] == noteNumber )
                break;
            else if( scale[i] < noteNumber && scale[i + 1] > noteNumber )
            {
                auto diff1 = noteNumber - scale[i];
                auto diff2 = scale[i + 1] - noteNumber;
                noteNumber = ( diff1 <= diff2 ? scale[i] : scale[i + 1] );
                break;
            }
        }
        // Update note number
        n.number = noteNumber;
        return 0;
    }

    void MTNoteList::modifyBars( std::vector<MTNote>& noteList, int /*fromTimeToEdit*/, int /*toTimeToEdit*/, int numOfBars, int timeToEdit, int newStartValue, int newEndValue, bool isMerge )
    {
        if( !isMerge )
        {
            deleteNoteInTimeRange( newStartValue, newEndValue );
        }
        for( auto note : noteList )
        {
            auto startValue = note.start.get() - timeToEdit;
            addOrModifyNote( note.number.get(), startValue, note.length.get() );
        }
    }

    void MTNoteList::modifyLength( int TimeToEdit, bool Insert, int AtPosition )
    {
        if( Insert )
        {
            for( auto it : vTree )
            {
                MTNote note( it );
                if( AtPosition <= note.start.get() )
                    note.start = TimeToEdit + note.start.get();
            }
        }
        else
        {
            auto startTime = AtPosition;
            auto endTime   = startTime + TimeToEdit;
            deleteNoteInTimeRange( startTime, endTime );
        }
    }

    int MTNoteList::explode()
    {
        std::vector<int> pads;
        for( auto it : vTree )
        {
            MTNote n( it );
            auto   isPresent{ false };
            for( auto i = 0; i < pads.size(); ++i )
                if( pads[i] == n.number.get() )
                    isPresent = true;
            if( !isPresent )
                pads.push_back( n.number.get() );
        }

        auto& ae          = getAudioInterface();
        auto  trackNo     = ae.getSelectedTrack();
        auto  seqNo       = ae.getSelectedSequence();
        auto  state       = ae.getTrackState( seqNo, trackNo );
        auto  parentChild = state.getChildWithName( IDs::NOTELIST );

        for( auto i = 0; i < pads.size(); ++i )
        {
            auto trackNum  = ae.createTrack( seqNo, "" );
            auto track     = ae.getTrackData( seqNo, trackNo );
            auto trackName = track->name.get() + " Pad " + juce::String( pads[i] + 1 );
            ae.renameTrack( seqNo, trackNum, trackName );

            auto trackState = ae.getTrackState( seqNo, ( ae.getNumTracks( seqNo ) - 1 ) );
            auto childState = trackState.getChildWithName( IDs::NOTELIST );

            childState.removeAllChildren( nullptr );

            for( auto it = 0; it < parentChild.getNumChildren(); ++it )
            {
                auto child = parentChild.getChild( it ).createCopy();
                if( (int)( child[IDs::no] ) == pads[i] )
                    childState.appendChild( child, nullptr );
            }
        }
        return 0;
    }

    int MTNoteList::copyNotesTo( const track::copyTrackNotes& Info )
    {
        juce::ValueTree tempVtre = parent;
        auto&           ae       = getAudioInterface();
        auto            srcState = ae.getTrackState( Info.FromSeq, Info.FromTrack );
        juce::ValueTree Parent   = srcState.getChildWithName( IDs::NOTELIST );

        std::set<int> pads;

        for( const auto& note : vTree )
        {
            MTNote n( note );
            pads.insert( n.number.get() );
        }

        auto copyNote = [&, Info]( const MTNote& n ) {
            bool copy = ( ( Info.SourceRange.noteStart == 0 && Info.SourceRange.noteEnd == 127 ) || Info.SourceRange.noteStart == n.number.get() );
            if( n.start.get() >= Info.SourceRange.startTime && n.start.get() < Info.SourceRange.endTime && copy )
            {
                int newStartPos = Info.DestinationRange.startTime + ( n.start.get() - Info.SourceRange.startTime );
                ae.addOrModifyNotesInTrack( Info.ToSeq, Info.ToTrack, n.number.get(), newStartPos, n.length.get() );
            }
        };

        if( Info.CopyEvents )
        {
            auto& selection = ae.getSelectedNoteList();
            for( int i = 0; i < selection.size(); ++i )
            {
                MTNote n( selection[i] );
                copyNote( n );
            }
        }
        else
        {
            auto size = Parent.getNumChildren();
            for( int i = 0; i < size; ++i )
            {
                MTNote n( Parent.getChild( i ) );
                if( Info.ToTrack == Info.FromTrack && Info.FromSeq == Info.ToSeq )
                    addOrModifyNote( n.number.get(), Info.DestinationRange.startTime + ( n.start.get() - Info.SourceRange.startTime ), n.length.get(), n.velocity.get(), n.probability.get(), n.ratchet.get() );
                else
                    copyNote( n );
            }
        }
        return 0;
    }
}
