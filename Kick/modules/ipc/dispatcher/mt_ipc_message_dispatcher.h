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

namespace ipc
{
    template<typename T>
    class MTIPCMessageQueue;
    class MTIPCConnectionManager;
    class MTIPCJson;
    class MTIPCMessageDispatcherImpl;
    class MTIPCMessageDispatcher 
    {
    public:
        MTIPCMessageDispatcher(std::shared_ptr<ipc::MTIPCConnectionManager> connectionManager);
        ~MTIPCMessageDispatcher();

    public:
        void start();
        void stop();
        void publish( const int connectionId, std::shared_ptr<ipc::MTIPCJson> message );
        void broadcast( std::shared_ptr<ipc::MTIPCJson> message );
        void multicast(std::vector<int>& connectionIds, std::shared_ptr<ipc::MTIPCJson> message);
        bool canDispatch(const int connectionId);

    private:
        std::unique_ptr<MTIPCMessageDispatcherImpl> impl; 
    };
}
