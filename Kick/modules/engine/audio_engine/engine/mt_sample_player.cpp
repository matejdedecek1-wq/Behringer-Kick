#include "mt_sample_player.h"

#define broadcastMessageLengthInMs 3
#define cuePreviewLengthInMs       30

namespace mt
{
    MTSamplePlayer::MTSamplePlayer()
    {
        formatManager.registerBasicFormats();
        player.setSource( &transportSource );
        transportSource.setSource( this );
        transportSource.addChangeListener( this );
    }

    MTSamplePlayer::~MTSamplePlayer()
    {
        transportSource.stop();
        player.setSource( nullptr );
        devmgr.removeAudioCallback( &player );
    }

    void MTSamplePlayer::setSource( juce::File source )
    {
        pos        = 0;
        sourceFile = source;
        transportSource.stop();
        if( sourceFile != juce::File{} )
        {
            devmgr.removeAudioCallback( &player );
            reader.reset( formatManager.createReaderFor( sourceFile ) );
            devmgr.addAudioCallback( &player );
        }
        else
        {
            devmgr.removeAudioCallback( &player );
            reader.reset();
        }
    }

    void MTSamplePlayer::start()
    {
        region = MTSampleRegion();
        start( region, MTSamplePlayMode::samplePlayFromStart );
    }

    void MTSamplePlayer::stop()
    {
        transportSource.stop();
    }

    bool MTSamplePlayer::isPlaying()
    {
        return transportSource.isPlaying();
    }

    void MTSamplePlayer::start( const MTSampleRegion& r, MTSamplePlayMode pm )
    {
        playingBackwords = false;
        transportSource.stop();

        region      = r;
        playMode    = pm;
        regionStart = region.start;
        regionEnd   = region.end;

        if( region.isValid() )
        {
            if( playMode == MTSamplePlayMode::samplePlayFromStart || playMode == MTSamplePlayMode::samplePlayModeLoopForward
                || playMode == MTSamplePlayMode::samplePlayModeLoopBidirectional )
            {
                pos    = region.start;
                length = region.end;
            }
            else if( playMode == MTSamplePlayMode::samplePlayToStart )
            {
                pos    = 0;
                length = region.start;
            }
            else if( playMode == MTSamplePlayMode::samplePlayToEnd )
            {
                pos    = 0;
                length = region.end;
            }
            else if( playMode == MTSamplePlayMode::samplePlayFromEnd )
            {
                pos    = region.end;
                length = reader->lengthInSamples;
            }
            else if( playMode == MTSamplePlayMode::samplePlayModeLoopBackward )
            {
                playingBackwords = true;
                length           = region.end;
                pos              = length - samplesPerBlock;
            }
            else if( playMode == MTSamplePlayMode::samplePlayModeCue )
            {
                int previewOffSet = static_cast<int>( ( reader->sampleRate * cuePreviewLengthInMs ) / 60 );
                pos               = region.getStart();
                length            = pos + previewOffSet;
                pos               = pos - previewOffSet;
            }
            if( reader )
                length = juce::jmin( length, reader->lengthInSamples );
            else
                length = 0;
        }
        else
        {
            pos    = 0;
            length = reader->lengthInSamples;
        }

        //length = juce::jmin( length, reader->lengthInSamples );

        looping = ( playMode == MTSamplePlayMode::samplePlayModeLoopForward || playMode == MTSamplePlayMode::samplePlayModeLoopBackward || playMode == MTSamplePlayMode::samplePlayModeLoopBidirectional );

        transportSource.start();
    }

    juce::int64 MTSamplePlayer::getNextReadPosition() const
    {
        if( pos == length )
            return length + 2;
        return pos;
    }

    juce::int64 MTSamplePlayer::getTotalLength() const
    {
        return length;
    }

    bool MTSamplePlayer::isLooping() const
    {
        return looping;
    }

    void MTSamplePlayer::prepareToPlay( int samplesPerBlockExpected, double /*sr*/ )
    {
        if( reader )
        {
            samplesPerBlock = samplesPerBlockExpected;
            reader.reset( formatManager.createReaderFor( sourceFile ) );
            broadcastElapsedSamples = static_cast<int>( ( reader->sampleRate * broadcastMessageLengthInMs ) / 60 );
        }
    }

    void MTSamplePlayer::releaseResources() {}

    void MTSamplePlayer::getNextAudioBlock( const juce::AudioSourceChannelInfo& bufferToFill )
    {
        bufferToFill.clearActiveBufferRegion();
        int sampleCopySize = 0;
        if( playingBackwords )
        {
            sampleCopySize = ( pos - regionStart < bufferToFill.numSamples ) ? static_cast<int>( pos - regionStart ) : bufferToFill.numSamples;
            reader->read( bufferToFill.buffer, 0, sampleCopySize, pos, true, true );
            bufferToFill.buffer->reverse( 0, sampleCopySize );
            pos -= sampleCopySize;
        }
        else
        {
            sampleCopySize = ( length - pos < bufferToFill.numSamples ) ? static_cast<int>( length - pos ) : bufferToFill.numSamples;
            reader->read( bufferToFill.buffer, 0, sampleCopySize, pos, true, true );
            pos += sampleCopySize;
        }

        // send playheade position message
        numSamplesElapsed += sampleCopySize;
        if( pos == length || numSamplesElapsed >= broadcastElapsedSamples )
        {
            numSamplesElapsed = 0;
            sendChangeMessage();
        }

        // reset pos if playmode is loop
        if( looping && ( pos >= regionEnd || pos <= regionStart ) )
        {
            if( playMode == MTSamplePlayMode::samplePlayModeLoopForward )
            {
                pos = regionStart;
            }
            else if( playMode == MTSamplePlayMode::samplePlayModeLoopBackward )
            {
                pos = regionEnd;
            }
            else if( playMode == MTSamplePlayMode::samplePlayModeLoopBidirectional )
            {
                if( pos <= regionStart )
                {
                    playingBackwords = false;
                }
                else
                {
                    pos              = length - samplesPerBlock;
                    playingBackwords = true;
                }
            }
        }
    }

    double MTSamplePlayer::getSampleRate()
    {
        if( reader )
            return reader->sampleRate;

        return 0.0;
    }

    void MTSamplePlayer::changeListenerCallback( ChangeBroadcaster* source )
    {
        // send playback stopped message
        if( source == &transportSource )
            sendChangeMessage();
    }
}
