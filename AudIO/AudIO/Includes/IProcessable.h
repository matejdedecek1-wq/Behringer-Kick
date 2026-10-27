#pragma once

#include "AudIOBuffer.h"

namespace AudIO
{
    struct IProcessable
    {
        virtual void prepareToPlay( uint32_t Samplerate, uint32_t BlockSize ) = 0;
        virtual void process( Buffer<float>& Buffer )                         = 0;
        virtual void teardown() {};
    };
}
