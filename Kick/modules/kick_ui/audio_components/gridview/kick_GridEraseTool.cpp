
namespace kick_ui
{
    GridEraseTool::GridEraseTool( GridComponent& gridComponent, GridData& gData, juce::Viewport& ViewPort )
        : IGridTool( gridComponent, gData, GridToolType::GridEraseTool, ViewPort )
    {}

    void GridEraseTool::paint( juce::Graphics& )
    {}

    void GridEraseTool::mouseUp( const juce::MouseEvent& event )
    {
        auto startTime = gridData.getTimeFromX( event.position.getX(), false );
        auto noteNum   = gridData.getNoteFromY( event.position.getY() );
        if( ae.isValidSelectedNote( noteNum, startTime, -1 ) )
            ae.deleteSelectedNotes( aeSelected );
        else
            ae.eraseNote( noteNum, startTime, -1 );
        gridNeedsRepaint();
    }

    void GridEraseTool::mouseDrag( const juce::MouseEvent& event )
    {
        if( event.mouseWasDraggedSinceMouseDown() )
        {
            auto noteNum   = gridData.getNoteFromY( event.position.getY() );
            auto startTime = gridData.getTimeFromX( event.position.getX(), false );

            ae.eraseNote( noteNum, startTime, -1 );
            gridNeedsRepaint();
        }
    }

} //namespace mt
