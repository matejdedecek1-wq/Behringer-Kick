
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
// this is a adapter class for the monolith kick application.

#include "contracts/audioengine/audio_engine_signals.h"
#include "contracts/audioengine/audio_engine_contracts_helpers.h"
#include "contracts/audioengine/audio_engine_object_properties.h"
#include "ipc/utils/mt_ipc_json.h"
#include "ipc/utils/mt_ipc_keys.h"
#include "contracts/audioengine/audio_engine_message_types.h"
#include "contracts/audioengine/audio_engine_ipc_keys.h"
#include "contracts/audioengine/audio_interface.h"
#include "contracts/audioengine/audio_engine_signals.h"
#include "juce_events/juce_events.h"

class dispatchResponseReceivedFromAudioEngine
{
public:
    dispatchResponseReceivedFromAudioEngine( const juce::String requestId,
                                             const int          messageType,
                                             const int          objectId,
                                             juce::var          response )
        : requestId( requestId ), messageType( messageType ), objectId( objectId ), response( response )
    {
    }

    void dispatch()
    {
        juce::MessageManager::callAsync(
            [&]() { contracts::AudioEngineSignals::ResponseReceivedFromAudioEngine( requestId, messageType, objectId, response ); } );
    }

public:
    juce::String requestId;
    int          messageType;
    int          objectId;
    juce::var    response;
};

class dispatchObjectPropertiesReceived
{
public:
    dispatchObjectPropertiesReceived( const juce::String requestId,
                                      const int          objectId,
                                      const contracts::AudioEngineObjectProperties& props )
        : requestId( requestId ), objectId( objectId )
    {
        properties.copy( props );
    }

    void dispatch()
    {
        juce::MessageManager::callAsync(
            [&]() { contracts::AudioEngineSignals::ObjectPropertiesReceived( requestId, objectId, properties); } );
    }

public:
    juce::String requestId;
    int          objectId;
    contracts::AudioEngineObjectProperties properties;
};


class KickAudioEngineAdapter
{
public:
    KickAudioEngineAdapter( std::shared_ptr<contracts::AudioInterface> audioInterface )
        : audioInterface(audioInterface)
    {
        contracts::AudioEngineSignals::SendRequestToAudioEngine.connect( [&]( std::shared_ptr<ipc::MTIPCJson> message ) { SendRequestToAudioEngine( message ); } );
    }

