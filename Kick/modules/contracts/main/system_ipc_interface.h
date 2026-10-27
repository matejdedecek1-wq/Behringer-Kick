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

#include "memory"
#include "../interface/ipc_interface.h"
#include "system_signals.h"


namespace ipc
{
    class MTIPCMessageDispatcher;
}

namespace contracts
{
    class SystemIPCIterface : public contracts::IPCInterface
    {
    public:
        SystemIPCIterface( std::shared_ptr<ipc::MTIPCMessageDispatcher> dispatcher, const int connectionId )
            : IPCInterface( dispatcher, connectionId )
        {
            contracts::SystemSignals::SendSystemCommand.connect( [&]( std::shared_ptr<ipc::MTIPCJson> message ) { this->send( message ); } );
        }
    };
}
