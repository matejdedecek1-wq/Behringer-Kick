#pragma once
namespace kick_ui
{
    class KeyGroupMaster : public ui::KickGrid
        , public KeyGroupInterface
    {
    public:
        KeyGroupMaster( std::vector<int>& Keys, mt::MTKeygroupProgram*& pKeyGroup );
        ~KeyGroupMaster() = default;

    private:
        ui::KickRotary levelSlider{ "LEVEL", juce::Slider::TextBoxLeft, true };
        ui::KickRotary panSlider{ "PAN", juce::Slider::TextBoxLeft, true };
        ui::KickRotary semiSlider{ "SEMI", juce::Slider::TextBoxLeft, true };
        ui::KickRotary fineSlider{ "FINE", juce::Slider::TextBoxLeft, true };

        ui::KickToggleButton insertsButton{ "INSERTS", "INSERTS", "INSERTS" };
        ui::KickListBox      outListBox{ "OUT" };

        ui::KickButton       keyTrack{ "KEY TRACK", "KEY TRACK" };
        ui::KickNumberT<int> muteGroup{ "Mute Group", MUTEGROUP::MIN, MUTEGROUP::MAX, MUTEGROUP::DEFAULT, true };
        ui::KickNumberT<int> kgPoly{ "KG POLY", POLYPHONY::MIN, POLYPHONY::MAX, POLYPHONY::DEFAULT, true };

        ui::KickToggleOptions layerPlay{ "LAYER PLAY", { "Cycle", "Velocity", "Random" } };
        ui::KickToggleOptions samplePlay{ "SAMPLE PLAY", { "One Shot", "Note On", "Note Off" } };

        std::vector<int>&                       keys;
        mt::MTKeygroupProgram*&                 pKeygroup;
        std::shared_ptr<mt::Kick::ScopeTracker> scope;

        void refresh() override;
        void handleCommand( CPktMgr::HWMsg_t& ) override;
        void visibilityChanged() override;
    };
}
