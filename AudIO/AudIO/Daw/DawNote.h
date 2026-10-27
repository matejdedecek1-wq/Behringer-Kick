#pragma once

#include <set>

namespace AudIO{
    struct Note{
        bool            mute{ false };
        unsigned int    id{ 0 };
        unsigned int    event{ 0 };
        unsigned int    note{ 72 };
        unsigned int    velocity{ 127 };
        unsigned int    color{ 0 };
        double          timeInSeconds{ 0.0 };
        double          lengthInSeconds{ 0.0 };
        double          parameter{ 0.0 };

        bool operator<( const Note& Other ) const{ return timeInSeconds < Other.timeInSeconds; }
    };

    class DawNoteList{
    public:
    private:
        std::multiset<double, Note> list;
    };
};
