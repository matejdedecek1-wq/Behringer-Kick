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

#pragma once

#include "juce_core/juce_core.h"
#include "ipc/utils/mt_ipc_keys.h"
#include "connection_id_keys.h"
#include "ipc_config_constants.h"

namespace contracts
{

    /*
        * Note - A Child process can spawn its own child processes. The arguments passed to child processes can have processes config in them.
        * something like
        * {"process_config": "children":[{
        * "arguments": {
        * "connection_config"{},
        * "process_config":{
        * "children":[]}}}]}
        */

    static juce::var prepareClientConfig( const int transport, const int connectionId, const int port, const juce::String& url, const int timeout,
                                   const juce::String& name )
    {
        juce::DynamicObject* pObj = new juce::DynamicObject();
        pObj->setProperty( ipc::IPC_KEY_TRANSPORT, transport );
        pObj->setProperty( ipc::IPC_KEY_CONNECTION_ID, connectionId );
        pObj->setProperty( ipc::IPC_KEY_PORT, port );
        pObj->setProperty( ipc::IPC_KEY_CONNECTION_TIMEOUT, timeout );
        pObj->setProperty( ipc::IPC_KEY_URL, url );
        pObj->setProperty( ipc::IPC_KEY_CONNECTION_NAME, name );
        return pObj;
    }

    static juce::var prepareConnectionValidationConfig( const int connectionId, const juce::String& uuid )
    {
        juce::DynamicObject* pObj = new juce::DynamicObject();
        pObj->setProperty( ipc::IPC_KEY_CONNECTION_ID, connectionId );
        pObj->setProperty( ipc::IPC_KEY_UUID, uuid );
        return pObj;
    }

    static juce::var prepareServerConfig(const juce::String& bind_address, const int port,
                                          bool enable_client_validation = false, const juce::Array<juce::var>& validConnections =  juce::var() )
    {
        juce::DynamicObject* pServer = new juce::DynamicObject();
        pServer->setProperty( ipc::IPC_KEY_CONNECTIONS, juce::var( validConnections ) );
        pServer->setProperty( ipc::IPC_KEY_BIND_ADDRESS, bind_address );
        pServer->setProperty( ipc::IPC_KEY_PORT, port );
        pServer->setProperty( ipc::IPC_KEY_CLIENT_ENABLE_VALIDATION, enable_client_validation );
        return pServer;
    }

    static juce::var prepareConnectionConfig( const juce::String uuid, bool hasClients, bool hasServer,
                                       const juce::var& clients = juce::var(), int clientStartDelay = contracts::CLIENT_START_DELAY,
                                       const juce::var& server = juce::var() )
    {
        juce::DynamicObject* pConnectionConfig = new juce::DynamicObject();
        pConnectionConfig->setProperty( ipc::IPC_KEY_UUID, uuid );
        if( hasServer )
        {
            pConnectionConfig->setProperty( ipc::IPC_KEY_SERVER, server );
        }
        if( hasClients )
        {
            pConnectionConfig->setProperty( ipc::IPC_KEY_CLIENTS, juce::var( clients ) );
            pConnectionConfig->setProperty( ipc::IPC_KEY_CLIENT_START_DELAY, clientStartDelay );
        }
        return pConnectionConfig;
    }

    static juce::var prepareChildConfig( int hostId, const juce::String& path, const juce::String& name, juce::var arguments )
    {
        juce::DynamicObject* pObj = new juce::DynamicObject();
        pObj->setProperty( ipc::IPC_KEY_ARGUMENTS, arguments );
        pObj->setProperty( ipc::IPC_KEY_HOST_ID, hostId );
        pObj->setProperty( ipc::IPC_KEY_PROCESS_PATH, path );
        pObj->setProperty( ipc::IPC_KEY_PROCESS_NAME, name );
        return pObj;
    }

    static juce::var prepareConnectionName( int connectionId, const juce::String& connectionName )
    {
        juce::DynamicObject* pObj = new juce::DynamicObject();
        pObj->setProperty( ipc::IPC_KEY_CONNECTION_ID, connectionId );
        pObj->setProperty( ipc::IPC_KEY_CONNECTION_NAME, connectionName );
        return pObj;
    }

    static juce::var prepareConnectionIdConfig( int parent, int logger, int main, int dcl, int engine, int gui, int model )
    {
        juce::DynamicObject* cfg = new juce::DynamicObject();
        cfg->setProperty( contracts::CONFIG_KEY_PARENT_CONNECTION_ID, parent );
        cfg->setProperty( contracts::CONFIG_KEY_LOGGER_CONNECTION_ID, logger );
        cfg->setProperty( contracts::CONFIG_KEY_MAIN_CONNECTION_ID, main );
        cfg->setProperty( contracts::CONFIG_KEY_DCL_CONNECTION_ID, dcl);
        cfg->setProperty( contracts::CONFIG_KEY_ENGINE_CONNECTION_ID, engine );
        cfg->setProperty( contracts::CONFIG_KEY_GUI_CONNECTION_ID, gui );
        cfg->setProperty( contracts::CONFIG_KEY_MODEL_CONNECTION_ID, model );
        return cfg;
    }

    static juce::var prepareConnectedHostConfig( int connectionId, int connectedHost )
    {
        juce::DynamicObject* cfg = new juce::DynamicObject();
        cfg->setProperty( ipc::IPC_KEY_CONNECTION_ID, connectionId);
        cfg->setProperty( ipc::IPC_KEY_CONNECTED_HOST, connectedHost );
        return cfg;
    }
}
