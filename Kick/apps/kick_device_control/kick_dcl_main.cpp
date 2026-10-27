
/*
 * Copyright (c) 2024 MUSIC Tribe
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

#include "input_events/input_events.h"

#include "ipc/dispatcher/mt_ipc_message_dispatcher.h"
#include "kick_device_message_handler.h"
#include "kick_hardware_event_publisher.h"

#include "contracts/config/dcl_ipc_config.h"
#include "contracts/config/ipc_config_constants.h"
#include "contracts/utils/app/ipc_application.h"

#include "ipc/utils/mt_ipc_helpers.h"
#include "thread"

#if JUCE_WINDOWS
    #include <Windows.h>
#endif

//std::mutex mutex1;
//std::mutex mutex2;
//
//void thread1()
//{
//    std::lock_guard<std::mutex> lock1( mutex1 );
//    std::this_thread::sleep_for( std::chrono::milliseconds( 7000 ) ); // Simulate some work
//    std::lock_guard<std::mutex> lock2( mutex2 );
//    std::cout << "Thread 1 has acquired both mutexes" << std::endl;
//}
//
//void thread2()
//{
//    std::lock_guard<std::mutex> lock2( mutex2 );
//    std::this_thread::sleep_for( std::chrono::milliseconds( 7000 ) ); // Simulate some work
//    std::lock_guard<std::mutex> lock1( mutex1 );
//    std::cout << "Thread 2 has acquired both mutexes" << std::endl;
//}

//==============================================================================
class DeviceControl : public contracts::IPCApplication
{
public:
    //==============================================================================
    DeviceControl()
        : IPCApplication( contracts::DCL_PROCESS_NAME,
                          contracts::DCL_PROCESS_NAME,
                          contracts::DCL_PROCESS_NAME,
                          true, true, true ) {}

    bool moreThanOneInstanceAllowed() override
    {
        return false;
    }

    //==============================================================================
    void initialise( const juce::String& commandLine ) override
    {
        contracts::IPCApplication::initialise( commandLine );
        publisher.reset( new kick::dcl::KickHardwareEventPublisher( dispatcher() ) );
        Controls::InputEvents::initialize();
       /* t1 = new std::thread( thread1 );
        t2 = new std::thread( thread2 );*/

    }

    void shutdown() override
    {
        Controls::InputEvents::shutdown();
        publisher.reset();
        contracts::IPCApplication::shutdown();
        /*t1->join();
        t2->join();
        delete t1;
        delete t2;*/
    }

    void addMessageHandlers( std::vector<std::shared_ptr<ipc::MTIIPCMessageHandler>>& handlers )
    {
        handlers.push_back( std::shared_ptr<ipc::MTIIPCMessageHandler>( new kick::dcl::KickDeviceMessageHandler() ) );
    }

    juce::var getConfig( const juce::String& commandLine )
    {
        return ( commandLine.isEmpty() ?
                     contracts::DclIPCConfig().getStandAloneConfig() :
                     ipc::createJsonObjectFromCommandLineString( commandLine ) );
    }

private:
    std ::unique_ptr<kick::dcl::KickHardwareEventPublisher> publisher;
    /*std::thread*                                            t1;
    std::thread*                                            t2;*/
};

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION( DeviceControl )