    void SendRequestToAudioEngine( std::shared_ptr < ipc::MTIPCJson> message)
    {
        int                                    objectId    = message->getRoot().getProperty( contracts::AUDIO_ENGINE_IPC_KEY_OBJECT_ID, -1 );
        int                                    messageType = message->getProperty( ipc::IPC_KEY_MESSAGE_TYPE );
        juce::String                           requestId   = message->getProperty( ipc::IPC_KEY_REQUEST_MESSAGE_ID );
        
        switch( messageType )
        {
            case contracts::AudioEngineMessageTypes::AUDIO_ENGINE_MESSAGE_GET_PROPERTIES:
            {
                auto      props    = audioInterface->getProperties( objectId );
                dispatchObjectPropertiesReceived(requestId,objectId, props).dispatch();
            break;
            }
            case contracts::AudioEngineMessageTypes::AUDIO_ENGINE_MESSAGE_GET_OBJECT_STATUS:
            {
                int                  status   = audioInterface->getObjectStatus( objectId );
                juce::DynamicObject* pObj     = new juce::DynamicObject();
                pObj->setProperty( contracts::AUDIO_ENGINE_IPC_KEY_OBJECT_STATUS, status );
                dispatchResponseReceivedFromAudioEngine( requestId, messageType, objectId, pObj ).dispatch();
                break;
            }
            case contracts::AudioEngineMessageTypes::AUDIO_ENGINE_MESSAGE_SET_PROPERTIES:
            {
                juce::var properties = message->getProperty( contracts::AUDIO_ENGINE_IPC_KEY_OBJECT_PROPERTIES );

                juce::Array<juce::var>* props = properties.getArray();
                juce::Array<juce::var>  setProps;
                for( auto& prop : *props )
                {
                    juce::var            propertyId     = properties.getProperty( contracts::AUDIO_ENGINE_IPC_KEY_PROPERTY_ID, -1 );
                    juce::var            requestedValue = properties.getProperty( contracts::AUDIO_ENGINE_IPC_KEY_PROPERTY_VALUE, -1 );
                    const juce::var      setValue       = audioInterface->setProperty( objectId,
                                                                                       propertyId,
                                                                                       requestedValue );
                    juce::DynamicObject* pProperty      = new juce::DynamicObject();

                    pProperty->setProperty( contracts::AUDIO_ENGINE_IPC_KEY_SET_VALUE, setValue );
                    pProperty->setProperty( contracts::AUDIO_ENGINE_IPC_KEY_REQUESTED_VALUE, requestedValue );
                    pProperty->setProperty( contracts::AUDIO_ENGINE_IPC_KEY_PROPERTY_ID, propertyId );
                    setProps.add( pProperty );
                }

                juce::DynamicObject* pObj = new juce::DynamicObject();
                pObj->setProperty( contracts::AUDIO_ENGINE_IPC_KEY_OBJECT_PROPERTIES, setProps );
                dispatchResponseReceivedFromAudioEngine( requestId, messageType, objectId, pObj ).dispatch();
                break;
            }
            case contracts::AudioEngineMessageTypes::AUDIO_ENGINE_MESSAGE_SET_PROPERTY:
            {
                juce::var properties     = message->getProperty( contracts::AUDIO_ENGINE_IPC_KEY_OBJECT_PROPERTIES );
                juce::var propertyId     = properties.getProperty( contracts::AUDIO_ENGINE_IPC_KEY_PROPERTY_ID, -1 );
                juce::var requestedValue = properties.getProperty( contracts::AUDIO_ENGINE_IPC_KEY_PROPERTY_VALUE, -1 );

                const juce::var setValue = audioInterface->setProperty( objectId,
                                                                        propertyId,
                                                                        requestedValue );

                juce::DynamicObject* pObj = new juce::DynamicObject();

                pObj->setProperty( contracts::AUDIO_ENGINE_IPC_KEY_SET_VALUE, setValue );
                pObj->setProperty( contracts::AUDIO_ENGINE_IPC_KEY_REQUESTED_VALUE, requestedValue );
                pObj->setProperty( contracts::AUDIO_ENGINE_IPC_KEY_PROPERTY_ID, propertyId );
                break;
            }
            case contracts::AudioEngineMessageTypes::AUDIO_ENGINE_MESSAGE_ALLOCATE:
            {
                int objectType            = message->getProperty( contracts::AUDIO_ENGINE_IPC_KEY_OBJECT_TYPE );
                objectId                  = audioInterface->createObject( objectType );
                juce::DynamicObject* pObj = new juce::DynamicObject();
                pObj->setProperty( contracts::AUDIO_ENGINE_IPC_KEY_OBJECT_ID, objectId );
                dispatchResponseReceivedFromAudioEngine( requestId, messageType, objectId, pObj ).dispatch();
                dispatchResponseReceivedFromAudioEngine( requestId, messageType, objectId, pObj ).dispatch();
                break;
            }
            case contracts::AudioEngineMessageTypes::AUDIO_ENGINE_MESSAGE_DEALLOCATE:
            {
                juce::Array<juce::var> destroyedObjectIds;
                audioInterface->destroyObject( objectId, destroyedObjectIds );

                juce::DynamicObject* pObj = new juce::DynamicObject();
                pObj->setProperty( contracts::AUDIO_ENGINE_IPC_KEY_DESTROYED_OBJECTS, destroyedObjectIds );
                dispatchResponseReceivedFromAudioEngine( requestId, messageType, objectId, pObj ).dispatch();
                break;
            }
            case contracts::AudioEngineMessageTypes::AUDIO_ENGINE_MESSAGE_CLEAR:
            {
                juce::Array<juce::var> destroyedObjectIds;
                audioInterface->clear( destroyedObjectIds );

                juce::DynamicObject* pObj = new juce::DynamicObject();
                pObj->setProperty( contracts::AUDIO_ENGINE_IPC_KEY_DESTROYED_OBJECTS, destroyedObjectIds );
                dispatchResponseReceivedFromAudioEngine( requestId, messageType, objectId, pObj ).dispatch();
                break;
            }
            case contracts::AudioEngineMessageTypes::AUDIO_ENGINE_MESSAGE_CONNECT:
            {
                juce::var connections = message->getProperty( contracts::AUDIO_ENGINE_IPC_KEY_OBJECT_CONNECTIONS );

                juce::DynamicObject* pConnections      = connections.getDynamicObject();
                juce::DynamicObject* actualConnections = new juce::DynamicObject();
                auto                 connectionObjects = pConnections->getProperties();
                for( auto& connectionObject : connectionObjects )
                {
                    try
                    {
                        juce::Array<juce::var> successfulConnections;
                        audioInterface->connect( connectionObject.name.toString().getIntValue(),
                                                 *connectionObject.value.getArray(), successfulConnections );
                        actualConnections->setProperty( connectionObject.name, successfulConnections );
                    } catch( ... )
                    {
                        juce::Logger::writeToLog( "Exception during connecting Objects to Object wiht Id " + connectionObject.name );
                    }
                }

                juce::DynamicObject* response = new juce::DynamicObject();
                response->setProperty( contracts::AUDIO_ENGINE_IPC_KEY_OBJECT_CONNECTIONS, actualConnections );
                dispatchResponseReceivedFromAudioEngine( requestId, messageType, objectId, response ).dispatch();
                break;
            }

            case contracts::AudioEngineMessageTypes::AUDIO_ENGINE_MESSAGE_DISCONNECT:
            {
                juce::var root = message->getRoot();

                bool disconnectionStatus = false;

                if( root.hasProperty( contracts::AUDIO_ENGINE_IPC_KEY_OBJECT_ID_CONNECTED ) )
                {
                    disconnectionStatus = audioInterface->disconnect( root.getProperty( contracts::AUDIO_ENGINE_IPC_KEY_OBJECT_ID, -1 ),
                                                                      root.getProperty( contracts::AUDIO_ENGINE_IPC_KEY_OBJECT_ID_CONNECTED, -1 ) );
                }
                else
                {
                    disconnectionStatus = audioInterface->disconnectAll( root.getProperty( contracts::AUDIO_ENGINE_IPC_KEY_OBJECT_ID, -1 ) );
                }

                juce::DynamicObject* response = new juce::DynamicObject();
                response->setProperty( contracts::AUDIO_ENGINE_IPC_KEY_DISCONNECION_STATUS, disconnectionStatus );
                dispatchResponseReceivedFromAudioEngine( requestId, messageType, objectId, response ).dispatch();
                break;
            }
        }
    }
    private:
    std::shared_ptr<contracts::AudioInterface> audioInterface;
};
