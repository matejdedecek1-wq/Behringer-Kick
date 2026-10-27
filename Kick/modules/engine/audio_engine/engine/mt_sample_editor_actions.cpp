#include "mt_sample_editor_actions.h"
#include <filesystem>
#include <fstream>

namespace mt
{
    ///////////////////////////////////////////
    /// ///////// MTSampleEditUndoAction
    //////////////////////////////////////////
    bool MTSampleEditUndoAction::clearHistory = false;
    int MTSampleEditUndoAction::bitDeapth = 0;
    int MTSampleEditUndoAction::numChannels = 0;
    juce::File MTSampleEditUndoAction::undoFile = juce::File{};
    std::list<MTSampleEditUndoAction*>* MTSampleEditUndoAction::undoActions = nullptr;

    MTSampleEditUndoAction::MTSampleEditUndoAction( MTSampleEditorActionsInterface* action )
    {
        actionImp.reset( action );
        if( !undoFile.exists() )
        {
            undoFile = actionImp->getSourceFile().getSiblingFile( "undo_sampleeditor.wav" );
        }
    }

    MTSampleEditUndoAction::~MTSampleEditUndoAction()
    {
        juce::int64 totalSamplesToRemove = getTotalSizeInSamples();
        if( !clearHistory )
        {
            if( this != *undoActions->begin() )
                jassertfalse;

            undoActions->erase( undoActions->begin() );
            removeOriginalSampleFromBackup();
            for( auto it = undoActions->begin(); it != undoActions->end(); it++ )
                (*it)->reduceSize( totalSamplesToRemove );
        }
    }

    void MTSampleEditUndoAction::removeOriginalSampleFromBackup()
    {
        juce::int64 totalSamplesToRemove = getTotalSizeInSamples();

        juce::String fName = undoFile.getFileNameWithoutExtension() + "_temp" + undoFile.getFileExtension();
        juce::File tempFile = undoFile.getSiblingFile( fName );

        juce::AudioFormatReader* undoReader = actionImp->getFormatManager().createReaderFor( undoFile );
        if( !undoReader )
        {
            jassertfalse;
        }
        juce::WavAudioFormat format;
        juce::FileOutputStream* outStream = new juce::FileOutputStream( tempFile );
        juce::AudioFormatWriter* undoWriter = format.createWriterFor( outStream, undoReader->sampleRate, undoReader->numChannels, (int)undoReader->bitsPerSample, {}, 0 );
        if( !undoWriter )
            jassertfalse;

        undoWriter->writeFromAudioReader( *undoReader, totalSamplesToRemove, undoReader->lengthInSamples - totalSamplesToRemove );
        undoWriter->flush();
        delete undoReader;
        delete undoWriter;

        undoFile.deleteFile();
        tempFile.copyFileTo( undoFile );
    }

    juce::int64 MTSampleEditUndoAction::getTotalSizeInSamples()
    {
        juce::int64 totalSamples = (endOfSample - startOfSample);
        if( actionImp->getProcessType() == MTSampleEditorActionsInterface::processOutofRegion )
            totalSamples += (endOfSample1 - startOfSample1);

        return totalSamples;
    }

