#pragma once

#if !defined(JUCE_VERSION)
#include "../3rdparty/AudioFile.h"
#include "VolumeAndPan.h"
#include "ADSR.h"
#endif

namespace AudIO
{
    class AudioSampler : public Object
    {
    public:
        AudioSampler();
        ~AudioSampler();

        enum PlayMode
        {
            CYCLIC,
            VELOCITY,
            RANDOM
        };

        void load( const std::string& file );

        void prepareToPlay( uint32_t samplerate, uint32_t blocksize ) override;

        void play( uint32_t velocity );
        void stop();

        void reset();

    private:
        void process( Buffer<float>& buffer ) override;

        std::vector<std::unique_ptr<AudioFile<float>>> layer;
        bool m_bypass{ false }, m_playing{ true };
        int readIndex{ 0 };
        int playIndex{ 0 };

        VolumeAndPan vp;
        ADSRObj env, flt, pitch;
        PlayMode mode{ RANDOM };
    };

    REGISTER_OBJECT( "AudioSampler", AudioSampler );
};
