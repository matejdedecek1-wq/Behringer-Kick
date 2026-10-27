#pragma once
namespace ui
{
    class KickAlertWindow : public juce::Component
        , public juce::Button::Listener
    {
    public:
        enum class KickAlertWindowType
        {
            Info,
            Error,
            Warning,
            Question
        };
        void paint( juce::Graphics& g ) override;

        KickAlertWindow( KickAlertWindowType type, const std::vector<juce::String>& buttons, const juce::String& message );
        ~KickAlertWindow();
        void        resized() override;
        void        buttonClicked( juce::Button* btn ) override;
        static void showYesNoCancelAlert( KickAlertWindowType type, const juce::String& message, const juce::String& title, std::function<void( int )> callback );
        static void showShutdownRestartAlert( KickAlertWindowType type, const juce::String& message, const juce::String& title, std::function<void( int )> callback );
        static void showYesNoAlert( KickAlertWindowType type, const juce::String& message, const juce::String& title, std::function<void( int )> callback );
        static void showCancelAlert( KickAlertWindowType type, const juce::String& message, const juce::String& title, std::function<void( int )> callback );

    private:
        static void showAlert( KickAlertWindowType type, const std::vector<juce::String>& buttons, const juce::String& message, const juce::String& title, std::function<void( int )> callback );

    private:
        KickAlertWindowType                    type = KickAlertWindowType::Info;
        std::vector<juce::String>              buttons;
        juce::String                           message;
        std::list<std::unique_ptr<KickButton>> alertButtons;
    };
}
