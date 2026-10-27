#pragma once

namespace kick_ui
{
    static sigslot::signal<> parameterViewNeedsRepaint;

    constexpr int gridParameterControlHeight{ 30 };
    constexpr int gridParameterViewHeight{ 140 };
    constexpr int gridParameterTextButton{ 70 };
    constexpr int gridParameterSymbolButton{ 50 };
    constexpr int gridParameterButtonBorderX{ 3 };
    constexpr int gridParameterButtonBorderY{ 0 };
    constexpr int gridParameterButtonGroupDivider{ 20 };
    constexpr int gridParameterAreaBorderWidth{ 10 };
    constexpr int gridParameterAreaBorderHeight{ 0 };

    class ParameterViewControl : public juce::Component
    {
    public:
        explicit ParameterViewControl();
        int getRecommendedHeight() { return gridParameterControlHeight; }

    public:
        ui::KickImageButton ecBtn{ "E", "UP", "Down" };

        ui::KickImageButton plusBtn{ "+", "Plus", "Plus_Active" };
        ui::KickImageButton minusBtn{ "-", "Minus", "Minus_Active" };

        ui::KickImageButton  paramTypeBtn{ "Velocity", "Velocity" };
        ui::KickToggleButton defaultZoomBtn{ "Z", "Z", "Z" };
        ui::KickToggleButton transposeBtn{ "Transpose", "Transpose", "Transpose" };
        ui::KickToggleButton snapBtn{ "Don't Snap", "Don't Snap", "Don't Snap" };
        ui::KickToggleButton eStartBtn{ "Edit Start", "Edit Start", "Edit Start" };
        ui::KickToggleButton eEndBtn{ "Edit End", "Edit End", "Edit End" };
        ui::KickToggleButton nudgeBtn{ "Nudge", "Nudge", "Nudge" };
        ui::KickLabel        Zoom{ "Zoom" };

        ui::KickListBox paramList{ ui::KickListBox::withoutTitleBar, "", { "Velocity", "Probability", "Ratchet" } };

    private:
        void resized() override;
        void paint( juce::Graphics& g ) override;
    };

    class ParameterView : public juce::Component
        , public juce::AsyncUpdater
    {
    public:
        ParameterView( ParameterViewControl& );

        void setMinimizeHeight( int mHeight ) { minimizeHeight = mHeight; }
        int  getMinimizeHeight() { return minimizeHeight; }
        int  getRecommendedtHeight() { return gridParameterViewHeight; }
        void setMinMaxValues( int Min, int Max );

    private:
        void paint( juce::Graphics& g ) override;
        void mouseUp( const juce::MouseEvent& event ) override;
        void mouseDrag( const juce::MouseEvent& event ) override;
        void handleAsyncUpdate() override;
        void handleMouseDrag( const juce::MouseEvent& event );

    private:
        mt::MTAudioInterface& ae{ mt::getAudioInterface() };
        const mt::MTSelected& aeSelected{ ae.getSelected() };
        GridData&             gridData{ GridData::Instance() };
        ParameterViewControl& paramControl;

        bool             minimized = false;
        int              minimizeHeight;
        std::list<float> markings;
        int              min{ MIDI::MIN }, max{ MIDI::MAX };
    };

    class ParameterViewScale : public juce::Component
    {
    public:
        ParameterViewScale()  = default;
        ~ParameterViewScale() = default;

        void setMinMaxValues( int Min, int Max );

    private:
        int  min{ MIDI::MIN }, max{ MIDI::MAX };
        void paint( juce::Graphics& ) override;
    };
}
