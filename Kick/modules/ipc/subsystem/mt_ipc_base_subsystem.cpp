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
#include "juce_events/juce_events.h"
#include "mt_ipc_base_subsystem.h"
#include "../connection/mt_ipc_connection_manager.h"
#include "../dispatcher/mt_ipc_message_dispatcher.h"
#include "../processor/mt_ipc_message_processor.h"
#include "../processor/mt_ipc_handler_store.h"
#include "../process/mt_ipc_process_manager.h"
#include "../utils/mt_ipc_constants.h"
#include "../utils/mt_ipc_keys.h"
#include "../utils/mt_ipc_json.h"
#include "../utils/mt_ipc_message_constructor.h"
#include "../connection/mti_ipc_connection_callback.h"
#include "../process/mti_ipc_process_manager_callback.h"
#include "mt_ipc_delayed_connector.h"
#include "mt_ipc_process_restarter.h"

namespace ipc
{
    class MTIPCProcessManagerCallback : public ipc::MTIIPCProcessManagerCallback
    {
    public:
        MTIPCProcessManagerCallback( std::function<void( int, const juce::String& )>      onChildProcessStarted,
                                     std::function<void( int, int, const juce::String& )> onChildProcessExited,
                                     std::function<void( int, const juce::String& )>      onChildProcessStartFailed,
                                     std::function<void( int )>                           restartChildProcess )
            : restartChildProcess( restartChildProcess )
            , onChildProcessStarted( onChildProcessStarted )
            , onChildProcessExited( onChildProcessExited )
            , onChildProcessStartFailed( onChildProcessStartFailed )
        {
        }

        void processStarted( int processIdentifier, const juce::String& processName ) override
        {
            onChildProcessStarted( processIdentifier, processName );
            //juce::Logger::writeToLog(juce::String(processIdentifier) + " Started..");
        }

        void failedToStartProcess( int processIdentifier, const juce::String& processName ) override
        {
            onChildProcessStartFailed( processIdentifier, processName );
            //juce::Logger::writeToLog(juce::String(processIdentifier) + " Started..");
        }

        void processExited( int processIdentifier, int exitCode, const juce::String& processName ) override
        {
            onChildProcessExited( processIdentifier, exitCode, processName );
            if( restartOnKill )
            {
                //juce::Logger::writeToLog(juce::String(processIdentifier) + " exited with code " + juce::String(exitCode));
                juce::MessageManager::callAsync( [&, processIdentifier]() { restartChildProcess( processIdentifier ); } );
            }
        }

        void enableRestartOnKill( bool enable )
        {
            restartOnKill = enable;
        }

    private:
        bool                                                 restartOnKill{ true };
        std::function<void( int )>                           restartChildProcess;
        std::function<void( int, const juce::String& )>      onChildProcessStarted;
        std::function<void( int, int, const juce::String& )> onChildProcessExited;
        std::function<void( int, const juce::String& )>      onChildProcessStartFailed;
    };

    // data class
    class MTIPCBaseSubsystemData
    {
    public:
        ~MTIPCBaseSubsystemData()
        {
            messageProcessor.reset();
            messageDispatcher.reset();
            connectionManager.reset();
            pmCallback.reset();
            processManager.reset();

            for( auto connector : delayedConnectors )
            {
                connector->stopConnectAttempt();
            }
            delayedConnectors.clear();
        }

    public:
        std::shared_ptr<ipc::MTIPCConnectionManager>                    connectionManager;
        std::shared_ptr<ipc::MTIPCMessageProcessor>                     messageProcessor;
        std::shared_ptr<ipc::MTIPCProcessManager>                       processManager;
        std::shared_ptr<ipc::MTIPCMessageDispatcher>                    messageDispatcher;
        std::shared_ptr<ipc::MTIPCProcessManagerCallback>               pmCallback;
        std::shared_ptr<ipc::MTIPCHandlerStore>                         handlerStore;
        juce::String                                                    name;
        bool                                                            reconnectOnDisconnect{ true };
        juce::HashMap<int, std::shared_ptr<ipc::MTIPCDelayedConnector>> delayedConnectors;
        int                                                             parentProcessConnectionId;
        juce::HashMap<int, std::shared_ptr<ipc::MTIPCProcessRestarter>> delayedRestarters;
        juce::HashMap<int, int>                                         connectionIdtoConnectedHost;

    };

