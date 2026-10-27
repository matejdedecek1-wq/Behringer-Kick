#pragma once

#include "../Dsp/lfo.h"
#include "../Dsp/adsr.h"
#include "SineWave.h"

namespace AudIO
{
    class SineWaveADSR : public SineWave
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

        SineWaveADSR()
            : SineWave()
        {
            m_adsr.setEnabled( true );
        }

        void keyOn();
        void keyOff();
        void setADSRParams( const AudIO::ADSR<float>::Params& p );

        void prepareToPlay( uint32_t samplerate, uint32_t blocksize ) override;

    private:
        void process( Buffer<float>& buffer ) override;

    private:
        ADSR<float> m_adsr;
    };

    REGISTER_OBJECT( "SineWaveADSR", SineWaveADSR );
};
