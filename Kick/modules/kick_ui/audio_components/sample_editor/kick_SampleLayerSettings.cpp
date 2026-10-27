#include "kick_SampleLayerSettings.h"
namespace mt
{
    namespace ac
    {
#define layerSettingsLabelWidth   160
#define layerSettingsLabelHeight  16
#define layerSettingsButtonWidth  160
#define layerSettingsButtonHeight 30

        SampleLayerSettings::SampleLayerSettings( SampleEditorData& data )
            : seData( data )
        {
            utils::addAndMakeVisible( this, linkSlices, looplock, zeroSnap );
            utils::addAndMakeVisible( this, linkOn, linkOff, loopOn, loopOff, zeroOn, zeroOff );
            utils::addListener( this, linkOn, linkOff, loopOn, loopOff, zeroOn, zeroOff );
            utils::setRadioGroupID( 1, linkOn, linkOff, loopOn, loopOff, zeroOn, zeroOff );

            refresh();
            setSize( 365, 255 );
        }

        SampleLayerSettings::~SampleLayerSettings()
        {
        }

        void SampleLayerSettings::paint( juce::Graphics& g )
        {
            g.setColour( ui::Theme::outlineColour( 2 ) );
            g.drawLine( 20.0f, linkOn.getBottom() + 16.0f, getWidth() - 20.0f, linkOn.getBottom() + 16.0f, 0.5f );
            g.drawLine( 20.0f, loopOn.getBottom() + 16.0f, getWidth() - 20.0f, loopOn.getBottom() + 16.0f, 0.5f );
        }

        void SampleLayerSettings::resized()
        {
            linkSlices.setBounds( 20, 10, layerSettingsLabelWidth, layerSettingsLabelHeight );
            linkOn.setBounds( 20, linkSlices.getBottom() + 5, layerSettingsButtonWidth, layerSettingsButtonHeight );
            linkOff.setBounds( linkOn.getRight() + 5, linkSlices.getBottom() + 5, layerSettingsButtonWidth, layerSettingsButtonHeight );

            looplock.setBounds( 20, linkOff.getBottom() + 30, layerSettingsLabelWidth, layerSettingsLabelHeight );
            loopOn.setBounds( 20, looplock.getBottom() + 5, layerSettingsButtonWidth, layerSettingsButtonHeight );
            loopOff.setBounds( loopOn.getRight() + 5, looplock.getBottom() + 5, layerSettingsButtonWidth, layerSettingsButtonHeight );

            zeroSnap.setBounds( 20, loopOff.getBottom() + 30, layerSettingsLabelWidth, layerSettingsLabelHeight );
            zeroOn.setBounds( 20, zeroSnap.getBottom() + 5, layerSettingsButtonWidth, layerSettingsButtonHeight );
            zeroOff.setBounds( zeroOn.getRight() + 5, zeroSnap.getBottom() + 5, layerSettingsButtonWidth, layerSettingsButtonHeight );
        }

        void SampleLayerSettings::buttonClicked( juce::Button* button )
        {
            if( button == &linkOn || button == &linkOff )
            {
                seData.setLinkSlices( linkOn.getToggleState() );
            }
            else if( button == &loopOn || button == &loopOff )
            {
                seData.setLoopLock( loopOn.getToggleState() );
            }
            else if( button == &zeroOn || button == &zeroOff )
            {
                seData.enableZeroCrossing( zeroOn.getToggleState() );
            }
        }

        void SampleLayerSettings::refresh()
        {
            linkOn.setToggleState( seData.getLinkSlices(), juce::dontSendNotification );
            linkOff.setToggleState( !seData.getLinkSlices(), juce::dontSendNotification );

            loopOn.setToggleState( seData.getLoopLock(), juce::dontSendNotification );
            loopOff.setToggleState( !seData.getLoopLock(), juce::dontSendNotification );

            zeroOn.setToggleState( seData.isZeroCrossingEnabled(), juce::dontSendNotification );
            zeroOff.setToggleState( !seData.isZeroCrossingEnabled(), juce::dontSendNotification );
        }

    }
}
