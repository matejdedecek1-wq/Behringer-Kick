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
    class InterprocessConnection;
}
namespace ipc
{
    class MTIPCJson;
    class MTIPCConnectionPimpl;
    class MTIIPCConnectionCallback;
    class MTIPCConnection
    {
    public:
        MTIPCConnection(const int connectionId, std::shared_ptr<ipc::MTIIPCConnectionCallback> pCallback = nullptr, bool callbacksOnMessageThread = true);
        MTIPCConnection(const int connectionId, const std::shared_ptr<juce::var> config, std::shared_ptr<ipc::MTIIPCConnectionCallback> = nullptr, bool callbacksOnMessageThread = true);
        ~MTIPCConnection();
        virtual bool                                     connect();
        virtual void                                     disconnect();
        virtual bool                                     publish( std::shared_ptr<ipc::MTIPCJson> message );
        virtual bool                                     isConnected();
        void                                             setConnectionIdentifier( const int identifier );
        const int                                        connectionIdentifier();
        const juce::String&                              connectionName();
        void                                             setConnectionName( const juce::String& name );
        const std::shared_ptr<juce::var>                 config();
        juce::InterprocessConnection*                    getConnectionObject();
        inline std::shared_ptr<MTIIPCConnectionCallback> getCallbackObject() { return callback; };
        void setCallback( std::shared_ptr<ipc::MTIIPCConnectionCallback> pCallback );


    protected:
        virtual void onConnected();
        virtual void onDisconnected();
        virtual void onMessage( std::shared_ptr<ipc::MTIPCJson> message );

    private:
        MTIPCConnection( const MTIPCConnection& )            = delete;
        MTIPCConnection& operator=( const MTIPCConnection& ) = delete;

    private:
        std::unique_ptr<ipc::MTIPCConnectionPimpl> impl{ nullptr };
        std::shared_ptr<MTIIPCConnectionCallback> callback{ nullptr };
    };
}
