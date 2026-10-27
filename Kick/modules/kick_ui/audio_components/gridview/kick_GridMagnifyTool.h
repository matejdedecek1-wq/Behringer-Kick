#pragma once

namespace kick_ui
{
    class GridMagnifyTool : public IGridTool
    {
    public:
        GridMagnifyTool( GridComponent& gridComponent, GridData& data, juce::Viewport& ViewPort );

        void paint( juce::Graphics& g ) override;

        void mouseDrag( const juce::MouseEvent& event );
        void mouseUp( const juce::MouseEvent& event ) override;
        void mouseDown( const juce::MouseEvent& event ) override;

    private:
        std::map<int, juce::Point<float>> activeTouches;
        std::map<int, juce::Point<float>> previousTouchPositions;
        std::map<int, juce::Point<float>> initialTouchPositions;
        float                             zoomFactor{ 1.0f };
        float                             initialDistance   = 0.0f;
        float                             initialZoomFactor = 1.0f;
        float                             initialTick{ 0 };
        float                             initialNote{ 0 };

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