    void MTSampleEditUndoAction::backupOriginalSample()
    {
        juce::String fName = undoFile.getFileNameWithoutExtension() + "_temp" + undoFile.getFileExtension();
        juce::File tempFile = undoFile.getSiblingFile( fName );

        bool undoFileExists = false;
        juce::AudioFormatWriter* undoWriter = nullptr;
        juce::AudioFormatReader* undoReader = actionImp->getFormatManager().createReaderFor( undoFile );

        juce::AudioFormatReader* sourceReader = actionImp->getFormatManager().createReaderFor( actionImp->getSourceFile() );
        bitDeapth = (int)sourceReader->bitsPerSample / 8;
        numChannels = (int)sourceReader->numChannels;

        startOfSample = 0;
        if( undoReader )
        {
            undoFileExists = true;
            startOfSample = undoReader->lengthInSamples;
            juce::FileOutputStream* outStream = new juce::FileOutputStream( tempFile );
            juce::WavAudioFormat format;
            undoWriter = format.createWriterFor( outStream, undoReader->sampleRate, undoReader->numChannels, (int)undoReader->bitsPerSample, {}, 0 );
            undoWriter->writeFromAudioReader( *undoReader, 0, undoReader->lengthInSamples );
            undoWriter->flush();
        } else
        {
            juce::WavAudioFormat format;
            juce::FileOutputStream* outStream = new juce::FileOutputStream( undoFile );
            undoWriter = format.createWriterFor( outStream, sourceReader->sampleRate, sourceReader->numChannels, (int)sourceReader->bitsPerSample, {}, 0 );
        }
        delete undoReader;

        // copy modified regions to undo file
        if( actionImp->getProcessType() == MTSampleEditorActionsInterface::processBetweenRegion )
        {
            juce::int64 startSample = actionImp->getRegion().getStart();
            juce::int64 endSample = actionImp->getRegion().getEnd();
            endOfSample = startOfSample + (endSample - startSample);
            undoWriter->writeFromAudioReader( *sourceReader, startSample, endSample - startSample );
            undoWriter->flush();
        } else if( actionImp->getProcessType() == MTSampleEditorActionsInterface::processOutofRegion )
        {
            juce::int64 startSample = 0;
            juce::int64 endSample = actionImp->getRegion().getStart();
            endOfSample = startOfSample + (endSample - startSample);
            undoWriter->writeFromAudioReader( *sourceReader, startSample, endSample - startSample );
            undoWriter->flush();

            startOfSample1 = endOfSample;
            startSample = actionImp->getRegion().getEnd();
            endSample = sourceReader->lengthInSamples;
            endOfSample1 = startOfSample1 + (endSample - startSample);
            undoWriter->writeFromAudioReader( *sourceReader, startSample, endSample - startSample );
            undoWriter->flush();
        } else
        {
            jassertfalse;
        }

        delete undoWriter;
        delete sourceReader;

        if( undoFileExists )
        {
            undoFile.deleteFile();
            tempFile.moveFileTo( undoFile );
        }
    }

    bool MTSampleEditUndoAction::perform()
    {
        if( actionImp->hasState() && (startOfSample > 0 || endOfSample > 0) )
            return false;

        if( actionImp->hasState() )
            backupOriginalSample();

        return actionImp->execute();
    }

    bool MTSampleEditUndoAction::undo()
    {
        if( actionImp->hasState() && (startOfSample < 0 || endOfSample < 0) )
            return false;

        if( actionImp->hasState() )
        {
            MTSampleEditorFromDestFileAction actionRestore( startOfSample, endOfSample, startOfSample1, endOfSample1, actionImp->getTotalSamples(), undoFile );
            actionRestore.setProcessType( actionImp->getProcessType() );
            std::list<juce::var> params;
            actionRestore.initialize( actionImp->getRegion(), params, actionImp->getSourceFile() );
            actionRestore.setProcessType( actionImp->getProcessType() );
            actionRestore.execute();
        } else
        {
            actionImp->execute();
        }

        startOfSample = -1;
        endOfSample = -1;
        startOfSample1 = -1;
        endOfSample1 = -1;
        return true;
    }

    int MTSampleEditUndoAction::getSizeInUnits()
    {
        int sizeOfObj = (int)sizeof( *this );
        if( !actionImp->hasState() )
            return sizeOfObj;

        sizeOfObj += static_cast<int>(bitDeapth * (endOfSample - startOfSample) * numChannels);
        if( actionImp->getProcessType() == MTSampleEditorActionsInterface::processOutofRegion )
        {
            sizeOfObj += static_cast<int>(bitDeapth * (endOfSample1 - startOfSample1));
        }
        return sizeOfObj;
    }

    void MTSampleEditUndoAction::reduceSize( juce::int64 noSamples )
    {
        startOfSample -= noSamples;
        endOfSample -= noSamples;
        if( startOfSample1 != -1 && endOfSample1 != -1 )
        {
            startOfSample1 -= noSamples;
            endOfSample1 -= noSamples;
        }
    }

