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

namespace juce
{
    class var;
    class String;
}

namespace ipc
{
    class MTIPCProcessManagerData;
    class MTIIPCProcessManagerCallback;
    class MTIPCProcessMonitorThread;
    class MTIPCProcessManager
    {
    public:
        MTIPCProcessManager( std::shared_ptr<ipc::MTIIPCProcessManagerCallback> callback );
        ~MTIPCProcessManager();

    public:
        void initialize( std::shared_ptr<juce::var> config);
        void start();
        void stop();
        void wait();
        int  exitCode( const int processIdentifier );
        bool isRunning( const int processIdentifier );
        bool startProcess( const int processIdentifier );
        void startProcessInThread(const int pid );
        bool stopProcess( const int processIdentifier );
        bool waitForProcessToExit( const int processIdentifier );
        const juce::String& processName( const int processIdentifier );
    private:
        void starterThreadExitNotifier( int pid );

    private:
        std::shared_ptr<MTIPCProcessManagerData>          data;
        std::unique_ptr<ipc::MTIPCProcessMonitorThread>    processMonitorThread;
    };
}
