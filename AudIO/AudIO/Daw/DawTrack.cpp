#include "DawTrack.h"

namespace AudIO
{
    DawTrack::DawTrack( const std::string& Name )
    {
        setName( Name );
    }

    DawTrack::~DawTrack()
    {
    }

    void DawTrack::setName( const std::string& Name )
    {
        _name = Name;
    }
    const std::string& DawTrack::getName() const { return _name; }

    // Track Parameters
    bool DawTrack::isMuted() const { return _mute; }
    bool DawTrack::isSolo() const { return _solo; }

    void DawTrack::setMute( const bool Mute )
    {
        _mute = Mute;
    }
    void DawTrack::setSolo( const bool Solo )
    {
        _solo = Solo;
    }
    void DawTrack::setMutedDueToSolo( const bool Mute )
    {
        _mutedDueToSolo = Mute;
    }
};
