namespace kick_ui
{
    HeaderBase::HeaderBase()
    {
        addAndMakeVisible( progLabel );
        progLabel.setText( "Drum Program", juce::dontSendNotification );
    }

    void HeaderBase::resized()
    {
        progLabel.setBounds( 0, 0, 340, 40 );
    }
}
