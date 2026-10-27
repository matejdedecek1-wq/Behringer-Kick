
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

#include "kick_log_message_handler.h"
#include "kick_logger_config.h"

#include "contracts/config/main_ipc_config.h"
#include "contracts/config/ipc_config_constants.h"
#include "contracts/utils/app/ipc_application.h"
#include "kick_heart_beat_message_handler.h"
#include "kick_heart_beat_monitor.h"
#include "ipc/subsystem/mt_ipc_subsystem.h"
#include "kick_heart_beat_data.h"
#include "ipc/process/mt_ipc_process_manager.h"
#include "ipc/connection/mt_ipc_connection_manager.h"
#include "kick_exit_message_handler.h"

#define COMMAND_LINE_ARGUMENT_STANDALONE "standalone"

class KickMain : public contracts::IPCApplication
{
public:
    //==============================================================================
    KickMain()
        : IPCApplication( contracts::MAIN_PROCESS_NAME,
                          kick::logger::LOG_FILE_BASE_NAME,
                          kick::logger::METRICS_FILE_BASE_NAME,
                          false ),
        heartBeatData( new kick::main::KickHeartBeatData(std::bind(&KickMain::onHeartBeatNotReceived, this, std::placeholders::_1) ))
    {}

    bool moreThanOneInstanceAllowed() override
    {
        return false;
    }

    void initialise( const juce::String& commandLine ) override
    {
        IPCApplication::initialise( commandLine );
        monitor.reset( new kick::main::KickHearBeatMonitor( heartBeatData, std::bind( &KickMain ::onHeartBeatNotReceived, this, std::placeholders::_1 ) ) );
        monitor->startMonitoring();
    }

    void shutdown()
    {
        monitor->stopMonintoring();
        monitor.reset();
        heartBeatData.reset();
        IPCApplication::shutdown();
    }

    juce::var getConfig( const juce::String& commandLine ) override
    {
        return (commandLine == COMMAND_LINE_ARGUMENT_STANDALONE ?
            contracts::KickIPCConfig().getStandAloneConfig()
            : contracts::KickIPCConfig().getConfig());
    }

    void addMessageHandlers( std::vector<std::shared_ptr<ipc::MTIIPCMessageHandler>>& handlers )
    {
        handlers.push_back( std::shared_ptr<ipc::MTIIPCMessageHandler>( new kick::main::KickLogMessageHandler() ) );
        handlers.push_back( std::shared_ptr<ipc::MTIIPCMessageHandler>(new kick::main::KickHeartBeatMessageHandler( heartBeatData) ));
        handlers.push_back( std::shared_ptr<ipc::MTIIPCMessageHandler>( new kick::main::KickExitMessageHandler() ) );
    }
private:
    void onHeartBeatNotReceived( const int hostId)
    {
        auto ss = subsystem(); // restart the complete system if one of the process is hanged.
        if( ss )
        {
            ss->stop();
            ss->start();
        }
            
    }

private:
    std::shared_ptr<kick::main::KickHeartBeatData> heartBeatData;
    std::unique_ptr<kick::main::KickHearBeatMonitor> monitor;

};

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION( KickMain )
