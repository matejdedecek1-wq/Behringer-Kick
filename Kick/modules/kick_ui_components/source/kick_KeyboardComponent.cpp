namespace ui
{
    static const juce::uint8 whiteNotes[] = { 0, 2, 4, 5, 7, 9, 11 };
    static const juce::uint8 blackNotes[] = { 1, 3, 6, 8, 10 };

    struct KeyboardComponent::UpDownButton : public juce::Button
    {
        UpDownButton( KeyboardComponent& c, int d )
            : Button( {} ), owner( c ), delta( d )
        {}

        void clicked() override
        {
            auto note = owner.getLowestVisibleKey();

            if( delta < 0 )
                note = ( note - 1 ) / 12;
            else
                note = note / 12 + 1;

            owner.setLowestVisibleKey( note * 12 );
        }

        using Button::clicked;

        void paintButton( juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown ) override
        {
            owner.drawUpDownButton( g, getWidth(), getHeight(),
                                    shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown,
                                    delta > 0 );
        }

    private:
        KeyboardComponent& owner;
        const int          delta;

        JUCE_DECLARE_NON_COPYABLE( UpDownButton )
    };

    KeyboardComponent::KeyboardComponent( juce::MidiKeyboardState& s, Orientation o )
        : state( s ), orientation( o )
    {
        // initialise with a default set of qwerty key-mappings..
        scrollDown.reset( new UpDownButton( *this, -1 ) );
        scrollUp.reset( new UpDownButton( *this, 1 ) );

        addChildComponent( scrollDown.get() );
        addChildComponent( scrollUp.get() );

        int note = 0;

        for( char c : "awsedftgyhujkolp;" )
            setKeyPressForNote( juce::KeyPress( c, 0, 0 ), note++ );

        mouseOverNotes.insertMultiple( 0, -1, 32 );
        mouseDownNotes.insertMultiple( 0, -1, 32 );

        colourChanged();
        setWantsKeyboardFocus( true );

        //state.addListener(this);
        //setSize( 512, 500 );
        //startTimerHz(20);
    }

    void KeyboardComponent::paint( juce::Graphics& g )
    {
        g.fillAll( findColour( whiteNoteColourId ) );

        auto lineColour = findColour( keySeparatorLineColourId );
        auto textColour = findColour( textLabelColourId );

        for( int octave = 0; octave < 128; octave += 12 )
        {
            for( int white = 0; white < 7; ++white )
            {
                auto noteNum = octave + whiteNotes[white];

                if( noteNum >= rangeStart && noteNum <= rangeEnd )
                    drawWhiteNote( noteNum, g, getRectangleForKey( noteNum ),
                                   state.isNoteOnForChannels( midiInChannelMask, noteNum ),
                                   mouseOverNotes.contains( noteNum ), lineColour, textColour );
            }
        }

        float x1 = 0.0f, y1 = 0.0f, x2 = 0.0f, y2 = 0.0f;
        auto  width  = getWidth();
        auto  height = getHeight() - 108;

        if( orientation == verticalKeyboardFacingLeft )
        {
            x1 = (float)width - 1.0f;
            x2 = (float)width - 5.0f;
        }
        else if( orientation == verticalKeyboardFacingRight )
            x2 = 5.0f;
        else
            y2 = 5.0f;

        auto x         = getKeyPos( rangeEnd ).getEnd();
        auto shadowCol = findColour( shadowColourId );

        if( !shadowCol.isTransparent() )
        {
            g.setGradientFill( juce::ColourGradient( shadowCol, x1, y1, shadowCol.withAlpha( 0.0f ), x2, y2, false ) );

            switch( orientation )
            {
                case horizontalKeyboard: g.fillRect( 0.0f, 0.0f, x, 5.0f ); break;
                case verticalKeyboardFacingLeft: g.fillRect( (float)width - 5.0f, 0.0f, 5.0f, x ); break;
                case verticalKeyboardFacingRight: g.fillRect( 0.0f, 0.0f, 5.0f, x ); break;
                default: break;
            }
        }

        if( !lineColour.isTransparent() )
        {
            g.setColour( lineColour );

            switch( orientation )
            {
                case horizontalKeyboard: g.fillRect( 0.0f, (float)height - 1.0f, x, 1.0f ); break;
                case verticalKeyboardFacingLeft: g.fillRect( 0.0f, 0.0f, 1.0f, x ); break;
                case verticalKeyboardFacingRight: g.fillRect( (float)width - 1.0f, 0.0f, 1.0f, x ); break;
                default: break;
            }
        }

        auto blackNoteColour = findColour( blackNoteColourId );

        for( int octave = 0; octave < 128; octave += 12 )
        {
            for( int black = 0; black < 5; ++black )
            {
                auto noteNum = octave + blackNotes[black];

                if( noteNum >= rangeStart && noteNum <= rangeEnd )
                    drawBlackNote( noteNum, g, getRectangleForKey( noteNum ),
                                   state.isNoteOnForChannels( midiInChannelMask, noteNum ),
                                   mouseOverNotes.contains( noteNum ), blackNoteColour );
            }
        }
    }

    void KeyboardComponent::setKeyPressForNote( const juce::KeyPress& key, int midiNoteOffsetFromC )
    {
        removeKeyPressForNote( midiNoteOffsetFromC );

        keyPressNotes.add( midiNoteOffsetFromC );
        keyPresses.add( key );
    }

    void KeyboardComponent::removeKeyPressForNote( int midiNoteOffsetFromC )
    {
        for( int i = keyPressNotes.size(); --i >= 0; )
        {
            if( keyPressNotes.getUnchecked( i ) == midiNoteOffsetFromC )
            {
                keyPressNotes.remove( i );
                keyPresses.remove( i );
            }
        }
    }

    bool KeyboardComponent::mouseDownOnKey( int, const juce::MouseEvent& ) { return true; }
    bool KeyboardComponent::mouseDraggedToKey( int, const juce::MouseEvent& ) { return true; }
    void KeyboardComponent::mouseUpOnKey( int, const juce::MouseEvent& ) {}

    void KeyboardComponent::mouseDown( const juce::MouseEvent& e )
    {
        float mousePositionVelocity;
        auto  newNote = xyToNote( e.position, mousePositionVelocity );

        if( newNote >= 0 && mouseDownOnKey( newNote, e ) )
            updateNoteUnderMouse( e, true );
    }

    void KeyboardComponent::mouseUp( const juce::MouseEvent& e )
    {
        updateNoteUnderMouse( e, false );

        float mousePositionVelocity;
        auto  note = xyToNote( e.position, mousePositionVelocity );

        if( note >= 0 )
            mouseUpOnKey( note, e );
    }

    void KeyboardComponent::mouseEnter( const juce::MouseEvent& )
    {
        //updateNoteUnderMouse(e, false);
    }

    void KeyboardComponent::mouseExit( const juce::MouseEvent& )
    {
        //updateNoteUnderMouse(e, false);
    }

    void KeyboardComponent::updateNoteUnderMouse( const juce::MouseEvent& e, bool isDown )
    {
        updateNoteUnderMouse( e.getEventRelativeTo( this ).position, isDown, e.source.getIndex() );
    }

    void KeyboardComponent::updateNoteUnderMouse( juce::Point<float> pos, bool isDown, int fingerNum )
    {
        float mousePositionVelocity = 0.0f;
        auto  newNote               = xyToNote( pos, mousePositionVelocity );
        auto  oldNote               = mouseOverNotes.getUnchecked( fingerNum );
        auto  oldNoteDown           = mouseDownNotes.getUnchecked( fingerNum );
        auto  eventVelocity         = useMousePositionForVelocity ? mousePositionVelocity * velocity : velocity;

        if( newNote != oldNoteDown )
        {
            repaintNote( oldNote );
            repaintNote( newNote );
            mouseOverNotes.set( fingerNum, newNote );
            if( noteSelectionChanged )
                noteSelectionChanged( newNote );
        }

        else
        {
            if( isDown == false )
            {
                repaintNote( oldNote );
                repaintNote( -1 );
                mouseOverNotes.set( fingerNum, -1 );
            }
        }

        if( isDown )
        {
            if( newNote != oldNoteDown )
            {
                if( oldNoteDown >= 0 )
                {
                    mouseDownNotes.set( fingerNum, -1 );

                    if( !mouseDownNotes.contains( oldNoteDown ) )
                        state.noteOff( midiChannel, oldNoteDown, eventVelocity );
                }

                if( newNote >= 0 && !mouseDownNotes.contains( newNote ) )
                {
                    state.noteOn( midiChannel, newNote, eventVelocity );
                    mouseDownNotes.set( fingerNum, newNote );
                }
            }
        }
        else if( oldNoteDown >= 0 )
        {
            mouseDownNotes.set( fingerNum, -1 );

            if( !mouseDownNotes.contains( oldNoteDown ) )
                state.noteOff( midiChannel, oldNoteDown, eventVelocity );
        }
    }

    void KeyboardComponent::handleNoteOn( juce::MidiKeyboardState*, int /*midiChannel*/, int /*midiNoteNumber*/, float /*velocity*/ )
    {
        shouldCheckState = true; // (probably being called from the audio thread, so avoid blocking in here)
    }

    void KeyboardComponent::handleNoteOff( juce::MidiKeyboardState*, int /*midiChannel*/, int /*midiNoteNumber*/, float /*velocity*/ )
    {
        shouldCheckState = true; // (probably being called from the audio thread, so avoid blocking in here)
    }

    void KeyboardComponent::drawUpDownButton( juce::Graphics& g, int w, int h,
                                              bool mouseOver,
                                              bool buttonDown,
                                              bool movesOctavesUp )
    {
        g.fillAll( findColour( upDownButtonBackgroundColourId ) );

        float angle = 0;

        switch( orientation )
        {
            case horizontalKeyboard: angle = movesOctavesUp ? 0.0f : 0.5f; break;
            case verticalKeyboardFacingLeft: angle = movesOctavesUp ? 0.25f : 0.75f; break;
            case verticalKeyboardFacingRight: angle = movesOctavesUp ? 0.75f : 0.25f; break;
            default: jassertfalse; break;
        }

        juce::Path path;
        path.addTriangle( 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.5f );
        path.applyTransform( juce::AffineTransform::rotation( juce::MathConstants<float>::twoPi * angle, 0.5f, 0.5f ) );

        g.setColour( findColour( upDownButtonArrowColourId )
                         .withAlpha( buttonDown ? 1.0f : ( mouseOver ? 0.6f : 0.4f ) ) );

        g.fillPath( path, path.getTransformToScaleToFit( 1.0f, 1.0f, (float)w - 2.0f, (float)h - 2.0f, true ) );
    }

    void KeyboardComponent::drawWhiteNote( int midiNoteNumber, juce::Graphics& g, juce::Rectangle<float> area,
                                           bool isDown, bool isOver, juce::Colour lineColour, juce::Colour textColour )
    {
        auto c = juce::Colours::transparentWhite;

        if( isDown ) c = ui::Theme::backgroundColour( 6 );
        if( isOver ) c = c.overlaidWith( findColour( mouseOverKeyOverlayColourId ) );

        g.setColour( c );
        g.fillRect( area );

        auto text = getWhiteNoteText( midiNoteNumber );

        if( text.isNotEmpty() )
        {
            auto fontHeight = juce::jmin( 12.0f, keyWidth * 0.9f );

            g.setColour( textColour );
            g.setFont( juce::Font( fontHeight ).withHorizontalScale( 0.8f ) );

            switch( orientation )
            {
                case horizontalKeyboard: g.drawText( text, area.withTrimmedLeft( 1.0f ).withTrimmedBottom( 2.0f ), juce::Justification::centredBottom, false ); break;
                case verticalKeyboardFacingLeft: g.drawText( text, area.reduced( 2.0f ), juce::Justification::centredLeft, false ); break;
                case verticalKeyboardFacingRight: g.drawText( text, area.reduced( 2.0f ), juce::Justification::centredRight, false ); break;
                default: break;
            }
        }

        if( !lineColour.isTransparent() )
        {
            g.setColour( lineColour );

            switch( orientation )
            {
                case horizontalKeyboard: g.fillRect( area.withWidth( 1.0f ) ); break;
                case verticalKeyboardFacingLeft: g.fillRect( area.withHeight( 1.0f ) ); break;
                case verticalKeyboardFacingRight: g.fillRect( area.removeFromBottom( 1.0f ) ); break;
                default: break;
            }

            if( midiNoteNumber == rangeEnd )
            {
                switch( orientation )
                {
                    case horizontalKeyboard: g.fillRect( area.expanded( 1.0f, 0 ).removeFromRight( 1.0f ) ); break;
                    case verticalKeyboardFacingLeft: g.fillRect( area.expanded( 0, 1.0f ).removeFromBottom( 1.0f ) ); break;
                    case verticalKeyboardFacingRight: g.fillRect( area.expanded( 0, 1.0f ).removeFromTop( 1.0f ) ); break;
                    default: break;
                }
            }
        }
    }

    void KeyboardComponent::drawBlackNote( int /*midiNoteNumber*/, juce::Graphics& g, juce::Rectangle<float> area,
                                           bool isDown, bool isOver, juce::Colour noteFillColour )
    {
        auto c = noteFillColour;

        if( isDown ) c = ui::Theme::backgroundColour( 6 );
        if( isOver ) c = c.overlaidWith( findColour( mouseOverKeyOverlayColourId ) );

        g.setColour( c );
        g.fillRect( area );

        if( isDown )
        {
            g.setColour( noteFillColour );
            g.drawRect( area );
        }
        else
        {
            g.setColour( c.brighter() );
            auto sideIndent = 1.0f; //1.0f / 8.0f;
            auto topIndent  = 1.0f; // 7.0f / 8.0f;
            auto w          = area.getWidth();
            auto h          = area.getHeight();

            switch( orientation )
            {
                case horizontalKeyboard: g.fillRect( area.reduced( w * sideIndent, 0 ).removeFromTop( h * topIndent ) ); break;
                case verticalKeyboardFacingLeft: g.fillRect( area.reduced( 0, h * sideIndent ).removeFromRight( w * topIndent ) ); break;
                case verticalKeyboardFacingRight: g.fillRect( area.reduced( 0, h * sideIndent ).removeFromLeft( w * topIndent ) ); break;
                default: break;
            }
        }
    }

    void KeyboardComponent::setOctaveForMiddleC( int octaveNum )
    {
        octaveNumForMiddleC = octaveNum;
        repaint();
    }

    juce::String KeyboardComponent::getWhiteNoteText( int midiNoteNumber )
    {
        if( midiNoteNumber % 12 == 0 )
            return juce::MidiMessage::getMidiNoteName( midiNoteNumber, true, true, octaveNumForMiddleC );

        return {};
    }

    juce::Range<float> KeyboardComponent::getKeyPosition( int midiNoteNumber, float targetKeyWidth ) const
    {
        jassert( midiNoteNumber >= 0 && midiNoteNumber < 128 );

        static const float notePos[] = { 0.0f, 1,
                                         1.5f, 3,
                                         3.5f,
                                         5.0f, 6,
                                         6.5f, 8,
                                         8.5f, 10,
                                         10.5f };

        auto octave = midiNoteNumber / 12;
        auto note   = midiNoteNumber % 12;

        auto start = octave * 12.0f * targetKeyWidth + notePos[note] * targetKeyWidth;
        auto width = juce::MidiMessage::isMidiNoteBlack( note ) ? blackNoteWidthRatio * targetKeyWidth : targetKeyWidth;

        return { start, start + width };
    }

    KeyboardComponent::~KeyboardComponent()
    {}

    void KeyboardComponent::resized()
    {
        auto w = getWidth();
        auto h = getHeight();

        if( w > 0 && h > 0 )
        {
            if( orientation != horizontalKeyboard )
                std::swap( w, h );

            auto kx2 = getKeyPos( rangeEnd ).getEnd();

            if( (int)firstKey != rangeStart )
            {
                auto kx1 = getKeyPos( rangeStart ).getStart();

                if( kx2 - kx1 <= (float)w )
                {
                    firstKey = (float)rangeStart;
                    sendChangeMessage();
                    repaint();
                }
            }

            xOffset = 0;

            if( canScroll )
            {
                auto scrollButtonW = juce::jmin( scrollButtonWidth, w / 2 );
                auto r             = getLocalBounds();

                if( orientation == horizontalKeyboard )
                {
                    scrollDown->setBounds( r.removeFromLeft( scrollButtonW ) );
                    scrollUp->setBounds( r.removeFromRight( scrollButtonW ) );
                }
                else if( orientation == verticalKeyboardFacingLeft )
                {
                    scrollDown->setBounds( r.removeFromTop( scrollButtonW ) );
                    scrollUp->setBounds( r.removeFromBottom( scrollButtonW ) );
                }
                else
                {
                    scrollDown->setBounds( r.removeFromBottom( scrollButtonW ) );
                    scrollUp->setBounds( r.removeFromTop( scrollButtonW ) );
                }

                auto endOfLastKey = getKeyPos( rangeEnd ).getEnd();

                float mousePositionVelocity;
                auto  spaceAvailable = w;
                auto  lastStartKey   = remappedXYToNote( { endOfLastKey - (float)spaceAvailable, 0 }, mousePositionVelocity ) + 1;

                if( lastStartKey >= 0 && ( (int)firstKey ) > lastStartKey )
                {
                    firstKey = (float)juce::jlimit( rangeStart, rangeEnd, lastStartKey );
                    sendChangeMessage();
                }

                xOffset = getKeyPos( (int)firstKey ).getStart();
            }
            else
            {
                firstKey = (float)rangeStart;
            }
            scrollDown->setVisible( canScroll && firstKey > (float)rangeStart );
            scrollUp->setVisible( canScroll && getKeyPos( rangeEnd ).getStart() > (float)w );

            setKeyWidth( w / 128 );

            repaint();
        }
    }

    juce::Rectangle<float> KeyboardComponent::getRectangleForKey( int note ) const
    {
        jassert( note >= rangeStart && note <= rangeEnd );

        auto pos = getKeyPos( note );
        auto x   = pos.getStart();
        auto w   = pos.getLength();

        if( juce::MidiMessage::isMidiNoteBlack( note ) )
        {
            auto blackNoteLength = getBlackNoteLength();

            switch( orientation )
            {
                case horizontalKeyboard: return { x, 0, w, blackNoteLength };
                case verticalKeyboardFacingLeft: return { getWidth() - blackNoteLength, x, blackNoteLength, w };
                case verticalKeyboardFacingRight: return { 0, getHeight() - x - w, blackNoteLength, w };
                default: jassertfalse; break;
            }
        }
        else
        {
            w *= getKeyWidthModifier( note );
            switch( orientation )
            {
                case horizontalKeyboard: return { x, 0, w, (float)getHeight() };
                case verticalKeyboardFacingLeft: return { 0, x, (float)getWidth(), w };
                case verticalKeyboardFacingRight: return { 0, getHeight() - x - w, (float)getWidth(), w };
                default: jassertfalse; break;
            }
        }

        return {};
    }

    float KeyboardComponent::getKeyWidthModifier( int midiNoteNumber ) const noexcept
    {
        auto name = juce::MidiMessage::getMidiNoteName( midiNoteNumber, true, false, 3 );

        if( name == "C" || name == "E" || name == "F" || name == "B" )
            return 1.5f;
        if( name == "D" || name == "G" || name == "A" )
            return 2;

        return 1;
    }

    void KeyboardComponent::setBlackNoteWidthProportion( float ratio ) noexcept
    {
        jassert( ratio >= 0.0f && ratio <= 1.0f );

        if( blackNoteWidthRatio != ratio )
        {
            blackNoteWidthRatio = ratio;
            resized();
        }
    }

    float KeyboardComponent::getBlackNoteLength() const noexcept
    {
        auto whiteNoteLength = orientation == horizontalKeyboard ? getHeight() : getWidth();
        return (float)whiteNoteLength * blackNoteLengthRatio;
    }

    juce::Range<float> KeyboardComponent::getKeyPos( int midiNoteNumber ) const
    {
        return getKeyPosition( midiNoteNumber, keyWidth )
               - xOffset
               - getKeyPosition( rangeStart, keyWidth ).getStart();
    }

    void KeyboardComponent::setLowestVisibleKey( int noteNumber )
    {
        setLowestVisibleKeyFloat( (float)noteNumber );
    }

    void KeyboardComponent::setLowestVisibleKeyFloat( float noteNumber )
    {
        noteNumber = juce::jlimit( (float)rangeStart, (float)rangeEnd, noteNumber );

        if( noteNumber != firstKey )
        {
            bool hasMoved = ( ( (int)firstKey ) != (int)noteNumber );
            firstKey      = noteNumber;

            if( hasMoved )
                sendChangeMessage();

            resized();
        }
    }

    void KeyboardComponent::setScrollButtonsVisible( bool newCanScroll )
    {
        if( canScroll != newCanScroll )
        {
            canScroll = newCanScroll;
            resized();
        }
    }

    void KeyboardComponent::colourChanged()
    {
        setOpaque( findColour( whiteNoteColourId ).isOpaque() );
        repaint();
    }

    float KeyboardComponent::getKeyStartPosition( int midiNoteNumber ) const
    {
        return getKeyPos( midiNoteNumber ).getStart();
    }

    float KeyboardComponent::getTotalKeyboardWidth() const noexcept
    {
        return getKeyPos( rangeEnd ).getEnd();
    }

    int KeyboardComponent::getNoteAtPosition( juce::Point<float> p )
    {
        float v;
        return xyToNote( p, v );
    }

    int KeyboardComponent::xyToNote( juce::Point<float> pos, float& mousePositionVelocity )
    {
        if( !reallyContains( pos.toInt(), false ) )
            return -1;

        auto p = pos;

        if( orientation != horizontalKeyboard )
        {
            p = { p.y, p.x };

            if( orientation == verticalKeyboardFacingLeft )
                p = { p.x, (float)getWidth() - p.y };
            else
                p = { (float)getHeight() - p.x, p.y };
        }

        return remappedXYToNote( p + juce::Point<float>( xOffset, 0 ), mousePositionVelocity );
    }

    int KeyboardComponent::remappedXYToNote( juce::Point<float> pos, float& mousePositionVelocity ) const
    {
        auto blackNoteLength = getBlackNoteLength();

        if( pos.getY() < blackNoteLength )
        {
            for( int octaveStart = 12 * ( rangeStart / 12 ); octaveStart <= rangeEnd; octaveStart += 12 )
            {
                for( int i = 0; i < 5; ++i )
                {
                    auto note = octaveStart + blackNotes[i];

                    if( note >= rangeStart && note <= rangeEnd )
                    {
                        if( getKeyPos( note ).contains( pos.x - xOffset ) )
                        {
                            mousePositionVelocity = juce::jmax( 0.0f, pos.y / blackNoteLength );
                            return note;
                        }
                    }
                }
            }
        }

        for( int octaveStart = 12 * ( rangeStart / 12 ); octaveStart <= rangeEnd; octaveStart += 12 )
        {
            for( int i = 0; i < 7; ++i )
            {
                auto note = octaveStart + whiteNotes[i];

                if( note >= rangeStart && note <= rangeEnd )
                {
                    if( getKeyPos( note ).contains( pos.x - xOffset ) )
                    {
                        auto whiteNoteLength  = ( orientation == horizontalKeyboard ) ? getHeight() : getWidth();
                        mousePositionVelocity = juce::jmax( 0.0f, pos.y / (float)whiteNoteLength );
                        return note;
                    }
                }
            }
        }

        mousePositionVelocity = 0;
        return -1;
    }

    void KeyboardComponent::repaintNote( int noteNum )
    {
        if( noteNum >= rangeStart && noteNum <= rangeEnd )
            repaint( getRectangleForKey( noteNum ).getSmallestIntegerContainer() );
    }
}
