#pragma once

#include <string>
#include <memory>

namespace AudIO
{
    class DawTrack
    {
    public:
        explicit DawTrack( const std::string& Name = "" );
        virtual ~DawTrack();

        void               setName( const std::string& Name );
        const std::string& getName() const;

        bool isMuted() const;
        bool isSolo() const;

        void setMute( const bool Mute );
        void setSolo( const bool Bypass );

        void setMutedDueToSolo( const bool Mute );

    private:
        std::string _name;
        bool        _solo{ false }, _mute{ false }, _mutedDueToSolo{ false };
    };
};
