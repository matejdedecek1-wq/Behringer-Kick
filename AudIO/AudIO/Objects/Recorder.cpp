
#include "Recorder.h"

namespace AudIO
{
    AudioRecorder::AudioRecorder()
        : Object( 2, 2, 0 )
    {
        m_file = std::make_unique<AudioFile<float>>();
        Object::setName( "AudioRecorder" );
    }

    AudioRecorder::~AudioRecorder()
    {
        m_preview.store( false );
        stop();
        m_file.reset();
    }

    void AudioRecorder::setVolumeAndPan( float Volume, float Pan )
    {
        setPan( Pan );
        setVolume( Volume );
    }

    void AudioRecorder::setPan( float Pan )
    {
        vp.setParameter( VolumeAndPan::kParamPan, Pan );
    }

    void AudioRecorder::setVolume( float Gain )
    {
        vp.setParameter( VolumeAndPan::kParamGain, Gain );
    }

    void AudioRecorder::setVolumeDB( float GainDB )
    {
        float gain = utils::logToLinear( GainDB );
        setVolume( gain );
    }

    void AudioRecorder::setThresholdDB( float DB )
    {
        auto g = utils::logToLinear( DB );
        setThreshold( g );
    }

    void AudioRecorder::reset()
    {
        m_preview.store( false );
        stop();
        m_file = std::make_unique<AudioFile<float>>();
        setLength( 0 );
        resetIndex();
    }

    void AudioRecorder::reInitialize()
    {
        setVolumeAndPan( 1.f, 0.f );
        setThreshold( 0.2f );
        m_bypass    = false;
        m_mute      = false;
        m_recording = false;
        m_loop      = false;
        m_overdub   = false;
        m_monitor   = false;
        dir         = FORWARD;
        mode_       = LR_STEREO;
    }

    void AudioRecorder::setLength( double timeInSeconds )
    {
        recorderLength = timeInSeconds;
        if( onRecorderLengthChanged )
            onRecorderLengthChanged( recorderLength );
    }

    void AudioRecorder::start( Mode RecMode, Direction Dir, bool FromBegin )
    {
        dir   = Dir;
        mode_ = RecMode;

        m_file->setSampleRate( sampleRate() );
        m_file->setNumSamplesPerChannel( (unsigned int)( recorderLength * sampleRate() ) );
        m_file->setNumChannels( L_STEREO <= mode_ ? 2 : 1 );
        maxSamples_ = m_file->getNumSamplesPerChannel();

        if( FromBegin )
        {
            resetIndex();
        }

        m_thresholdCrossed.store( false );
        m_preview.store( false );
        m_recording.store( true );

        if( onRecorderStarted )
            onRecorderStarted();
    }

    void AudioRecorder::stop()
    {
        m_recording.store( false );
        if( FORWARD == dir )
        {
            m_file->setNumSamplesPerChannel( writeIndex_ );
            maxSamples_ = m_file->getNumSamplesPerChannel();
            if( onRecorderLengthChanged )
                onRecorderLengthChanged( ( (double)( maxSamples_ ) / (double)sampleRate() ) );
        }
        if( onRecorderStopped )
            onRecorderStopped();
    }

    void AudioRecorder::pause()
    {
        m_recording.store( false );
        if( onRecorderPaused )
            onRecorderPaused();
    }

    void AudioRecorder::resume()
    {
        m_recording.store( true );
        if( onRecorderResumed )
            onRecorderResumed();
    }

    void AudioRecorder::startPreview( double fromStartTimeInSeconds )
    {
        readIndex_ = utils::max<unsigned int>( 0, (unsigned int)( fromStartTimeInSeconds * sampleRate() ) );
        readIndex_ = utils::min<unsigned int>( ( maxSamples_ - 1 ), readIndex_ );
        m_preview.store( true );
        if( onRecorderPreviewStarted )
            onRecorderPreviewStarted();
    }

    void AudioRecorder::stopPreview()
    {
        m_preview.store( false );
        if( onRecorderPreviewStopped )
            onRecorderPreviewStopped();
    }

    void AudioRecorder::save( const std::string& Path )
    {
        stop(); // Ensure, we have stopped recording.
        m_preview.store( false ); // Also, stop playback.
        m_file->save( Path );
    }

    void AudioRecorder::prepareToPlay( uint32_t samplerate, uint32_t blocksize )
    {
        Object::prepareToPlay( samplerate, blocksize );
        vp.prepareToPlay( samplerate, blocksize );
    }

