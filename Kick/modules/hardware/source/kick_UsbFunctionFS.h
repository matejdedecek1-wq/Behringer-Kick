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

#define INFO  ( ( 'I' ) | ( 'N' << 8 ) | ( 'F' << 16 ) | ( 'O' << 24 ) )
#define FAIL  ( ( 'F' ) | ( 'A' << 8 ) | ( 'I' << 16 ) | ( 'L' << 24 ) )
#define OKAY  ( ( 'O' ) | ( 'K' << 8 ) | ( 'A' << 16 ) | ( 'Y' << 24 ) )
#define DATA  ( ( 'D' ) | ( 'A' << 8 ) | ( 'T' << 16 ) | ( 'A' << 24 ) )
#define CONN  ( ( 'C' ) | ( 'O' << 8 ) | ( 'N' << 16 ) | ( 'N' << 24 ) )
#define READ  0
#define WRITE 1

namespace mt
{

    class USBFunctionFS : public juce::Thread
        , juce::Timer
    {
    public:
        USBFunctionFS();
        ~USBFunctionFS() override;

        void start();
        void stop();
        void run() override;
        void init();
        bool getStatus() { return isConnected.load(); }

        virtual void timerCallback();

        static USBFunctionFS& getUSBInstance()
        {
            static USBFunctionFS _usbFunctionfs;
            return _usbFunctionfs;
        }

        void setUsbcallback( UsbStatus func )
        {
            usbCallback = func;
        }

    private:
        UsbStatus         usbCallback;
        std::string       rFilePath;
        size_t            tfilesize        = 0;
        bool              timerRunning     = false; // Define timerRunning as a member variable
        bool              responseReceived = false;
        int               g_ep_write       = -1;
        int               g_ep_read        = -1;
        int               write_filefd     = -1; //linux will be writing to this file
        int               read_filefd      = -1; //linux will be writing to this file
        int               stdoutfd         = -1;
        std::atomic<bool> isConnected      = false;
        int               openFileDescriptor( const juce::String& filePath );
        void              closeFileDescriptor( int fd );
        void              handleOpenFileError( const juce::String& filePath );
        void              readCommandFromUSB( juce::String& command );
        int               handleCommand( const juce::String& command );
        int               writeData( void* p, size_t size );
        int               writeFile( int fp, void* p, size_t size );

        int sendAck();
        int handleWFileCommand( std::string& filename, size_t size );
        int handleWDataCommand( uint32_t data_size );
        int handleReadFileCommand( std::string path );
        int handleCloseFileCommand();
        int handleDeviceInfoCommand();
        int handlPCstartOpration();
        int handlPCendOpration();

        size_t calculateSizeofFile( std::ifstream* file );
#if defined( JUCE_LINUX ) || defined( __linux__ )
        pid_t popen2( const char* command, int* infp, int* outfp );
#endif
    };
}