    class MTIPCSubsystemConnectionCallback : public MTIIPCConnectionCallback
    {
    public:
        MTIPCSubsystemConnectionCallback( std::function<void( int, const juce::String& )>             disconnectedCB,
                                          std::function<void( int, const juce::String& )>             connectedCB,
                                          std::function<void( int, std::shared_ptr<ipc::MTIPCJson> )> messageCB )
            : disconnectedCB( disconnectedCB ), connectedCB( connectedCB ), messageCB( messageCB ){};

        inline void onConnected( const int connectionId, const juce::String& connectionName ) override
        {
            try
            {
                if( connectedCB )
                    connectedCB( connectionId, connectionName );
            } catch( ... )
            {
                // log exception
            }
        }

        inline void onDisconnected( const int connectionId, const juce::String& connectionName )
        {
            try
            {
                if( disconnectedCB )
                    disconnectedCB( connectionId, connectionName );
            } catch( ... )
            {
                // log exception
            }
        }

        inline void onMessage( const int connectionId, std::shared_ptr<MTIPCJson> message )
        {
            try
            {
                if( messageCB )
                    messageCB( connectionId, message );
            } catch( ... )
            {
                // log exception
            }
        }

    private:
        std::function<void( int, const juce::String& )>            disconnectedCB;
        std::function<void( int, const juce::String& )>            connectedCB;
        std::function<void( int, std::shared_ptr<ipc::MTIPCJson> )> messageCB;
    };

    MTIPCBaseSubsystem::MTIPCBaseSubsystem( const juce::String& name,
        const juce::var& config, std::vector<std::shared_ptr<ipc::MTIIPCMessageHandler>> handlers, const int parentProcessConnectionId)
        : data( new MTIPCBaseSubsystemData() )

    {
        data->name = name;
        data->parentProcessConnectionId = parentProcessConnectionId;
        ipc::MTIPCMessageConstructor::setMessageOriginator( name );

        juce::var               arr = config.getProperty( ipc::IPC_KEY_CONNECTED_HOSTS, juce::Array<juce::var>() );
        juce::Array<juce::var>* ids = arr.getArray();
        for( auto element : *ids )
        {
            int id =  element.getProperty( ipc::IPC_KEY_CONNECTION_ID, -1);
            int host = element.getProperty( ipc::IPC_KEY_CONNECTED_HOST, -1 );
            if( host >= 0 && id >= 0 )
            {
                data->connectionIdtoConnectedHost.set( id, host );
            }
        }

        if( config.hasProperty( ipc::IPC_KEY_CONNECTION_CONFIG ) )
        {
            juce::Logger::writeToLog( "Initializing Connections..." );
            std::shared_ptr<juce::var> connectionConfig( new juce::var( config.getProperty( ipc::IPC_KEY_CONNECTION_CONFIG, juce::var() ) ) );

            data->connectionManager.reset(
                new ipc::MTIPCConnectionManager( connectionConfig,
                                                 std::shared_ptr<ipc::MTIIPCConnectionCallback>(
                                                     new MTIPCSubsystemConnectionCallback(
                                                         std::bind( &MTIPCBaseSubsystem::disconnected, this, std::placeholders::_1, std::placeholders::_2 ),
                                                         std::bind( &MTIPCBaseSubsystem::connected, this, std::placeholders::_1, std::placeholders::_2 ),
                                                         std::bind( &MTIPCBaseSubsystem::processMessage, this, std::placeholders::_1, std::placeholders::_2 ) ) ) ) );
            data->messageDispatcher.reset( new ipc::MTIPCMessageDispatcher( data->connectionManager ) );
            data->handlerStore.reset( new ipc::MTIPCHandlerStore( handlers ) );
            data->messageProcessor.reset( new ipc::MTIPCMessageProcessor( data->messageDispatcher, data->handlerStore ) );

        }

        if( config.hasProperty( ipc::IPC_KEY_PROCESS_CONFIG ) )
        {
            juce::Logger::writeToLog( "Initializing Processes..." );
            data->pmCallback.reset(
                new ipc::MTIPCProcessManagerCallback( std::bind( &ipc::MTIPCBaseSubsystem::childProcessStarted, this, std::placeholders::_1, std::placeholders::_2 ),
                                                      std::bind( &ipc::MTIPCBaseSubsystem::childProcessStopped, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 ),
                                                      std::bind( &ipc::MTIPCBaseSubsystem::childProcessStartFailed, this, std::placeholders::_1, std::placeholders::_2 ),
                                                      std::bind( &ipc::MTIPCBaseSubsystem::restartChildProcess, this, std::placeholders::_1 ) ) );

            std::shared_ptr<juce::var> processConfig( new juce::var( config.getProperty( ipc::IPC_KEY_PROCESS_CONFIG, juce::var() ) ) );
            data->processManager.reset( new ipc::MTIPCProcessManager( data->pmCallback ) );
            data->processManager->initialize( processConfig );         
        }
    }

