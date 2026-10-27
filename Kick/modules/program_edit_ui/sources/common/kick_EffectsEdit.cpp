#include "kick_EffectsEdit.h"

namespace kick_ui
{

    KickEffectsEdit::KickEffectsEdit( std::vector<int>& Pads )
        : pads( Pads ), KickGrid( 4, 1, true )
    {
        std::vector<juce::String> columTitle( 1 );
        columTitle[0] = "Insert";
        setRowTitles( columTitle );

        std::vector<juce::Component*> columControls( 5 );
        columControls[0] = nullptr;
        columControls[1] = nullptr;
        columControls[2] = nullptr;
        columControls[3] = nullptr;
        columControls[4] = &onAll;
        setTitleControls( columControls );

        addComponent( layerOne, 0, 0, 334 );
        addComponent( layerTwo, 1, 0, 334 );
        addComponent( layerThree, 2, 0, 334 );
        addComponent( layerFour, 3, 0, 334 );

        deleteL1.addListener( this );
        deleteL2.addListener( this );
        deleteL3.addListener( this );
        deleteL4.addListener( this );

        auto sampleFiles = mt::getAudioInterface().getAssets( mt::AssetTypes::Samples );

        layerOne.setListOptions( sampleFiles );
        layerTwo.setListOptions( sampleFiles );
        layerThree.setListOptions( sampleFiles );
        layerFour.setListOptions( sampleFiles );

        layerOne.onValueChanged = [this]( const juce::String& value ) {
            if( value != "None" )
                addLayerOptions( LayerType::L1 );
        };

        layerTwo.onValueChanged = [this]( const juce::String& value ) {
            if( value != "None" )
                addLayerOptions( LayerType::L2 );
        };

        layerThree.onValueChanged = [this]( const juce::String& value ) {
            if( value != "None" )
                addLayerOptions( LayerType::L3 );
        };

        layerFour.onValueChanged = [this]( const juce::String& value ) {
            if( value != "None" )
                addLayerOptions( LayerType::L4 );
        };

        ui::themeChanged.connect( [&]() {
            editL1.setImages( "effect1" );
            editL2.setImages( "effect1" );
            editL3.setImages( "effect1" );
            editL4.setImages( "effect1" );
            deleteL1.setImages( "effect2" );
            deleteL2.setImages( "effect2" );
            deleteL3.setImages( "effect2" );
            deleteL4.setImages( "effect2" );
        } );
    }

    KickEffectsEdit::~KickEffectsEdit()
    {
    }

    void KickEffectsEdit::buttonClicked( juce::Button* button )
    {
        if( button == &deleteL1 )
        {
            removeComponent( &dummy1 );
            removeComponent( &editL1 );
            removeComponent( &deleteL1 );
            removeComponent( &onL1 );
            removeComponent( &sendL1 );
            layerOne.setSelected( 0 );
        }
        else if( button == &deleteL2 )
        {
            removeComponent( &dummy2 );
            removeComponent( &editL2 );
            removeComponent( &deleteL2 );
            removeComponent( &onL2 );
            removeComponent( &sendL2 );
            layerTwo.setSelected( 0 );
            l2added = false;
        }
        else if( button == &deleteL3 )
        {
            removeComponent( &dummy3 );
            removeComponent( &editL3 );
            removeComponent( &deleteL3 );
            removeComponent( &onL3 );
            removeComponent( &sendL3 );
            layerThree.setSelected( 0 );
        }
        else if( button == &deleteL4 )
        {
            removeComponent( &dummy4 );
            removeComponent( &editL4 );
            removeComponent( &deleteL4 );
            removeComponent( &onL4 );
            removeComponent( &sendL4 );
            layerFour.setSelected( 0 );
        }
        resized();
    }

    void KickEffectsEdit::addLayerOptions( LayerType type )
    {
        if( type == LayerType::L1 )
        {
            addComponent( dummy1, 0, 0, 197 );
            addComponent( editL1, 0, 0, 60 );
            addComponent( deleteL1, 0, 0, 60 );
            addComponent( onL1, 0, 0, 60, 60 );
            addComponent( sendL1, 0, 0, 173 );
        }
        else if( type == LayerType::L2 && !l2added )
        {
            addComponent( dummy2, 1, 0, 197 );
            addComponent( editL2, 1, 0, 60 );
            addComponent( deleteL2, 1, 0, 60 );
            addComponent( onL2, 1, 0, 60, 60 );
            addComponent( sendL2, 1, 0, 173 );
            l2added = true;
        }
        else if( type == LayerType::L3 )
        {
            addComponent( dummy3, 2, 0, 197 );
            addComponent( editL3, 2, 0, 60 );
            addComponent( deleteL3, 2, 0, 60 );
            addComponent( onL3, 2, 0, 60, 60 );
            addComponent( sendL3, 2, 0, 173 );
        }
        else if( type == LayerType::L4 )
        {
            addComponent( dummy4, 3, 0, 197 );
            addComponent( editL4, 3, 0, 60 );
            addComponent( deleteL4, 3, 0, 60 );
            addComponent( onL4, 3, 0, 60, 60 );
            addComponent( sendL4, 3, 0, 173 );
        }
        resized();
    }

    void KickEffectsEdit::refresh()
    {
    }

}
