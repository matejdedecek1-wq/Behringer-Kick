#include "mt_sample_editor_actions.h"
#include "mt_sample_player.h"

namespace mt
{
#define CLOSE_TO_ZERO 0.000128508

    class MTSampleEditorInterface::MTSampleEditorData : public juce::ChangeListener
    {
    public:
        MTSampleEditorData()
        {
            formatManager.registerBasicFormats();
            //transportSource.addChangeListener( this );
            undoManager.addChangeListener( this );
            MTSampleEditUndoAction::setUndoActions( &undoActions );
            //move to AppHome and set global undo size limits
            //undoManager.setMaxNumberOfStoredUnits(75000, 1);
        }

        ~MTSampleEditorData()
        {
            undoManager.removeChangeListener( this );
            MTSampleEditUndoAction::cleanup();
        }

        juce::AudioThumbnailBase* getThumbnail()
        {
            return &thumbnail;
        }

        void addListener( MTSamplePlayerListener* listener )
        {
            listeners.push_back( listener );
        }

        void removeListener( MTSamplePlayerListener* listener )
        {
            auto it = std::find( listeners.begin(), listeners.end(), listener );
            if( it != listeners.end() )
                listeners.erase( it );
        }

        void addListener( MTSampleModifiedListener* listener )
        {
            smpModifiedlisteners.push_back( listener );
        }

        void removeListener( MTSampleModifiedListener* listener )
        {
            auto it = std::find( smpModifiedlisteners.begin(), smpModifiedlisteners.end(), listener );
            if( it != smpModifiedlisteners.end() )
                smpModifiedlisteners.erase( it );
        }

        void changeListenerCallback( juce::ChangeBroadcaster* source )
        {
            if( !transportSource )
                return;

            if( source == transportSource.get() )
            {
                juce::int64 playHeadPos = transportSource->getNextReadPosition();
                for( MTSamplePlayerListener* listner : listeners )
                {
                    if( transportSource->isPlaying() )
                        listner->positionChanged( playHeadPos );
                    else
                        listner->stopped();
                }
            }
            if( source == &undoManager )
            {
                setSource( sourceFile );
                for( MTSamplePlayerListener* listner : listeners )
                {
                    if( sourceFile.getLastModificationTime() != sourceModifiedTime )
                    {
                        sourceModifiedTime = sourceFile.getLastModificationTime();
                        listner->sampleModified();
                    }
                }
                //for (MTSampleModifiedListener* listner : smpModifiedlisteners)
                //    listner->sampleModified();
            }
        }
        int getSampleSign( float sample ) const
        {
            if( sample < 0 )
                return -1;
            else if( sample > 0 )
                return 1;
            else
                return 0;
        }

        bool getNextZeroSanp( juce::int64& zeroSnapValue, juce::int64 currentSample, int copySize, std::vector<int> currentSampleSign, std::unique_ptr<juce::AudioFormatReader>& zeroSnapReader, bool reverseDirection = false )
        {
            juce::AudioSampleBuffer buffer( (int)zeroSnapReader->numChannels, copySize );
            zeroSnapReader->read( &buffer, 0, copySize, currentSample, true, true );
            if( reverseDirection )
                buffer.reverse( 0, copySize );

            for( int i = 0; i < copySize; i++ )
            {
                for( int j = 0; j < static_cast<int>( zeroSnapReader->numChannels ); j++ )
                {
                    if( currentSampleSign[(unsigned int)j] != getSampleSign( buffer.getSample( j, i ) ) )
                    {
                        zeroSnapValue = reverseDirection ? currentSample + ( copySize - i ) : currentSample + i;
                        return true;
                    }
                }
            }
            return false;
        }

