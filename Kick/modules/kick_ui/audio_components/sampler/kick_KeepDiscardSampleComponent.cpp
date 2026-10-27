
namespace kick_ui
{
    KeepDiscardSampleComponent::KeepDiscardSampleComponent( int ButtonState )
        : buttonState( ButtonState )
    {
        mt::utils::addAndMakeVisible( this, sampleName, programs, assignPad, rootNote, programLabel, padLabel, rootNoteLabel, createNewPgmLabel, createNewProgram );
        mt::utils::makeHidden( createNewPgmLabel, createNewProgram );

        mt::utils::setValueColour( ui::Theme::textColour( 1 ), programLabel, padLabel, rootNoteLabel, createNewPgmLabel );
        mt::utils::setValueBackgroundColour( ui::Theme::backgroundColour( 9 ), programLabel, padLabel, rootNoteLabel, createNewPgmLabel );
        mt::utils::setValueFontHeight( 16, programLabel, padLabel, rootNoteLabel, createNewPgmLabel );
        mt::utils::setTitleJustification( juce::Justification::centredTop, sampleName );
        mt::utils::setValueBackgroundColour( ui::Theme::backgroundColour( 3 ), sampleName, rootNote );

        rootNote.showDropdown( true );
        rootNote.setTitleColour( ui::Theme::textColour( 1 ) );
        rootNote.setPadding( 10 );

        sampleName.setTitleColour( ui::Theme::textColour( 1 ) );
        sampleName.setText( "New Sample", juce::dontSendNotification );
        sampleName.setTitleFontHeight( 16 );
        sampleName.setValueFontHeight( 20 );
        sampleName.setValueJustification( juce::Justification::centred );
        sampleName.setEditable( false, true );
        int letterAsciiCode = 65;
        padData.clear();
        for( int i = 0; i <= kick_Constants::totalPianoKeys; i += 16 )
        {
            for( int j = 0; j < 16; j++ )
            {
                juce::String noteLabel;
                noteLabel = noteLabel.formatted( "%c%02d", letterAsciiCode, ( j + 1 ) );
                padData.push_back( noteLabel );
            }
            letterAsciiCode++;
        }
        assignPad.setListOptions( padData );

        progList.clear();
        progList.push_back( "None" );
        auto progs = ae.getProgramList();
        for( auto prog : progs )
            progList.push_back( prog );
        programs.setListOptions( progList, progList.size() > 1 ? 1 : 0 );

        programs.onSelectionChanged = [&]( int Index, const juce::String& Program ) {
            mt::utils::setEnabled( !( Index == 0 ), assignPad, padLabel );
            if( Index != 0 )
                ae.setSelectedProgram( Index - 1 );
        };

        if( programs.getSize() == 1 || buttonState == 1 )
        {
            mt::utils::makeVisible( createNewPgmLabel, createNewProgram );
            mt::utils::makeHidden( programs, assignPad, rootNote, programLabel, padLabel, rootNoteLabel );
        }
        else if( buttonState == 2 || buttonState == 3 )
            mt::utils::makeHidden( programs, assignPad, rootNote, programLabel, padLabel, rootNoteLabel, createNewPgmLabel, createNewProgram );
    }

    void KeepDiscardSampleComponent::resized()
    {
        sampleName.setBoundsRelative( .167f, .061f, .665f, .288f );
        programLabel.setBoundsRelative( .1892f, .4545f, .1846f, .1081f );
        programs.setBoundsRelative( .3892f, .4545f, .423f, .1081f );
        createNewPgmLabel.setBoundsRelative( .1815f, .4545f, .2769f, .1081f );
        createNewProgram.setBoundsRelative( .3923f, .4545f, .423f, .1081f );
        padLabel.setBoundsRelative( .1892f, .6142f, .1846f, .1081f );
        assignPad.setBoundsRelative( .3892f, .6142f, .423f, .1081f );
        rootNoteLabel.setBoundsRelative( .1892f, .7739f, .423f, .1081f );
        rootNote.setBoundsRelative( .3892f, .7739f, .423f, .1081f );
    }
}
