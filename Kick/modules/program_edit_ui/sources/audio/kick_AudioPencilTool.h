#pragma once

namespace kick_ui
{
    class AudioGridPencilTool : public IAudioTools
    {
    public:
        AudioGridPencilTool( AudioGridComponent&, AudioGridData&, juce::Viewport& ViewPort );

        void paint( juce::Graphics& ) override;
        void mouseUp( const juce::MouseEvent& ) override;
        void mouseDrag( const juce::MouseEvent& ) override;
    };
}
