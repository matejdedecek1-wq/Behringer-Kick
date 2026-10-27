
#include "kick_MixerHandlerInterface.h"

#pragma once
namespace ui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////PadMixerKeygroupProgramHandler
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class PadMixerKeygroupProgramHandler : public MixerHandlerInterface
    {
    public:
        std::vector<juce::Component*> getIoOptions1() override { return { &buttons[0], &buttons[1] }; };
        std::vector<juce::String>     getInputs() override { return { "Input1", "Input2" }; };
        std::vector<juce::String>     getOutputs() override { return { "Output1", "Output2" }; };

        explicit PadMixerKeygroupProgramHandler( MixerTabView* View )
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

                mute.setToggleState( pads.getMute( Index ), juce::dontSendNotification );
                io.setPan( pads.getPan( Index ) );
                //io.setGain( CommonUtils::midiToDb( pads.getValue( Index ) ) );
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
        }

        void refreshContent() override
        {
            auto prog = ae.getProgram( ae.getSelectedProgram() );
            if( prog && prog->type == mt::MTProgramType::Keygroup )
            {
                pads.clearAllPadData();
                pads.setPrefix( "KG" );

                ae.getProgramSamples( ae.getSelectedProgram(), samples );
                for( auto s : samples )
                {
                    auto note = s->keyNote.get();
                    //for( auto note = s.minNote; note <= s.maxNote; ++note )
                    {
                        pads.setActive( note, true );
                        pads.setValue( note, CommonUtils::gainToMidi( s->gain.get() ), juce::dontSendNotification );
                        pads.setPan( note, ( s->pan.get() ), juce::dontSendNotification );
                        pads.setMute( note, ( s->mute.get() ), juce::dontSendNotification );
                        auto ss = juce::File( s->getSource( 0 ) ).getFileNameWithoutExtension();
                        pads.setRow1Text( note, ss, juce::dontSendNotification );
                    }
                }
            }
        };

    private:
        std::vector<mt::MTISample*> samples;

    }; // Class PadMixerKeygroupProgramHandler
}
