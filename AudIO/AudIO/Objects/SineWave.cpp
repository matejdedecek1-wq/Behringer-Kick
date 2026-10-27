#include "SineWave.h"

namespace AudIO
{
    void SineWave::setParameter( int index, float value )
    {
        switch( index )
        {
            case SineWave::kParamBypass:
                m_lfo.setBypass( value >= 1.0 );
                break;
            case SineWave::kParamFreq:
                m_lfo.setFrequency( value );
                break;
            case SineWave::kParamAmplitude:
                m_amplitude = utils::logToLinear( value );
                break;
            case SineWave::kParamPhase:
                m_lfo.setPhase( value );
                break;
            default:
                break; // unknown parameter
        }
    }

    float SineWave::getParameter( int index )
    {
        switch( index )
        {
            case SineWave::kParamBypass:
                return m_lfo.isBypassed() ? 1.f : 0.f;
            case SineWave::kParamFreq:
                return m_lfo.getFrequency();
            case SineWave::kParamAmplitude:
                return utils::linearTolog( m_amplitude.value() );
            case SineWave::kParamPhase:
                return m_lfo.getPhase();
            default:
                break; // unknown parameter
        }
        return 0.0;
    }

    float SineWave::getMin( uint32_t index ) const
    {
        switch( index )
        {
            case SineWave::kParamBypass:
                return 0.0;
            case SineWave::kParamFreq:
                return 20;
            case SineWave::kParamAmplitude:
                return -90.0;
            case SineWave::kParamPhase:
                return 0.0;
            default:
                break; // unknown parameter
        }
        return 0.0;
    }

    float SineWave::getMax( uint32_t index ) const
    {
        switch( index )
        {
            case SineWave::kParamBypass:
                return 1.0;
            case SineWave::kParamFreq:
                return 20000;
            case SineWave::kParamAmplitude:
                return 0.0;
            case SineWave::kParamPhase:
                return 1.0;
            default:
                break; // unknown parameter
        }
        return 0.0;
    }

    const char* SineWave::getType() const
    {
        return "Sine";
    }

    void SineWave::getParameterLabel( int index, char* text )
    {
        if( text )
        {
            switch( index )
            {
                case SineWave::kParamFreq:
                    strcpy( text, "Hz" );
                    break;
                case SineWave::kParamAmplitude:
                    strcpy( text, "dB" );
                    break;
                case SineWave::kParamPhase:
                    strcpy( text, "Phase" );
                    break;
                default:
                    break;
            }
            strcpy( text, utils::emptyString );
        }
    }

    void SineWave::getParameterName( int index, char* text )
    {
        if( text )
        {
            switch( index )
            {
                case SineWave::kParamBypass:
                    strcpy( text, "Bypass" );
                    break;
                case SineWave::kParamFreq:
                    strcpy( text, "Freq" );
                    break;
                case SineWave::kParamAmplitude:
                    strcpy( text, "Level" );
                    break;
                case SineWave::kParamPhase:
                    strcpy( text, "Phase" );
                    break;
                default:
                    break;
            }
            strcpy( text, utils::emptyString );
        }
    }

    void SineWave::prepareToPlay( uint32_t samplerate, uint32_t blocksize )
    {
        Object::prepareToPlay( samplerate, blocksize );
        m_lfo.setSampleRate( samplerate );
    }

    void SineWave::process( AudIO::Buffer<float>& buffer )
    {
        if( m_lfo.skipProcess() )
            buffer.clearOutputs();

        auto   outs = buffer.getOutputs();
        float* outL = outs[0];
        float* outR = outs[1];

        if( !outL )
            return;

        auto nSamples = buffer.getBlockSize();
        for( auto sample = 0; sample < (int)nSamples; ++sample )
        {
            auto sineOut = m_amplitude.getNext() * m_lfo.process();
            // iterate over all available output channels
            *outL++ = sineOut;
            if( outR )
                *outR++ = sineOut;
        }
    }
}