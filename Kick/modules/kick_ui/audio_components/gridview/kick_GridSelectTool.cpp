namespace kick_ui
{
    GridSelectTool::GridSelectTool( GridComponent& gridComponent, GridData& gData, juce::Viewport& ViewPort )
        : IGridTool( gridComponent, gData, GridToolType::GridSelectTool, ViewPort )
    {
    }

    void GridSelectTool::clearSelectRegion()
    {
        startX = endX = startY = endY = 0;
    }

    bool GridSelectTool::isSelectRegionValid()
    {
        bool isValid = false;
        if( startX >= 0 && startY >= 0 && endX > 0 && endY > 0 ) isValid = true;
        return isValid;
    }

    void GridSelectTool::paint( juce::Graphics& g )
    {
        //draw selection region
        if( isSelectRegionValid() )
        {
            juce::Rectangle<float> selRect( startX, startY, ( endX - startX ), ( endY - startY ) );
            g.setColour( ui::Theme::selectionColour( 1 ) );
            g.drawRect( selRect, 1.5f );
        }
    }

    void GridSelectTool::drawInfoBar( int noteNum, int startTime, juce::MouseEvent event )
    {
        auto data = ae.getNoteData( noteNum, startTime, 0 );
        if( 0 <= data.id )
        {
            auto             Start      = ae.toBnB( aeSelected, (unsigned int)data.start );
            auto             resizeText = juce::String::formatted( "%d :: %d:%d:%d = %d", data.number.get() + 1, Start.bar + 1, Start.beat + 1, Start.tick, data.length.get() );
            juce::Point<int> dragPoint( event.x, event.y );
            gridComp.infoBar.displayTip( dragPoint, resizeText );
        }
    }

    void GridSelectTool::mouseUp( const juce::MouseEvent& event )
    {
        gridComp.infoBar.hideTip();
        if( event.mouseWasDraggedSinceMouseDown() )
        {
            if( 0 == ae.getNumSelectedNotes( aeSelected ) )
            {
                ae.selectAllNotes( false );
            }
        }
        else
        {
            ae.selectAllNotes( false );
            ae.selectNote( aeSelected, mDownNoteID, true );
        }
        clearSelectRegion();
        gridNeedsRepaint();
    }

    void GridSelectTool::mouseDrag( const juce::MouseEvent& event )
    {
        if( gridComp.getMouseCursor() == juce::MouseCursor::DraggingHandCursor )
        {
            auto noteNum  = gridData.getNoteFromY( event.position.getY() );
            auto noteTime = gridData.getTimeFromX( event.position.getX() );
            // Check if any note exists at mouse down
            if( 0 < mDownNoteID )
            {
                auto deltaNote = noteNum - mDownNoteNum;
                auto deltaTime = noteTime - mDownTime;

                if( deltaNote || deltaTime )
                {
                    if( mDownSelectedNote )
                    {
                        ae.editSelectedNotes( aeSelected, deltaTime, deltaNote, 0 );
                    }
                    else
                    {
                        ae.editNote( mDownNoteID, deltaTime, deltaNote, 0 );
                    }
                    mDownTime    = noteTime;
                    mDownNoteNum = noteNum;
                    gridNeedsRepaint();
                }
            }
            drawInfoBar( noteNum, noteTime, event );
        }
        else
        {
            float x1 = juce::jmax<float>( 0.0, event.mouseDownPosition.getX() );
            float x2 = juce::jmax<float>( 0.0, event.position.getX() );

            if( x1 != x2 )
            {
                float y1 = juce::jmax<float>( 0.0, event.mouseDownPosition.getY() );
                float y2 = juce::jmax<float>( 0.0, event.position.getY() );

                // Save selected region...
                if( startX <= 0 )
                    startX = x1;
                if( x2 <= gridData.getWidth() && x2 >= 0 )
                {
                    if( startX > x2 && endX <= startX )
                    {
                        endX   = startX;
                        startX = x2;
                    }
                    else
                        startX < x2 ? endX = juce::jmax<float>( startX, x2 ) : startX = x2;
                }
                if( startY <= 0 )
                    startY = y1;
                if( y2 <= gridData.getHeight() && y2 >= 0 )
                {
                    if( startY > y2 && endY <= startY )
                    {
                        endY   = startY;
                        startY = y2;
                    }
                    else
                        startY < y2 ? endY = juce::jmax<float>( startY, y2 ) : startY = y2;
                }

                auto start = gridData.getTimeFromX( startX, false );
                auto end   = gridData.getTimeFromX( endX, false );

                int note1 = gridData.getNoteFromY( startY );
                int note2 = gridData.getNoteFromY( endY );

                mt::NotesInRange range( note1, note2, start, end );
                if( !juce::ModifierKeys::getCurrentModifiers().isShiftDown() )
                    ae.selectAllNotes( false );

                ae.selectNotesInRange( range, true );

                gridNeedsToMove( x2, y2 );
                //gridNeedsRepaint();
            }
        }
    }

    void GridSelectTool::mouseMove( const juce::MouseEvent& event )
    {
        auto mouseNoteNum  = gridData.getNoteFromY( event.position.getY() );
        auto mouseNoteTime = gridData.getTimeFromX( event.position.getX(), false );

        auto isSelectedNote = ( 0 < ae.getSelectedNoteID( mouseNoteNum, mouseNoteTime, -1 ) );

        gridComp.setMouseCursor( isSelectedNote ? juce::MouseCursor::DraggingHandCursor : juce::MouseCursor::NormalCursor );
        gridNeedsRepaint();
    }

    bool GridSelectTool::keyPressed( const juce::KeyPress& key, juce::Component* /*originatingComponent*/ )
    {
        if( key.getKeyCode() == juce::KeyPress::deleteKey )
        {
            ae.deleteSelectedNotes( aeSelected );
            gridComp.repaint();
            return true;
        }
        return false;
    }
}
