#pragma once

namespace kick_ui
{

    class FilterEnvEditKG : public juce::Component
        , public juce::Button::Listener
    {
    public:
        FilterEnvEditKG();
        ~FilterEnvEditKG();
        void buttonClicked( juce::Button* ) override;

    private:
        ui::KickLabel         filterLabel{ TRANS( "Filter" ) };
        ui::KickLabel         msLabel{ TRANS( "Modulation Sources" ) };
        ui::KickToggleOptions feOptions{ TRANS( "Filter Envelope" ) };
        ui::KickToggleOptions ampEnvOptions{ TRANS( "Amp Envelope" ) };

        ui::KickListBox filterType{ "Type", 1 };
        ui::KickRotary  filterCutoff{ "Cutoff", juce::Slider::TextBoxLeft, true };
        ui::KickRotary  filterReso{ "Reso", juce::Slider::TextBoxLeft, true };
        ui::KickRotary  filterEnv{ "Env", juce::Slider::TextBoxLeft, true };

        ui::KickRotary msVelStat{ "Vel>Sta", juce::Slider::TextBoxLeft, true };
        ui::KickRotary msVelAtk{ "Vel>Atk", juce::Slider::TextBoxLeft, true };
        ui::KickRotary msVelEnv{ "Vel>Envg", juce::Slider::TextBoxLeft, true };
        ui::KickRotary msVelFlt{ "Vel>Flt", juce::Slider::TextBoxLeft, true };

        ui::KickNumber        feAttack{ "Attack", MIDI::MIN, MIDI::MAX, 0, 1, true, false };
        ui::KickNumber        feHold{ "Hold", MIDI::MIN, MIDI::MAX, 0, 1, true, false };
        ui::KickNumber        feDecay{ "Decay", MIDI::MIN, MIDI::MAX, 0, 1, true, false };
        ui::KickNumber        feSustain{ "Sustain", MIDI::MIN, MIDI::MAX, 0, 1, true, false };
        ui::KickImageButton   feAhdsrBtn{ "ahdsr", "effect3" };
        ui::KickToggleOptions feDecayFromOptions{ TRANS( "Decay From" ) };

        ui::KickNumber        ampAttack{ "Attack", MIDI::MIN, MIDI::MAX, 0, 1, true, false };
        ui::KickNumber        ampHold{ "Hold", MIDI::MIN, MIDI::MAX, 0, 1, true, false };
        ui::KickNumber        ampDecay{ "Decay", MIDI::MIN, MIDI::MAX, 0, 1, true, false };
        ui::KickNumber        ampSustain{ "Sustain", MIDI::MIN, MIDI::MAX, 0, 1, true, false };
        ui::KickImageButton   ampAhdsrBtn{ "ahdsr", "effect3" };
        ui::KickToggleOptions ampDecayFromOptions{ TRANS( "Decay From" ) };
    };

}