    void MTSampleEditUndoAction::cleanup()
    {
        undoFile.deleteFile();
    }

    void MTSampleEditUndoAction::setClearHistory( bool /*setClear*/ )
    {
        if( clearHistory )
            undoFile.deleteFile();
    }

    //////////////////////////////////////////////
    /// ///////// MTSampleEditorActionsInterface
    ///////////////////////////////////////////////
    MTSampleEditorActionsInterface::MTSampleEditorActionsInterface()
    {
        formatManager.registerBasicFormats();
    }

    void MTSampleEditorActionsInterface::initializeReaderWriter( juce::String& tempPath )
    {
        currentSample = 0;
        reader = formatManager.createReaderFor( sourceFile );
        startSample = region.getStart();
        endSample = region.getEnd();
        totalSmaples = reader->lengthInSamples;

        juce::String fName = sourceFile.getFileNameWithoutExtension() + "_temp" + sourceFile.getFileExtension();
        juce::File tempFile = sourceFile.getSiblingFile( fName );
        tempPath = tempFile.getFullPathName();

        juce::FileOutputStream* outStream = new juce::FileOutputStream( tempFile );
        juce::WavAudioFormat format;
        writer = format.createWriterFor( outStream, reader->sampleRate, reader->numChannels, (int)reader->bitsPerSample, {}, 0 );
    }

    juce::String MTSampleEditorActionsInterface::checkForDuplicateAndConstructNewPath( const juce::String file )
    {
        //conhstruct copy path
        juce::String fileName = file;
        if( !fileName.endsWith( ".wav" ) )
            fileName += ".wav";

        return sourceFile.getSiblingFile( fileName ).getNonexistentSibling().getFullPathName();
    }

    bool MTSampleEditorActionsInterface::extractFromSample( juce::int64 firstSample, juce::int64 lastSample, const juce::String& destFileName )
    {
        juce::String samplePath = checkForDuplicateAndConstructNewPath( destFileName );
        juce::File copyPath( samplePath );

        // create writer
        juce::WavAudioFormat format;
        juce::FileOutputStream* outStream = new juce::FileOutputStream( copyPath );
        juce::AudioFormatWriter* writerTmp = format.createWriterFor( outStream, reader->sampleRate, reader->numChannels, (int)reader->bitsPerSample, {}, 0 );
        if( !writerTmp )
            jassertfalse;

        writerTmp->writeFromAudioReader( *reader, firstSample, lastSample - firstSample );
        writerTmp->flush();
        delete writerTmp;

        return false;
    }

    void MTSampleEditorActionsInterface::executeprocessBetweenRegion()
    {
        // process from startSample to endSample
        regionProcessing = 1;
        juce::AudioSampleBuffer bufferTmp;
        int copySampleSize = sampleEditorBufferSize;
        writer->writeFromAudioReader( *reader, 0, startSample );

        currentSample = startSample;
        while( performAction( bufferTmp, copySampleSize ) )
        {
            currentSample += copySampleSize;
            if( bufferTmp.getNumSamples() == copySampleSize )
                writer->writeFromAudioSampleBuffer( bufferTmp, 0, copySampleSize );
        }

        writer->writeFromAudioReader( *reader, endSample, (totalSmaples - endSample) );
    }

    int MTSampleEditorActionsInterface::read( juce::AudioSampleBuffer& buffer, juce::int64 /*startS*/, juce::int64 endS, juce::AudioFormatReader* r, juce::int64 current )
    {
        int numSamples = 0;
        if( currentSample >= endS )
            return numSamples;

        if( current + sampleEditorBufferSize >= endS )
        {
            numSamples = static_cast<int>(endS - currentSample);
        } else
        {
            numSamples = sampleEditorBufferSize;
        }

        buffer.setSize( (int)r->numChannels, numSamples );
        r->read( &buffer, 0, numSamples, currentSample, true, true );

        return numSamples;
    }

