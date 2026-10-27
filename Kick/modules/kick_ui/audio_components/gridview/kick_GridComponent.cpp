
namespace kick_ui
{

    ///////////////////////////////////////////////////////////////////////
    // Grid Notes
    ///////////////////////////////////////////////////////////////////////
    GridNotes::GridNotes()
    {
        setName( "GridNote-Canvas" );
        setInterceptsMouseClicks( false, false );
    }

    void GridNotes::paint( juce::Graphics& g )
    {
        auto  h                = gridData.getNoteHeight();
        auto  noteList         = ae.getNoteList();
        auto& selectedNoteList = ae.getSelectedNoteList();

        for( auto it : noteList )
        {
            mt::MTNote           note( it );
            juce::Rectangle<int> rect;
            if( note.isValid() )
            {
                auto ys  = gridData.getYFromNote( note.number.get() );
                auto xs  = gridData.getXFromTime( note.start.get() );
                auto bar = gridData.getBarFromX( xs );
                auto w   = note.length.get() * gridData.getXPerTick( bar );

                rect.setBounds( xs, ys, w, h );
                // Fill...
                mt::utils::fillRoundedRect( g, rect, ui::Theme::noteColour( note.velocity.get() ) );
                // Outline...
                auto isSel = selectedNoteList.contains( it );
                if( rect.getWidth() <= 1 ) rect.setWidth( 1 );
                mt::utils::renderRoundedRect( g, isSel ? rect.getWidth() <= 1 ? rect : rect.reduced( 1 ) : rect, ui::Theme::outlineColour( isSel ? 1 : 4 ) );
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////
    // Grid Component
    ///////////////////////////////////////////////////////////////////////
    GridComponent::GridComponent( juce::Viewport& ViewPort )
        : infoBar( this, 0 ), viewPort( ViewPort )
    {
        setName( "GridComponent" );
        setWantsKeyboardFocus( true );

        mt::utils::addChildComponents( this, notes );

        {
            auto p = new GridPencilTool( *this, gridData, viewPort );
            auto e = new GridEraseTool( *this, gridData, viewPort );
            auto s = new GridSelectTool( *this, gridData, viewPort );
            auto c = new GridCopyTool( *this, gridData, viewPort );
            auto m = new GridMagnifyTool( *this, gridData, viewPort );

            tools.insert( std::make_pair( p->getType(), p ) );
            tools.insert( std::make_pair( e->getType(), e ) );
            tools.insert( std::make_pair( s->getType(), s ) );
            tools.insert( std::make_pair( c->getType(), c ) );
            tools.insert( std::make_pair( m->getType(), m ) );
        }

        activeTool = tools.begin()->second;

        gridNeedsRepaint.connect( [&]() {
            triggerAsyncUpdate();
        } );
    }

    GridComponent::~GridComponent()
    {
        for( auto tool : tools )
        {
            delete tool.second;
        }
    }

    void GridComponent::noteSelectionChanged( int note )
    {
        noteSelected = note;
        repaint();
    }

    void GridComponent::setPosition( int bar, int beat, int tick )
    {
        currentBar  = bar;
        currentBeat = beat;
        currentTick = tick;
    }

    void GridComponent::paint( juce::Graphics& g )
    {
        auto canvas = getLocalBounds().toFloat();

        auto qColour    = ui::Theme::gridColour( 1 ).withMultipliedBrightness( 1.2 );
        auto beatColour = ui::Theme::gridColour( 2 );
        auto barColour  = ui::Theme::gridColour( 3 );

        auto numBars = gridData.getNumBars();

        // NOTE overlay...
        g.fillCheckerBoard( canvas, canvas.getWidth(), (float)gridData.getNoteHeight(), juce::Colours::grey.withAlpha( 0.5f ), juce::Colours::black.withAlpha( 0.5f ) );

        if( mt::getAudioInterface().getNumSequences() > 0 )
        {
            ////Draw bar beat
            auto barWidth  = gridData.getBarWidth();
            auto drawWidth = barWidth * numBars;

            float x  = 0.f;
            float qx = 0.f;

            juce::Rectangle<float> barArea{ x, canvas.getY(), drawWidth, canvas.getHeight() };

            //Draw Bars and Beats
            for( int bar = 0; bar < numBars; ++bar )
            {
                auto qlWidth   = gridData.getQLWidth( bar );
                auto beatWidth = gridData.getBarWidth() / gridData.getBeatsPerBar( bar );
                //Draw Beat Background
                auto thisBarArea = barArea.removeFromLeft( barWidth );
                g.fillCheckerBoard( thisBarArea, beatWidth, thisBarArea.getHeight(), ui::Theme::backgroundColour( 1 ).withAlpha( 0.9f ), ui::Theme::backgroundColour( 3 ).withAlpha( 0.9f ) );
                //Draw Bar line
                g.setColour( barColour );
                g.drawLine( x, 0.f, x + 1.f, canvas.getHeight() );
                x += barWidth;

                //QuantizationLevels
                g.setColour( qColour );
                auto drawQl = gridData.getNumQlPerBar( bar );
                for( int q = 0; q < drawQl; ++q )
                {
                    qx += qlWidth;
                    g.drawLine( qx, 0.f, qx + 1.f, canvas.getHeight() );
                }
            }

            // Draw highlighted row.
            if( juce::isPositiveAndBelow( noteSelected, gridData.getNotesMax() ) )
            {
                g.setColour( barColour );
                juce::Rectangle<float> bounds = { 1.f, gridData.getYFromNote( noteSelected ), canvas.getWidth() - 1.f, gridData.getNoteHeight() };
                g.drawRoundedRectangle( bounds, 1.f, 1.f );
            }

            //paint active tools
            if( activeTool )
            {
                activeTool->paint( g );
            }
        }
        else
        {
            float x             = 0.f;
            auto  gridCompWidth = getWidth();

            // Draw the beats and ticks within the single bar
            juce::Rectangle<float> barArea{ x, (float)canvas.getY(), (float)gridCompWidth, (float)canvas.getHeight() };
            g.fillCheckerBoard( barArea, (float)gridCompWidth / 4, canvas.getHeight(), ui::Theme::backgroundColour( 1 ).withAlpha( 0.9f ), ui::Theme::backgroundColour( 3 ).withAlpha( 0.9f ) );

            qColour = ui::Theme::gridColour( 1 );
            for( int beat = 0; beat < 4; ++beat ) // Default 4 beats per bar
            {
                float beatX = beat * gridCompWidth / 4;
                for( int tick = 1; tick < 4; ++tick ) // Default 4 ticks per beat
                {
                    float tickX = beatX + ( tick * gridCompWidth / 16 );
                    g.drawLine( tickX, 0, tickX, canvas.getHeight(), 0.5f ); // Draw tick line
                }
            }
        }
        parameterViewNeedsRepaint();
        return;
    }

    void GridComponent::resized()
    {
        notes.setBounds( getLocalBounds() );
    }

    void GridComponent::insertTimeSig( int bar, int num, int den )
    {
        ai.insertTimeSig( aiSel, bar - 1, num, den );
    }

    void GridComponent::deleteTimeSig( int bar )
    {
        ai.deleteTimSig( aiSel, bar );
    }

    void GridComponent::mouseUp( const juce::MouseEvent& event )
    {
        if( activeTool ) activeTool->mouseUp( event );
    }

    void GridComponent::mouseMove( const juce::MouseEvent& event )
    {
        if( activeTool ) activeTool->mouseMove( event );
    }

    void GridComponent::mouseDrag( const juce::MouseEvent& event )
    {
        if( activeTool ) activeTool->mouseDrag( event );
    }

    void GridComponent::mouseDown( const juce::MouseEvent& event )
    {
        if( activeTool ) activeTool->mouseDown( event );
    }

    void GridComponent::mouseDoubleClick( const juce::MouseEvent& event )
    {
        if( activeTool ) activeTool->mouseDoubleClick( event );
    }

    void GridComponent::toolTypeChanged( GridToolType toolType )
    {
        auto it    = tools.find( toolType );
        activeTool = ( it != tools.end() ) ? it->second : nullptr;
    }

    void GridComponent::visibilityChanged()
    {
        for( auto it : getChildren() )
            it->setVisible( isVisible() );
    }

    void GridComponent::handleAsyncUpdate()
    {
        repaint();
    }

    int GridComponent::getMouseDownNote()
    {
        if( activeTool )
            return activeTool->getMouseDownNoteNumber();
        return -1;
    }

} // namespace kick_ui
