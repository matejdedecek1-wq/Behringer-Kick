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

#include "ipc/utils/mt_ipc_json.h"
#include "ipc/utils/mt_ipc_message_constructor.h"
#include "ipc/utils/mt_ipc_keys.h"
#include "audio_engine_message_types.h"
#include "audio_engine_ipc_keys.h"
#include "audio_engine_contracts_helpers.h"

namespace contracts
{
    std::shared_ptr<ipc::MTIPCJson> AudioEngineContractsHeleper::makeAllocateMessage( const int objectType )
    {
        auto message = ipc::MTIPCMessageConstructor::
            constructRequestMessage( contracts::AudioEngineMessageTypes::AUDIO_ENGINE_MESSAGE_ALLOCATE );
        message->setProperty( contracts::AUDIO_ENGINE_IPC_KEY_OBJECT_TYPE, objectType );
        return message;
    }

    std::shared_ptr<ipc::MTIPCJson> AudioEngineContractsHeleper::makeDeallocateMessage( const int objectId )
    {
        auto message = ipc::MTIPCMessageConstructor::
            constructRequestMessage( contracts::AudioEngineMessageTypes::AUDIO_ENGINE_MESSAGE_DEALLOCATE );
        message->setProperty( contracts::AUDIO_ENGINE_IPC_KEY_OBJECT_ID, objectId );
        return message;
    }

    std::shared_ptr<ipc::MTIPCJson> AudioEngineContractsHeleper::makeClearMessage()
    {
        auto message = ipc::MTIPCMessageConstructor::
            constructRequestMessage( contracts::AudioEngineMessageTypes::AUDIO_ENGINE_MESSAGE_CLEAR );
        return message;
    }

    std::shared_ptr<ipc::MTIPCJson> AudioEngineContractsHeleper::makeConnectMessage( const int objectId1, const int objectId2 )
    {
        auto message = ipc::MTIPCMessageConstructor::constructRequestMessage( contracts::AudioEngineMessageTypes::AUDIO_ENGINE_MESSAGE_CONNECT );
        juce::DynamicObject* pObj    = new juce::DynamicObject();
        juce::Array<juce::var> peerObjects{ objectId2 };
        pObj->setProperty( juce::String(objectId1),  peerObjects); // while decoding convert objectId1 to integer.
        message->setProperty(contracts::AUDIO_ENGINE_IPC_KEY_OBJECT_CONNECTIONS, pObj);
        return message;
    }

    std::shared_ptr<ipc::MTIPCJson> AudioEngineContractsHeleper::makeConnectMessage( const int objectId, juce::Array<int> toObjectIds )
    {
        auto message = ipc::MTIPCMessageConstructor::constructRequestMessage( contracts::AudioEngineMessageTypes::AUDIO_ENGINE_MESSAGE_CONNECT );
        juce::DynamicObject* pObj    = new juce::DynamicObject();
        juce::Array<juce::var> peerObjects;

        for( const auto& id : toObjectIds )
        {
            peerObjects.add(id);
        }
        pObj->setProperty( juce::String( objectId ), peerObjects );
        message->setProperty( contracts::AUDIO_ENGINE_IPC_KEY_OBJECT_CONNECTIONS, pObj );
        return message;
    }

    std::shared_ptr<ipc::MTIPCJson> AudioEngineContractsHeleper::makeConnectMessage( juce::HashMap<int, juce::Array<int>> connectionsMap )
    {
        auto message = ipc::MTIPCMessageConstructor::constructRequestMessage( contracts::AudioEngineMessageTypes::AUDIO_ENGINE_MESSAGE_CONNECT );

        juce::DynamicObject* pObj    = new juce::DynamicObject();
        for( auto itr = connectionsMap.begin(); itr != connectionsMap.end(); itr.next())
        {
            auto arr = itr.getValue();
            juce::Array<juce::var> peerObjects;
            for( auto val : arr )
            {
                peerObjects.add( val );
            }
            pObj->setProperty( juce::String(itr.getKey()), peerObjects );
        }
        message->setProperty( contracts::AUDIO_ENGINE_IPC_KEY_OBJECT_CONNECTIONS, pObj );
        return message;
    }

    std::shared_ptr<ipc::MTIPCJson> AudioEngineContractsHeleper::makeDisconnectMessage( const int objectId )
    {
        auto message = ipc::MTIPCMessageConstructor::
            constructRequestMessage( contracts::AudioEngineMessageTypes::AUDIO_ENGINE_MESSAGE_DISCONNECT);
        message->setProperty(contracts::AUDIO_ENGINE_IPC_KEY_OBJECT_ID, objectId);
        return message;
    }

