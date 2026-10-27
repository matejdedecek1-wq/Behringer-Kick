#pragma once

#include "../Includes/AudioObject.h"

namespace AudIO
{
    class VolumeAndPan : public Object
    {
    public:
        enum
        {
            kParamGain,
            kParamPan,
            kParamMax
        };

        VolumeAndPan( uint32_t inputs = 2 );
        virtual void  setParameter( int index, float value ) override;
        virtual float getParameter( int index ) override;

        inline float getGain( uint32_t channel );

    private:
        RampedParameter<float> pan{ 0.0f };
        RampedParameter<float> gain{ 1.f };

        void process( Buffer<float>& buffer ) override;
    };

    REGISTER_OBJECT( "VolumeAndPan", VolumeAndPan );
};
