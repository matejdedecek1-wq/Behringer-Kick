#pragma once

namespace ui
{
    class KickAnimationText
        : public juce::Label
        , public mt::MTProperties
        , private juce::Timer
    {
    public:
        explicit KickAnimationText( const juce::String& text );

    private:
        void paint( juce::Graphics& ) override;
        void timerCallback() override;

        void mouseEnter( const juce::MouseEvent& ) override;
        void mouseExit( const juce::MouseEvent& ) override;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( KickAnimationText )
    };
};
