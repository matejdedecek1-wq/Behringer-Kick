#include "kick_TimeSigSetting.h"

namespace mt
{
    TimeSigSetting::TimeSigSetting( int tsNum, int tsDen, bool hasOnlyOneBar )
    {
        addAndMakeVisible( &timeSigNumerator );
        timeSigNumerator.setMultiLine( false );
        timeSigNumerator.setReturnKeyStartsNewLine( false );
        timeSigNumerator.setReadOnly( false );
        timeSigNumerator.setScrollbarsShown( true );
        timeSigNumerator.setCaretVisible( true );
        timeSigNumerator.setPopupMenuEnabled( true );
        juce::String numVal = tsNum == -1 ? "4" : juce::String( tsNum );
        timeSigNumerator.setText( numVal );
        timeSigNumerator.setBounds( 48, 10, 24, 24 );
        timeSigNumerator.setInputRestrictions( 10, "0123456789" );

        addAndMakeVisible( &timeSigDenominator );
        timeSigDenominator.setMultiLine( false );
        timeSigDenominator.setReturnKeyStartsNewLine( false );
        timeSigDenominator.setReadOnly( false );
        timeSigDenominator.setScrollbarsShown( true );
        timeSigDenominator.setCaretVisible( true );
        timeSigDenominator.setPopupMenuEnabled( true );
        juce::String denVal = tsDen == -1 ? "4" : juce::String( tsDen );
        timeSigDenominator.setText( denVal );

        timeSigDenominator.setInputRestrictions( 10, "0123456789" );

        addAndMakeVisible( &labelSlash );
        labelSlash.setText( "/", juce::NotificationType::dontSendNotification );
        labelSlash.setJustificationType( juce::Justification::centredLeft );
        labelSlash.setEditable( false, false, false );
        labelSlash.setColour( juce::TextEditor::textColourId, ui::Theme::backgroundColour( 5 ) );
        labelSlash.setColour( juce::TextEditor::backgroundColourId, juce::Colour( 0x00000000 ) );

        addAndMakeVisible( &cancelButton );
        cancelButton.setButtonText( TRANS( "cancel" ) );
        cancelButton.addListener( this );

        addAndMakeVisible( &deleteButton );
        deleteButton.setButtonText( TRANS( "Delete" ) );
        deleteButton.addListener( this );

        addAndMakeVisible( &addButton );
        addButton.setButtonText( TRANS( "Add" ) );
        addButton.addListener( this );

        if( tsNum != -1 && !hasOnlyOneBar )
        {
            addButton.setButtonText( TRANS( "Ok" ) );
        }
        else
        {
            deleteButton.setVisible( false );
        }

        setSize( 240, 100 );
    }

    TimeSigSetting::~TimeSigSetting()
    {
        cancelButton.removeListener( this );
        deleteButton.removeListener( this );
        addButton.removeListener( this );
    }

    void TimeSigSetting::paint( juce::Graphics& g )
    {
        g.fillAll( juce::Colour( 0xff323e44 ) );
    }

    void TimeSigSetting::resized()
    {
        timeSigDenominator.setBounds( 112, 10, 24, 24 );
        labelSlash.setBounds( 88, 10, 15, 24 );
        cancelButton.setBounds( 5, 60, 70, 24 );
        deleteButton.setBounds( 80, 60, 70, 24 );
        addButton.setBounds( 155, 60, 70, 24 );
    }

    void TimeSigSetting::buttonClicked( juce::Button* buttonThatWasClicked )
    {
        juce::DialogWindow* dw = findParentComponentOfClass<juce::DialogWindow>();
        if( dw == nullptr )
            return;

        if( buttonThatWasClicked == &addButton )
        {
            dw->exitModalState( UI::BUTTONS::APPLY );
        }
        else if( buttonThatWasClicked == &cancelButton )
        {
            dw->exitModalState( UI::BUTTONS::CANCEL );
        }
        else if( buttonThatWasClicked == &deleteButton )
        {
            dw->exitModalState( UI::BUTTONS::DELETE );
        }
    }
    juce::String TimeSigSetting::getNumerator()
    {
        return timeSigNumerator.getText();
    }
    juce::String TimeSigSetting::getDenominator()
    {
        return timeSigDenominator.getText();
    }
}
