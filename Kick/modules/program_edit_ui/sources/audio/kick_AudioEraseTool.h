#pragma once
namespace kick_ui
{
    class AudioGridEraseTool : public IAudioTools
    {
    public:
        AudioGridEraseTool( AudioGridComponent&, AudioGridData& GridData, juce::Viewport& ViewPort );

        void paint( juce::Graphics& g ) override;
        void mouseUp( const juce::MouseEvent& ) override;
        void mouseDrag( const juce::MouseEvent& ) override;
    };

}
