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
#include "ipc/dispatcher/mt_ipc_message_dispatcher.h"

namespace ipc
{
    class MTIPCMessageDispatcher;
    class MTIPCJson;
}

namespace contracts
    {
        // This will be base class of Kick<subsystem>IPCInterface classes.
        // client objects can communicate to the subclasses using signals and the subclasses will generate signals on events.

        class IPCInterface 
        {
        public:
            IPCInterface( std::shared_ptr<ipc::MTIPCMessageDispatcher> dispatcher, const int connectionId )
                : connectionId( connectionId ), pDispatcher( dispatcher ){};

            ~IPCInterface()
            {
                pDispatcher.reset();
            }

        protected:
            bool send( std::shared_ptr<ipc::MTIPCJson> message )
            {
                if( !message ) return false;

                std::shared_ptr<ipc::MTIPCMessageDispatcher> dispatcher = pDispatcher.lock();
                if( !dispatcher )
                    return false;
                dispatcher->publish( connectionId, message );
                return true;
            }

        protected:
            std::weak_ptr<ipc::MTIPCMessageDispatcher> pDispatcher;
            int                                       connectionId;
        };
    }
