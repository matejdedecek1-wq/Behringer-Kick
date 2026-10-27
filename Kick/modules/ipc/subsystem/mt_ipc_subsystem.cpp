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

#include "juce_core/juce_core.h"
#include "mt_ipc_subsystem.h"
#include "../utils/mt_ipc_constants.h"
#include "../utils/mt_ipc_message_constructor.h"
#include "../utils/mt_ipc_message_types.h"
#include "../utils/mt_ipc_keys.h"
#include "../utils/mt_ipc_enums.h"

namespace ipc
{
    MTIPCSubsystem::MTIPCSubsystem( const juce::String&                                    name,
                                    const juce::var&                                       config,
                                    std::vector<std::shared_ptr<ipc::MTIIPCMessageHandler>> handlers,
                                    const int                                              parentProcessConnectionId )
        : ipc::MTIPCBaseSubsystem( name, config, handlers, parentProcessConnectionId )
    {
            start();
    }
   
    MTIPCSubsystem::~MTIPCSubsystem()
    {
       
    }

    void MTIPCSubsystem::start()
    {
        ipc::MTIPCBaseSubsystem::start();
        /*bool established = waitForClientConnections();
        int status = ipc::MTIPCConnectionStatus::IPC_CONNECTION_STATUS_CONNECTED;
        juce::Array<juce::var> unconnected;
        std::vector<int>       unconnectedConnectionIds;

        if( established )
        {
            juce::Logger::writeToLog( "All connections established.." );
        }
        else
        {
            updateUnconnectedClientConnectionids(unconnectedConnectionIds);

            if( unconnectedConnectionIds.empty() )
            {
                juce::Logger::writeToLog( "waitForClientConnections failed but there are no unconnected connections.." );
                return;
            }

            if( tryReconnect( unconnectedConnectionIds ) )
            {
                status = ipc::MTIPCConnectionStatus::IPC_CONNECTION_STATUS_CONNECTED;
            }
            else
            {
                const int    parentConnectionId = parentProcessConnectionId();

                bool quit = false;

                juce::String logIds;

                for( auto id : unconnectedConnectionIds )
                {
                    if( parentConnectionId >= 0 && parentConnectionId == id )
                    {
                        quit = true;
                        break;
                    }
                    logIds += juce::String( id ) + " ";
                    unconnected.add( id );
                }

                juce::Logger::writeToLog( "Connection not established with connections " + logIds );
                if( quit )
                {
                    juce::MessageManager::callAsync( [&]() { juce::JUCEApplicationBase::getInstance()->quit(); } );
                    return;
                }
                else
                {
                    status = ipc::MTIPCConnectionStatus::IPC_CONNECTION_STATUS_NOT_CONNECTED;
                }
            }   
        }

        auto md = dispatcher();

        if( !md ) return;
           
        auto message = ipc ::MTIPCMessageConstructor::constructEventMessage( ipc::MTIPCMessageType ::IPC_MESSAGE_TYPE_CONNECTION_STATUS );
        message->setProperty( ipc::IPC_KEY_CONNECTION_STATUS, status );

        if( !unconnected.isEmpty() )
            message->setProperty( ipc::IPC_KEY_UN_CONNECTED_CONNECTION_IDS, unconnected );

        md->publish( parentProcessConnectionId(), message );*/

    }

   /* bool MTIPCSubsystem::tryReconnect( std::vector<int> connectionIds )
    {
        auto cm = connectionManager();
        if( !cm ) return true;
        bool connected = true;
        for( auto id : connectionIds )
        {
            connected = connected && cm->connect( id );
        }
        return connected;
    }*/

    void MTIPCSubsystem::handleDisconnect( int connectionId, const juce::String& name )
    {
        juce::Logger::writeToLog( "Connection " + name + " Disconnected" );
    }

    void MTIPCSubsystem::handleConnect( int connectionId, const juce::String& name )
    {
        juce::Logger::writeToLog( name + " is Connected" );
    };

    void MTIPCSubsystem::reconnectionAttemptFailed( int connectionId, const juce::String& name )
    {
        juce::Logger::writeToLog( "Reconnecting  " + name + " Failed" );
    };

    void MTIPCSubsystem::reconnectionAttemptSuccess( int connectionId, const juce::String& name )
    {
        juce::Logger::writeToLog( "Reconnecting " + name + " Succeeded" );
    };
    void MTIPCSubsystem::childProcessStopped( [[maybe_unused]] int processIdentifier, [[maybe_unused]] int exitCode, const juce::String& name )
    {
        juce::Logger::writeToLog( name + " Exited with Code " + juce::String( exitCode ) );
    };

    void MTIPCSubsystem::childProcessStarted( [[maybe_unused]] int processIdentifier, const juce::String& name )
    {
        //juce::Logger::writeToLog( name + " Started" );
    }

    void MTIPCSubsystem::childProcessStartFailed( [[maybe_unused]] int processIdentifier, const juce::String& name )
    {
       // juce::Logger::writeToLog( "Failed to Start " + name );
    }

    bool MTIPCSubsystem::handleMessage( int connectionId, std::shared_ptr<ipc::MTIPCJson> message )
    {
        int messageType = message->getProperty( ipc::IPC_KEY_MESSAGE_TYPE, ipc::MTIPCMessageType::IPC_MESSAGE_TYPE_INVALID );

        if( messageType == ipc::MTIPCMessageType::IPC_MESSAGE_TYPE_CONNECTION_STATUS )
        {
            if( ipc::MTIPCConnectionStatus::IPC_CONNECTION_STATUS_NOT_CONNECTED ==
                (int)message->getProperty( ipc::IPC_KEY_CONNECTION_STATUS, ipc::MTIPCConnectionStatus::IPC_CONNECTION_STATUS_CONNECTED ) )
            {
                auto pm = processManager();
                if( pm )
                {
                    pm->stop();
                    juce::MessageManager::callAsync( [&, pm]() { pm->start(); } );
                }
            }
            return true;
        }
        return false;
    }
}
