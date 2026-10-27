namespace kick_ui
{
    AudioGridCutTool::AudioGridCutTool( AudioGridComponent& GridComp, AudioGridData& GridData, juce::Viewport& ViewPort )
        : IAudioTools( GridComp, GridData, ViewPort, AudioGridToolType::GridCutTool )
    {
    }

    void AudioGridCutTool::paint( juce::Graphics& G )
    {
        if( x1 != x2 && x1 >= 0 && x2 > 0 )
        {
            juce::Rectangle<float> selRect( x1, 0, x2 - x1, gridComp.getHeight() );
            juce::Colour           transparentRed = juce::Colour::fromRGBA( 128, 128, 128, 64 );
            G.setColour( transparentRed );
            G.fillRect( selRect );
            G.setColour( juce::Colours::lightgrey );
            G.drawRect( selRect, 1.5f );
        }
    }

    void AudioGridCutTool::mouseUp( const juce::MouseEvent& event )
    {
        if( event.mouseWasClicked() )
        {
            // Need to check the sample present at mouseposition
            if( auto pAudioProgram = dynamic_cast<mt::MTAudioProgram*>( ae.getProgram( ae.getSelectedProgram() ) ) )
            {
                if( auto sample = pAudioProgram->getAudioSample( mDownTime ) )
                {
                    auto source    = sample->source.get();
                    auto pSample   = mt::getAudioSoundCache().exists( source );
                    auto startTime = sample->startTime.get();
                    auto length    = sample->length.get();
                    auto loopStart = sample->start.get();
                    auto maxTicks  = gridData.getTicksPerQuantization( 0 ); //default bar
                    auto loopEnd   = sample->end.get() == 0 ? pSample->getNumSamplesPerChannel() : sample->end.get();

                    if( length > maxTicks )
                    {
                        auto   pSeqData       = ae.getSequenceData( ae.getSelectedSequence() );
                        auto   tempo          = pSeqData ? pSeqData->tempo.get() : 120;
                        double ticksPerSecond = gridData.getTicksPerBeat( 0 ) / ( BPM::TOSEC / tempo );
                        double samplesPerTick = sample->src->getSourceSampleRate() / ticksPerSecond;

                        auto lengthForNewSample = length + startTime - mDownTime;
                        auto prevSampleLength   = length - lengthForNewSample;
                        if( lengthForNewSample < maxTicks )
                        {
                            lengthForNewSample = maxTicks;
                            prevSampleLength   = length - lengthForNewSample;
                        }
                        else if( prevSampleLength < maxTicks && length > maxTicks * 2 )
                        {
                            prevSampleLength   = maxTicks;
                            lengthForNewSample = length - prevSampleLength;
                        }
                        auto startTimeForNewSample = startTime + prevSampleLength;
                        auto prevSampleLoopEnd     = loopStart + prevSampleLength * samplesPerTick;

                        // update prevsample data and add newsample
                        pAudioProgram->addSample( source, startTimeForNewSample, lengthForNewSample, 0 );
                        pAudioProgram->setLoopPointsForSample( startTimeForNewSample, prevSampleLoopEnd, loopEnd );
                        pAudioProgram->setLoopPointsForSample( startTime, loopStart, prevSampleLoopEnd );
                        pAudioProgram->setSampleLength( startTime, prevSampleLength );
                        pAudioProgram->selectAllSamples( false );
                        pAudioProgram->setSelectAudioSample( startTimeForNewSample, true );

                        sampleNeedRepaint();
                        return;
                    }
                    pAudioProgram->selectAllSamples( false );
                    pAudioProgram->setSelectAudioSample( startTime, true );
                }
                else
                    pAudioProgram->selectAllSamples( false );
                sampleNeedRepaint();
            }
        }
        x1 = x2 = 0;
        gridComp.repaint();
    }

    void AudioGridCutTool::mouseDrag( const juce::MouseEvent& Event )
    {
        auto startTime = gridData.getTimeFromX( Event.position.x );
        if( auto pProg = dynamic_cast<mt::MTAudioProgram*>( ae.getProgram( ae.getSelectedProgram() ) ) )
        {
            pProg->selectAudioSample( startTime );
            sampleNeedRepaint();
            if( auto pSample = pProg->getAudioSample( mDownTime ) )
                return;
        }
        auto currX  = gridData.getXFromTime( startTime );
        auto mDownX = gridData.getXFromTime( gridData.getTimeFromX( Event.getMouseDownX() ) );
        if( currX == mDownX )
        {
            x1 = mDownX;
            x2 = currX + gridData.getQLWidth( 0 );
        }
        else
        {
            auto deltaX = currX - mDownX;
            if( deltaX < 0 )
            {
                x1 = currX < 0 ? 0 : currX;
                x2 = mDownX + gridData.getQLWidth( 0 );
            }
            else
            {
                x1 = mDownX;
                x2 = currX + gridData.getQLWidth( 0 );
            }
        }
        gridComp.repaint();
    }
}
