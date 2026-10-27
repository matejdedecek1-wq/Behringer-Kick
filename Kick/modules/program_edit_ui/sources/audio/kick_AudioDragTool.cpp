namespace kick_ui
{
    AudioGridDragTool::AudioGridDragTool( AudioGridComponent& GridComp, AudioGridData& GridData, juce::Viewport& ViewPort )
        : IAudioTools( GridComp, GridData, ViewPort, AudioGridToolType::GridDragTool )
    {
    }

    void AudioGridDragTool::paint( juce::Graphics& G )
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

    void AudioGridDragTool::mouseUp( const juce::MouseEvent& Event )
    {
        x1 = x2 = 0;
        gridComp.repaint();
        prevDragTime = 0;
    }

    void AudioGridDragTool::mouseDrag( const juce::MouseEvent& Event )
    {
        if( Event.mouseWasDraggedSinceMouseDown() )
        {
            auto startTime = gridData.getTimeFromX( Event.position.x );

            if( gridComp.getMouseCursor() == juce::MouseCursor::DraggingHandCursor )
            {
                if( auto pProg = dynamic_cast<mt::MTAudioProgram*>( ae.getProgram( ae.getSelectedProgram() ) ) )
                {
                    auto& sampleTree = pProg->getSelectedAudioSampleList();
                    if( sampleTree.size() <= 1 )
                    {
                        for( auto& vt : sampleTree )
                        {
                            mt::MTIAudio sample{ vt, &ae.getUndoManager() };
                            //CHECK THE MDOWN POS AND VALIDATE THE DRAG
                            auto sampleStart = sample.startTime.get();
                            auto endTime     = sampleStart + sample.length.get();

                            auto downTime = gridData.getTimeFromX( gridData.getXFromTime( mDownTime ) );
                            if( prevDragTime == 0 && downTime != startTime )
                            {
                                auto deltaTime = downTime - startTime;
                                if( deltaTime < 0 )
                                {
                                    sampleStart = sampleStart - gridData.getTicksPerQuantization( 0 );
                                    if( sampleStart < 0 ) sampleStart = 0;
                                }
                                else if( deltaTime > 0 )
                                {
                                    auto newStart = sampleStart + gridData.getTicksPerQuantization( 0 );
                                    sampleStart   = gridData.getTimeFromX( gridData.getXFromTime( newStart ) );
                                }
                                prevDragTime = startTime;
                            }
                            else if( prevDragTime > 0 && prevDragTime != startTime )
                            {
                                auto deltaTime = startTime - prevDragTime;

                                if( deltaTime < 0 )
                                {
                                    sampleStart = sampleStart - gridData.getTicksPerQuantization( 0 );
                                    if( sampleStart < 0 ) sampleStart = 0;
                                }
                                else if( deltaTime > 0 )
                                {
                                    auto newStart = sampleStart + gridData.getTicksPerQuantization( 0 );
                                    sampleStart   = gridData.getTimeFromX( gridData.getXFromTime( newStart + 3 ) );
                                }
                                prevDragTime = startTime;
                            }

                            if( sample.startTime.get() != sampleStart )
                            {
                                sample.startTime = sampleStart;
                                sampleNeedRepaint();
                            }
                        }
                    }
                }
            }
            else
            {
                if( auto pProg = dynamic_cast<mt::MTAudioProgram*>( ae.getProgram( ae.getSelectedProgram() ) ) )
                {
                    pProg->selectAudioSample( startTime );
                    if( auto pSample = pProg->getAudioSample( mDownTime ) )
                        pProg->eraseSelectedSample();
                    sampleNeedRepaint();
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
    }

    void AudioGridDragTool::mouseMove( const juce::MouseEvent& Event )
    {
        auto mouseNoteTime = gridData.getTimeFromX( Event.position.getX(), false );

        if( auto* audioProgram = dynamic_cast<mt::MTAudioProgram*>( ae.getProgram( ae.getSelectedProgram() ) ) )
        {
            if( auto pSample = audioProgram->getAudioSample( mouseNoteTime ) )
                gridComp.setMouseCursor( juce::MouseCursor::DraggingHandCursor );
            else
                gridComp.setMouseCursor( juce::MouseCursor::NormalCursor );
        }
        else
            gridComp.setMouseCursor( juce::MouseCursor::NormalCursor );
    }
}
