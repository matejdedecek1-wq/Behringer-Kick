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

#include <vector>
#include <memory>

namespace juce
{
    class String;
}

namespace ipc
{
    // message will have information of connectionId and requestId( in case of handleResponse). get the values and use them as requried.
    // in case of sending response the message Id has to be added in the response message as request_message_id for the requestor to validate the response.
    class MTIPCJson;
    class MTIPCMessageDispatcher;
    class MTIIPCMessageHandler
    {
    public:
        virtual bool isDefaultHandler() { return false; }; // return true if this is the default handler for the messageids to which processors are not assigned..

        virtual const std::vector<int>& messageTypes() = 0; // the message types handled by this processor.

        virtual void handleRequest( const int                                   messageType,                                    
                                    std::shared_ptr<ipc::MTIPCJson>              message,
                                    std::weak_ptr<ipc::MTIPCMessageDispatcher> dispatcher ){};

        virtual void handleResponse( const int                                   messageType,                                     
                                     std::shared_ptr<ipc::MTIPCJson>              message,
                                     std::weak_ptr<ipc::MTIPCMessageDispatcher> dispatcher ){};

        virtual void handleEvent( const int                                   messageType,
                                     std::shared_ptr<ipc::MTIPCJson>              message,
                                  std::weak_ptr<ipc::MTIPCMessageDispatcher> dispatcher){};
    };
}
