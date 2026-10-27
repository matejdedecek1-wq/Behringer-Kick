#include "kick_SWUpdateView.h"

namespace kick_ui
{
    SWUpdateView::SWUpdateView()
        : kick_ui::IScreen( kick_ui::Screen::SWUpdateView )
    {
        table.reset( new ui::KickTableView( "List", provider ) );
        mt::utils::addAndMakeVisible( this, *table, back, status, deviceName, updateLabel, header, apply );
        mt::utils::addChildComponents( this, latestVersionlabel, checkLabel );
        header.showOptions( false, false );
        header.onBackButtonPressed = [this]( bool ) { closeScreen(); };
        table->setRowHeight( 100 );
        table->setRowSpacing( 10 );
        table->setRowBackgroundColour( ui::Theme::backgroundColour( 9 ) );
        table->setSelectedRowBackgroundColour( ui::Theme::backgroundColour( 0 ) );
        table->setSelectedRowOutlineColour( ui::Theme::outlineColour( 0 ) );
        back.onClick = [&]() {
            closeScreen();
            mt::USBFunctionFS::getUSBInstance().stop();
        };

        latestVersionlabel.setValueBackgroundColour( ui::Theme::backgroundColour( 9 ) );
        checkLabel.setValueBackgroundColour( ui::Theme::backgroundColour( 9 ) );

        status.setValueBackgroundColour( ui::Theme::backgroundColour( 9 ) );
        deviceName.setValueBackgroundColour( ui::Theme::backgroundColour( 9 ) );
        updateLabel.setValueBackgroundColour( ui::Theme::backgroundColour( 9 ) );

        latestVersionlabel.setFontHeight( 30 );
        checkLabel.setFontHeight( 30 );

        status.setFontHeight( 20 );
        deviceName.setFontHeight( 20 );
        updateLabel.setFontHeight( 20 );

        status.setPadding( 20 );
        deviceName.setPadding( 40 );

        latestVersionlabel.setValueJustification( juce::Justification::centred );
        checkLabel.setValueJustification( juce::Justification::centredTop );

        status.setValueJustification( juce::Justification::left );
        deviceName.setValueJustification( juce::Justification::left );
        updateLabel.setValueJustification( juce::Justification::centred );

        latestVersionlabel.setTextColour( ui::Theme::textColour( 2 ) );
        checkLabel.setTextColour( ui::Theme::textColour( 1 ) );

        back.setValueColour( ui::Theme::textColour( 1 ) );
        apply.setValueColour( ui::Theme::textColour( 1 ) );
        mt::utils::setValueFontHeight( 20, back, apply );

        setupProvider();
    }

    void SWUpdateView::refresh()
    {
        DBG( "refreshing data ###############" );
        status.setVisible( thread->hasPCAppImage || thread->hasUSBImage || thread->hasSDCardImage );
        deviceName.setVisible( thread->hasPCAppImage || thread->hasUSBImage || thread->hasSDCardImage );
        updateLabel.setVisible( thread->hasPCAppImage || thread->hasUSBImage || thread->hasSDCardImage );
        latestVersionlabel.setVisible( !( thread->hasPCAppImage || thread->hasUSBImage || thread->hasSDCardImage ) );
        checkLabel.setVisible( !( thread->hasPCAppImage || thread->hasUSBImage || thread->hasSDCardImage ) );

        updateDeviceData( "PC", PCVersion, thread->hasPCAppImage );
        updateDeviceData( "USBDisk", USBVersion, thread->hasUSBImage );
        updateDeviceData( "SDCard", SDCardVersion, thread->hasSDCardImage );
        DBG( "PCVersion " << PCVersion );
        repaint();
    }

    void SWUpdateView::updateDeviceData( const juce::String& dName, const juce::String& version, bool hasImage )
    {
        bool deviceFound = false;
        for( auto& device : deviceData )
        {
            if( ( device.deviceName == dName ) && hasImage )
            {
                deviceFound = true;
                break;
            }
        }
        if( !deviceFound && hasImage )
        {
            DeviceData newDevice;
            newDevice.deviceName = dName;
            newDevice.version    = version;
            deviceData.push_back( newDevice );
        }
    }