        juce::int64 getNextZeroSnap( juce::int64 pos, bool forward )
        {
            if( !reader )
                return 0;

            std::unique_ptr<juce::AudioFormatReader> zeroSnapReader( formatManager.createReaderFor( sourceFile ) );
            juce::int64                              startSample = pos;
            if( startSample >= zeroSnapReader->lengthInSamples )
                return pos;

            int         buffSize      = 1024;
            juce::int64 currentSample = startSample + 1;

            // check sign of start sample
            std::vector<int>        currentSampleSign( zeroSnapReader->numChannels );
            juce::AudioSampleBuffer bufferTmp( (int)zeroSnapReader->numChannels, 1 );
            zeroSnapReader->read( &bufferTmp, 0, 1, startSample, true, true );
            for( int i = 0; i < static_cast<int>( zeroSnapReader->numChannels ); i++ )
                currentSampleSign[(unsigned int)i] = getSampleSign( bufferTmp.getSample( i, 0 ) );

            juce::int64 zeroSnapPos = pos;
            if( forward )
            {
                while( currentSample < zeroSnapReader->lengthInSamples )
                {
                    int copySize = ( zeroSnapReader->lengthInSamples - currentSample ) < buffSize ? static_cast<int>( zeroSnapReader->lengthInSamples - currentSample ) : buffSize;
                    if( getNextZeroSanp( zeroSnapPos, currentSample, copySize, currentSampleSign, zeroSnapReader ) )
                        return zeroSnapPos;

                    currentSample += copySize;
                }
            }
            else
            {
                while( currentSample >= 0 )
                {
                    int copySize = ( currentSample < buffSize ) ? static_cast<int>( buffSize - currentSample ) : buffSize;
                    currentSample -= copySize;

                    if( getNextZeroSanp( zeroSnapPos, currentSample, copySize, currentSampleSign, zeroSnapReader, true ) )
                        return zeroSnapPos;
                }
            }

            return zeroSnapPos;
        }

        void play()
        {
            if( transportSource ) transportSource->start();
        }

        void stop()
        {
            if( transportSource ) transportSource->stop();
        }

        bool performAction( const MTSampleRegion& region, MTSampleEditorAction actionType, const std::list<juce::var>& params )
        {
            if( !reader )
                return false;

            MTSampleEditorActionsInterface* actionObject = nullptr;
            switch( actionType )
            {
                case seActionDiscard:
                    actionObject = new MTSampleEditorDiscardAction;
                    break;
                case seActionDelete:
                    actionObject = new MTSampleEditorDeleteAction;
                    break;
                case seActionSilence:
                    actionObject = new MTSampleEditorSilenceAction;
                    break;
                case seActionNormalize:
                    actionObject = new MTSampleEditorNormaliseAction;
                    break;
                case seActionReverse:
                    actionObject = new MTSampleEditorReverseAction;
                    break;
                case seActionFadeIn:
                    actionObject = new MTSampleEditorFadeInAction;
                    break;
                case seActionFadeOut:
                    actionObject = new MTSampleEditorFadeOutAction;
                    break;
                case seActionExtract:
                    actionObject = new MTSampleEditorExtractAction;
                    break;
                case seActionPitchShift:
                    return false;
                case seActionTimeStretch:
                    return false;
                case seActionGainChange:
                    actionObject = new MTSampleEditorGainChangeAction;
                    break;
                case seActionCopy:
                    actionObject = new MTSampleEditorCopyAction;
                    break;
                case seActionBitReduce:
                    return false;
                case seActionStereoToMono:
                    actionObject = new MTSampleEditorStereoToMonoAction;
                    break;
            }

            if( !actionObject )
                return false;

            actionObject->initialize( region, params, sourceFile );
            bool performed = true;
            if( actionObject->canUndo() )
            {
                getAudioInterface().getUndoManager().beginNewTransaction();
                auto action = new MTSampleEditUndoAction( actionObject );
                undoActions.push_back( action );
                performed = getAudioInterface().getUndoManager().perform( action );
            }
            else
            {
                performed = actionObject->execute();
                delete actionObject;
            }

            if( performed )
                return true;

            return false;
        }

