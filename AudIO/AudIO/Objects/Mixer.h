#pragma once

#if !defined( JUCE_VERSION )
    #include "../Includes/AudioObject.h"
#endif

namespace AudIO
{
    class Mixer : public AudIO::Object
    {
    public:
        enum
        {
            kParamInputGain = 0,
            kParamOutputGain,
            kParamPan,
            kParamMax
        };

        Mixer( uint32_t inputs = 8 );
        virtual void  setParameter( int index, float value ) override;
        virtual float getParameter( int index ) override;

    private:
        float                                      pan{ 0.0f };
        AudIO::RampedParameter<float>              oGain{ 1.f };
        std::vector<AudIO::RampedParameter<float>> iGain;

        void process( AudIO::Buffer<float>& buffer ) override;
    };

    REGISTER_OBJECT( "Mixer", Mixer );
};
