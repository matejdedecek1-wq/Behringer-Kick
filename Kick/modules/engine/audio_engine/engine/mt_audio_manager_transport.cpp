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
    ///  MTAudioTransportManager
    ///

    template<typename T>
    class ScopedAtomicResetter
    {
    public:
        ScopedAtomicResetter( std::atomic<T>& val, T resetValue, T initialValue )
            : v( val ), rv( resetValue ) { v.store( initialValue ); }
        ~ScopedAtomicResetter() { v.store( rv ); };
        ScopedAtomicResetter( const ScopedAtomicResetter& )            = delete;
        ScopedAtomicResetter& operator=( const ScopedAtomicResetter& ) = delete;

    private:
        std::atomic<T>& v;
        T               rv;
    };

    MTAudioTransportManager::MTAudioTransportManager( MTSelected& Selected, MTAudioManager& Manager )
        : MTAudioSequenceManager( Selected, Manager )
    {
        // Defaults...
        runtimeVT = juce::ValueTree{ IDs::RUNTIME };
        runtimeVT.getOrCreateChildWithName( IDs::TRANSPORT, nullptr );
        runtimeVT.getOrCreateChildWithName( IDs::PLAYHEAD, nullptr );

        setTransportState( TransportState::Idle );

        /// Connect MIDI callbacks...
        midiClock.onTick           = [&]( uint32_t T ) { onTick( T ); };
        midiClock.onSync24Callback = [&]( uint32_t T, uint32_t U ) {};

        /// Connect sequence playback callbacks.
        signals::on1_2.connect( [&]() {} );
        signals::on1_4.connect( [&]() {} );
        signals::on1_8.connect( [&]() {} );
        signals::on1_16.connect( [&]() {} );

        signals::onBeat.connect( [&]( int ) {} );
        signals::onBar.connect( [&]( int ) { onBar(); } );

        auto tickProcessingFunc = [&]() { processTick(); };
        tickProcessingThread    = new std::thread( tickProcessingFunc );
    }

    MTAudioTransportManager::~MTAudioTransportManager()
    {
        close();

        if( tickProcessingThread )
        {
            stopTickProcessing.store( true );
            tickProcessingThread->join();
            delete tickProcessingThread;
            tickProcessingThread = nullptr;
        }
    }

    void MTAudioTransportManager::close()
    {
        stop();
        playingSequence = nullptr;
    }

    void MTAudioTransportManager::setup()
    {
        // Initialize transport state...
        setTransportState( TransportState::Idle );
    }

    ////////////////////////////
    //// Transport ...
    ////////////////////////////
    TransportState MTAudioTransportManager::getTransportState()
    {
        auto t = runtimeVT.getOrCreateChildWithName( IDs::TRANSPORT, nullptr );
        return (TransportState)( (int)t.getProperty( IDs::state, TransportState::Idle ) );
    }

    void MTAudioTransportManager::setTransportState( TransportState TState )
    {
        auto t        = runtimeVT.getOrCreateChildWithName( IDs::TRANSPORT, nullptr );
        auto oldState = (TransportState)( (int)t.getProperty( IDs::state, TransportState::Idle ) );

        if( oldState != TState )
        {
            // Set new transport state...
            t.setProperty( IDs::state, TState, nullptr );

            if( transportStateChanged )
                transportStateChanged( TState );
        }
    }

    bool MTAudioTransportManager::isArmed()
    {
        auto pSeq = getSequence();
        return pSeq ? pSeq->isArmed() : false;
    }

    bool MTAudioTransportManager::isRecording()
    {
        auto pSeq = getSequence();
        return pSeq ? pSeq->isRecording() : false;
    }

    // Playback API's

    int MTAudioTransportManager::play( int Seq )
    {
        int retVal = -1;

        if( auto s = getSequence( Seq ) )
        {
            if( playingSequence && ( s != playingSequence ) )
            {
                playingSequence->stop();
            }
            playingSequence = s;
            //playingSequence->updatePlayhead(); // redundant call as playingSequence->play() updates the playhead.

            if( isPaused() )
            {
                playingSequence->resume();
            }
            else
            {
                playingSequence->play();
            }

            setTransportState( TransportState::Playing );
            retVal = 0;
        }

        return retVal;
    }

    int MTAudioTransportManager::resume( int Seq )
    {
        return play( Seq );
    }

    int MTAudioTransportManager::pause()
    {
        midiClock.stop();
        setTransportState( TransportState::Paused );

        int retVal = -1;
        if( playingSequence )
        {
            playingSequence->pause();
            retVal = 0;
        }
        return retVal;
    }

    int MTAudioTransportManager::stop()
    {
        midiClock.stop();
        setTransportState( TransportState::Stopped );

        int retVal = -1;
        if( playingSequence )
        {
            playingSequence->stop();
            retVal = 0;
        }
        return retVal;
    }

    int MTAudioTransportManager::play( int Seq, bool FromBeginning, bool Now )
    {
        if( Now )
            return FromBeginning ? play( Seq ) : resume( Seq );
        else
        {
            if( auto s = getSequence( Seq ) )
            {
                _nextBar.seq.store( Seq );
                _nextBar.fromBegin.store( FromBeginning );
                _nextBar.isEnabled.store( true );
            }
        }
        return -1;
    }

    void MTAudioTransportManager::seek( int posInTicks )
    {
        if( playingSequence )
        {
            playingSequence->seek( posInTicks );
        }
    }

    /// Callback functions...
    void MTAudioTransportManager::onTick( unsigned int Ticks )
    {
        /*if( playingSequence )
        {
            playingSequence->onTick( Ticks );
        }
        updatePlayheads();*/

        /*juce::Logger::writeToLog( "is Tick Processing, " + juce::String( threadIsProcessingTick.load() ? "yes, " : "No, " ) +
            "Prev Tick, " + juce::String( prevTick.load() ) + ", CurTick, " + juce::String(curTick.load()) + ", Ticks" + juce::String(Ticks));*/

        if( threadIsProcessingTick )
            return;

        curTick = Ticks;
        onTickEvent.signal();
        if( curTick < prevTick )
            prevTick = 0;
        playTime += ( curTick - prevTick );

        if( !playQueue.empty() )
        {
            if( nextSequenceUpdateTicks == -1 ) nextSequenceUpdateTicks = playQueue.size() ? playQueue.front().nextSeqUpdateTicks : -1;
            if( playQueue.front().timeInTicks <= playTime )
            {
                ++stepIndex;
                resetSong();
                if( playQueue.front().isEnabled )
                {
                    setSelectedSequence( playQueue.front().seq );
                    play( playQueue.front().seq );
                }
                playQueue.pop();
                signals::onSongSequencePlay( playingSequence->getIndex(), -1, stepIndex );
            }
            else if( ( nextSequenceUpdateTicks <= playTime ) && ( nextSequenceUpdateTicks >= 0 ) )
            {
                nextSequenceUpdateTicks = -2;
                signals::onNextSeqAboutToPlayInSong( playQueue.front().seq );
            }
        }
        else
        {
            if( lastSeqTicks <= playTime )
            {
                signals::onSongStopped();
                resetSong();
            }
        }

        //Next Sequence
        if( playingSequence && nextSequenceToPlay >= 0 )
        {
            auto length        = playingSequence->isLooping.get() ? playingSequence->loopEnd.get() - playingSequence->loopStart.get() : (int)playingSequence->length.get();
            auto lengthInTicks = length * playingSequence->getTicksPerBar();
            if( curTick % lengthInTicks == 0 )
            {
                play( nextSequenceToPlay );
                nextSequenceToPlay = -1;
                signals::onNextSequencePlay( playingSequence->getIndex() );
            }
        }
    }

    void MTAudioTransportManager::processTick()
    {
        while( !stopTickProcessing )
        {
            onTickEvent.wait( 1 );
            if( prevTick == curTick )
                continue;
            try
            {
                const ScopedAtomicResetter<bool> resetter( threadIsProcessingTick, false, true );

                prevTick.store( curTick );

                if( playingSequence )
                {
                    playingSequence->onTick( curTick );
                }
            } catch( ... )
            {
                DBG( "Exception While tick Processing..." );
            }
        }
    }

    void MTAudioTransportManager::updatePlayheads()
    {
        if( playingSequence )
        {
            playingSequence->playheadLocalToGlobal();

            if( MTTrack* pTrack = playingSequence->getSelectedTrackPtr() )
                MTAudioSequenceManager::getTrackPlayhead() = pTrack->playhead();
        }
    }

    void MTAudioTransportManager::onBeat()
    {
        if( playingSequence )
            playingSequence->onBeat();
    }

    void MTAudioTransportManager::onBar()
    {
        if( playingSequence )
        {
            if( _nextBar.isEnabled.load() )
            {
                _nextBar.isEnabled.store( false );
                _nextBar.fromBegin.load() ? play( _nextBar.seq.load() ) : resume( _nextBar.seq.load() );
                signals::onNextSequencePlay( _nextBar.seq.load() );
            }
        }
    }

    void MTAudioTransportManager::onStart( unsigned int BeginTick )
    {
        if( playingSequence )
            playingSequence->onStart( BeginTick );
    }

    void MTAudioTransportManager::onLoopBegin( unsigned int BeginTick )
    {
        if( playingSequence )
            playingSequence->onLoopBegin( BeginTick );
    }

    void MTAudioTransportManager::onLoopEnd( unsigned int EndTick, unsigned int LoopCount )
    {
        if( playingSequence )
            playingSequence->onLoopEnd( EndTick, LoopCount );
    }

    void MTAudioTransportManager::setupSong( MTSongInfo SongInfo, int Index )
    {
        if( SongInfo.name.isNotEmpty() && SongInfo.steps.size() )
        {
            auto index{ 0 };
            bool first{ true };
            int  firstSeq{ -1 };
            int  prevSeq{ -1 };
            int  repeat{ 1 };
            while( !playQueue.empty() )
                playQueue.pop();
            resetSong();
            for( auto& step : SongInfo.steps )
            {
                if( index >= Index )
                {
                    if( first )
                    {
                        firstSeq = step.seqNo;
                        first    = false;
                        prevSeq  = firstSeq;
                        repeat   = step.repeatCount;
                    }
                    else
                    {
                        auto pSeq   = getSequence( prevSeq );
                        auto length = pSeq->isLooping.get() ? pSeq->loopEnd.get() - pSeq->loopStart.get() : (int)pSeq->length.get();

                        PlayManager data;
                        data.timeInTicks        = pSeq->getTicksPerBar() * ( repeat * length );
                        data.nextSeqUpdateTicks = pSeq->getTicksPerBar() * ( ( repeat - 1 ) * length );
                        data.seq                = step.seqNo;
                        data.repeatCount        = step.repeatCount;
                        prevSeq                 = step.seqNo;
                        repeat                  = step.repeatCount;
                        playQueue.push( data );
                    }
                }
                ++index;
            }

            auto pSeq    = getSequence( playQueue.size() ? playQueue.back().seq : firstSeq );
            auto length  = pSeq->isLooping.get() ? pSeq->loopEnd.get() - pSeq->loopStart.get() : (int)pSeq->length.get();
            lastSeqTicks = pSeq->getTicksPerBar() * ( playQueue.size() ? playQueue.back().repeatCount * length : SongInfo.steps[Index].repeatCount * length );
            stepIndex    = Index;
            setSelectedSequence( firstSeq );
            play( firstSeq );

            //Update UI
            if( SongInfo.steps[Index].bars == 1 )
                signals::onSongSequencePlay( SongInfo.steps[Index].seqNo, Index + 1 < SongInfo.steps.size() ? SongInfo.steps[Index + 1].seqNo : -1, Index );
            else
                signals::onSongSequencePlay( SongInfo.steps[Index].seqNo, -1, Index );
            if( SongInfo.steps[Index].repeatCount == 1 )
            {
                if( Index + 1 < SongInfo.steps.size() )
                    signals::onNextSeqAboutToPlayInSong( SongInfo.steps[Index + 1].seqNo );
            }
        }
    }

    void MTAudioTransportManager::resetSong()
    {
        playTime                = 0;
        prevTick                = 0;
        nextSequenceUpdateTicks = -1;
    }

    void MTAudioTransportManager::playSequence( int Sequence )
    {
        play( Sequence );
    }

    void MTAudioTransportManager::stopSequence()
    {
        pause();
    }

    void MTAudioTransportManager::setNextSequence( int NextSequence )
    {
        if( nextSequenceToPlay != NextSequence )
            nextSequenceToPlay = NextSequence;
    }

    void MTAudioTransportManager::setNextBarFlag( bool NextBarFlag )
    {
        _nextBar.isEnabled = NextBarFlag;
        _nextBar.seq       = nextSequenceToPlay == -1 ? playingSequence->getIndex() : nextSequenceToPlay;
    }
}
