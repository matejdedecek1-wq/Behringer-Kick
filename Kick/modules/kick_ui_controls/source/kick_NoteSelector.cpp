namespace ui
{
    NoteSelectorEditor::NoteSelectorEditor()
    {
        mt::utils::addAndMakeVisible( this, textEditor, inc, dec, slider, sharp, flat, startAt, style, defaultOctave1, defaultOctave2, octave, note );
        inc.setConnectedEdges( (int)juce::Button::ConnectedEdgeFlags::ConnectedOnRight );
        dec.setConnectedEdges( (int)juce::Button::ConnectedEdgeFlags::ConnectedOnLeft );

        textEditor.setFont( 24 );
        textEditor.setColour( juce::TextEditor::backgroundColourId, ui::Theme::backgroundColour( 13 ) );
        textEditor.setColour( juce::TextEditor::textColourId, ui::Theme::textColour( 2 ) );
        textEditor.setCaretVisible( true );
        textEditor.setFocusContainerType( juce::Component::FocusContainerType::focusContainer );
        textEditor.setBorder( { 22, 2, 20, 2 } );
        textEditor.setJustification( juce::Justification::centred );
        textEditor.setText( noteName );

        mt::utils::setValueBackgroundColour( ui::Theme::backgroundColour( 9 ), startAt, style, octave, note );
        mt::utils::setValueJustification( juce::Justification::centred, startAt, style );
        mt::utils::setPlacementFlag( juce::Justification::centred, inc, dec );
        mt::utils::setValueFontHeight( 16, sharp, flat, defaultOctave1, defaultOctave2, startAt, style, octave, note );
        mt::utils::setRadioGroupID( (int)kick_ui::RadioGroupId::DefaultOctaves, defaultOctave1, defaultOctave2 );
        mt::utils::setRadioGroupID( (int)kick_ui::RadioGroupId::NoteSelector, sharp, flat );

        auto& octaveList = mt::ScaleAndNotes::octave;
        for( auto id = 0; id < octaves.size(); ++id )
        {
            mt::utils::addAndMakeVisible( this, octaves[id] );
            octaves[id].setToggleButtonText( octaveList[id], octaveList[id] );
            octaves[id].setValueColour( ui::Theme::textColour( 1 ) );
            octaves[id].setValueFontHeight( 18 );
            mt::utils::setRadioGroupID( (int)kick_ui::RadioGroupId::NoteSelectorOctaves, octaves[id] );
            octaves[id].onClick = [&, id]() {
                // Update the Value
                tempOct = id;
                refresh();
            };
        }

        for( auto id = 0; id < rootNotes.size(); ++id )
        {
            mt::utils::addAndMakeVisible( this, rootNotes[id] );
            rootNotes[id].setValueColour( ui::Theme::textColour( 1 ) );
            rootNotes[id].setValueFontHeight( 18 );
            mt::utils::setRadioGroupID( (int)kick_ui::RadioGroupId::NoteSelectorRootNotes, rootNotes[id] );
            rootNotes[id].onClick = [&, id]() {
                // Update the Value
                tempRoot = id;
                refresh();
            };
        }
        sharp.onClick = [&]() {
            for( auto id = 0; id < rootNotes.size(); ++id )
                rootNotes[id].setToggleButtonText( rootNoteSharp[id], rootNoteSharp[id] );
            refresh();
        };

        flat.onClick = [&]() {
            for( auto id = 0; id < rootNotes.size(); ++id )
                rootNotes[id].setToggleButtonText( rootNoteFlat[id], rootNoteFlat[id] );
            refresh();
        };

        sharp.triggerClick();
        octaves[0].triggerClick();
        rootNotes[0].triggerClick();
    }

    void NoteSelectorEditor::refresh()
    {
        auto text = rootNotes[tempRoot].getButtonText() + octaves[tempOct].getButtonText();
        textEditor.setText( text );
    }

    void NoteSelectorEditor::resized()
    {
        auto b = getLocalBounds();
        // text label
        b.removeFromTop( buttonSpace1 );
        textEditor.setBounds( b.removeFromTop( inputTextHeight ).reduced( spaceAround1, 0 ) );

        //// Row 1 -? inc, dec, slider.
        b.removeFromTop( buttonSpace1 );
        auto row1 = b.removeFromTop( buttonHeight2 ).reduced( spaceAround1, 0 );
        dec.setBounds( row1.removeFromLeft( buttonWidth3 ) );
        row1.removeFromRight( 3 );
        inc.setBounds( row1.removeFromRight( buttonWidth3 ) );
        slider.setBounds( row1.reduced( buttonSpace2, 0 ) );
        b.removeFromTop( 10 );
        auto optionX = .3125f;
        // Octaves
        {
            startAt.setBoundsRelative( .1787f, .4226f, .0687f, .054f );
            style.setBoundsRelative( .19f, .7371f, .0462f, .054f );
            octave.setBoundsRelative( .025f, .5356f, .0825f, .054f );
            note.setBoundsRelative( .025f, .855f, .0825f, .054f );
            sharp.setBoundsRelative( .1625f, .8157f, .1f, .0835f );
            flat.setBoundsRelative( .1625f, .9238f, .1f, .0786f );
            defaultOctave2.setBoundsRelative( .1625f, .5061f, .1f, .0835f );
            defaultOctave1.setBoundsRelative( .1625f, .6142f, .1f, .0835f );

            for( auto id = 0; id < octaves.size(); ++id )
            {
                if( id <= 5 )
                {
                    octaves[id].setBoundsRelative( optionX, .4226f, .1f, .1203f );
                    optionX += .1125f;
                    if( id == 5 )
                        optionX = .3125f;
                }
                else
                {
                    octaves[id].setBoundsRelative( optionX, .57f, .1f, .1203f );
                    optionX += .1125f;
                }
            }
        }
        optionX = .3125f;
        //root notes
        for( auto id = 0; id < rootNotes.size(); ++id )
        {
            if( id <= 5 )
            {
                rootNotes[id].setBoundsRelative( optionX, .7371f, .1f, .113f );
                optionX += .1125f;
                if( id == 5 )
                    optionX = .3125f;
            }
            else
            {
                rootNotes[id].setBoundsRelative( optionX, .8845f, .1f, .113f );
                optionX += .1125f;
            }
        }
    }

    void NoteSelectorEditor::paint( juce::Graphics& g )
    {
        g.setColour( ui::Theme::textColour( 3 ).withMultipliedBrightness( .5 ) );
        g.drawLine( textBounds1.getX(), textBounds1.getY(), textBounds1.getWidth(), textBounds1.getY(), 1 );
        g.drawLine( textBounds2.getX(), textBounds2.getY(), textBounds2.getWidth(), textBounds2.getY(), 1 );
    }

    void NoteSelectorEditor::show( const juce::String& Title, std::function<void( int )> CallbackOnExit )
    {
        refresh();
        octaves[octaveIndex].triggerClick();
        rootNotes[rootNoteIndex].triggerClick();
        mt::utils::showOptions(
            Title, this, [this, CallbackOnExit]( int R ) {
                if( R == UI::APPLY )
                {
                    rootNoteIndex = tempRoot;
                    octaveIndex   = tempOct;
                    noteName      = rootNotes[rootNoteIndex].getButtonText() + octaves[octaveIndex].getButtonText();
                    noteValue     = sharp.getToggleState() ? mt::ScaleAndNotes::getNoteNumber( noteName ) : mt::Note::getRootNumber( noteName );
                }
                else
                {
                    tempRoot = rootNoteIndex;
                    tempOct  = octaveIndex;
                }
                if( CallbackOnExit )
                    CallbackOnExit( R );
            },
            ui::layout::keyBoardPopup );
    }

    KickNoteSelector::KickNoteSelector( const juce::String Name )
    {
        setTitle( Name );
        setText( mt::ScaleAndNotes::getNoteName( noteValue ), juce::dontSendNotification );
        setValueJustification( juce::Justification::centred );
        setTitleJustification( juce::Justification::centred );
        setValueFontHeight( 18 );
        setTitleFontHeight( 16 );
        setTitleProportion( .35f );
        setValueBackgroundColour( ui::Theme::backgroundColour( 13 ) );
    }

    void KickNoteSelector::mouseDoubleClick( const juce::MouseEvent& e )
    {
        if( isVisible() && !isEditing )
        {
            isEditing        = true;
            auto& noteEditor = NoteSelectorEditor::Instance();
            noteEditor.setNote( noteValue );
            noteEditor.show( "NOTE SELECTOR", [&]( int R ) {
                isEditing = false;
                if( R == UI::APPLY )
                {
                    noteValue = noteEditor.getNoteNumber();
                    setText( noteEditor.getNoteName(), juce::dontSendNotification );
                    if( onNoteChanged )
                        onNoteChanged( noteValue );
                }
            } );
        }
    }
}
