#pragma once
#include "mt_audio_interface_types.h"

namespace mt
{

    class MTAudioPlayer
    {
    public:
        MTAudioPlayer();
        ~MTAudioPlayer();
        void play( const juce::String& file, MTSampleRegion region );

    private:
        class MTAudioPlayerData;
        MTAudioPlayerData* pimpl;
    };

}
