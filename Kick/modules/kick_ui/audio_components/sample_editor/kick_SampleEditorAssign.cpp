namespace mt
{
    namespace ac
    {
        SampleEditorAssign::SampleEditorAssign( SampleEditorData& d )
            : seData( d )
        {
            addAndMakeVisible( supportedActionsCombo );
            addAndMakeVisible( cancel );

            supportedActionsCombo.addListener( this );
            cancel.addListener( this );

            std::vector<juce::String>& supportedActions = seData.getSupportedActions();
            int                        index            = 1;
            for( juce::String& action : supportedActions )
            {
                supportedActionsCombo.addItem( action, index );
                index++;
            }
            setSize( 240, 100 );
        }

        void SampleEditorAssign::resized()
        {
            supportedActionsCombo.setBounds( 10, 10, 100, 30 );
            cancel.setBounds( 10, supportedActionsCombo.getBottom() + 5, 90, 30 );
        }

        void SampleEditorAssign::comboBoxChanged( juce::ComboBox* comboBoxThatHasChanged )
        {
            selectedId             = comboBoxThatHasChanged->getSelectedId();
            juce::DialogWindow* dw = findParentComponentOfClass<juce::DialogWindow>();
            if( dw == nullptr )
                return;

            dw->exitModalState( UI::BUTTONS::APPLY );
        }

        void SampleEditorAssign::buttonClicked( juce::Button* /*button*/ )
        {
            juce::DialogWindow* dw = findParentComponentOfClass<juce::DialogWindow>();
            if( dw == nullptr )
                return;

            dw->exitModalState( UI::BUTTONS::CANCEL );
        }

    }

}
