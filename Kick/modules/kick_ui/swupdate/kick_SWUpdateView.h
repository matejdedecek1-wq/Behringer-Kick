
#include <hardware/source/kick_FirmwareUpdate.h>
#include <hardware/source/kick_Hwcommon.h>
#include <hardware/source/kick_HardwareProbe.h>

#pragma once
namespace kick_ui
{
    static juce::String PCVersion{ "" };
    static juce::String USBVersion{ "" };
    static juce::String SDCardVersion{ "" };

    struct DeviceData
    {
        juce::String deviceName;
        juce::String version;
    };

    class SWUpdatePopup : public juce::Component
    {
    public:
        explicit SWUpdatePopup( int );
        ~SWUpdatePopup() = default;
        int getRowId();

    private:
        void                   resized() override;
        ui::Kick2LineNameLabel info{ "This will stop all the current running operations." };
        ui::KickLabel          warningMsg{ "Are you sure to update your device?" };
        ui::KickButton         updateBtn{ "Update" };
        ui::KickButton         cancelBtn{ "Cancel" };
        int                    rowId;
    };

    class ImageCheckingThread : public juce::Thread
    {
    public:
        std::function<void()> onThreadEnd;
        ImageCheckingThread();

        void stopThreadSafely()
        {
            signalThreadShouldExit(); // Signals the thread to exit
            juce::Thread::stopThread( 500 ); // Waits for 500 ms for the thread to stop safely
        }

        std::atomic<bool> hasPCAppImage;
        std::atomic<bool> hasUSBImage;
        std::atomic<bool> hasSDCardImage;

    private:
        void run() override;
    };

    class SWUpdateContent : public juce::Component
    {
    public:
        std::function<void( int )> updateBtnCallback;
        explicit SWUpdateContent( int );
        ~SWUpdateContent() = default;
        void setProperties( DeviceData& deviceData );
        int  getRowId();

        ui::Kick2LineNameLabel updates{ "v1.21", "Improves stability" };
        ui::KickLabel          device;
        ui::KickButton         updateBtn{ "Update" };

    private:
        void resized() override;
        void paint( juce::Graphics& g ) override{ /*Not Required*/ };
        int  rowId;
    };

    class SWUpdateView : public IScreen
        , public juce::Component
    {
    public:
        SWUpdateView();
        ~SWUpdateView() = default;

        std::vector<DeviceData> deviceData;

    private:
        juce::Rectangle<int>                          topBounds    = { 0, 0, 1022, 100 };
        juce::Rectangle<int>                          centreBounds = { 10, 90, 1002, 450 };
        std::unique_ptr<ui::KickTableView>            table;
        std::unique_ptr<kick_ui::ImageCheckingThread> thread;
        ui::KickTableViewProvider                     provider;
        void                                          resized() override;
        bool                                          requiresControlBar() override { return false; }
        bool                                          requiresStatusBar() override { return false; }
        void                                          visibilityChanged() override;
        void                                          handleCommand( CPktMgr::HWMsg_t& f_HwMsg ) override
        { /*TODO*/
        }
        void paint( juce::Graphics& g );
        void getUsbupdateStatus( mt::usbmsg_t& data );

        ui::KickButton          back{ "Go Back" };
        ui::KickButton          apply{ "Apply" };
        ui::KickLabel           status{ "Status" };
        ui::KickLabel           deviceName{ "Device" };
        ui::KickLabel           updateLabel{ "Updation" };
        ui::KickLabel           latestVersionlabel{ "You are on latest Version" };
        ui::KickLabel           checkLabel{ "No updates available" };
        void                    setupProvider();
        void                    refresh();
        kick_ui::KickModeHeader header{ "Firmware Update", "Icon_SettingsScreen" };
        // Covers the update list when swupdate is missing; the header's back arrow and "Go Back" stay usable.
        std::unique_ptr<ui::FeatureUnavailable> featureUnavailable;
        void                    updateDeviceData( const juce::String& deviceName, const juce::String& version, bool hasImage );
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( SWUpdateView )
    };
}
