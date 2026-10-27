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

namespace mt
{

    ///
    /// SONG STEP
    ///

    MTSongStep::MTSongStep( juce::ValueTree& VT, juce::UndoManager* Undo )
        : vTree( VT ), undo( Undo )
    {}

    bool MTSongStep::isValid() { return vTree.isValid() && vTree.hasType( IDs::step ); }
    bool MTSongStep::isInValid() { return !isValid(); }

    void MTSongStep::setRepeat( int Number ) { vTree.setProperty( IDs::repeat, Number, undo ); }
    void MTSongStep::setSequence( const juce::String& SequenceID ) { vTree.setProperty( IDs::id, SequenceID, undo ); }
    void MTSongStep::setSequence( const int SequenceIndex )
    {
        if( auto seq = getAudioInterface().getManager()->getSequence( SequenceIndex ) )
        {
            setSequence( seq->getID() );
        }
    }

    int          MTSongStep::getRepeat() { return vTree.getProperty( IDs::repeat, 1 ); }
    juce::String MTSongStep::getSequence() { return vTree.getProperty( IDs::id, "" ); }

    MTSongStepInfo MTSongStep::getInfo()
    {
        MTSongStepInfo info;

        info.seqID       = getSequence();
        info.repeatCount = getRepeat();

        if( auto seq = getAudioInterface().getManager()->getSequenceByID( info.seqID ) )
        {
            info.bars     = seq->getLength();
            info.bpm      = seq->getBPM();
            info.seqNo    = seq->getIndex();
            info.dispName = juce::String( info.seqNo + 1 ) + " : " + seq->getName();
        }
        return info;
    }

    ///
    /// SONG
    ///

    MTSong::MTSong( juce::ValueTree& VT, juce::UndoManager* Undo )
        : vTree( VT ), undo( Undo )
    {}

    bool MTSong::isValid() { return vTree.isValid() && vTree.hasType( IDs::song ); }
    bool MTSong::isInValid() { return !isValid(); }

    juce::String MTSong::getID() const { return vTree.getProperty( IDs::id, "" ); };
    juce::String MTSong::getName() const { return vTree.getProperty( IDs::name, "" ); };

    void MTSong::addStep( MTSongStepInfo& Step, int AtIndex )
    {
        auto id = Step.seqID;
        if( 0 >= id.length() )
            if( auto seq = getAudioInterface().getManager()->getSequence( Step.seqNo ) )
            {
                id = seq->getID();
            }

        auto vt = createVTree( IDs::step,
                               IDs::id, id,
                               IDs::repeat, Step.repeatCount );
        vTree.addChild( vt, AtIndex, undo );
    }

    void MTSong::editStep( MTSongStepInfo& Step, int AtIndex )
    {
        auto       vt = vTree.getChild( AtIndex );
        MTSongStep step( vt, undo );
        if( step.isValid() )
        {
            if( 0 < Step.seqID.length() )
                step.setSequence( Step.seqID );
            else
                step.setSequence( Step.seqNo );
            step.setRepeat( Step.repeatCount );
        }
    }

    void MTSong::removeStep( int Index ) { vTree.removeChild( Index, undo ); };
    void MTSong::removeAllSteps() { vTree.removeAllChildren( undo ); }

    MTSongInfo MTSong::getInfo() const
    {
        MTSongInfo info;
        info.name = getName();
        for( auto it : vTree )
        {
            MTSongStep step( it, nullptr );
            info.steps.emplace_back( step.getInfo() );
        }
        return info;
    }

    ///
    /// SONG MANAGER
    ///

    MTSongManager::MTSongManager()
    {
        close();
    }

    void MTSongManager::close()
    {
        undo  = nullptr;
        vTree = EMPTY::VTState;
    }

    void MTSongManager::init( juce::ValueTree VT, juce::UndoManager* Undo )
    {
        undo  = Undo;
        vTree = VT;
    }

    juce::ValueTree MTSongManager::getSong( const juce::String& SongName )
    {
        for( auto it : vTree )
        {
            auto s = MTSong( it, nullptr );
            if( s.isValid() && s.getName().equalsIgnoreCase( SongName ) )
            {
                return it;
            }
        }
        return {};
    }

    std::vector<juce::String> MTSongManager::getSongsList()
    {
        std::vector<juce::String> songs;
        for( auto it : vTree )
        {
            auto s = MTSong( it, nullptr );
            if( s.isValid() )
            {
                songs.push_back( s.getName() );
            }
        }
        return songs;
    }

    void MTSongManager::createSong( const juce::String& SongName )
    {
        auto vt = createVTree( IDs::song,
                               IDs::id, CommonUtils::createID(),
                               IDs::name, SongName );
        vTree.appendChild( vt, undo );
    }

    void MTSongManager::deleteSong( const juce::String& SongName ) { vTree.removeChild( getSong( SongName ), undo ); }
    void MTSongManager::deleteSong( int Index ) { vTree.removeChild( Index, undo ); }

    MTSongInfo MTSongManager::getSongInfo( const juce::String& SongName )
    {
        auto   vt = getSong( SongName );
        MTSong song( vt, nullptr );
        return song.isValid() ? song.getInfo() : MTSongInfo{};
    }

    void MTSongManager::addSongStep( const juce::String& SongName, MTSongStepInfo& Step, int AtIndex )
    {
        auto   vt = getSong( SongName );
        MTSong song( vt, nullptr );
        if( song.isValid() )
            song.addStep( Step, AtIndex );
    }

    void MTSongManager::editSongStep( const juce::String& SongName, MTSongStepInfo& Step, int AtIndex )
    {
        auto   vt = getSong( SongName );
        MTSong song( vt, nullptr );
        if( song.isValid() )
            song.editStep( Step, AtIndex );
    }

    void MTSongManager::deleteSongStep( const juce::String& SongName, int AtIndex )
    {
        auto   vt = getSong( SongName );
        MTSong song( vt, nullptr );
        if( song.isValid() )
            song.removeStep( AtIndex );
    }

    void MTSongManager::deleteAllSongSteps( const juce::String& SongName )
    {
        auto   vt = getSong( SongName );
        MTSong song( vt, nullptr );
        if( song.isValid() )
            song.removeAllSteps();
    }

    bool MTSongManager::isSongExists( const juce::String& SongName )
    {
        if( vTree.isValid() )
        {
            for( int i = 0; i < vTree.getNumChildren(); ++i )
            {
                auto song = vTree.getChild( i );

                if( song.hasProperty( "name" ) )
                {
                    juce::String existingSongName = song.getProperty( "name" ).toString();
                    if( existingSongName.equalsIgnoreCase( SongName ) )
                    {
                        return true;
                    }
                }
            }
        }
        return false;
    }

}
