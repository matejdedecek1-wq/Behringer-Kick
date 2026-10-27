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
    class MTIIPCMessageHandler;
    class MTIPCHandlerStoreData;
    class MTIPCHandlerStore
    {
    public:
        MTIPCHandlerStore();
        MTIPCHandlerStore( const std::vector<std::shared_ptr<ipc::MTIIPCMessageHandler>>& handlers);
        ~MTIPCHandlerStore();

    public:
        std::shared_ptr<ipc::MTIIPCMessageHandler> handler( int messageType );
        std::shared_ptr<ipc::MTIIPCMessageHandler> defaultHandler();
        // use this function to clear default handler.
        void                                       setDefaultHandler( std::shared_ptr<ipc::MTIIPCMessageHandler>  handler);
        void                                       registerHandler( std::shared_ptr<ipc::MTIIPCMessageHandler>  handler);
        void                                       registerHandlers( const std::vector<std::shared_ptr<ipc::MTIIPCMessageHandler>>& handlers );
        void                                       removeHandler(int messageType);
        void                                       removeHandler( std::shared_ptr<ipc::MTIIPCMessageHandler> handler);

    private:
        std::unique_ptr<MTIPCHandlerStoreData> data{nullptr};
    };
}
