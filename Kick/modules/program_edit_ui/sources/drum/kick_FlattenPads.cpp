#include "kick_FlattenPads.h"

namespace kick_ui
{
    FlattenPads::FlattenPads( std::vector<int>& Pads )
        : pads( Pads )
    {
        pad.setListOptions( mt::Kick::padNames );
        pad.setSelected( 0 );
        mt::utils::addAndMakeVisible( this, pad, audioTail, sampleName, sampleNameEdit );
        apply.makeVisibleAndAddListner( this );
        cancel.makeVisibleAndAddListner( this );
        sampleNameEdit.setEditable( true );

        setSize( 511, 313 );
    }

    void FlattenPads::resized()
    {
        pad.setBounds( 41, 43, 80, 48 );
        audioTail.setBounds( pad.getRight() + 60, 43, 80, 48 );
        sampleName.setBounds( 20, pad.getBottom() + 48, 471, 16 );
        sampleNameEdit.setBounds( 20, sampleName.getBottom() + 3, 471, 50 );
        apply.setBounds( 20, getHeight() - 20 - 34, 160, 34 );
        cancel.setBounds( 332, getHeight() - 20 - 34, 160, 34 );
    }

    void FlattenPads::buttonClicked( juce::Button* )
    {
    }

    FlattenPads::~FlattenPads()
    {
    }

    void FlattenPads::paint( juce::Graphics& g )
    {
        g.setColour( ui::Theme::outlineColour( 2 ) );
        g.drawLine( 20, 29, 20, 100 );
        g.drawLine( 161, 29, 161, 100 );
    }

    void FlattenPads::refresh()
    {
    }
}
