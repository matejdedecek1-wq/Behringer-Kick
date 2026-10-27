/*
 * Copyright (c) 2022 MUSIC Tribe
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

#include "contracts/config/engine_ipc_config.h"
#include "contracts/config/ipc_config_constants.h"
#include "contracts/utils/app/ipc_application.h"
#include "ipc/utils/mt_ipc_helpers.h"
#include "allocate_message_handler.h"
#include "deallocate_message_handler.h"
#include "clear_message_handler.h"
#include "connect_message_handler.h"
#include "disconnect_message_handler.h"
#include "set_properties_message_handler.h"
#include "set_property_message_handler.h"
#include "get_object_status_message_handler.h"
#include "get_properties_message_handler.h"
#include "kick_audio_interface.h"


//==============================================================================
class Engine : public contracts::IPCApplication
{
public:
    //==============================================================================
    Engine()
        : IPCApplication( contracts::ENGINE_PROCESS_NAME,
                          contracts::ENGINE_PROCESS_NAME,
                          contracts::ENGINE_PROCESS_NAME,
                          true, true, true )
        , iface( new kick::ae::KickAudioInterface() )
        
    {
        
    }

    bool moreThanOneInstanceAllowed() override
    {
        return false;
    }

    void addMessageHandlers( std::vector<std::shared_ptr<ipc::MTIIPCMessageHandler>>& handlers )
    {
        handlers.push_back( std::shared_ptr<ipc::MTIIPCMessageHandler>(new kick::ae::AllocateMessageHandler(iface) ));
        handlers.push_back( std::shared_ptr<ipc::MTIIPCMessageHandler>(new kick::ae::DeallocateMessageHandler(iface) ));
        handlers.push_back( std::shared_ptr<ipc::MTIIPCMessageHandler>(new kick::ae::ConnectMessageHandler(iface) ));
        handlers.push_back( std::shared_ptr<ipc::MTIIPCMessageHandler>(new kick::ae::DisconnectMessageHandler(iface) ));
        handlers.push_back( std::shared_ptr<ipc::MTIIPCMessageHandler>(new kick::ae::ClearMessageHandler(iface) ));
        handlers.push_back( std::shared_ptr<ipc::MTIIPCMessageHandler>(new kick::ae::SetPropertiesMessageHandler(iface) ));
        handlers.push_back( std::shared_ptr<ipc::MTIIPCMessageHandler>(new kick::ae::SetPropertyMessageHandler(iface) ));
        handlers.push_back( std::shared_ptr<ipc::MTIIPCMessageHandler>(new kick::ae::GetPropertiesMessageHandler(iface) ));
        handlers.push_back( std::shared_ptr<ipc::MTIIPCMessageHandler>(new kick::ae::GetObjectStatusMessageHandler(iface) ));

    }
    juce::var getConfig( const juce::String& commandLine )
    {
        return (commandLine.isEmpty() ?
            contracts::EngineIPCConfig().getStandAloneConfig() :
            ipc::createJsonObjectFromCommandLineString(commandLine));
    }

private:
    std::shared_ptr<contracts::AudioInterface> iface;
};

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION( Engine )
