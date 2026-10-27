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
#include <memory>
#include <vector>

namespace juce
{
    class String;
    class var;
}

namespace ipc
{
    class MTIPCBaseSubsystemData;
    class MTIPCJson;
    class MTIIPCMessageHandler;
    class MTIPCMessageDispatcher;
    class MTIPCMessageProcessor;
    class MTIPCProcessManager;
    class MTIPCConnectionManager;

    class MTIPCBaseSubsystem
    {
    public:
        MTIPCBaseSubsystem( const juce::String&                                     name,
                            const juce::var&                                        config,
                            std::vector<std::shared_ptr<ipc::MTIIPCMessageHandler>> handlers,
                            const int                                               parentProcessConnectionId = -1 );

        ~MTIPCBaseSubsystem();
        MTIPCBaseSubsystem( const MTIPCBaseSubsystem& )             = delete;
        MTIPCBaseSubsystem& operator=( const MTIPCBaseSubsystem& )  = delete;
        MTIPCBaseSubsystem( const MTIPCBaseSubsystem&& )            = delete;
        MTIPCBaseSubsystem& operator=( const MTIPCBaseSubsystem&& ) = delete;

    public:
        void                                         enableRestartProcessOnKill( bool b );
        void                                         enableReconnectOnDisconnect( bool b );
        std::shared_ptr<ipc::MTIPCMessageDispatcher> dispatcher();
        std::shared_ptr<ipc::MTIPCMessageProcessor>  processor();
        std::shared_ptr<ipc::MTIPCConnectionManager> connectionManager();
        std::shared_ptr<ipc::MTIPCProcessManager>    processManager();
        const juce::String&                          name();
        const juce::String&                          connectionName( const int connectionId );
        virtual void                                 start();
        virtual void                                 stop();
        const int                                    parentProcessConnectionId() const;
        const int                                    hostIdForConnectionId( const int connectionId );
        void                                         getConnectedHostIds( std::vector<int>& hosts );
        void                                         restartHost( const int hostId );

    protected:
        // reimplement the methods to handle process and message life cycles.
        virtual void handleDisconnect( [[maybe_unused]] int connectionId, [[maybe_unused]] const juce::String& name ){};
        virtual void handleConnect( [[maybe_unused]] int connectionId, [[maybe_unused]] const juce::String& name ){};
        // reimplement this method and return true if the message is handled, this can be used for handling interuppt or high priority messages.
        virtual bool        handleMessage( [[maybe_unused]] int connectionId, [[maybe_unused]] std::shared_ptr<ipc::MTIPCJson> message ) { return false; };
        virtual void        childProcessStopped( [[maybe_unused]] int processIdentifier, [[maybe_unused]] int exitCode, [[maybe_unused]] const juce::String& processName ){};
        virtual void        childProcessStarted( [[maybe_unused]] int processIdentifier, [[maybe_unused]] const juce::String& processName ){};
        virtual void        childProcessStartFailed( [[maybe_unused]] int processIdentifier, [[maybe_unused]] const juce::String& processName ){};
        virtual void        reconnectionAttemptFailed( [[maybe_unused]] int connectionId, [[maybe_unused]] const juce::String& name ){};
        virtual void        reconnectionAttemptSuccess( [[maybe_unused]] int connectionId, [[maybe_unused]] const juce::String& name ){};
        bool                waitForClientConnections( const int maxWaitInMilliseconds = 120000 );
        void                updateUnconnectedClientConnectionids( std::vector<int>& ids );
        

    private:
        void onConnectionAttemptCompleted( int connectionId, bool status );
        void tryReconnect( int connectionId );
        void restartChildProcess( int processIdentifier );
        void disconnected( int connectionId, const juce::String& connName );
        void connected( int connectionId, const juce::String& connName ) { handleConnect( connectionId, connName ); };
        void processMessage( int connectionId, std::shared_ptr<ipc::MTIPCJson> message );
        void onRestartAttemptCompleted( int processId, bool status );

    protected:
        std::shared_ptr<ipc::MTIPCBaseSubsystemData> data{ nullptr };
    };
}