    void MTSampleEditorActionsInterface::executeprocessOutofRegion()
    {
        // process from 0 to startSample
        regionProcessing = 0;
        juce::AudioSampleBuffer bufferTmp;
        int copySampleSize = sampleEditorBufferSize;
        while( performAction( bufferTmp, copySampleSize ) )
        {
            currentSample += copySampleSize;
            if( bufferTmp.getNumSamples() == copySampleSize )
                writer->writeFromAudioSampleBuffer( bufferTmp, 0, copySampleSize );
        }

        // process from startSample to endSample
        writer->writeFromAudioReader( *reader, startSample, (endSample - startSample) );

        //process from endSample to totalSamples
        currentSample += endSample - startSample;
        regionProcessing = 2;
        while( performAction( bufferTmp, copySampleSize ) )
        {
            currentSample += copySampleSize;
            if( bufferTmp.getNumSamples() == copySampleSize )
                writer->writeFromAudioSampleBuffer( bufferTmp, 0, copySampleSize );
        }
    }

    void MTSampleEditorActionsInterface::cleanupReaderWriter( juce::String& tempPath )
    {
        juce::File tempFile( tempPath );
        delete writer;
        writer = nullptr;
        delete reader;
        reader = nullptr;

        if( !sourceFile.deleteFile() )
        {
            jassertfalse;
        }

        tempFile.moveFileTo( sourceFile );
    }

    void MTSampleEditorActionsInterface::initialize( const MTSampleRegion& r, const std::list<juce::var>& p, juce::File& source )
    {
        region = r;
        params = p;
        sourceFile = source;
    }

    bool MTSampleEditorActionsInterface::execute()
    {
        juce::String tempPath;
        initializeReaderWriter( tempPath );
        if( reader == nullptr || writer == nullptr )
        {
            if( reader )
                delete reader;
            if( writer )
                delete writer;

            return false;
        }

        juce::AudioSampleBuffer bufferTmp;
        int copySampleSize = sampleEditorBufferSize;
        if( type == noProcessing )
        {
            while( performAction( bufferTmp, copySampleSize ) ) {}
            return true;
        }
        if( type == processBetweenRegion )
        {
            executeprocessBetweenRegion();
        } else
        {
            executeprocessOutofRegion();
        }

        cleanupReaderWriter( tempPath );
        return true;
    }

    ///////////////////////////////////////////
    /// ///////// reverse action
    //////////////////////////////////////////
    bool MTSampleEditorReverseAction::performAction( juce::AudioSampleBuffer& buffer, int& numSamples )
    {
        if( currentSample >= endSample )
        {
            reverseCurrentSample = -1;
            return false;
        }
        if( reverseCurrentSample == -1 )
            reverseCurrentSample = endSample;

        numSamples = reverseCurrentSample - sampleEditorBufferSize <= startSample ? static_cast<int>(reverseCurrentSample - startSample) : sampleEditorBufferSize;
        reverseCurrentSample = reverseCurrentSample - sampleEditorBufferSize <= startSample ? startSample : reverseCurrentSample - sampleEditorBufferSize;
        buffer.setSize( (int)reader->numChannels, numSamples );

        reader->read( &buffer, 0, numSamples, reverseCurrentSample, true, true );
        buffer.reverse( 0, numSamples );

        return true;
    }

    ///////////////////////////////////////////
    /// ///////// normalise action
    //////////////////////////////////////////
    void MTSampleEditorNormaliseAction::initialize( const MTSampleRegion& r, const std::list<juce::var>& p, juce::File& source )
    {
        MTSampleEditorActionsInterface::initialize( r, p, source );
        juce::AudioFormatReader* readerTmp = formatManager.createReaderFor( sourceFile );

        startSample = region.getStart();
        endSample = region.getEnd();
        float lmin, lmax, rmin, rmax;
        readerTmp->readMaxLevels( startSample, endSample - startSample, lmin, lmax, rmin, rmax );

        auto maxLevel = juce::jmax( -lmin, lmax, -rmin, rmax );
        gainFactor = CommonUtils::dbToGain( float( 0.0 ) ) / maxLevel;

        delete readerTmp;
    }

