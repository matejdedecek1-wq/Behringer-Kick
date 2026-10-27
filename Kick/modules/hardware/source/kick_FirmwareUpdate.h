/*
 * Copyright (c) 2021 MUSIC Tribe
 *
 * This file is part of Kick.
 *
 * Kick is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Affero General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * Kick is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with Kick. If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#pragma once
#if defined( __linux__ ) || defined( __linux ) || defined( linux )
    #include <iostream>
    #include <cstring>
    #include <unistd.h>
    #include <sys/socket.h>
    #include <sys/un.h>
    #include <sys/stat.h>
#endif
#include <thread>
#include "PktMgr/PktMgr.hpp"
#include "kick_Hwpref.h"
#include "kick_Hwcommon.h"

#define DEBUG_FWUPDATE
namespace mt
{

    class FirmwareUpdate
    {
    public:
        FirmwareUpdate();
        ~FirmwareUpdate() = default;

        static FirmwareUpdate& getSwupdateInstance()
        {
            static FirmwareUpdate _swupdate;
            return _swupdate;
        }

        //Linux Updates
        SwUpdateErrorCode_t                              validateImage( bool downloaded = false );
        void                                             updateFirmware( void );
        MCUUpdateAction                                  getMcuUpdateState( int f_data );
        SwUpdateMode_t                                   getSwUpdateMode( void ) { return swUpdateMode; }
        std::string                                      getFwFilepath( SwUpdateMode_t mode );
        std::string                                      getFwFileVersion( SwUpdateMode_t id );
        SwUpdateErrorCode_t                              setSwUpdateMode( SwUpdateMode_t id );
        VersionComparisonResult                          compareVersion( std::string filename );
        VersionComparisonResult                          compareVersion( std::string filePath, linuxFwInfo finfo );
        std::string                                      getPcSwVersion();
        void                                             setPcSwVersion( std::string version );
        void                                             startFirmwareScan();
        bool                                             isFirmwarePresent( SwUpdateMode_t mode );
        SwUpdateMode_t                                   getSwUpdateModeFromString( const juce::String& deviceName );
        std::vector<std::pair<std::string, linuxFwInfo>> getSDCardFirmwareFiles() { return sd_card_firmware_files; }
        std::vector<std::pair<std::string, linuxFwInfo>> getUSBFirmwareFiles() { return usb_firmware_files; };

        void setMcuUpdateState( MCUUpdateAction mcuUpdate )
        {
            mcuUpdateState = mcuUpdate;
        }

        std::vector<std::pair<std::string, linuxFwInfo>> usb_firmware_files;
        std::vector<std::pair<std::string, linuxFwInfo>> sd_card_firmware_files;
        std::string                                      pcFwFile;
        bool                                             pcAppStatus     = false;
        int                                              linuxPercentage = 0;
        int                                              linuxsteps      = 0;
        std::string                                      pcFwVersion     = "";

    private:
        int linuxUpdate( bool status = true );
        int connect_ipc( bool reconnect );
        int progress_ipc_receive( int* connfd, struct progress_msg* msg );

        //MCU updates

        bool                                             isMCUImagePresent();
        int                                              MCUUpdate( juce::String& path );
        bool                                             scanFirmwareOnPCApp();
        std::vector<std::pair<std::string, linuxFwInfo>> scanFirmwareOnUSBDrives();
        std::vector<std::pair<std::string, linuxFwInfo>> scanFirmwareOnSDCards();

        juce::String                imageFormat           = ".img";
        juce::String                mcuFilePath           = "/tmp/fwupdate/Kick_MCU.hex";
        int                         totalProgress         = 1;
        bool                        updateStatus          = false;
        bool                        boDataTransferPending = true;
        bool                        boUpdateComplete      = false;
        std::atomic<SwUpdateMode_t> swUpdateMode          = SwUpdateMode_t::NONE;
        //MCU Related
        std::atomic<MCUUpdateAction> mcuUpdateState;
        std::atomic<SwUpdateMode_t>  fwUpdateMode = SwUpdateMode_t::NONE;

        const char* socketpath = "/tmp/swupdateprog";

        class HexParser
        {
        public:
            void        AddPath( const juce::String& hexFilePath );
            int         readNext( char redData[] );
            juce::int64 getSize();

        private:
            std::unique_ptr<juce::FileInputStream> fileStream;
        };

    public:
        void setSwUpdatecallback( SwupdateStatus func )
        {
            swupdateStatus = func;
        }
        HexParser        parser;
        CPktMgr::HWMsg_t hwMessage;
        SwupdateStatus   swupdateStatus;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( FirmwareUpdate )
    };
}
