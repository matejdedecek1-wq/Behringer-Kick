#include "SineWave_adsr.h"

namespace AudIO
{

    void SineWaveADSR::prepareToPlay( uint32_t samplerate, uint32_t blocksize )
    {
        SineWave::prepareToPlay( samplerate, blocksize );
        m_adsr.setSampleRate( (double)samplerate );
    }

    void SineWaveADSR::keyOn() { m_adsr.keyOn(); }
    void SineWaveADSR::keyOff() { m_adsr.keyOff(); }
    void SineWaveADSR::setADSRParams( const AudIO::ADSR<float>::Params& p ) { m_adsr.setParameters( p ); };

    void SineWaveADSR::process( AudIO::Buffer<float>& buffer )
    {
        SineWave::process( buffer );
        m_adsr.process( buffer );
    }
}