        void resetThumbnail()
        {
            thumbnail.clear();
            thumbnailCache.clear();
            if( sourceFile != juce::File{} )
                thumbnail.setSource( new juce::FileInputSource( sourceFile ) );
        }

        std::list<juce::int64> splitByThreshold( int thresholdDb, int minTime, juce::int64 startSample, juce::int64 endSample )
        {
            std::list<juce::int64> valuesInSamples;
            if( !reader )
                return valuesInSamples;

            std::unique_ptr<juce::AudioFormatReader> readerTmp( formatManager.createReaderFor( sourceFile ) );
            if( !readerTmp )
                return valuesInSamples;

            //calculate threshold value
            float lmin, lmax, rmin, rmax;
            readerTmp->readMaxLevels( startSample, endSample - startSample, lmin, lmax, rmin, rmax );
            float avgMax    = readerTmp->numChannels == 2 ? ( -lmin + lmax + rmax - rmin ) / 4 : ( -lmin + lmax ) / 2;
            float threshold = ( avgMax * thresholdDb ) / 100;

            juce::int64            currentSample     = startSample;
            juce::int64            lastThreshValue   = 0;
            double                 prevMaxGain       = -1;
            juce::int64            prevMaxGainSample = 0;
            int                    silenceCount      = 0;
            juce::int64            prevSilenceSample = -1;
            std::list<juce::int64> minValues;

            while( currentSample < endSample )
            {
                int samplesToCopy = 1025;
                if( currentSample + samplesToCopy >= readerTmp->lengthInSamples )
                    samplesToCopy = static_cast<int>( readerTmp->lengthInSamples - currentSample );

                juce::AudioSampleBuffer buffer( (int)readerTmp->numChannels, samplesToCopy );
                readerTmp->read( &buffer, 0, samplesToCopy, currentSample, true, true );
                for( int i = 0; i < samplesToCopy; i++ )
                {
                    //calculate channel average and check if it exceeds threshold
                    float channelAverage = 0.0f;
                    for( int j = 0; j < static_cast<int>( readerTmp->numChannels ); j++ )
                        if( buffer.getSample( j, i ) < 0 )
                            channelAverage += -buffer.getSample( j, i );
                        else
                            channelAverage += buffer.getSample( j, i );

                    channelAverage = channelAverage / readerTmp->numChannels;
                    if( std::abs( channelAverage ) <= CLOSE_TO_ZERO /* channelAverage == 0*/ )
                    {
                        silenceCount++;
                        if( silenceCount > 5 )
                        {
                            prevSilenceSample = currentSample + i;
                        }
                    }
                    else if( channelAverage < threshold && ( threshold / channelAverage ) > prevMaxGain )
                    {
                        prevMaxGain       = threshold / channelAverage;
                        prevMaxGainSample = currentSample + i;

                        if( silenceCount > 5 )
                        {
                            prevSilenceSample = currentSample + i;
                        }
                        silenceCount = 0;
                    }

                    if( channelAverage < threshold )
                        continue;

                    double distance = ( ( ( currentSample + i - lastThreshValue ) * 1.0f ) / readerTmp->sampleRate ) * 1000;
                    if( distance >= minTime )
                    {
                        //valuesInSamples.push_back(currentSample + i);
                        //lastThreshValue = currentSample + i;
                        //DBG(lastThreshValue);
                        //continue;
                        if( prevSilenceSample != -1 )
                            minValues.push_back( prevSilenceSample );
                        else if( prevMaxGainSample != -1 )
                            minValues.push_back( prevMaxGainSample );
                        else
                            continue;

                        if( minValues.empty() )
                            continue;

                        if( std::find( valuesInSamples.begin(), valuesInSamples.end(), minValues.back() ) == valuesInSamples.end() )
                        {
                            valuesInSamples.push_back( minValues.back() );
                            lastThreshValue   = minValues.back();
                            prevMaxGain       = 0;
                            prevMaxGainSample = -1;
                            prevSilenceSample = -1;
                        }

                        if( valuesInSamples.size() == 127 )
                            return valuesInSamples;
                    }
                }

                currentSample += samplesToCopy;
            }

            return valuesInSamples;
        }

