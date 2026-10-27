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
#include "ipc_application.h"
#include "ipc/subsystem/mt_ipc_subsystem.h"
#include "contracts/logger/logger.h"
#include "ipc/utils/mt_ipc_helpers.h"
#include "../../main/system_ipc_interface.h"
#include "contracts/main/system_ipc_helpers.h"
#include "contracts/main/system_signals.h"
#include "ipc/utils/mt_ipc_json.h"

// start a hearbeat thread. 

namespace contracts
{
    const int HEART_BEAT_TIMEOUT_IN_MS( 1000 );
    const int HEART_BEAT_EXIT_WAIT_IN_MS( 500);

    class IPCHeartBeat : public juce::Thread
    {
    public:
        IPCHeartBeat( const int hostId )
            : juce::Thread( "Heart_Beat_Thread_" + juce::String(hostId) )
        {
           // timer.reset( new ipc::MTIPCTimer( HEART_BEAT_TIMEOUT_IN_MS, std::bind( &IPCHeartBeat::dispatchFirstHeartBeat, this ), true ) );
            heartBeatMessage = contracts::SystemIPCHelpers::prepareHeartBeatMessage();
            heartBeatMessage->setProperty(ipc::IPC_KEY_HOST_ID, hostId);
        }

        void start()
        {
            startThread();
        }

        void stop()
        {
            stopThread( HEART_BEAT_EXIT_WAIT_IN_MS );
        }

        virtual void run() override
        {
            int count         = 0;
            int sleepDuration = 100;
            int totalCount    = ( HEART_BEAT_TIMEOUT_IN_MS ) / sleepDuration;
            while( !threadShouldExit() )
            {
                juce::Thread::sleep( sleepDuration );
                count++;
                if( count == totalCount )
                {
                    count = 0;
                    contracts::SystemSignals::SendSystemCommand( heartBeatMessage );
                }
            }
        }    
    private:
        std::shared_ptr<ipc::MTIPCJson>  heartBeatMessage;

    };

    class IPCApplicationImpl
    {
    public:
        IPCApplicationImpl( const juce::String& name,
                            const juce::String& logFileBaseName,
                            const juce::String& metricsFileBaseName,
                            bool                createSystemInterface,
                            bool                enableHeartBeat,
                            bool                logToServer,
                            const juce::String& version )
            : name( name ),
            logFileBaseName( logFileBaseName ),
            metricsFileBaseName( metricsFileBaseName ),
            version( version ),
            logToServer( logToServer ),
            createSystemInterface( createSystemInterface ),
            enableHeartBeat(enableHeartBeat)
        {
        }

        ~IPCApplicationImpl()
        {
            shutdown();
        }

        const juce::String getApplicationName()
        {
            return name;
        }

        const juce::String getApplicationVersion()
        {
            return version;
        }

        bool initialise( const juce::var& config, std::vector<std::shared_ptr<ipc::MTIIPCMessageHandler>>& handlers )
        {
           initializeLogger( logFileBaseName, metricsFileBaseName, name, logToServer );

            if( config.isUndefined() )
            {
                juce::Logger::writeToLog( "Failed to Initailize. Reason - Undefined configuration" );
                return false;
            }

            contracts::ConnectionIdStore::initialize( config );
            
            subsystem.reset( new ipc::MTIPCSubsystem( name,
                                                      config,
                                                      handlers,
                                                      contracts::ConnectionIdStore::PARENT_CONNECTION_ID ) );

            if( logToServer )
                logger->setDispatcher( contracts::ConnectionIdStore::LOGGER_CONNECTION_ID, subsystem->dispatcher() );

            if(createSystemInterface)
                systemIPCIface.reset( new contracts::SystemIPCIterface( subsystem->dispatcher(), contracts::ConnectionIdStore::MAIN_CONNECTION_ID ) );

            if( enableHeartBeat )
            {
                hostId = config.getProperty( ipc::IPC_KEY_HOST_ID, -1 );
                heartBeat.reset( new IPCHeartBeat(hostId) );
                heartBeat->start();
            }

            return true;
        }

        void shutdown()
        {
            if( logger )
            {
                juce::Logger::setCurrentLogger( nullptr );
            }
            if( heartBeat )
                heartBeat->stop();
        }

        std::shared_ptr<ipc::MTIPCMessageDispatcher> dispatcher()
        {
            if( subsystem )
                return subsystem->dispatcher();
            return nullptr;
        }

        std::shared_ptr<ipc::MTIPCSubsystem> getSubsystem()
        {
            return subsystem;
        }

    private:
        void initializeLogger( const juce::String& logFileBaseName,
                               const juce::String& metricsFileBaseName,
                               const juce::String& processName,
                               bool                logToServer = false )
        {
            logger.reset( new contracts::Logger( logFileBaseName, metricsFileBaseName, processName, logToServer ) );
            juce::Logger::setCurrentLogger( logger.get() );
        }

    private:
        juce::String                         name;
        juce::String                         logFileBaseName;
        juce::String                         metricsFileBaseName;
        juce::String                         version;
        bool                                 logToServer;
        bool                                 createSystemInterface;
        bool                                 enableHeartBeat;
        int                                  hostId;
        std::shared_ptr<ipc::MTIPCSubsystem> subsystem;
        std::unique_ptr<contracts::Logger>   logger;

        std::unique_ptr<contracts::SystemIPCIterface> systemIPCIface;
        std::unique_ptr<IPCHeartBeat>                 heartBeat;
       
    };

    IPCApplication::IPCApplication( const juce::String& name,
                                    const juce::String& logFileBaseName,
                                    const juce::String& metricsFileBaseName,
                                    bool                createSystemInterface,
                                    bool                enableHeartBeat,
                                    bool                logToServer,
                                    const juce::String& version )
        : impl( new IPCApplicationImpl( name,
                                        logFileBaseName,
                                        metricsFileBaseName,
                                        createSystemInterface,
                                        enableHeartBeat,
                                        logToServer,
                                        version ) )
    {
    }

    IPCApplication::~IPCApplication()
    {
        impl.reset();
    }
    const juce::String IPCApplication::getApplicationName()
    {
        return impl->getApplicationName();
    }

    const juce::String IPCApplication::getApplicationVersion()
    {
        return impl->getApplicationVersion();
    }

    void IPCApplication::initialise( const juce::String& commandLine )
    {
        std::vector<std::shared_ptr<ipc::MTIIPCMessageHandler>> handlers{};
        addMessageHandlers( handlers );
        juce::var config = getConfig(commandLine);
        impl->initialise( config, handlers );
    }

    void IPCApplication::shutdown()
    {
        impl.reset();
    }

    std::shared_ptr<ipc::MTIPCMessageDispatcher> IPCApplication::dispatcher()
    {
        auto subsystem = impl->getSubsystem();
        if( subsystem )
            return subsystem->dispatcher();
        return nullptr;
    }

    std::shared_ptr<ipc::MTIPCSubsystem> IPCApplication::subsystem()
    {
        return impl->getSubsystem();
    }

}
