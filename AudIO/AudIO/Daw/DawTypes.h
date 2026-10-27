#pragma once

#include <string>
#include "DawTime.h"

namespace AudIO
{

    enum class DeviceType
    {
        In,
        Out
    };

    enum class BusWidth
    {
        Mono_L,
        Mono_R,
        Stereo,
    };

    enum class BusType
    {
        In,
        Out,
        SendReturn,
        Submix,
        Aux,
        Object,
        Other,
    };

    struct sequenceData
    {
        std::string Name{ "" };
        int         Bars{ 0 };
        int         BPM{ 0 };
        double      Start{ 0 };
        double      End{ 0 };
        bool        Transpose{ false };
        bool        isLooping{ false };
        bool        isMasterLength{ false };
        bool        isMasterBPM{ false };
        float       gain{ 1.f };
        float       pan{ 0.f };
    };

};
