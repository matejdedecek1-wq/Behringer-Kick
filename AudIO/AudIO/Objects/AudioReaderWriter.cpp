
#include "AudioReaderWriter.h"

namespace AudIO
{
    AudioSampler::AudioSampler()
        : Object( 0, 2, 1 )
    {
        env.setBypass( true );
        flt.setBypass( true );
        pitch.setBypass( true );
    }

    AudioSampler::~AudioSampler() {}

    void AudioSampler::load( const std::string& File )
    {
        layer.emplace_back( std::make_unique<AudioFile<float>>() );
        layer.back()->load( File );
        readIndex = 0;
    }

    void AudioSampler::prepareToPlay( uint32_t samplerate, uint32_t blocksize )
    {
        Object::prepareToPlay( samplerate, blocksize );
        vp.prepareToPlay( samplerate, blocksize );
        env.prepareToPlay( samplerate, blocksize );
        flt.prepareToPlay( samplerate, blocksize );
        pitch.prepareToPlay( samplerate, blocksize );
    }

    void AudioSampler::process( AudIO::Buffer<float>& buffer )
    {
        if( !m_bypass && m_playing )
        {
            auto ch0 = buffer.getOutputBuffer( 0 );
            auto ch1 = buffer.getOutputBuffer( 1 );

            auto maxSamples = layer[playIndex]->getNumSamplesPerChannel();
            if( readIndex > maxSamples )
            {
                for( uint32_t sample = 0; sample < blockSize(); ++sample )
                {
                    if( ch0 )
                        ch0[sample] = 0.f;
                    if( ch1 )
                        ch1[sample] = 0.f;
                }
                //stop();
                play( 0 );
            }
            else
            {
                auto p0 = vp.getGain( 0 );
                auto p1 = vp.getGain( 1 );

                for( uint32_t sample = 0; sample < blockSize(); ++sample )
                {
                    auto gEnv   = env.processADSR();
                    auto gFlt   = flt.processADSR();
                    auto gPitch = pitch.processADSR();

                    auto g0 = p0 * gEnv * gFlt * gPitch;
                    auto g1 = p1 * gEnv * gFlt * gPitch;

                    if( readIndex < maxSamples )
                    {
                        if( ch0 )
                            ch0[sample] = ( g0 * layer[playIndex]->samples[0][readIndex] );
                        if( ch1 )
                            ch1[sample] = ( g1 * layer[playIndex]->samples[1][readIndex] );
                    }
                    ++readIndex;
                }

                //smb::smbPitchShift( 1.0, blockSize, _blockSize, 32, sampleRate, ch0->getWritePointer(), ch0->getWritePointer() );
                //smb::smbPitchShift( 1.0, blockSize, _blockSize, 32, sampleRate, ch1->getWritePointer(), ch1->getWritePointer() );
            }
        }
    }

    void AudioSampler::reset()
    {
        readIndex = 0;
        env.reset();
        flt.reset();
        pitch.reset();
    }

    void AudioSampler::play( uint32_t velocity )
    {
        velocity;
        switch( mode )
        {
            break;
            case CYCLIC:
                playIndex = ( playIndex < ( layer.size() - 1 ) ) ? ( playIndex + 1 ) : 0;
                break;
            case RANDOM:
                playIndex = utils::random( 0, (int)layer.size() );
                break;
            case VELOCITY: playIndex = 0;
        }
        playIndex = utils::clamp( playIndex, 0, (int)( layer.size() - 1 ) );
        readIndex = 0;
        m_playing = true;
    }

    void AudioSampler::stop()
    {
        m_playing = false;
    }
};
