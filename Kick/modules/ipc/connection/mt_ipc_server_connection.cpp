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

#include "mt_ipc_server_connection.h"
#include "../utils/mt_ipc_json.h"
#include "juce_core/juce_core.h"
#include "../utils/mt_ipc_constants.h"
#include "../utils/mt_ipc_keys.h"
#include "../utils/mt_ipc_message_types.h"
#include "../utils/mt_ipc_enums.h"
#include "../utils/mt_ipc_message_constructor.h"

namespace ipc
{
    MTIPCServerConnection::MTIPCServerConnection(const juce::String& uuid, const int connectionId, const std::shared_ptr<juce::var> config,
                                                  std::shared_ptr<ipc::MTIIPCConnectionCallback> pCallback )
        : ipc::MTIPCConnection( connectionId, config, pCallback )
        ,uuid(uuid)
    {
       
    }
    
    void MTIPCServerConnection::onDisconnected()
    {
        isRegistered = false;
        ipc::MTIPCConnection::onDisconnected();
    }

    void MTIPCServerConnection::onConnected()
    {
        std::shared_ptr<ipc::MTIPCJson> handshakeMessage = ipc::MTIPCMessageConstructor::constructHeaderMessage(ipc::MTIPCMessageType::IPC_MESSAGE_TYPE_REGISTER);
        handshakeMessage->setProperty( ipc::IPC_KEY_CONNECTION_ID, connectionIdentifier() );
        handshakeMessage->setProperty( ipc::IPC_KEY_UUID, uuid);
        handshakeMessage->setProperty( ipc::IPC_KEY_CONNECTION_NAME, connectionName() );
        publish( handshakeMessage );
    }

    void MTIPCServerConnection::onMessage( std::shared_ptr<ipc::MTIPCJson> message )
    {
        if( isRegistered )
        {
            MTIPCConnection::onMessage( message );
            return;
        }
        int type = message->getProperty( ipc::IPC_KEY_MESSAGE_TYPE );
        if( type == ipc::MTIPCMessageType::IPC_MESSAGE_TYPE_REGISTERATION_STATUS )
        {
            int status   = message->getProperty( ipc::IPC_KEY_REGISTERATION_STATUS );
            isRegistered = ( status == ipc::MTIPCRegistrationStatus::IPC_MESSAGE_REGISTERATION_SUCCESS );
            if( isRegistered )
                MTIPCConnection::onConnected();
        }
    }

}
