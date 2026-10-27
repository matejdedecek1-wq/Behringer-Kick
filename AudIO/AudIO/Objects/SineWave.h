#pragma once

#include "../Dsp/lfo.h"
#include "../Includes/AudioObject.h"

namespace AudIO
{
    class SineWave : public Object
    {
    public:
        enum
        {
            kParamBypass = 0,
            kParamFreq,
            kParamAmplitude,
            kParamPhase,
            kParamMax
        };

        SineWave()
            : Object( 0, 2, SineWave::kParamMax )
        {
            m_lfo.setType( LFOType::SINE );
            m_lfo.setEnabled( true );
            m_lfo.setFrequency( 1000 );
        }
        virtual void prepareToPlay( uint32_t samplerate, uint32_t blocksize );
        void         setParameter( int index, float value ) override;
        float        getParameter( int index ) override;
        const char*  getType() const override;
        void         getParameterLabel( int index, char* text ) override;
        void         getParameterName( int index, char* text ) override;
        int          getNumParameters() override { return SineWave::kParamMax; }
        float        getMin( uint32_t index ) const override;
        float        getMax( uint32_t index ) const override;

    protected:
        virtual void process( Buffer<float>& buffer );

    private:
        RampedParameter<float> m_amplitude{ 1.0 };
        LFO                    m_lfo;
    };

    REGISTER_OBJECT( "SineWave", SineWave );
};
