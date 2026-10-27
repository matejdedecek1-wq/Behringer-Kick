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
#if( JUCE_LINUX || __linux__ )
    #include <sys/sysinfo.h>
    #include <chrono>
    #include <cstdio>
    #include <cstring>
    #include <thread>
    #include <sstream>
    #include <unistd.h>
    #include <fstream>
    #include <cstdlib>
    #include <linux/input.h>
    #include <sys/types.h>
    #include <termios.h>
    #include <sys/ioctl.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <net/if.h>
    #include <unistd.h>
#endif
#include <fstream>
#include <iostream>
#include <filesystem>
#include <string>
#include <regex>
#include <functional>

#define USB_DEVICE "/dev/ttyGS0"
#define BAUDRATE   B115200

namespace mt
{
    struct sysEthInfo
    {
        bool        status  = false;
        std::string IP      = "0.0.0.0";
        std::string subnet  = "0.0.0.0";
        std::string gateway = "0.0.0.0";
    };

    struct MemoryStatus
    {
        char total[15];
        char available[15];
        char used[15];
    };

    struct InternalStorageStatus
    {
        char totalSpace[15];
        char availableSpace[15];
        char usedSpace[15];
    };

    struct USBStorageStatus
    {
        char totalSpace[25];
        char availableSpace[25];
    };

    struct DeviceInfo
    {
        char                         serialNumber[32];
        char                         firmwareVersion[16];
        char                         firmwareBuildDate[16];
        struct MemoryStatus          memoryStatus;
        struct InternalStorageStatus internalStorageStatus;
        struct USBStorageStatus      usbStorageStatus;
    };

    struct dateInfo
    {
        int DD   = 0;
        int MM   = 0;
        int YYYY = 0;
    };
    struct timeInfo
    {
        int HH = 0;
        int MM = 0;
    };

    struct linuxFwInfo
    {
        std::string linuxname = "";
        int         major     = 0;
        int         minor     = 0;
        int         patch     = 0;
    };
    struct mcuFwInfo
    {
        std::string mcuname = "";
        int         major   = 0;
        int         minor   = 0;
    };

    enum class VersionComparisonResult
    {
        Less,
        Equal,
        Greater,
        Error
    };

    class HwPref : public juce::Thread
    {
        typedef std::function<void()> PowerButtonUI;

    public:
        void                                             getIP( const std::string interface, sysEthInfo& info );
        std::string                                      getGateway( const std::string& interface );
        int                                              setIP( const std::string& interface, const std::string& ipAddress, const std::string& subnetMask, const std::string& gateway );
        std::string                                      getSubnetMask( const std::string& interface );
        int                                              getDateTime( dateInfo& date, timeInfo& time, const juce::String& timezone );
        int                                              setTimeZone( juce::String& str );
        int                                              setBrightness( int value );
        int                                              getDaysInMonth( int month, int year );
        bool                                             isEthernetCableConnected();
        bool                                             ethernetEnable( bool status );
        bool                                             isInterfaceUp( void );
        std::vector<std::string>                         getSWUFiles( const std::string& folderPath, const std::string& imageFormat = ".img" );
        std::vector<std::pair<std::string, linuxFwInfo>> getSWUFileswithVersion( const std::string& folderPath, const std::string& imageFormat = ".img" );
        int                                              startDhcp( void );
        int                                              stopDhcp( void );
        int                                              factoryReset( void );
        int                                              shutdown( void );
        void                                             setDate( int Day, int Month, int year );
        void                                             setTime( int hour, int minute );
        DeviceInfo                                       getDeviceInfo();
        bool                                             isStorageconnected( std::string id );
        bool                                             isImgFile( std::string filepath );
        std::string                                      isImageFound( juce::String Path, juce::String format );
        bool                                             getFwVersion( std::string& path, linuxFwInfo& linfo, mcuFwInfo& minfo );
        bool                                             getLinuxVersion( std::string& path, linuxFwInfo& linfo );
        bool                                             decryptopenSSLImage( std::string fwfilepath );
        bool                                             removeHeaderFromImage( std::string fwfilepath );
        bool                                             extractImage( std::string path = "/tmp/fwupdate" );
        juce::String                                     getDevSerial();
        bool                                             addVersionNotoFile( std::string filename );
        bool                                             getVersionfromFile( std::string filename, linuxFwInfo& info );

        VersionComparisonResult isLinuxFirmwareVersionLess( const linuxFwInfo& current, const linuxFwInfo& stored );
        VersionComparisonResult isMCUFirmwareVersionLess( const mcuFwInfo& current, const mcuFwInfo& stored );
        static HwPref&          getHwPref()
        {
            static HwPref _hwPref;
            return _hwPref;
        }
        void        run() override;
        std::string getFwFileName( void ) { return fwFileName; }
        void        setPopupFunction( PowerButtonUI func ) { powerButtonUI = func; }
        void        setSurfaceVersion( juce::String str ) { surfaceVerison = str; }

        juce::String getFirmwareVersion();
        juce::String getSurfaceVersion() { return surfaceVerison; }

    private:
        HwPref();
        std::string  fwFileName      = "";
        std::string  decrptionDir    = "/tmp/image/";
        std::string  extractDir      = "/tmp/fwupdate";
        std::string  fwHeaderFile    = "Kick_header.img";
        std::string  fwFileFullpath  = "";
        juce::String surfaceVerison  = "";
        std::string  decryptedFwname = "Kick.img";
        //        std::string imageFormat = ".img";
        std::string   decrptKey    = "PJSDSOMKICK";
        int           idecrptKey   = 123;
        int           fw_ver_index = -1;
        int           serialComPortHandler;
        std::ifstream inputDevice;
        PowerButtonUI powerButtonUI;
        // Setter for the function pointer
        std::string executeCommand( const std::string& command );
        int         executeSetCommand( const std::string& command );
    };
}
