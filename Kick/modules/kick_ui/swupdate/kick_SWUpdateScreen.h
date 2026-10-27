#pragma once
#include <hardware/source/kick_Hwcommon.h>
#include <hardware/source/kick_FirmwareUpdate.h>
#include <hardware/source/kick_UsbFunctionFS.h>

namespace kick_ui
{
    class SWUpdateScreen : public kick_ui::IScreen
        , public juce::Component
        , private juce::Timer
    {
    public:
        SWUpdateScreen();
        ~SWUpdateScreen() {}

        void getSwupdateStatus( mt::swupdatemsg_t& swupdate );
        void getUsbupdateStatus( mt::usbmsg_t& usbupdate );
        void setSwUpdateCallback()
        {
            mt::FirmwareUpdate::getSwupdateInstance().setSwUpdatecallback( std::bind( &SWUpdateScreen::getSwupdateStatus, this, std::placeholders::_1 ) );
        }
        void setUsbCallback()
        {
            mt::USBFunctionFS::getUSBInstance().setUsbcallback( std::bind( &SWUpdateScreen::getUsbupdateStatus, this, std::placeholders::_1 ) );
        }

    private:
        bool requiresControlBar() override { return false; };
        bool requiresHandleCommand() override { return true; };

        void                 timerCallback();
        bool                 requiresStatusBar() override { return false; };
        void                 handleCommand( CPktMgr::HWMsg_t& ) override;
        void                 paint( juce::Graphics& g ) override;
        juce::Rectangle<int> centreBounds = { 10, 90, 1002, 500 };
        void                 resized() override;
        void                 visibilityChanged() override;

        ui::KickLabel           updateLabel{ "UPDATING FIRMWARE" };
        ui::KickLabel           poweroffLabel{ "Do not power off the device." };
        ui::KickLabel           restartLabel{ "Kick will automatically restart after updating." };
        kick_ui::KickModeHeader header{ "Firmware Update", "Icon_SettingsScreen" };
        ui::KickProgressBar     stepProgressbar;
        ui::KickProgressBar     overallProgressbar;
        double                  _step_percentage;
        double                  _overallpercentage;
        juce::String            _overallLabel = "Downloading";

        ui::KickImageButton logo1{ "", "logo_pref", "" };
        ui::KickImageButton logo{ "", "_Layer_.png", "" };
        ui::KickLabel       stepLabel{ "Step 1/4" };
        ui::KickLabel       overallLabel{ "Download done.." };

        //temporary
        ui::KickButton backBtn{ "Go Back" };
    };
}
