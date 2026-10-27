#pragma once

#if !defined(JUCE_VERSION)
#include "../Includes/IAudioDevice.h"
#endif

namespace AudIO
{
    class StereoOut : public AudIO::IAudioDevice
    {
    public:
        enum
        {
            kParamStart = 0,
            kParamMax
        };
        StereoOut();
        void start( uint32_t samplerate, uint32_t blocksize ) override;
        void resume() override;
        void pause() override;
        void stop() override;

    void process(float* data);

    private:
        enum State
        {
            Stopped,
            Playing,
        };
        std::atomic<int> _state{ Stopped };
    };
};