    void AudioRecorder::resetIndex()
    {
        readIndex_.store( 0 );
        progress = 0.0;
        if( FORWARD == dir )
            writeIndex_ = 0;
        else
            writeIndex_ = utils::max<unsigned int>( 0, maxSamples_ - 1 );
    }

    float AudioRecorder::readProgress()
    {
        if( FORWARD == dir )
            progress = ( (double)writeIndex_.load() / (double)maxSamples_.load() );
        else
            progress = 1.0 - ( (double)writeIndex_.load() / (double)maxSamples_.load() );
        return progress;
    }

    const std::vector<std::vector<float>>& AudioRecorder::getRecorderData()
    {
        return m_file->samples;
    }

    void AudioRecorder::advanceIndex()
    {
        if( FORWARD == dir )
        {
            ++writeIndex_;
            if( writeIndex_ >= maxSamples_ )
            {
                if( m_loop.load() )
                    resetIndex();
                else
                    stop();
            }
        }
        else
        {
            --writeIndex_;
            if( ( writeIndex_ == 0 ) || ( writeIndex_ == (unsigned int)( -1 ) ) )
            {
                if( m_loop.load() )
                    resetIndex();
                else
                    stop();
            }
        }
    }

    unsigned int AudioRecorder::availableSpace() const
    {
        return ( FORWARD == dir ) ? ( maxSamples_ - writeIndex_.load() ) : writeIndex_.load();
    }

