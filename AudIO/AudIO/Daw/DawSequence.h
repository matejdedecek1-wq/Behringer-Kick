#pragma once

#include "DawEngine.h"
#include "DawTrack.h"
#include "DawTime.h"

namespace AudIO
{
#define MAX_BUS ( 6 )

    class DawSequence
    {
    public:
        enum TransportState
        {
            Idle,
            Playing,
            Paused,
            Stopped,
        };

        struct Playback
        {
            bool isLooping{ true };
            bool isPlayng{ false };
        };

        explicit DawSequence( const std::string& Name = "", void* Data = nullptr );
        virtual ~DawSequence();

        void* getDataArg() { return _argument; }

        const std::string& getName() const;
        void               setName( const std::string& Name );

        DawTrack* createTrack( const std::string& Name );
        bool      deleteTrack( int i );
        bool      deleteTrack( const std::string& Name );
        bool      deleteTrack( const DawTrack* Track );

        bool      isTrackExists( const std::string& Name ) const;
        DawTrack* getTrack( const std::string& Name ) const;
        DawTrack* getTrack( uint32_t Index ) const;
        int       getTrackIndex( const std::string& Name ) const;

        const std::vector<std::unique_ptr<DawTrack>>& getTracks() const;
        std::vector<std::string>                      getTrackNames() const;

        const bool getTrackSolo( const uint32_t Trk );
        const bool getTrackSolo( const std::string& Trk );
        int        setTrackSolo( const uint32_t Trk, const bool Solo );
        int        setTrackSolo( const std::string& Trk, const bool Solo );

        const bool getTrackMute( const uint32_t Trk );
        const bool getTrackMute( const std::string& Trk );
        int        setTrackMute( const uint32_t Trk, const bool Mute );
        int        setTrackMute( const std::string& Trk, const bool Mute );

        void forAllTracks( std::function<void( DawTrack* )> Func );

        /////// Transport related API's
        inline const TransportState getTransportState() const { return _state; };
        bool                        isPlaying();
        bool                        isStopped();
        bool                        isPaused();

        // Do not call these API's directly, instead use DawEngine class for transport functionality.
        void play();
        void pause();
        void stop();
        void resume();

    protected:
        Playback       playbackInfo;
        TransportState _state{ Idle };

    private:
        int               setTrackSolo( DawTrack* Trk, const bool Solo );
        inline const bool getTrackSolo( DawTrack* Trk );

        int               setMute( DawTrack* Trk, const bool Mute );
        inline const bool getTrackMute( DawTrack* Trk );

        std::string                            _name;
        std::vector<std::unique_ptr<DawTrack>> _tracks;
        void*                                  _argument{ nullptr };
    };
};