    std::shared_ptr<ipc::MTIPCJson> AudioEngineContractsHeleper::makeDisconnectMessage( const int objectId1, const int objectId2 )
    {
        auto message = ipc::MTIPCMessageConstructor::
            constructRequestMessage( contracts::AudioEngineMessageTypes::AUDIO_ENGINE_MESSAGE_DISCONNECT );
        message->setProperty( contracts::AUDIO_ENGINE_IPC_KEY_OBJECT_ID, objectId1 );
        message->setProperty( contracts::AUDIO_ENGINE_IPC_KEY_OBJECT_ID_CONNECTED, objectId2 );
        return message;
    }

    std::shared_ptr<ipc::MTIPCJson> AudioEngineContractsHeleper::makeSetPropertiesMessage( const int                            objectId,
                                                                                           const juce::HashMap<int, juce::var>& properties )
    {
        auto message = ipc::MTIPCMessageConstructor::
            constructRequestMessage( contracts::AudioEngineMessageTypes::AUDIO_ENGINE_MESSAGE_SET_PROPERTIES );

        message->setProperty( contracts::AUDIO_ENGINE_IPC_KEY_OBJECT_ID, objectId );
        juce::DynamicObject* pObj = new juce::DynamicObject();
        juce::Array<juce::var> props; 
        for( auto itr = properties.begin(); itr != properties.end(); itr.next() )
        {
            juce::DynamicObject* pProperty = new juce::DynamicObject();
            pObj->setProperty( contracts::AUDIO_ENGINE_IPC_KEY_PROPERTY_ID, itr.getKey() );
            pObj->setProperty( contracts::AUDIO_ENGINE_IPC_KEY_PROPERTY_VALUE, itr.getValue() );
            props.add( pProperty );
        }
        message->setProperty( contracts::AUDIO_ENGINE_IPC_KEY_OBJECT_PROPERTIES, pObj);
        return message;
    }

    std::shared_ptr<ipc::MTIPCJson> AudioEngineContractsHeleper::makeSetPropertyMessage( const int        objectId,
                                                                                         const int        propertyId,
                                                                                         const juce::var& value )
    {
        auto message = ipc::MTIPCMessageConstructor::
            constructRequestMessage( contracts::AudioEngineMessageTypes::AUDIO_ENGINE_MESSAGE_SET_PROPERTY );

        message->setProperty( contracts::AUDIO_ENGINE_IPC_KEY_OBJECT_ID, objectId );
        juce::DynamicObject* pObj = new juce::DynamicObject();
        pObj->setProperty( contracts::AUDIO_ENGINE_IPC_KEY_PROPERTY_ID, propertyId);
        pObj->setProperty( contracts::AUDIO_ENGINE_IPC_KEY_PROPERTY_VALUE, value);
        message->setProperty( contracts::AUDIO_ENGINE_IPC_KEY_OBJECT_PROPERTIES, pObj );

        return message;
    }

    std::shared_ptr<ipc::MTIPCJson> AudioEngineContractsHeleper::makeGetPropertiesMessage( const int objectId )
    {
        auto message = ipc::MTIPCMessageConstructor::
            constructRequestMessage( contracts::AudioEngineMessageTypes::AUDIO_ENGINE_MESSAGE_GET_PROPERTIES );

        message->setProperty( contracts::AUDIO_ENGINE_IPC_KEY_OBJECT_ID, objectId );
        return message;
    }

    std::shared_ptr<ipc::MTIPCJson> AudioEngineContractsHeleper::makeGetStatusMessage( const int objectId )
    {
        auto message = ipc::MTIPCMessageConstructor::
            constructRequestMessage( contracts::AudioEngineMessageTypes::AUDIO_ENGINE_MESSAGE_GET_OBJECT_STATUS );

        message->setProperty( contracts::AUDIO_ENGINE_IPC_KEY_OBJECT_ID, objectId );
        return message;
    }

    std::shared_ptr<ipc::MTIPCJson> AudioEngineContractsHeleper::makeResponseMessage( const int           objectId,
                                                                                      const juce::String& requestMessageId,
                                                                                      const int           requestMessageType,
                                                                                      const int           responseCode,
                                                                                      const juce::var&    response )
    {
        auto message = ipc::MTIPCMessageConstructor::
            constructResponseMessage( contracts::AudioEngineMessageTypes::AUDIO_ENGINE_RESPONSE_MESSAGE, requestMessageId);

        message->setProperty( contracts::AUDIO_ENGINE_IPC_KEY_REQUEST_MESSAGE_TYPE, requestMessageType);
        message->setProperty( contracts::AUDIO_ENGINE_IPC_KEY_OBJECT_ID, objectId);
        message->setProperty( contracts::AUDIO_ENGINE_IPC_KEY_RESPONSE_CODE, responseCode );
        message->setProperty( contracts::AUDIO_ENGINE_IPC_KEY_RESPONSE_DATA, response );
        return message;
    }

}
