#pragma once

#include "VolumeAndPan.h"
#include "../3rdparty/AudioFile.h"

namespace AudIO
{

    class AudioPlayback : public Object
    {
    public:
        AudioPlayback();
        ~AudioPlayback() = default;

        enum PlayMode
        {
            FORWARD,
            REVERSE,
        };

        void reset();
        void load( const std::string& file );

        void setVolumeAndPan( float Volume, float Pan );
        void setBypass( bool Bypass ) { m_bypass.store( Bypass ); };
        void setMute( bool Mute ) { m_mute.store( Mute ); };
        void setLooping( bool Loop ) { m_loop.store( Loop ); };

        float getVolume() { return vp.getParameter( VolumeAndPan::kParamGain ); };
        float getPan() { return vp.getParameter( VolumeAndPan::kParamPan ); };

        bool isBypass() const { return m_bypass.load(); };
        bool isMuted() const { return m_mute.load(); };
        bool isPlaying() const { return m_playing.load(); };
        bool isLooping() const { return m_loop.load(); };

        void play( bool FromBegin );
        void stop();

    private:
        void prepareToPlay( uint32_t samplerate, uint32_t blocksize ) override;
        void process( Buffer<float>& buffer ) override;

        std::unique_ptr<AudioFile<float>> m_file;
        std::atomic<bool>                 m_bypass{ false }, m_mute{ false }, m_playing{ false }, m_loop{ false };
        int                               readIndex{ 0 }, maxSamples{ 0 };

        VolumeAndPan vp;
        PlayMode     mode{ FORWARD };
    };

    REGISTER_OBJECT( "AudioPlayback", AudioPlayback );
};
