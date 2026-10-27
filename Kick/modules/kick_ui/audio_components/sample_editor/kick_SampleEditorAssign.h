
#pragma once

namespace mt
{
    namespace ac
    {
        class SampleEditorAssign : public juce::Component
            , public juce::ComboBox::Listener
            , public juce::TextButton::Listener
        {
        public:
            SampleEditorAssign( SampleEditorData& d );

            int getSelectedId() { return selectedId; }

        private:
            SampleEditorData& seData;
            juce::ComboBox    supportedActionsCombo;
            juce::TextButton  cancel{ TRANS( "Cancel" ) };
            int               selectedId = -1;

            void resized() override;
            void comboBoxChanged( juce::ComboBox* comboBoxThatHasChanged ) override;
            void buttonClicked( juce::Button* ) override;
        };
    }

}