    void SWUpdateView::setupProvider()
    {
        provider.numRows = [this]() {
            return static_cast<int>( deviceData.size() );
        };

        provider.cellComponent = [&]( int Row, juce::Component* Existing ) -> juce::Component* {
            if( juce::isPositiveAndBelow( Row, 3 ) )
            {
                auto comp = static_cast<SWUpdateContent*>( Existing );
                if( comp == nullptr )
                {
                    comp = new SWUpdateContent( Row );
                    comp->setInterceptsMouseClicks( false, true );
                }

                comp->setProperties( deviceData[Row] );
                comp->updateBtnCallback = [&]( int row ) {
                    auto popup = new SWUpdatePopup( row );
                    mt::utils::showDialog( "WARNING!", popup, [&, popup, row]( int R ) {
                        if( R == UI::APPLY )
                        {
                            juce::String            deviceName = deviceData[row].deviceName;
                            bool                    flag       = true;
                            mt::SwUpdateErrorCode_t statusCode = mt::FirmwareUpdate::getSwupdateInstance().setSwUpdateMode( mt::FirmwareUpdate::getSwupdateInstance().getSwUpdateModeFromString( deviceName ) );
                            if( statusCode == mt::SwUpdateErrorCode_t::SW_SUCCESS )
                            {
                                juce::String message;

                                if( flag )
                                {
                                    mt::SwUpdateErrorCode_t error = mt::FirmwareUpdate::getSwupdateInstance().validateImage();
                                    if( error == mt::SwUpdateErrorCode_t::SW_SUCCESS )
                                    {
                                        int rowId = popup->getRowId();
                                        DBG( "Firmware Image Verification successFul" );
                                        DBG( "Row" << row );
                                        signals::sigScreenOpen( Screen::SWUpdateWindow );
                                    }
                                    else
                                    {
                                        juce::String info;
                                        if( error == mt::SwUpdateErrorCode_t::IMAGE_EXTRACTION_E || error == mt::SwUpdateErrorCode_t::IMAGE_DYCRYPTION_E )
                                        {
                                            info = "Please place correct image on selected Drive";
                                        }
                                        else
                                            info = "Image Invalid";

                                        auto warningPopup = new ui::FilePopup( info, true, false, false );
                                        mt::utils::showDialog( "WARNING!", warningPopup, [&]( int R ) {
                                            if( R == UI::CANCEL )
                                            {
                                                if( auto dw = getCurrentlyModalComponent() )
                                                    dw->exitModalState( 0 );
                                            }
                                            if( warningPopup ) delete warningPopup;
                                        } );
                                    }
                                }
                                else
                                {
                                    auto warningPopup = new ui::FilePopup( message, true, false, false );
                                    mt::utils::showDialog( "WARNING!", warningPopup, [&]( int R ) {
                                        if( R == UI::CANCEL )
                                        {
                                            if( auto dw = getCurrentlyModalComponent() )
                                                dw->exitModalState( 0 );
                                        }
                                        if( warningPopup ) delete warningPopup;
                                    } );
                                }
                            }
                        }
                        else if( R == UI::CANCEL )
                        {
                            if( auto dw = getCurrentlyModalComponent() )
                                dw->exitModalState( 0 );
                        }
                        if( popup ) delete popup;
                    },
                                           { 0, 0, 400, 400 } );
                };

                return comp;
            }
            return nullptr;
        };
    }

    void SWUpdateView::resized()
    {
        auto b = getBounds();
        header.setBounds( b.removeFromTop( ui::layout::header.getHeight() ) );
        auto b1 = b.removeFromTop( 100 );
        status.setBounds( b1.removeFromLeft( 350 ) );
        deviceName.setBounds( b1.removeFromLeft( 350 ).withTrimmedLeft( 50 ) );
        updateLabel.setBounds( b1.removeFromLeft( 380 ).withTrimmedLeft( 100 ).withTrimmedRight( 100 ) );
        // table->setBounds( b.removeFromTop( 400 ) );
        table->setBounds( 20, 150, 990, 100 );
        back.setBounds( 10, 550, 120, 34 );
        apply.setBounds( 894, 550, 120, 34 );
        auto b2 = getLocalBounds();
        latestVersionlabel.setBounds( b2.removeFromTop( 300 ).withTrimmedTop( 200 ).removeFromLeft( 710 ).withTrimmedLeft( 300 ) );
        checkLabel.setBounds( b2.removeFromTop( 30 ).removeFromLeft( 714 ).withTrimmedLeft( 300 ) );
    }

    void SWUpdateView::paint( juce::Graphics& g )
    {
        g.setColour( ui::Theme::backgroundColour( 4 ) );
        g.fillAll();

        auto b = getLocalBounds();

        //Centre Rectangle
        mt::utils::fillRoundedRect( g, centreBounds, ui::Theme::backgroundColour( 1 ), 6.0f, false, false, true, true );

        //Draw Border
        g.setColour( ui::Theme::outlineColour( 3 ) );
        g.drawRoundedRectangle( 10, 90, 1002, 450, cornerSizeX, lineThickness );
    }