    bool MTSampleEditorNormaliseAction::performAction( juce::AudioSampleBuffer& buffer, int& numSamples )
    {
        if( currentSample >= endSample )
            return false;

        numSamples = static_cast<int>(endSample - startSample);
        buffer.setSize( (int)reader->numChannels, numSamples );

        reader->read( &buffer, 0, numSamples, currentSample, true, true );
        buffer.applyGain( gainFactor );

        return true;
    }

    ///////////////////////////////////////////
    /// ///////// fadein action
    //////////////////////////////////////////
    bool MTSampleEditorFadeInAction::performAction( juce::AudioSampleBuffer& buffer, int& numSamples )
    {
        if( currentSample >= endSample )
            return false;

        numSamples = static_cast<int>(endSample - startSample);
        buffer.setSize( (int)reader->numChannels, numSamples );
        reader->read( &buffer, 0, numSamples, currentSample, true, true );

        int inputCurveType = *params.begin();
        //AudioFadeCurve::Type curveType = AudioFadeCurve::linear;
        //if (inputCurveType == 0)
        //    curveType = AudioFadeCurve::linear;
        //else if (inputCurveType == 1)
        //    curveType = AudioFadeCurve::convex;
        //else if (inputCurveType == 2)
        //    curveType = AudioFadeCurve::concave;

        //AudioFadeCurve::applyCrossfadeSection(buffer,
        //    0, numSamples,
        //    curveType,
        //    0.0,
        //    1.0);

        return true;
    }

    ///////////////////////////////////////////
    /// ///////// fadeout action
    //////////////////////////////////////////
    bool MTSampleEditorFadeOutAction::performAction( juce::AudioSampleBuffer& buffer, int& numSamples )
    {
        if( currentSample >= endSample )
            return false;

        numSamples = static_cast<int>(endSample - startSample);
        buffer.setSize( (int)reader->numChannels, numSamples );
        reader->read( &buffer, 0, numSamples, currentSample, true, true );

        int curveTypeInput = *params.begin();
        //AudioFadeCurve::Type curveType = AudioFadeCurve::linear;
        //if( curveTypeInput == 0 )
        //    curveType = AudioFadeCurve::linear;
        //else if( curveTypeInput == 1 )
        //    curveType = AudioFadeCurve::convex;
        //else if( curveTypeInput == 2 )
        //    curveType = AudioFadeCurve::concave;

        //AudioFadeCurve::applyCrossfadeSection( buffer,
        //    0, numSamples,
        //    curveType,
        //    1.0,
        //    0.0 );

        return true;
    }

    /////////////////////////////////////////////////////////
    /// ///////// MTSampleEditorFromDestFileAction
    ////////////////////////////////////////////////////////
    MTSampleEditorFromDestFileAction::MTSampleEditorFromDestFileAction( juce::int64 start, juce::int64 end, juce::int64 start1, juce::int64 end1, juce::int64 ol, juce::File& df )
        : destStartSample( start )
        , destEndSample( end )
        , destStartSample1( start1 )
        , destEndSample1( end1 )
        , originalLength( ol )
        , destFile( df )
    {
    }

    bool MTSampleEditorFromDestFileAction::execute()
    {
        juce::String tempPath;
        initializeReaderWriter( tempPath );

        if( reader == nullptr || writer == nullptr )
        {
            if( reader )
                delete reader;
            if( writer )
                delete writer;

            return false;
        }

        //discard
        if( type == processOutofRegion )
        {
            startSample = 0;
            endSample = reader->lengthInSamples;
            MTSampleEditorActionsInterface::executeprocessOutofRegion();
        } else if( reader->lengthInSamples != originalLength ) //delete
        {
            endSample = startSample;
            executeprocessBetweenRegion();
        } else
        {
            executeprocessBetweenRegion();
        }
        cleanupReaderWriter( tempPath );

        return true;
    }

