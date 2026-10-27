namespace kick_ui
{
    AudioGridMagnifyTool::AudioGridMagnifyTool( AudioGridComponent& GridComp, AudioGridData& GridData, juce::Viewport& ViewPort )
        : IAudioTools( GridComp, GridData, ViewPort, AudioGridToolType::GridMagnifyTool )
    {
    }

    void AudioGridMagnifyTool::mouseDown( const juce::MouseEvent& Event )
    {
        if( Event.source.isTouch() )
        {
            auto touchIndex                    = Event.source.getIndex();
            activeTouches[touchIndex]          = Event.position;
            previousTouchPositions[touchIndex] = Event.position;
            initialTouchPositions[touchIndex]  = Event.position;
            if( activeTouches.size() == 2 )
            {
                initialDistance   = calculateDistance();
                initialZoomFactor = zoomFactor;
                initialTick       = gridData.getTimeFromX( ( activeTouches[0].x + activeTouches[1].x ) / 2 );
            }
        }
    }

    void AudioGridMagnifyTool::mouseUp( const juce::MouseEvent& Event )
    {
        if( Event.source.isTouch() )
        {
            auto touchIndex = Event.source.getIndex();
            activeTouches.erase( touchIndex );
            previousTouchPositions.erase( touchIndex );
            initialTouchPositions.erase( touchIndex );
            initialTick = 0;
        }
    }

    void AudioGridMagnifyTool::mouseDrag( const juce::MouseEvent& Event )
    {
        if( Event.source.isTouch() )
        {
            auto touchIndex           = Event.source.getIndex();
            auto currentPosition      = Event.position;
            activeTouches[touchIndex] = currentPosition;
            if( activeTouches.size() == 2 ) //Zoom in - Zoom out
            {
                auto  prevZoomFactor  = zoomFactor;
                float currentDistance = calculateDistance();
                zoomFactor            = initialZoomFactor * ( currentDistance / initialDistance );

                auto it = activeTouches.begin();
                auto p1 = it->second;
                ++it;
                auto p2       = it->second;
                auto midpoint = juce::Point<float>( ( p1.x + p2.x ) / 2.0f, ( p1.y + p2.y ) / 2.0f );

                auto viewWidth  = viewPort.getViewWidth();
                auto viewHeight = viewPort.getViewHeight();

                auto viewPos = viewPort.getViewPosition();

                float initialXDistance = std::abs( initialTouchPositions[0].x - initialTouchPositions[1].x );
                float initialYDistance = std::abs( initialTouchPositions[0].y - initialTouchPositions[1].y );
                float currentXDistance = std::abs( p1.x - p2.x );
                float currentYDistance = std::abs( p1.y - p2.y );

                bool isHorizontalZoom = std::abs( currentXDistance - initialXDistance ) > 10.0f;
                bool isVerticalZoom   = std::abs( currentYDistance - initialYDistance ) > 10.0f;

                if( currentDistance > initialDistance )
                    gridData.setZoomFactor( isHorizontalZoom && isVerticalZoom ? 20 : isHorizontalZoom ? 20 :
                                                                                                         0 );
                else
                    gridData.setZoomFactor( isHorizontalZoom && isVerticalZoom ? -20 : isHorizontalZoom ? -20 :
                                                                                                          0 );

                if( gridData.getWidth() <= 864 ) //GridViewPort Minimum Width
                    zoomFactor = prevZoomFactor;

                auto xPos = gridData.getXFromTime( initialTick );
                viewPort.setViewPosition( isHorizontalZoom ? xPos : viewPos.x,
                                          viewPos.y );
            }
            else if( activeTouches.size() == 1 ) // For Scroll
            {
                auto previousPosition = previousTouchPositions[touchIndex];
                auto delta            = currentPosition - previousPosition;

                auto viewPos = viewPort.getViewPosition();
                viewPort.setViewPosition( viewPos + juce::Point<int>( -delta.x, -delta.y ) );

                previousTouchPositions[touchIndex] = currentPosition;
            }
        }
    }
}
