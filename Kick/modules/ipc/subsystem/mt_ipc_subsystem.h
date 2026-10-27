
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

#include "mt_ipc_base_subsystem.h"

namespace ipc
{
    class MTIPCLogger;
    class MTIPCSubsystem : public ipc::MTIPCBaseSubsystem
    {
    public:
        MTIPCSubsystem( const juce::String&                                name,
                        const juce::var&                                   config,
                        std::vector<std::shared_ptr<MTIIPCMessageHandler>> handlers,
                        const int                                          parentProcessConnectionId );
        ~MTIPCSubsystem();

    public:
        virtual void start() override;
    protected:
        virtual void handleDisconnect( int connectionId, const juce::String& name ) override;
        virtual void handleConnect( int connectionId, const juce::String& name ) override;
        virtual bool handleMessage( int connectionId, std::shared_ptr<ipc::MTIPCJson> message ) override;
        virtual void childProcessStopped( int processIdentifier, int exitCode, const juce::String& name ) override;
        virtual void childProcessStarted( int processIdentifier, const juce::String& name );
        virtual void childProcessStartFailed( int processIdentifier, const juce::String& name );
        virtual void reconnectionAttemptFailed( int connectionId, const juce::String& name );
        virtual void reconnectionAttemptSuccess( int connectionId, const juce::String& name );
        

    private:
       // bool tryReconnect( std::vector<int> connectionIds );
    };

}