    void SWUpdateView::visibilityChanged()
    {
        if( isVisible() )
        {
            if( thread == nullptr )
            {
                DBG( "Thread Created " );
                thread = std::make_unique<kick_ui::ImageCheckingThread>();
            }

            mt::USBFunctionFS::getUSBInstance().setUsbcallback(
                std::bind( &SWUpdateView::getUsbupdateStatus, this, std::placeholders::_1 ) );

            mt::utils::LoadAnimation::getInstance().startAnimation();

            thread->startThread();

            thread->onThreadEnd = [&]() {
                DBG( "Thread end" );
                mt::utils::LoadAnimation::getInstance().stopAnimation();

                // Ensure all UI updates are done on the Message Thread
                juce::MessageManager::callAsync( [this]() {
                    refresh();
                    table->refresh();
                    setupProvider();
                } );

                // Stop the thread and safely delete it
                juce::MessageManager::callAsync( [&]() {
                    if( thread != nullptr )
                    {
                        deviceData.clear();
                        thread->signalThreadShouldExit(); // Signal the thread to exit
                        thread->waitForThreadToExit( 1000 ); // Wait for the thread to exit (1 second)
                        thread.reset(); // Reset (delete) the thread instance
                    }
                } );
            };
        }
    }
    /*
    void SWUpdateView::visibilityChanged()
    {
	    if( isVisible() )
	    {
		    if(thread==nullptr)
		    {
			    thread = std::make_unique<kick_ui::ImageCheckingThread>();
		    }
		    mt::USBFunctionFS::getUSBInstance().setUsbcallback( std::bind( &SWUpdateView::getUsbupdateStatus, this, std::placeholders::_1 ) );
		    auto p = std::make_shared<mt::utils::LoadAnimation>();
		    p->startAnimation();
		    thread->startThread();
		    thread->onThreadEnd = [&, p]() {
			    DBG( "Thread end" );
			    if( p ) p->stopAnimation();
			    juce::MessageManager::callAsync( [&]() {
					    refresh();
					    table->refresh();
					    } );
		    };

		// Stop the thread and safely delete it
            juce::MessageManager::callAsync([this]() {
                thread.reset();       // Reset the unique_ptr to delete the thread instance
            });

	    }
    }*/

    void SWUpdateView::getUsbupdateStatus( mt::usbmsg_t& data )
    {
        if( data.command_status == mt::USB_STATUS::U_INIT )
        {
            DBG( "SWUpdateView ::INIT" );
            mt::FirmwareUpdate::getSwupdateInstance().setSwUpdateMode( mt::SwUpdateMode_t::PC );
            signals::sigScreenOpen( Screen::SWUpdateWindow );
            DBG( "Firmware is Getting Download Done " << data.totalfilesize );
        }
    }

    ImageCheckingThread::ImageCheckingThread()
        : juce::Thread( "Update" )
    {
    }
    void ImageCheckingThread::run()
    {
        mt::FirmwareUpdate& swUpdater = mt::FirmwareUpdate::getSwupdateInstance();
        mt::USBFunctionFS&  usb       = mt::USBFunctionFS::getUSBInstance();

        hasSDCardImage.store( false );
        hasUSBImage.store( false );
        hasPCAppImage.store( false );

        usb.start();
        swUpdater.startFirmwareScan();

        // Check SD Card firmware version
        SDCardVersion = swUpdater.getFwFileVersion( mt::SwUpdateMode_t::SDCARD );
        if( !SDCardVersion.isEmpty() && !threadShouldExit() ) // Check thread exit
        {
            hasSDCardImage = true;
            DBG( "SDCardVersion " << SDCardVersion );
        }

        // Check USB firmware version
        USBVersion = swUpdater.getFwFileVersion( mt::SwUpdateMode_t::USBDISK );
        if( !USBVersion.isEmpty() && !threadShouldExit() ) // Check thread exit
        {
            hasUSBImage = true;
            DBG( "USBVersion " << USBVersion );
        }

        // Check PC application version
        PCVersion = swUpdater.getPcSwVersion();
        if( !PCVersion.isEmpty() && !threadShouldExit() ) // Check thread exit
        {
            hasPCAppImage = true;
            DBG( "#######PCAppVersion " << PCVersion );
        }

        if( onThreadEnd && !threadShouldExit() ) // Ensure the thread has not been asked to exit
        {
            // usb.stop();  // Uncomment if USB functionality needs to stop when the thread ends
            onThreadEnd();
        }
    }

