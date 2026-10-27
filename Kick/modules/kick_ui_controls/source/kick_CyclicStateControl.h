#pragma once
namespace ui
{
    class KickCyclicStateControl : public juce::Component
        , public juce::Button::Listener
    {
    public:
        KickCyclicStateControl();
        void resized() override;
        void setStates( const std::vector<juce::String>& newState )
        {
            states = newState;
            setCurrentState( currentState );
        }
        void setCurrentState( unsigned int cs );

        void  setNormalized( float Value );
        float getNormalized();

    private:
        virtual void buttonClicked( juce::Button* );

        SIGNALS : sigslot::signal<int> stateChanged;

    private:
        KickButton                leftNav{ "<" };
        KickButton                rightNav{ ">" };
        KickImageButton           stateButton{ "" };
        unsigned int              currentState = 0;
        std::vector<juce::String> states;
    };
}