        void setSource( juce::File& source )
        {
            if( sourceFile != source )
            {
                // clear undo state and history
                MTSampleEditUndoAction::setClearHistory( true );
                undoActions.clear();
                getAudioInterface().getUndoManager().clearUndoHistory();
                MTSampleEditUndoAction::setClearHistory( false );
                sourceModifiedTime = source.getLastModificationTime();
            }

            sourceFile = source;
            if( source.existsAsFile() )
            {
                reader.reset( formatManager.createReaderFor( source ) );

                if( !transportSource )
                {
                    transportSource.reset( new MTSamplePlayer() );
                    if( transportSource )
                        transportSource->addChangeListener( this );
                }
                transportSource->setSource( sourceFile );
                resetThumbnail();
            }
            else
                reader.reset();
        }

    public:
        juce::AudioFormatManager                 formatManager;
        std::unique_ptr<MTSamplePlayer>          transportSource{ nullptr };
        std::unique_ptr<juce::AudioFormatReader> reader = nullptr;
        juce::File                               sourceFile;
        juce::AudioThumbnailCache                thumbnailCache{ 5 };
        juce::AudioThumbnail                     thumbnail{ 256, formatManager, thumbnailCache };
        //TE::TracktionThumbnail thumbnail{ 256, formatManager, thumbnailCache };
        double                                 numBeats = 0.0;
        double                                 bpm      = 0.0;
        std::vector<MTSamplePlayerListener*>   listeners;
        std::vector<MTSampleModifiedListener*> smpModifiedlisteners;
        juce::UndoManager&                     undoManager{ getAudioInterface().getUndoManager() };
        juce::Time                             sourceModifiedTime;
        std::list<MTSampleEditUndoAction*>     undoActions;

    private:
        JUCE_DECLARE_NON_MOVEABLE( MTSampleEditorData )
    };

    MTSampleEditorInterface::MTSampleEditorInterface()
        : pimpl( new MTSampleEditorData ) {}

    MTSampleEditorInterface::~MTSampleEditorInterface()
    {
        delete pimpl;
    }

    void MTSampleEditorInterface::setSource( juce::File source )
    {
        pimpl->setSource( source );
    }

