
#include "PlayBack.h"

namespace AudIO
{
    AudioPlayback::AudioPlayback()
        : Object( 0, 2, 1 )
    {}

    void AudioPlayback::load( const std::string& File )
    {
        maxSamples = 0;
        readIndex  = 0;
        m_file     = std::make_unique<AudioFile<float>>();
        if( m_file )
        {
            stop();
            m_file->load( File );
            maxSamples = m_file->getNumSamplesPerChannel();
        }
    }

    void AudioPlayback::setVolumeAndPan( float Volume, float Pan )
    {
        vp.setParameter( VolumeAndPan::kParamGain, Volume );
        vp.setParameter( VolumeAndPan::kParamPan, Pan );
    }

    void AudioPlayback::prepareToPlay( uint32_t samplerate, uint32_t blocksize )
    {
        Object::prepareToPlay( samplerate, blocksize );
        vp.prepareToPlay( samplerate, blocksize );
    }

    void AudioPlayback::process( AudIO::Buffer<float>& buffer )
    {
        if( m_playing && m_file )
        {
            if( readIndex > maxSamples )
            {
                if( m_loop.load() )
                    readIndex = 0;
                else
                    m_playing.store( false );
            }

            if( m_mute.load() || m_bypass.load() )
            {
                readIndex += blockSize();
                buffer.clearOutputs();
            }
            else
            {
                auto numChannels = m_file->getNumChannels();
                auto g0          = m_mute.load() ? 0.f : vp.getGain( 0 );
                auto g1          = m_mute.load() ? 0.f : vp.getGain( 1 );

                auto ch0 = buffer.getOutputBuffer( 0 );
                auto ch1 = buffer.getOutputBuffer( 1 );

                for( uint32_t sample = 0; sample < blockSize(); ++sample )
                {
                    if( readIndex < maxSamples )
                    {
                        if( ch0 )
                            ch0[sample] = ( m_file->samples[0][readIndex] * g0 );
                        if( ch1 )
                            ch1[sample] = ( ( 1 < numChannels ) ? m_file->samples[1][readIndex] * g1 : 0.0 );
                    }
                    ++readIndex;
                }
            }
        }
        else
        {
            buffer.clearOutputs();
        }
    }

    void AudioPlayback::reset()
    {
        m_file.reset();
        readIndex = 0;
    }

    void AudioPlayback::play( bool FromBegin )
    {
        if( FromBegin )
            readIndex = 0;

        m_playing.store( true );
    }

    void AudioPlayback::stop()
    {
        m_playing.store( false );
    }
};
