#include <chrono>
#include <thread>
#include "DawSequence.h"

namespace AudIO
{
    DawSequence::DawSequence( const std::string& Name, void* Data )
        : _argument( Data )
    {
        setName( Name );
        // Reserve memory for data structures...
        _tracks.reserve( 128 );
    }

    DawSequence::~DawSequence()
    {
        // Remove this sequence from transport control...
        stop();
        _tracks.clear();
        _argument = nullptr;
    }

    const std::string& DawSequence::getName() const { return _name; }

    void DawSequence::setName( const std::string& Name )
    {
        _name = Name;
    }

    // Sequence Parameters
    DawTrack* DawSequence::createTrack( const std::string& Name )
    {
        if( !isTrackExists( Name ) )
        {
            auto track  = std::make_unique<DawTrack>( Name );
            auto pTrack = track.get();
            _tracks.push_back( std::move( track ) );
            return pTrack;
        }
        return nullptr;
    };

    bool DawSequence::deleteTrack( int i )
    {
        if( utils::isLessOrEuqal( i, (int)_tracks.size() ) )
        {
            auto it = _tracks.begin() + i;
            _tracks.erase( it );
            return true;
        }
        return false;
    };

    bool DawSequence::deleteTrack( const std::string& Name )
    {
        return deleteTrack( getTrack( Name ) );
    }

    bool DawSequence::deleteTrack( const DawTrack* Track )
    {
        int index = 0;
        for( auto& it : _tracks )
        {
            if( it.get() == Track )
            {
                return deleteTrack( index );
            }
            ++index;
        }
        return false;
    }

    bool DawSequence::isTrackExists( const std::string& Name ) const
    {
        return ( getTrack( Name ) ) ? true : false;
    }

    DawTrack* DawSequence::getTrack( const std::string& Name ) const
    {
        for( const auto& it : _tracks )
            if( it->getName() == Name )
                return it.get();
        return nullptr;
    }

    DawTrack* DawSequence::getTrack( uint32_t Index ) const
    {
        if( utils::isLessOrEuqal( Index, (uint32_t)_tracks.size() ) )
        {
            return _tracks[Index].get();
        }
        return nullptr;
    }

    int DawSequence::getTrackIndex( const std::string& Name ) const
    {
        int index = 0;
        for( const auto& it : _tracks )
        {
            if( it->getName() == Name )
                return index;
            ++index;
        }
        return -1;
    }

    const std::vector<std::unique_ptr<DawTrack>>& DawSequence::getTracks() const
    {
        return _tracks;
    }

    std::vector<std::string> DawSequence::getTrackNames() const
    {
        std::vector<std::string> trk;
        for( const auto& it : _tracks )
        {
            trk.push_back( it->getName() );
        }
        return trk;
    }

    const bool DawSequence::getTrackSolo( const uint32_t Trk )
    {
        return getTrackSolo( getTrack( Trk ) );
    }

    const bool DawSequence::getTrackSolo( const std::string& Trk )
    {
        return getTrackSolo( getTrack( Trk ) );
    }

    const bool DawSequence::getTrackSolo( DawTrack* Trk )
    {
        return ( Trk ) ? Trk->isSolo() : false;
    }

    int DawSequence::setTrackSolo( const uint32_t Trk, const bool Solo )
    {
        return setTrackSolo( getTrack( Trk ), Solo );
    }

    int DawSequence::setTrackSolo( const std::string& Trk, const bool Solo )
    {
        return setTrackSolo( getTrack( Trk ), Solo );
    }

    int DawSequence::setTrackSolo( DawTrack* Trk, const bool Solo )
    {
        if( Trk )
        {
            forAllTracks( [Solo, Trk]( DawTrack* T ) {
                if( Solo )
                {
                    T->setMutedDueToSolo( Trk != T );
                    T->setSolo( Trk == T );
                }
                else
                {
                    T->setMutedDueToSolo( false );
                    T->setSolo( false );
                }
            } );
            return 0;
        }
        return -1;
    }

    const bool DawSequence::getTrackMute( const uint32_t Trk )
    {
        return getTrackMute( getTrack( Trk ) );
    }

    const bool DawSequence::getTrackMute( const std::string& Trk )
    {
        return getTrackMute( getTrack( Trk ) );
    }

    const bool DawSequence::getTrackMute( DawTrack* Trk )
    {
        return ( Trk ) ? Trk->isMuted() : false;
    }

    int DawSequence::setTrackMute( const uint32_t Trk, const bool Mute )
    {
        return setMute( getTrack( Trk ), Mute );
    }

    int DawSequence::setTrackMute( const std::string& Trk, const bool Mute )
    {
        return setMute( getTrack( Trk ), Mute );
    }

    int DawSequence::setMute( DawTrack* Trk, const bool Mute )
    {
        if( Trk )
        {
            Trk->setMute( Mute );
            return 0;
        }
        return -1;
    }

    void DawSequence::forAllTracks( std::function<void( DawTrack* )> Func )
    {
        if( Func )
            for( auto& it : _tracks )
                Func( it.get() );
    }

    void DawSequence::play()
    {
        _state = Playing;
    }

    void DawSequence::pause()
    {
        _state = Paused;
    }

    void DawSequence::stop()
    {
        _state = Stopped;
    }

    void DawSequence::resume()
    {
        _state = Playing;
    }

    bool DawSequence::isPlaying() { return Playing == _state; }
    bool DawSequence::isStopped() { return Stopped == _state; }
    bool DawSequence::isPaused() { return Paused == _state; }
};