    const int MTIPCBaseSubsystem::parentProcessConnectionId() const
    {
        return data->parentProcessConnectionId;
    }

    void MTIPCBaseSubsystem::start()
    {
        // start client connections after the child processes have started but, start the server before the child processes.
        if( data->connectionManager )
        {         
            data->connectionManager->startServer();           
        }
            
        if(data->processManager)
        {            
            data->processManager->start();            
        }
        
        if( data->connectionManager )
        {           
            data->connectionManager->startClients();         
        }
            
    }

    bool MTIPCBaseSubsystem::waitForClientConnections( const int maxWaitInMilliseconds )
    {
        if( !data->connectionManager ) return true;

        bool isComplete = false;

        int count = maxWaitInMilliseconds <= IPC_WAIT_FOR_IPC_LOOP_TIMEOUT ? 1 : maxWaitInMilliseconds / IPC_WAIT_FOR_IPC_LOOP_TIMEOUT;

        for( int i = 0; i < count; i++ )
        {
            isComplete = data->connectionManager->areAllClientsConnected();
            if(  isComplete)
                break;
            juce::Thread::sleep( IPC_WAIT_FOR_IPC_LOOP_TIMEOUT );
        }
        return isComplete;
    }

    void MTIPCBaseSubsystem::updateUnconnectedClientConnectionids( std::vector<int>& ids )
    {
        if( !data->connectionManager ) return;
        data->connectionManager->updateUnConnectedConnectionIds( ids );
    }

    void MTIPCBaseSubsystem::stop()
    {
        try
        {
            if( data->messageProcessor )
                data->messageProcessor->stop();
            if( data->messageDispatcher )
                data->messageDispatcher->stop();
            if( data->connectionManager )
                data->connectionManager->stop();
            if( data->processManager )
                data->processManager->stop();
            data = nullptr;
        } catch( ... )
        {
            // log exception.
        }
    }


    MTIPCBaseSubsystem::~MTIPCBaseSubsystem()
    {
        data.reset();
    }

    const juce::String& MTIPCBaseSubsystem::name()
    {
        return data->name;
    }

    const juce::String&  MTIPCBaseSubsystem::connectionName( const int connectionId )
    {
        return data->connectionManager->connectionName( connectionId );
    }

    void MTIPCBaseSubsystem::enableRestartProcessOnKill( bool b )
    {
        data->pmCallback->enableRestartOnKill( b );
    }

    void MTIPCBaseSubsystem::enableReconnectOnDisconnect( bool b )
    {
        data->reconnectOnDisconnect = b;
    }

    std::shared_ptr<ipc::MTIPCMessageDispatcher> MTIPCBaseSubsystem::dispatcher()
    {
        return data->messageDispatcher;
    }

    std::shared_ptr<ipc::MTIPCMessageProcessor> MTIPCBaseSubsystem::processor()
    {
        return data->messageProcessor;
    }

    std::shared_ptr<ipc::MTIPCConnectionManager> MTIPCBaseSubsystem::connectionManager()
    {
        return data->connectionManager;
    }

    std::shared_ptr<ipc::MTIPCProcessManager> MTIPCBaseSubsystem::processManager()
    {
        return data->processManager;
    }

    void MTIPCBaseSubsystem::restartChildProcess( int processIdentifier )
    {
        if( data->processManager )
        {
            if( !data->delayedRestarters.contains( processIdentifier ) )
            {
                data->delayedRestarters.set( processIdentifier,
                                             std::shared_ptr<ipc::MTIPCProcessRestarter>(
                                                 new ipc::MTIPCProcessRestarter( processIdentifier,
                                                                                 data->processManager,
                                                                                 std::bind( &MTIPCBaseSubsystem::onRestartAttemptCompleted,
                                                                                            this, std::placeholders::_1, std::placeholders::_2 ) ) ) );
            }

           data->delayedRestarters[processIdentifier]->restart();
        }
    }

