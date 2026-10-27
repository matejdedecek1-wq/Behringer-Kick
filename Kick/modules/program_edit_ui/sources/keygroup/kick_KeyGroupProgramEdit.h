#pragma once

#include "kick_LfoModulationKG.h"
#include "kick_PanVelocityKG.h"
#include "kick_KeyGroupMaster.h"
#include "../../sources/drum/kick_DrumProgramEdit.h"

namespace kick_ui
{
#define KEY_GROUP_NAV_BUTTONS 6
    class KeyGroupProgramEdit : public ProgEditInterface
        , public juce::Button::Listener
    {
    public:
        KeyGroupProgramEdit();
        ~KeyGroupProgramEdit() = default;

        void refresh() override;
        void setupHeader( KickModeHeader& Hdr ) override{};
        void handleCommand( CPktMgr::HWMsg_t& ) override;

    private:
        void resized() override;
        void visibilityChanged() override;
        void buttonClicked( juce::Button* ) override;

        enum ViewType
        {
            firstTime,
            secondTime
        };

    private:
        mt::MTKeygroupProgram*                        pKeyGroup{ nullptr };
        std::vector<int>                              keys;
        std::vector<std::unique_ptr<juce::Component>> comps;

        ui::KickToggleButton mainButton{ "Main", "Main", "Main" };
        ui::KickToggleButton samplesButton{ "Samples", "Samples", "Samples" };
        ui::KickToggleButton layerMixButton{ "Layer Mix", "Layer Mix", "Layer Mix" };
        ui::KickToggleButton envelopesButton{ "Envelopes", "Envelopes", "Envelopes" };
        ui::KickToggleButton lfoButton{ "LFO", "LFO", "LFO" };

        int                             tIndex{ 0 }, mainTab{ 0 }, sampleTab{ 1 };
        bool                            isDefault{ false }, isMain{ true }, isSample{ false };
        std::unique_ptr<juce::Drawable> mainImage;
    };

    class KeyGroupSampleTab1 : public ui::KickGrid
        , ui::KickToggleButton::Listener
        , public KeyGroupInterface
    {
    public:
        std::function<void( const int SelectedLayer )> onLayerSelected;

        KeyGroupSampleTab1( std::vector<int>& Keys, mt::MTKeygroupProgram*& pKeyGroup );
        ~KeyGroupSampleTab1() = default;

        void refresh() override;
        void refreshKeys();

    private:
        void buttonClicked( juce::Button* ) override{};
        void buttonStateChanged( juce::Button* ) override;
        void handleCommand( CPktMgr::HWMsg_t& ) override;

    private:
        std::vector<int>&       keys;
        mt::MTKeygroupProgram*& pKeygroup;

        ui::WaveRenderer wav;
        int              selectedLayer{ 0 };

        std::array<ui::KickToggleButton, 4> layer;

        ui::KickNumberT<juce::int64> start{ "START" };
        ui::KickNumberT<juce::int64> end{ "END" };
        ui::KickNumberT<juce::int64> semi{ "SEMI" };
        ui::KickNumberT<juce::int64> fine{ "FINE" };

        void visibilityChanged() override;
    };

    class KeyGroupSampleTab2 : public ui::KickGrid
        , public KeyGroupInterface
    {
    public:
        KeyGroupSampleTab2( std::vector<int>& Keys, mt::MTKeygroupProgram*& pKeyGroup );
        ~KeyGroupSampleTab2() = default;

        void selectLayer( int SelectedLayer );
        void refresh() override;

    private:
        std::vector<int>&       keys;
        mt::MTKeygroupProgram*& pKeygroup;

        int selectedLayer{ 0 };

        ui::WaveRenderer wav;

        ui::KickNumberT<int> slice{ "SLICE" };
        ui::KickNumberT<int> loopStart{ "LOOP START" };
        ui::KickNumberT<int> loopEnd{ "LOOP END" };
        ui::KickButton       padLoop{ " ", "Pad Loop >>" };

        ui::KickNumberT<int> offSet{ "OFFSET" };
        ui::KickNumberT<int> tailLength{ "TAIL LENGTH" };
        ui::KickNumberT<int> tailStart{ "TAIL START" };
        ui::KickButton       reverse{ " ", "<< Reverse" };

        ui::KickNumberT<int> layerPan{ "LAYER PAN", 1, 120, 1 };
        ui::KickNumberT<int> level{ "LEVEL", 1, 100, 1 };

        void visibilityChanged() override;
        void handleCommand( CPktMgr::HWMsg_t& ) override;
    };
}
