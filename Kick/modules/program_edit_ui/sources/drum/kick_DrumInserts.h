#pragma once
namespace kick_ui
{
    class DrumGroupInsert : public juce::Component
        , public DrumInterface
    {
    public:
        DrumGroupInsert( std::vector<int>& Pads, mt::MTDrumProgram*& PDrum );
        ~DrumGroupInsert() = default;

        void refresh() override;
        void handleCommand( CPktMgr::HWMsg_t& ) override;

    private:
        ui::KickLabel insert1{ "Insert 01" };
        ui::KickLabel insert2{ "Insert 02" };
        ui::KickLabel insert3{ "Insert 03" };
        ui::KickLabel insert4{ "Insert 04" };
        ui::KickLabel insert1Label{ "" };
        ui::KickLabel insert2Label{ "" };
        ui::KickLabel insert3Label{ "" };
        ui::KickLabel insert4Label{ "" };

        ui::KickListBox     type{ "TYPE" };
        mt::MTDrumProgram*& pDrum;
        std::vector<int>    pads;

        ui::KickRotary cutoff{ "CUTOFF", juce::Slider::TextBoxLeft, true };
        ui::KickRotary resonance{ "RESONANCE", juce::Slider::TextBoxLeft, true };
        ui::KickRotary dryWet1{ "DRY/WET", juce::Slider::TextBoxLeft, true };
        ui::KickRotary time1{ "TIME", juce::Slider::TextBoxLeft, true };
        ui::KickRotary feedBack{ "FEEDBACK", juce::Slider::TextBoxLeft, true };
        ui::KickRotary width1{ "WIDTH", juce::Slider::TextBoxLeft, true };
        ui::KickRotary dryWet2{ "DRY/WET", juce::Slider::TextBoxLeft, true };
        ui::KickRotary time2{ "TIME", juce::Slider::TextBoxLeft, true };
        ui::KickRotary ratio{ "RATIO", juce::Slider::TextBoxLeft, true };
        ui::KickRotary width2{ "WIDTH", juce::Slider::TextBoxLeft, true };
        ui::KickRotary dryWet3{ "DRY/WET", juce::Slider::TextBoxLeft, true };
        ui::KickRotary rotary1{ "-", juce::Slider::TextBoxLeft, true };
        ui::KickRotary rotary2{ "-", juce::Slider::TextBoxLeft, true };
        ui::KickRotary rotary3{ "-", juce::Slider::TextBoxLeft, true };
        ui::KickRotary rotary4{ "-", juce::Slider::TextBoxLeft, true };

        ui::KickButton edit1{ "E", "E" };
        ui::KickButton edit2{ "E", "E" };
        ui::KickButton edit3{ "E", "E" };
        ui::KickButton edit4{ "E", "E" };
        ui::KickButton del1{ "D", "D" };
        ui::KickButton del2{ "D", "D" };
        ui::KickButton del3{ "D", "D" };
        ui::KickButton del4{ "D", "D" };

        void deleteInsert( unsigned int Postion );
        void paint( juce::Graphics& ) override;
        void resized() override;
        void visibilityChanged() override;
    };

    class DrumGroup : public juce::Component
    {
    public:
        DrumGroup();
        ~DrumGroup() = default;

    private:
        ui::KickListBox type{ "TYPE" }, type1{ "TYPE" };
        ui::KickRotary  rotary1{ "-", juce::Slider::TextBoxLeft, true };
        ui::KickRotary  rotary2{ "-", juce::Slider::TextBoxLeft, true };

        void paint( juce::Graphics& ) override;
        void resized() override;
    };

    class DrumGroupComponent : public juce::Component
        , public DrumInterface
    {
    public:
        DrumGroupComponent( std::vector<int>& Pads, mt::MTDrumProgram*& PDrum );
        ~DrumGroupComponent() = default;

        void refresh() override {}
        void handleCommand( CPktMgr::HWMsg_t& ) override;

    private:
        DrumGroup d1;
        DrumGroup d2;
        DrumGroup d3;
        DrumGroup d4;
        void      paint( juce::Graphics& ) override;
        void      resized() override;

        mt::MTDrumProgram*& pDrum;
        std::vector<int>    pads;
    };

};