    /*
    void ImageCheckingThread::run()
    {
        mt::FirmwareUpdate& swUpdater = mt::FirmwareUpdate::getSwupdateInstance();
        mt::USBFunctionFS&  usb       = mt::USBFunctionFS::getUSBInstance();
        hasSDCardImage.store( false );
        hasUSBImage.store( false );
        hasPCAppImage.store( false );
        usb.start();
        swUpdater.startFirmwareScan();
        SDCardVersion = swUpdater.getFwFileVersion( mt::SwUpdateMode_t::SDCARD );
        if( !SDCardVersion.isEmpty() )
        {
            hasSDCardImage = true;
            DBG( "SDCardVersion " << SDCardVersion );
        }
        USBVersion = swUpdater.getFwFileVersion( mt::SwUpdateMode_t::USBDISK );
        if( !USBVersion.isEmpty() )
        {
            hasUSBImage = true;
            DBG( "USBVersion " << USBVersion );
        }
        PCVersion = swUpdater.getPcSwVersion();
        if( !PCVersion.isEmpty() )
        {
            hasPCAppImage = true;
            DBG( "PCAppVersion " << PCVersion );
        }

        if( onThreadEnd )
        {
            //  usb.stop();
            onThreadEnd();
        }
    }
*/
    SWUpdateContent::SWUpdateContent( int RowId )
        : rowId( RowId )
    {
        mt::utils::addAndMakeVisible( this, updates, device, updateBtn );

        updates.setLine2Text( "Improves stability" );
        updates.setValueBackgroundColour( ui::Theme::backgroundColour( 9 ) );
        updates.setTitleJustification( juce::Justification::left );
        updates.setTitleFontHeight( 18 );
        updates.setValueFontHeight( 18 );

        device.setTextColour( ui::Theme::textColour( 1 ) );
        device.setFontHeight( 14.0f );
        device.setPadding( 10 );
        updateBtn.setValueColour( ui::Theme::textColour( 1 ) );
        device.setValueBackgroundColour( ui::Theme::backgroundColour( 9 ) );
        updateBtn.onClick = [this]() {
            if( updateBtnCallback )
                updateBtnCallback( rowId );
        };
    }

    int SWUpdateContent::getRowId()
    {
        return rowId;
    }
    void SWUpdateContent::resized()
    {
        auto b = getLocalBounds();

        updates.setBounds( b.removeFromLeft( 300 ) );
        device.setBounds( b.removeFromLeft( 390 ).withTrimmedLeft( 130 ) );
        updateBtn.setBounds( b.removeFromLeft( 400 ).withTrimmedLeft( 90 ).withTrimmedRight( 50 ).withTrimmedTop( 20 ).withTrimmedBottom( 20 ) );
    }

    void SWUpdateContent::setProperties( DeviceData& deviceData )
    {
        device.setText( deviceData.deviceName, juce::dontSendNotification );
        updates.setText( deviceData.version, juce::dontSendNotification );
    }

    ///////////////////////
    //Popup
    //////////////////////
    SWUpdatePopup::SWUpdatePopup( int RowId )
        : rowId( RowId )
    {
        mt::utils::addAndMakeVisible( this, updateBtn, cancelBtn, info, warningMsg );
        info.setTextColour( ui::Theme::textColour( 1 ) );
        info.setTitleFontHeight( 18 );
        info.setValueFontHeight( 18 );
        info.setPadding( 10 );
        info.setValueBackgroundColour( ui::Theme::backgroundColour( 9 ) );
        info.setLine2Text( "The current firmware will be updated, and you will not be able to go back." );

        warningMsg.setFontHeight( 18 );
        warningMsg.setTextColour( ui::Theme::textColour( 2 ) );
        warningMsg.setValueBackgroundColour( ui::Theme::backgroundColour( 9 ) );
        warningMsg.setValueJustification( juce::Justification::centredTop );

        updateBtn.onClick = [&]() { mt::utils::exitDialog( this, UI::APPLY ); };
        cancelBtn.onClick = [&]() { mt::utils::exitDialog( this, UI::CANCEL ); };
    }

    int SWUpdatePopup::getRowId()
    {
        return rowId;
    }

    void SWUpdatePopup::resized()
    {
        auto b  = getLocalBounds();
        auto b2 = b.removeFromTop( 150 );
        warningMsg.setBounds( b2.withTrimmedTop( 50 ) );
        info.setBounds( b2.withTrimmedTop( 100 ) );
        auto b1 = b.removeFromBottom( 70 ).withTrimmedBottom( 20 );
        updateBtn.setBounds( b1.removeFromLeft( 120 ).withTrimmedLeft( 20 ) );
        cancelBtn.setBounds( b1.removeFromRight( 120 ).withTrimmedRight( 20 ) );
    }
}
