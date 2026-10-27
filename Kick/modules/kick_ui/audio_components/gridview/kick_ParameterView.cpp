

namespace kick_ui
{
    ParameterViewControl::ParameterViewControl()
    {
        mt::utils::addAndMakeVisible( this, transposeBtn, snapBtn, eStartBtn, eEndBtn, plusBtn, minusBtn, ecBtn, Zoom, paramList, nudgeBtn, defaultZoomBtn );
        mt::utils::setRadioGroupID( (int)RadioGroupId::ParamViewControl, transposeBtn, eStartBtn, eEndBtn, nudgeBtn );
        mt::utils::setValueBackgroundColour( ui::Theme::backgroundColour( 3 ), Zoom );
        mt::utils::setValueColour( ui::Theme::textColour( 1 ), Zoom, transposeBtn, snapBtn, eStartBtn, eEndBtn, nudgeBtn, defaultZoomBtn );
        mt::utils::setValueFontHeight( 18, Zoom, transposeBtn, snapBtn, eStartBtn, eEndBtn, nudgeBtn, defaultZoomBtn );
        mt::utils::setValueJustification( juce::Justification::centred, Zoom );
        mt::utils::setPlacementFlag( juce::Justification::centred, plusBtn, minusBtn );

        minusBtn.toFront( true );
        plusBtn.toFront( true );

        nudgeBtn.triggerClick();

        ui::themeChanged.connect( [&]() {
            ecBtn.setImages( "UP", "Down" );
            paramTypeBtn.setImages( "Velocity" );
            transposeBtn.setToggleButtonText( "Transpose", "Transpose" );
            snapBtn.setToggleButtonText( "Don't Snap", "Don't Snap" );
            eStartBtn.setToggleButtonText( "Edit Start", "Edit Start" );
            eEndBtn.setToggleButtonText( "Edit End", "Edit End" );
            plusBtn.setImages( "Plus " );
            minusBtn.setImages( "Minus " );
        } );
    }

    void ParameterViewControl::resized()
    {
        defaultZoomBtn.setBoundsRelative( .0097f, .18f, .0313f, .629f );
        snapBtn.setBoundsRelative( .045f, .18f, .1174f, .629f );
        eStartBtn.setBoundsRelative( .1663f, .18f, .1174f, .629f );
        eEndBtn.setBoundsRelative( .2876f, .18f, .1174f, .629f );
        transposeBtn.setBoundsRelative( .408f, .18f, .1174f, .629f );
        nudgeBtn.setBoundsRelative( .529f, .18f, .1174f, .629f );

        Zoom.setBoundsRelative( .655f, .16f, .156f, .666f );
        minusBtn.setBoundsRelative( .65f, .18f, .043f, .629f );
        plusBtn.setBoundsRelative( .77f, .18f, .043f, .629f );

        paramList.setBoundsRelative( .814f, .18f, .127f, .629f );
        ecBtn.setBoundsRelative( .947f, .18f, .043f, .629f );
    }

    void ParameterViewControl::paint( juce::Graphics& g )
    {
        mt::utils::fillRoundedRect( g, getLocalBounds(), ui::Theme::backgroundColour( 4 ) );
    }

    ParameterView::ParameterView( ParameterViewControl& Control )
        : paramControl( Control )
    {
        parameterViewNeedsRepaint.connect( [&]() {
            triggerAsyncUpdate();
        } );
    }

    void ParameterView::paint( juce::Graphics& g )
    {
        auto b          = getLocalBounds();
        int  viewHeight = b.getHeight();

        // Draw background...
        auto color = ui::Theme::backgroundColour( 1 );
        g.fillAll( color );

        // Draw vertical grid lines
        g.setColour( ui::Theme::gridColour( 1 ) );
        auto xPos = 0.f;
        for( auto bar = 0; bar < gridData.getNumBars(); ++bar )
        {
            auto qlWidth = gridData.getQLWidth( bar );
            auto numQl   = gridData.getNumQlPerBar( bar );
            auto x       = xPos;
            for( ; x < b.getWidth(); x += qlWidth )
            {
                g.drawVerticalLine( x, 0, b.getHeight() );
            }
            xPos = x;
        }

        for( int y = 0; y <= viewHeight; y += 20 )
        {
            if( y == 0 )
                g.setColour( ui::Theme::backgroundColour( 6 ) );
            else
                g.setColour( ui::Theme::gridColour( 1 ) );
            g.drawHorizontalLine( y, 0, b.getWidth() );
        }

        // Draw notes
        for( auto it : ae.getNoteList() )
        {
            mt::MTNote note( it );

            auto w  = gridData.getXFromTime( note.length );
            auto xs = gridData.getXFromTime( note.start );
            auto ys = viewHeight;
            auto h  = 0;
            if( paramControl.paramList.getSelected() == "Velocity" )
            {
                h = (int)( ys * ( (float)note.velocity.get() / 127.f ) );
                g.setColour( ui::Theme::noteColour( note.velocity.get() ) );
            }
            else if( paramControl.paramList.getSelected() == "Probability" )
            {
                h = (int)( ys * ( (float)note.probability.get() / 100.f ) );
                g.setColour( juce::Colours::skyblue );
            }
            else if( paramControl.paramList.getSelected() == "Ratchet" )
            {
                auto ratProp = ( note.ratchet.get() - 1.f ) / ( max - 1.f );
                h            = (int)( ys * ( ratProp < 0 ? 0 : ratProp ) );
                g.setColour( juce::Colours::skyblue );
            }
            auto yAxis = ys - ( h - 1 );
            g.fillRect( xs, yAxis < b.getHeight() ? yAxis : b.getHeight() - 1, w, h > 0 ? h : 1 );

            g.setColour( juce::Colours::white );
            g.fillRect( xs, yAxis < b.getHeight() ? yAxis : b.getHeight() - 1, w, 1 );
            if( isMouseButtonDown() )
            {
                auto event = getMouseXYRelative();
                if( event.getX() >= xs && event.getX() < xs + w && ae.getNumSelectedNotes( aeSelected ) < 2 )
                {
                    g.setColour( juce::Colours::grey );
                    g.drawRoundedRectangle( { (float)xs, yAxis < b.getHeight() ? (float)yAxis : b.getHeight() - 1.f, (float)w, h > 0 ? (float)h : 1.f }, 1.f, 1.f );
                }
            }
        }
    }