    void MTSampleEditorFromDestFileAction::deleteRegionFromDest( juce::AudioFormatReader* destReader )
    {
        juce::String fName = destFile.getFileNameWithoutExtension() + "_temp" + destFile.getFileExtension();
        juce::File tempFile = destFile.getSiblingFile( fName );

        juce::FileOutputStream* outStream = new juce::FileOutputStream( tempFile );
        juce::WavAudioFormat format;
        juce::AudioFormatWriter* writerTmp = format.createWriterFor( outStream, destReader->sampleRate, destReader->numChannels, (int)destReader->bitsPerSample, {}, 0 );
        writerTmp->writeFromAudioReader( *destReader, 0, destStartSample );

        delete writerTmp;
        //delete destReader;

        if( !destFile.deleteFile() )
        {
            jassertfalse;
        }

        tempFile.moveFileTo( destFile );
    }

    bool MTSampleEditorFromDestFileAction::performAction( juce::AudioSampleBuffer& buffer, int& numSamples )
    {
        if( regionProcessing == 0 && regionsProcessed == 1 )
            return false;
        if( (regionProcessing == 2 || regionProcessing == 1) && regionsProcessed == 2 )
            return false;

        numSamples = 0;
        juce::AudioFormatReader* destReder = formatManager.createReaderFor( destFile );
        juce::int64 destReadPos = 0;
        if( regionsProcessed == -1 )
        {
            destReadPos = destStartSample;
            numSamples = static_cast<int>(destEndSample - destStartSample);
            if( destEndSample1 == -1 && destStartSample1 == -1 )
            {
                regionsProcessed = 2;
            } else
            {
                regionsProcessed = 1;
            }
        } else if( regionsProcessed == 1 )
        {
            destReadPos = destStartSample1;
            numSamples = static_cast<int>(destEndSample1 - destStartSample1);
            regionsProcessed = 2;
        } else
        {
            jassertfalse;
        }

        if( destReder )
        {
            buffer.setSize( (int)reader->numChannels, numSamples );
            destReder->read( &buffer, 0, numSamples, destReadPos, true, true );

            if( regionsProcessed == 2 )
                deleteRegionFromDest( destReder );

            delete destReder;
        } else
        {
            jassertfalse;
            return false;
        }

        return true;
    }

    ///////////////////////////////////////////
    /// ///////// MTSampleEditorExtractAction
    //////////////////////////////////////////
    void MTSampleEditorExtractAction::initialize( const MTSampleRegion& r, const std::list<juce::var>& p, juce::File& source )
    {
        MTSampleEditorActionsInterface::initialize( r, p, source );
        type = noProcessing;
    }

    bool MTSampleEditorExtractAction::performAction( juce::AudioSampleBuffer& /*buffer*/, int& /*numSamples*/ )
    {
        extractFromSample( startSample, endSample, params.begin()->toString() );
        return false;
    }

    ///////////////////////////////////////////
    /// ///////// MTSampleEditorSilenceAction
    //////////////////////////////////////////

    bool MTSampleEditorSilenceAction::performAction( juce::AudioSampleBuffer& buffer, int& numSamples )
    {
        if( currentSample >= endSample )
            return false;

        numSamples = static_cast<int>(endSample - startSample);
        buffer.setSize( (int)reader->numChannels, numSamples );
        buffer.clear();

        return true;
    }

    void MTSampleEditorDiscardAction::initialize( const MTSampleRegion& r, const std::list<juce::var>& p, juce::File& source )
    {
        MTSampleEditorActionsInterface::initialize( r, p, source );
        type = processOutofRegion;
    }

    ///////////////////////////////////////////
    /// ///////// MTSampleEditorDiscardeAction
    //////////////////////////////////////////
    bool MTSampleEditorDiscardAction::performAction( juce::AudioSampleBuffer& /*buffer*/, int& numSamples )
    {
        if( (currentSample >= startSample && currentSample < endSample) || currentSample >= reader->lengthInSamples )
            return false;

        if( currentSample == 0 )
        {
            numSamples = static_cast<int>(startSample - currentSample);
        } else if( currentSample == endSample )
        {
            numSamples = static_cast<int>(reader->lengthInSamples - currentSample);
        } else
        {
            jassertfalse;
        }

        return true;
    }