    void AudioRecorder::process( AudIO::Buffer<float>& buffer )
    {
        if( m_bypass.load() )
            return;

        if( m_monitor.load() )
        {
            switch( mode_ )
            {
                default:
                case LR_STEREO:
                    buffer.copyInputsToOutputs();
                    break;
                case L_MONO:
                    buffer.copyInputToOutput( 0 );
                    buffer.clearOutput( 1 );
                    break;
                case R_MONO:
                    buffer.clearOutput( 0 );
                    buffer.copyInputToOutput( 1 );
                    break;
                case LR_MONO:
                    buffer.stereoToMono( 1.f, true );
                    break;
                case L_STEREO:
                    buffer.copyInputToOutput( 0, 0, 1.f );
                    buffer.copyInputToOutput( 0, 1, 1.f );
                    break;
                case R_STEREO:
                    buffer.copyInputToOutput( 1, 0, 1.f );
                    buffer.copyInputToOutput( 1, 1, 1.f );
                    break;
            }
        }
        else
            buffer.clearOutputs();

        auto _blkSize = blockSize();

        if( m_threshold.load() < buffer.getInputRMSValue( 0 ) )
            m_thresholdCrossed = true;

        if( m_thresholdCrossed.load() && isRecording() )
        {
            if( m_file && 0 != m_file->getNumSamplesPerChannel() )
            {
                auto g0 = m_mute.load() ? 0.f : vp.getGain( 0 );
                auto g1 = m_mute.load() ? 0.f : vp.getGain( 1 );

                float* out0 = m_file->samples[0].data();
                float* out1 = ( 1 < m_file->getNumChannels() ) ? m_file->samples[1].data() : nullptr;

                if( !isMuted() )
                {
                    auto in0 = buffer.getInputBuffer( 0 );
                    auto in1 = buffer.getInputBuffer( 1 );

                    if( isOverdubbing() )
                    {
                        float temp0{ 0 };
                        switch( mode_ )
                        {
                            case L_MONO:
                            case R_MONO:
                            {
                                // Record in mono mode... In 1 => out 1 or In 2 => out 1
                                auto pIn = ( L_MONO == mode_ ) ? in0 : in1;
                                auto g   = ( R_MONO == mode_ ) ? g0 : g1;
                                for( uint32_t sample = 0; sample < _blkSize; ++sample, advanceIndex() )
                                {
                                    out0[writeIndex_] = ( out0[writeIndex_] + g * pIn[sample] ) * 0.5f;
                                }
                            }
                            break;
                            case LR_MONO:
                            {
                                // Record stereo in downmix mono mode... In 1 + 2 => out 1
                                for( uint32_t sample = 0; sample < _blkSize; ++sample, advanceIndex() )
                                {
                                    temp0             = ( ( in0[sample] * g0 ) + ( in1[sample] * g1 ) );
                                    out0[writeIndex_] = ( out0[writeIndex_] + temp0 ) * 0.3333f; // avg( L + R + Out )
                                }
                            }
                            break;
                            case L_STEREO:
                            case R_STEREO:
                            {
                                // Record mono inout in stereo channel... In 1 => out 1, 2 or In 2 => out 1, 2
                                auto pIn = ( L_MONO == mode_ ) ? in0 : in1;
                                auto g   = ( R_MONO == mode_ ) ? g0 : g1;
                                for( uint32_t sample = 0; sample < _blkSize; ++sample, advanceIndex() )
                                {
                                    out0[writeIndex_] = out1[writeIndex_] = ( out0[writeIndex_] + g * pIn[sample] ) * 0.5f;
                                }
                            }
                            break;
                            default:
                            case LR_STEREO:
                            {
                                // Record in stereo mode... In 1,2 => out 1,2
                                for( uint32_t sample = 0; sample < _blkSize; ++sample, advanceIndex() )
                                {
                                    out0[writeIndex_] = ( out0[writeIndex_] + g0 * in0[sample] ) * 0.5f;
                                    out1[writeIndex_] = ( out1[writeIndex_] + g1 * in1[sample] ) * 0.5f;
                                }
                            }
                            break;
                        }
                    }
                    else
                    {
                        switch( mode_ )
                        {
                            case L_MONO:
                            case R_MONO:
                            {
                                // Record in mono mode... In 1 => out 1 or In 2 => out 1
                                auto pIn = ( L_MONO == mode_ ) ? in0 : in1;
                                auto g   = ( R_MONO == mode_ ) ? g0 : g1;
                                for( uint32_t sample = 0; sample < _blkSize; ++sample, advanceIndex() )
                                {
                                    out0[writeIndex_] = g * pIn[sample];
                                }
                            }
                            break;
                            case LR_MONO:
                            {
                                // Record stereo in downmix mono mode... In 1 + 2 => out 1
                                for( uint32_t sample = 0; sample < _blkSize; ++sample, advanceIndex() )
                                {
                                    out0[writeIndex_] = ( ( in0[sample] * g0 ) + ( in1[sample] * g1 ) ) * 0.5f; // avg( L + R )
                                }
                            }
                            break;
                            case L_STEREO:
                            case R_STEREO:
                            {
                                // Record in mono mode... In 1 => out 1 or In 2 => out 1
                                auto pIn = ( L_MONO == mode_ ) ? in0 : in1;
                                auto g   = ( R_MONO == mode_ ) ? g0 : g1;
                                for( uint32_t sample = 0; sample < _blkSize; ++sample, advanceIndex() )
                                {
                                    out0[writeIndex_] = out1[writeIndex_] = g * pIn[sample];
                                }
                            }
                            break;
                            default:
                            case LR_STEREO:
                            {
                                // Record in stereo mode... In 1,2 => out 1,2
                                for( uint32_t sample = 0; sample < _blkSize; ++sample, advanceIndex() )
                                {
                                    out0[writeIndex_] = g0 * in0[sample];
                                    out1[writeIndex_] = g1 * in1[sample];
                                }
                            }
                            break;
                        }
                    }
                }
                else
                {
                    // Record silence...
                    for( uint32_t sample = 0; sample < _blkSize; ++sample, advanceIndex() )
                    {
                        if( out0 )
                            out0[writeIndex_] = 0;
                        if( out1 )
                            out1[writeIndex_] = 0;
                    }
                }
            }
        }
        else if( isPreviewing() )
        {
            if( m_file && 0 != m_file->getNumSamplesPerChannel() )
            {
                auto         out0 = buffer.getOutputBuffer( 0 );
                auto         out1 = buffer.getOutputBuffer( 1 );
                const float* pIn0 = m_file->samples[0].data();
                if( 1 == m_file->getNumChannels() )
                {
                    for( uint32_t sample = 0; sample < _blkSize; ++sample, ++readIndex_ )
                    {
                        if( readIndex_ < maxSamples_ )
                            out0[sample] = pIn0[readIndex_];
                        else
                            break;
                    }
                }
                else
                {
                    const float* pIn1 = m_file->samples[1].data();
                    for( uint32_t sample = 0; sample < _blkSize; ++sample, ++readIndex_ )
                    {
                        if( readIndex_ < maxSamples_ )
                        {
                            out0[sample] = pIn0[readIndex_];
                            out1[sample] = pIn1[readIndex_];
                        }
                        else
                            break;
                    }
                }

                if( readIndex_ >= maxSamples_ )
                {
                    m_preview.store( false );
                    readIndex_ = 0;
                    if( onRecorderPreviewStopped )
                        onRecorderPreviewStopped();
                }
            }
        }
    }
};
