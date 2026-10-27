#pragma once
namespace kick_ui
{
    class AudioGridCutTool : public IAudioTools
    {
    public:
        AudioGridCutTool( AudioGridComponent& GridComp, AudioGridData& GridData, juce::Viewport& ViewPort );

        void paint( juce::Graphics& g ) override;
        void mouseUp( const juce::MouseEvent& ) override;
        void mouseDrag( const juce::MouseEvent& ) override;
    };

}