    ///////////////////////////////////////////
    /// ///////// MTSampleEditorDiscardeAction
    //////////////////////////////////////////
    bool MTSampleEditorDeleteAction::performAction( juce::AudioSampleBuffer& /*buffer*/, int& numSamples )
    {
        if( currentSample >= endSample )
            return false;

        numSamples = static_cast<int>(endSample - startSample);
        return true;
    }

    ///////////////////////////////////////////
    /// ///////// MTSampleEditorGainChangeAction
    //////////////////////////////////////////

    void MTSampleEditorGainChangeAction::initialize( const MTSampleRegion& r, const std::list<juce::var>& p, juce::File& source )
    {
        region = r;
        params = p;
        sourceFile = source;
        gain = CommonUtils::dbToGain( *p.begin() );
    }

    bool MTSampleEditorGainChangeAction::performAction( juce::AudioSampleBuffer& buffer, int& numSamples )
    {
        if( currentSample >= endSample )
            return false;

        numSamples = read( buffer, startSample, endSample, reader, currentSample );
        buffer.applyGain( gain );

        return true;
    }

    ///////////////////////////////////////////
    /// ///////// MTSampleEditorCopyAction
    //////////////////////////////////////////
    void MTSampleEditorCopyAction::initialize( const MTSampleRegion& r, const std::list<juce::var>& p, juce::File& source )
    {
        region = r;
        params = p;
        sourceFile = source;
        setProcessType( noProcessing );
    }

    bool MTSampleEditorCopyAction::performAction( juce::AudioSampleBuffer& /*buffer*/, int& /*numSamples*/ )
    {
        juce::String samplePath = checkForDuplicateAndConstructNewPath( params.begin()->toString() );
        juce::File copyPath( samplePath );
        sourceFile.copyFileTo( copyPath );
        return false;
    }

    ///////////////////////////////////////////
    /// ///////// MTSampleEditorStereoToMonoAction
    //////////////////////////////////////////
    void MTSampleEditorStereoToMonoAction::initialize( const MTSampleRegion& r, const std::list<juce::var>& p, juce::File& source )
    {
        region = r;
        params = p;
        sourceFile = source;
        setProcessType( noProcessing );
    }

    bool MTSampleEditorStereoToMonoAction::performAction( juce::AudioSampleBuffer& /*buffer*/, int& /*numSamples*/ )
    {
        juce::String samplePath = checkForDuplicateAndConstructNewPath( params.back().toString() );
        juce::File monoFile( samplePath );

        // create writer
        juce::WavAudioFormat format;
        juce::FileOutputStream* outStream = new juce::FileOutputStream( monoFile );
        std::unique_ptr<juce::AudioFormatWriter> writerTmp( format.createWriterFor( outStream, reader->sampleRate, 1, (int)reader->bitsPerSample, {}, 0 ) );
        if( !writerTmp )
            jassertfalse;

        currentSample = 0;
        int copyType = params.front();
        while( currentSample < reader->lengthInSamples )
        {
            int copySize = sampleEditorBufferSize;
            if( currentSample + sampleEditorBufferSize >= reader->lengthInSamples )
                copySize = static_cast<int>(reader->lengthInSamples - currentSample);

            if( copyType == 0 || copyType == 1 )
            {
                juce::AudioSampleBuffer buff( 1, copySize );
                if( copyType == 0 )
                    reader->read( &buff, 0, copySize, currentSample, true, false );
                else
                    reader->read( &buff, 0, copySize, currentSample, false, true );
                writerTmp->writeFromAudioSampleBuffer( buff, 0, copySize );
            } else
            {
                juce::AudioSampleBuffer buff1( 1, copySize );
                juce::AudioSampleBuffer buff2( 1, copySize );
                reader->read( &buff1, 0, copySize, currentSample, true, false );
                reader->read( &buff2, 0, copySize, currentSample, false, true );
                buff1.addFrom( 0, 0, buff2, 0, 0, copySize );

                writerTmp->writeFromAudioSampleBuffer( buff1, 0, copySize );
            }

            currentSample += copySize;
        }
        writerTmp->flush();

        return false;
    }
}