    void MTIPCBaseSubsystem::onRestartAttemptCompleted( int processId, bool status )
    {
        if( data->delayedRestarters.contains( processId ) )
        {
            data->delayedRestarters[processId].reset();
            data->delayedRestarters.remove(processId);
        }
        if( !status ) // if process restart fails
        {
            data->processManager->stop();
            juce::MessageManager::callAsync( juce::JUCEApplicationBase::getInstance()->quit ); 
        }
        
    }

    void MTIPCBaseSubsystem::onConnectionAttemptCompleted( int connectionId, bool status )
    {
        if( data->delayedConnectors.contains( connectionId ) )
        {
            data->delayedConnectors[connectionId].reset();
            data->delayedConnectors.remove( connectionId );
        }
        if( status )
        {
            reconnectionAttemptSuccess( connectionId, connectionName( connectionId ) );
        }

        else
        {
            reconnectionAttemptFailed( connectionId, connectionName(connectionId));            
        }
    }

    void MTIPCBaseSubsystem::disconnected( const int connectionId, const juce::String& connName )
    {
        if( connectionId == data->parentProcessConnectionId )
        {
            // if the parent process is crashed then reconnection to parent process may result multple instances of the same processes,
            // to avoid that the processes kills itself if parent is killed and does not attempt any reconnection.
            juce::MessageManager::callAsync( juce::JUCEApplicationBase::getInstance()->quit); 
            return;
        }

       juce::MessageManager::callAsync( [&, connectionId]() { tryReconnect( connectionId ); } );
       juce::MessageManager::callAsync( [&, connectionId, connName]() { handleDisconnect( connectionId, connName ); } );
    }

    void MTIPCBaseSubsystem::tryReconnect( int connectionId )
    {
        if( data->reconnectOnDisconnect && data->connectionManager->canAttemptReconnection( connectionId ) )
        {
            if( data->delayedConnectors.contains( connectionId ) ) return;

            std::shared_ptr<ipc::MTIPCDelayedConnector> conn( new ipc::MTIPCDelayedConnector( data->connectionManager, connectionId,
                                                                                              std::bind( &ipc::MTIPCBaseSubsystem::onConnectionAttemptCompleted, this, std::placeholders::_1, std::placeholders::_2 ) ) );

            data->delayedConnectors.set( connectionId, conn );
            juce::MessageManager::callAsync( [&, conn]() { conn->connect(); } );
        }
    }

    void MTIPCBaseSubsystem::processMessage(int connectionId, std::shared_ptr<ipc::MTIPCJson> message )
    {
        if( handleMessage( connectionId, message ) ) return;
        if(data && data->messageProcessor)
            data->messageProcessor->process( connectionId , message );
    }

    const int MTIPCBaseSubsystem::hostIdForConnectionId( const int connectionId )
    {
        if( !data || data->connectionIdtoConnectedHost.contains( connectionId ) ) return -1;

        return data->connectionIdtoConnectedHost[connectionId];
    }

    void MTIPCBaseSubsystem::getConnectedHostIds( std::vector<int>& hosts )
    {
        if( !data ) return ;
        for( auto host : data->connectionIdtoConnectedHost )
            hosts.push_back( host );
    }

    void MTIPCBaseSubsystem::restartHost( const int hostId )
    {
        if( !data ) return;
        int connectionId = -1;

        for( auto itr = data->connectionIdtoConnectedHost.begin(); itr != data->connectionIdtoConnectedHost.end(); itr.next() )
        {
            if( itr.getValue() == hostId )
            {
                connectionId = itr.getKey();
                break;
            }
        }
        auto processName = data->processManager->processName( hostId );
        juce::Logger::writeToLog( "Attempting to Restart " + processName + " with host id " + juce::String( hostId ) + " As Host did not send heartbeat" );
        data->processManager->stopProcess( hostId );
        data->processManager->waitForProcessToExit( hostId );
        juce::MessageManager::callAsync( [&, hostId]() { data->processManager->startProcess( hostId ); } );
    }
}
