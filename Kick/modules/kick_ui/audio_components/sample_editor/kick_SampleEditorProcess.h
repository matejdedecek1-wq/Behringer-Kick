#pragma once
namespace mt
{
    namespace ac
    {
        class SampleEditorProcessAction : public juce::Component
            , public juce::Button::Listener
        {
        public:
            SampleEditorProcessAction( unsigned int aid, SampleEditorData& d );
            ~SampleEditorProcessAction() {}
            const juce::String&  getTitle();
            std::list<juce::var> getParams() { return params; }
            unsigned int         getId() { return actionId; }

            void resized() override;
            void buttonClicked( juce::Button* btn ) override;

            static void performAction( unsigned int actionId, SampleEditorData& data );

        private:
            std::unique_ptr<juce::Component> actionComponent;
            unsigned int                     actionId = 0;
            SampleEditorData&                seData;
            ui::KickButton                   ok{ TRANS( "Ok" ) };
            ui::KickButton                   cancel{ TRANS( "Cancel" ) };
            std::list<juce::var>             params;
        };

        class SampleEditorProcess : public ui::KickMenuComponent
        {
        public:
            SampleEditorProcess( SampleEditorData& d );
            unsigned int getSelectedId() { return selectedId; }

        private:
            unsigned int selectedId = 0;

            void paint( juce::Graphics& g ) override { g.fillAll( ui::Theme::backgroundColour( 5 ) ); }
        };
    }
}
