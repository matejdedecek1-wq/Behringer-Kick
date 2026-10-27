#pragma once
namespace kick_ui
{
    class AudioGridMagnifyTool : public IAudioTools
    {
    public:
        AudioGridMagnifyTool( AudioGridComponent&, AudioGridData&, juce::Viewport& );

        void paint( juce::Graphics& ) override{};
        void mouseDrag( const juce::MouseEvent& );
        void mouseDown( const juce::MouseEvent& ) override;
        void mouseUp( const juce::MouseEvent& ) override;

    private:
        std::map<int, juce::Point<float>> activeTouches;
        std::map<int, juce::Point<float>> previousTouchPositions;
        std::map<int, juce::Point<float>> initialTouchPositions;
        float                             zoomFactor{ 1.0f };
        float                             initialDistance   = 0.0f;
        float                             initialZoomFactor = 1.0f;
        float                             initialTick{ 0 };

        float calculateDistance()
        {
            if( activeTouches.size() == 2 )
            {
                auto it = activeTouches.begin();
                auto p1 = it->second;
                ++it;
                auto p2 = it->second;
                return p1.getDistanceFrom( p2 );
            }
            return 0.f;
        }
    };
}
