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

namespace ipc
{
    class MTIIPCMessageHandler;
    class MTIPCMessageProcessorData;
    class MTIPCJson;
    class MTIPCMessageProcessorImpl;
    class MTIPCMessageDispatcher;
    class MTIPCHandlerStore;

    // This class assigns one processor per message id, the latest processor set to the id will be used to process the message.
    // IF pipeline processing of messages is necessary then that needst be accomodated later. however it has a large number of use cases.
    class MTIPCMessageProcessor
    {
    public:
        MTIPCMessageProcessor( std::shared_ptr<ipc::MTIPCMessageDispatcher> dispatcher, std::shared_ptr<ipc::MTIPCHandlerStore> store );
        ~MTIPCMessageProcessor();

    public:
        void start();
        void stop();
        void process( const int connectionId, std::shared_ptr<ipc::MTIPCJson> message );
    private:
        void processMessage(std::shared_ptr<ipc::MTIPCJson> message);
    private:
        std::unique_ptr<MTIPCMessageProcessorData> data{ nullptr };
        std::unique_ptr<MTIPCMessageProcessorImpl> impl{ nullptr }; 
    };
}
