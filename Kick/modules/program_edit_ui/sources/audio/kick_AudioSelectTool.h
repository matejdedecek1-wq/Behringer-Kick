#pragma once
namespace kick_ui
{
    class AudioGridSelectTool : public IAudioTools
    {
    public:
        AudioGridSelectTool( AudioGridComponent&, AudioGridData& GridData, juce::Viewport& );

        void paint( juce::Graphics& g ) override;
        void mouseUp( const juce::MouseEvent& ) override;
        void mouseDrag( const juce::MouseEvent& ) override;
    };
}