    double MTSampleEditorInterface::detectBpm()
    {
        if( !pimpl->reader )
            return 0.0;

            //return 0.0;
            //BeatDetect detect;
            //detect.setSensitivity( 0.5 );
            //detect.setSampleRate( pimpl->reader->sampleRate );
#if 1
            //auto blockLength = detect.getBlockSize();
            //auto blockSize = choc::buffer::Size::create( pimpl->reader->numChannels, blockLength );
            //tracktion_engine::SampleCount pos = 0;

            //juce::int64 end = pimpl->reader->lengthInSamples;
            //choc::buffer::ChannelArrayBuffer<float> buffer( blockSize );

            //while( pos + blockLength < end ){
            //    if( !pimpl->reader->read( buffer.getView().data.channels, (int)pimpl->reader->numChannels, pos, (int)blockLength ) )
            //        break;

            //    detect.audioProcess( buffer );
            //    pos += blockLength;
            //}
#else
        int blockSize = detect.getBlockSize();

        juce::int64              numLeft      = pimpl->reader->lengthInSamples;
        juce::int64              startSample  = 0;
        const bool               useRightChan = pimpl->reader->numChannels > 1;
        juce::AudioBuffer<float> buffer( pimpl->reader->numChannels, blockSize );
        while( numLeft > 0 )
        {
            const int numThisTime = (int)juce::jmin( numLeft, (juce::int64)blockSize );
            pimpl->reader->read( &buffer, 0, numThisTime, startSample, true, useRightChan );
            detect.audioProcess( buffer.getArrayOfReadPointers(), pimpl->reader->numChannels );

            startSample += numThisTime;
            numLeft -= numThisTime;
        }

        // calculate beats
        pimpl->numBeats = detect.getNumBeats();
#endif
        //pimpl->numBeats = detect.getBeats().size();
        if( pimpl->numBeats == 0 )
            pimpl->numBeats = 65;

        //calculate bpm
        double lengthInTime = pimpl->reader->lengthInSamples / pimpl->reader->sampleRate;
        pimpl->bpm          = pimpl->numBeats * 60 / lengthInTime;
        return pimpl->bpm;

        /*juce::File source = pimpl->sourceFile;

        if (pimpl->reader == nullptr)
            return 0;

        auto numChannels = (int)pimpl->reader->numChannels;
        auto numSamples = pimpl->reader->lengthInSamples;
        auto sampleRate = pimpl->reader->sampleRate;

        if (numSamples <= 0)
            return 0;

        TE::TempoDetect detector(numChannels, sampleRate);
        const int blockSize = 2000;
        const bool useRightChan = numChannels > 1;

        // can't use an AudioScratchBuffer yet
        AudioBuffer<float> buffer(numChannels, blockSize);

        int64 numLeft = numSamples;
        int64 startSample = 0;

        while (numLeft > 0)
        {
            const int numThisTime = (int)jmin(numLeft, (int64)blockSize);
            pimpl->reader->read(&buffer, 0, numThisTime, startSample, true, useRightChan);
            detector.processSection(buffer, numThisTime);

            startSample += numThisTime;
            numLeft -= numThisTime;
        }

        return detector.finishAndDetect();*/
    }

    double MTSampleEditorInterface::getNumberOfBeats()
    {
        if( pimpl->numBeats == 0 )
            detectBpm();

        return pimpl->numBeats;
    }

    bool MTSampleEditorInterface::performAction( const MTSampleRegion& region, MTSampleEditorAction action, const std::list<juce::var>& params )
    {
        return pimpl->performAction( region, action, params );
    }

    std::list<juce::int64> MTSampleEditorInterface::splitByThreshold( int threshold, int minTime, juce::int64 startSample, juce::int64 endSample )
    {
        return pimpl->splitByThreshold( threshold, minTime, startSample, endSample );
    }

    juce::int64 MTSampleEditorInterface::getLengthInSamples()
    {
        if( pimpl->reader )
            return pimpl->reader->lengthInSamples;

        return 0;
    }

    double MTSampleEditorInterface::getLengthInSec()
    {
        if( pimpl->reader )
            return pimpl->reader->lengthInSamples / pimpl->reader->sampleRate;

        return 0;
    }

    void MTSampleEditorInterface::undo()
    {
        getAudioInterface().getUndoManager().undo();
    }

    void MTSampleEditorInterface::redo()
    {
        getAudioInterface().getUndoManager().redo();
    }

    void MTSampleEditorInterface::addListener( MTSamplePlayerListener* listener )
    {
        pimpl->addListener( listener );
    }

    void MTSampleEditorInterface::removeListener( MTSamplePlayerListener* listener )
    {
        pimpl->removeListener( listener );
    }

    juce::int64 MTSampleEditorInterface::getNextZeroSnapPos( juce::int64 pos, bool forward )
    {
        return pimpl->getNextZeroSnap( pos, forward );
    }

    void MTSampleEditorInterface::play()
    {
        pimpl->play();
    }

    void MTSampleEditorInterface::play( const MTSampleRegion& region, MTSamplePlayMode mode )
    {
        if( pimpl->transportSource ) pimpl->transportSource->start( region, mode );
    }

    void MTSampleEditorInterface::stop()
    {
        pimpl->stop();
    }

    juce::AudioThumbnailBase* MTSampleEditorInterface::getThumbnail()
    {
        return pimpl->getThumbnail();
    }
}
