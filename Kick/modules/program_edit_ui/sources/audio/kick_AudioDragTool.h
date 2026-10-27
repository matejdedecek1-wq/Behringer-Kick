#pragma once
namespace kick_ui
{
    class AudioGridDragTool : public IAudioTools
    {
    public:
        AudioGridDragTool( AudioGridComponent& GridComp, AudioGridData& GridData, juce::Viewport& ViewPort );

        void paint( juce::Graphics& g ) override;
        void mouseUp( const juce::MouseEvent& event ) override;
        void mouseDrag( const juce::MouseEvent& event ) override;
        void mouseMove( const juce::MouseEvent& ) override;

    private:
        int prevDragTime{ 0 };
    };

}