    void ParameterView::setMinMaxValues( int Min, int Max )
    {
        min = Min;
        max = Max;
        repaint();
    }

    void ParameterView::mouseDrag( const juce::MouseEvent& event )
    {
        handleMouseDrag( event );
    }

    void ParameterView::mouseUp( const juce::MouseEvent& event )
    {
        handleMouseDrag( event );
    }

    void ParameterView::handleMouseDrag( const juce::MouseEvent& event )
    {
        auto b = getLocalBounds();
        if( b.contains( event.getPosition() ) )
        {
            mt::NotesInRange range;
            float            y = event.position.getY();
            range.startTime    = gridData.getTimeFromX( event.position.getX() );
            auto bar           = gridData.getBarFromX( event.position.getX() );
            range.endTime      = range.startTime + gridData.getTicksPerQuantization( bar );
            if( !ae.isSelectedNoteInRange( range ) )
            {
                ae.selectAllNotes( false );
                ae.selectNotesInRange( range, true );
            }
            if( paramControl.paramList.getSelected() == "Velocity" )
            {
                auto vel = (int)( max * ( 1 - ( y / b.getHeight() ) ) );
                if( 0 < ae.getNumSelectedNotes( aeSelected ) )
                {
                    if( ae.isSelectedNoteInRange( range ) )
                        ae.editSelectedNotesVelocity( aeSelected, vel );
                }
                if( ae.isValidNoteInRange( range ) && !ae.isSelectedNoteInRange( range ) )
                    ae.setNoteVelocities( range, vel );
            }
            else if( paramControl.paramList.getSelected() == "Probability" )
            {
                auto prob = (int)( max * ( 1 - ( y / b.getHeight() ) ) );
                if( 0 < ae.getNumSelectedNotes( aeSelected ) )
                {
                    if( ae.isSelectedNoteInRange( range ) )
                        ae.editSelectedNotesProbability( aeSelected, prob );
                }
                if( ae.isValidNoteInRange( range ) && !ae.isSelectedNoteInRange( range ) )
                    ae.setNoteProbabilities( range, prob );
            }
            else if( paramControl.paramList.getSelected() == "Ratchet" )
            {
                auto rat = juce::jlimit( min, max, (int)round( (float)( max * (float)( 1 - ( y / b.getHeight() ) ) ) ) ); // Need Calculation
                if( 0 < ae.getNumSelectedNotes( aeSelected ) )
                {
                    if( ae.isSelectedNoteInRange( range ) )
                        ae.editSelectedNotesRatchet( aeSelected, rat );
                }
                if( ae.isValidNoteInRange( range ) && !ae.isSelectedNoteInRange( range ) )
                    ae.setNoteRatchets( range, rat );
            }
            repaint();
            gridNeedsRepaint();
        }
    }

    void ParameterView::handleAsyncUpdate()
    {
        repaint();
    }

    void ParameterViewScale::setMinMaxValues( int Min, int Max )
    {
        min = Min;
        max = Max;
        repaint();
    }

    void ParameterViewScale::paint( juce::Graphics& g )
    {
        g.fillAll( ui::Theme::backgroundColour( 2 ) );
        auto b  = getLocalBounds();
        auto x1 = b.getWidth() - 20;
        auto x2 = b.getWidth() - 15;
        auto x3 = b.getWidth() - 1;
        auto y  = 2.f;
        g.setColour( ui::Theme::backgroundColour( 8 ) );
        auto marker = 0;
        for( auto i = 0; i <= b.getHeight(); i += 20 )
        {
            if( marker ^= 1 )
                g.drawLine( x1, y, x3, y );
            else
                g.drawLine( x2, y, x3, y );
            y += 19.75;
        }
        mt::utils::fillRoundedRect( g, { 0, 0, getWidth(), 1 }, ui::Theme::backgroundColour( 6 ), 1.f, false, false, false, false );
        g.setColour( ui::Theme::textColour( 1 ) );
        g.drawFittedText( juce::String( max ), { x1 - 20, 5, 20, 10 }, juce::Justification::centredLeft, 1 );
        g.drawFittedText( juce::String( min ), { x1 - 20, getHeight() - 10, 20, 10 }, juce::Justification::centred, 1 );
    }
}
