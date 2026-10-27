#include "kick_SaveSampleDialog.h"
namespace kick_ui
{
    SaveSampleDialog::SaveSampleDialog()
    {
        setSize( 512, 313 );

        mt::utils::addAndMakeVisible( this, sampleName, saveOptions, ok, cancel );
        mt::utils::addListener( this, ok, cancel );
    }

    void SaveSampleDialog::buttonClicked( juce::Button* button )
    {
        juce::DialogWindow* dw = findParentComponentOfClass<juce::DialogWindow>();
        if( dw == nullptr )
            return;

        if( button == &ok )
            dw->exitModalState( UI::BUTTONS::APPLY );
        else if( button == &cancel )
            dw->exitModalState( UI::BUTTONS::CANCEL );
    }

    void SaveSampleDialog::resized()
    {
        sampleName.setBounds( 20, 48, 471, 50 );
        saveOptions.setBounds( 21, 118, 471, 50 );
        ok.setBounds( 21, 208, 160, 34 );
        cancel.setBounds( 332, 208, 160, 34 );
    }

}
