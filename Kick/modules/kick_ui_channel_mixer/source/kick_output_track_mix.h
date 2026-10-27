#pragma once
namespace kick_ui
{

    class OutputTrackMixer : public kick_ui::IRowView
    {
    public:
        OutputTrackMixer();
        ~OutputTrackMixer() = default;
        void knobCommand( int knob, int value );

    private:
        void setupProvider() override;
        void visibilityChanged() override;
        void meterUpdate() override;

        mt::MTBusManager& _busMgr{ ae.getBusManager() };
        std::atomic_bool  _initialized{ false };

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( OutputTrackMixer )
    };

    class OutputTrackDisplay : public juce::Component
    {
    public:
        std::function<void()> onSoloChanged;

        OutputTrackDisplay();
        ~OutputTrackDisplay() = default;

        void setOutputNumber( const int ID );
        void updateData();
        void meterUpdate();

        void                 linkRotated( int ID, int value );
        juce::Rectangle<int> getElementBounds( int state );

    private:
        mt::MTAudioInterface& ae{ mt::getAudioInterface() };
        mt::MTBusManager&     busMgr{ ae.getBusManager() };

        int         busNo{ -1 };
        mt::MTIBus* pBus{ nullptr };

        std::shared_ptr<mt::Kick::ScopeTracker> scope;

        ui::KickSerialNumber    srNo{ "" };
        ui::KickImageButton     icon{ "Icon" };
        ui::KickImageButton     solo{ "Solo", "Solo_normal", "Solo_active" };
        ui::KickImageButton     mute{ "Mute", "Mute_normal", "Mute_active" };
        ui::KickLabel           outPutName{ "" };
        ui::KickLabel           outPutHeader{ "OUTPUT BUS" };
        ui::KickButton          inserts{ "Inserts", "Inserts" };
        ui::KickMeterHorizantal levelMeter1;
        ui::KickMeterHorizantal levelMeter2;
        ui::KickLevelSlider     levelSlider;
        ui::KickPanRotary       panSlider;

        void clear();
        void resized() override;
        void visibilityChanged() override;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( OutputTrackDisplay )
    };
}
