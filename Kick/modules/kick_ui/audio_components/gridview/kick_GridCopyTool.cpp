
namespace kick_ui
{
    GridCopyTool::GridCopyTool( GridComponent& gridComponent, GridData& gData, juce::Viewport& ViewPort )
        : IGridTool( gridComponent, gData, GridToolType::GridCopyTool, ViewPort )
    {
    }

    void GridCopyTool::mouseUp( const juce::MouseEvent& event )
    {
        if( gridComp.getMouseCursor() == juce::MouseCursor::CopyingCursor )
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
                        ae.pasteSelectedNotes( aeSelected, deltaTime, deltaNote, 0 );
                    }
                    else
                    {
                        ae.pasteNote( mDownNoteID, deltaTime, deltaNote, 0 );
                    }
                    gridComp.setMouseCursor( juce::MouseCursor::NormalCursor );
                    gridNeedsRepaint();
                }
            }
        }
    }

    void GridCopyTool::mouseDrag( const juce::MouseEvent& event )
    {
        if( 0 < mDownNoteID )
            gridComp.setMouseCursor( juce::MouseCursor::CopyingCursor );
        else
            gridComp.setMouseCursor( juce::MouseCursor::NormalCursor );
    }

}
