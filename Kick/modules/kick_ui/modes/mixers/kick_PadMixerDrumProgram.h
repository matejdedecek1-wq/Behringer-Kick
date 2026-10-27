
#include "kick_MixerHandlerInterface.h"

#pragma once
namespace ui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////PadMixersDrumProgramHandler
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class PadMixersDrumProgramHandler : public MixerHandlerInterface
    {
    public:
        std::vector<juce::Component*>     getIoOptions1() override { return { &buttons[0], &buttons[1] }; };
        virtual std::vector<juce::String> getInputs() override { return { "Input1", "Input2" }; };
        virtual std::vector<juce::String> getOutputs() override { return { "Output1", "Output2" }; };

        explicit PadMixersDrumProgramHandler( MixerTabView* View )
            : MixerHandlerInterface( View )
        {
            tabData.push_back( { "Level", 1, { KickPadButton::PadButtonType::levelMode } } );
            tabData.push_back( { "Pan", 1, { KickPadButton::PadButtonType::panMode } } );
            tabData.push_back( { "Mute", 1, { KickPadButton::PadButtonType::muteMode, KickPadButton::PadButtonType::muteMode, KickPadButton::PadButtonType::muteMode } } );
            //tabData.push_back( { "Send", 3, { KickPadButton::PadButtonType::insertMode, KickPadButton::PadButtonType::insertMode, KickPadButton::PadButtonType::insertMode } } );
            //tabData.push_back( { "Insert", 3, { KickPadButton::PadButtonType::insertMode, KickPadButton::PadButtonType::insertMode, KickPadButton::PadButtonType::insertMode } } );
            //tabData.push_back( { "Route", 1, { KickPadButton::PadButtonType::insertMode } } );

            auto& solo = buttons[0];
            auto& mute = buttons[1];

            solo.setToggleButtonText( "Solo", "Solo" );
            mute.setToggleButtonText( "Mute", "Mute" );

            pads.OnButtonClicked = [&]( int Index ) {
                selectedPad = Index;
                pads.setSelected( selectedPad, juce::dontSendNotification );

                //solo.setToggleState( pads.getS( Index ), dontSendNotification );
                mute.setToggleState( pads.getMute( Index ), juce::dontSendNotification );
                io.setPan( pads.getPan( Index ) );
                io.setGain( CommonUtils::midiToDb( pads.getValue( Index ) ) );
            };

            pads.onSliderChanged = [&]( int Index, double Value ) {
                int level = std::round<int>( Value );
                ae.setLevelForSelectedProgram( Index, level );
                io.setGain( CommonUtils::midiToDb( level ) );
            };

            pads.onPanChanged = [&]( int Index, double Value ) {
                ae.setPanForSelectedProgram( Index, Value );
                io.setPan( Value );
            };

            pads.onMuteChanged = [&]( int Index, bool Value ) {
                ae.setMuteForSelectedProgram( Index, Value );
            };

            io.gainChanged.connect( [&]( float Value ) {
                int gain = std::round<int>( CommonUtils::dbToMidi( Value ) );
                pads.setValue( selectedPad, gain, juce::sendNotification );
            } );

            io.panSliderChanged.connect( [&]( double Value ) {
                pads.setPan( selectedPad, Value, juce::sendNotification );
            } );

            solo.onClick = [&]() {
                //pads.setMute( selectedPad, mute.getToggleState() );
            };

            mute.onClick = [&]() {
                pads.setMute( selectedPad, mute.getToggleState(), juce::sendNotification );
            };
        }

        std::vector<juce::String> getButtonLabels()
        {
            return ae.getProgramSampleNames( ae.getSelectedProgram() );
        }

        void refreshContent() override
        {
            auto progType = ae.getProgramType( ae.getSelectedProgram() );
            if( progType == mt::MTProgramType::Drum )
            {
                pads.clearAllPadData();
                pads.setPrefix( "DR" );

                ae.getProgramSamples( ae.getSelectedProgram(), samples );
                for( auto s : samples )
                {
                    pads.setActive( s->keyNote.get(), true );
                    pads.setValue( s->keyNote.get(), CommonUtils::gainToMidi( s->gain.get() ), juce::dontSendNotification );
                    pads.setPan( s->keyNote.get(), ( s->pan.get() ), juce::dontSendNotification );
                    pads.setMute( s->keyNote.get(), ( s->mute ), juce::dontSendNotification );
                    auto ss = juce::File( s->getSource( 0 ) ).getFileNameWithoutExtension();
                    pads.setRow1Text( s->keyNote.get(), ss, juce::dontSendNotification );
                }
            }
        }

    private:
        std::vector<mt::MTISample*> samples;
    };

}
