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

#include "../interface/ipc_interface.h"
#include "dcl_ipc_interface.h"
#include "dcl_signals.h"
#include "dcl_contracts.h"
#include "ipc/utils/mt_ipc_json.h"
#include "ipc/utils/mt_ipc_message_constructor.h"
#include "dcl_ipc_message_types.h"
#include "dcl_ipc_keys.h"
#include "PktMgr/PktMgr.hpp"
#include "juce_events/juce_events.h"

namespace contracts
{
    class DclIPCInterfaceImpl : public IPCInterface
    {
    public:
        DclIPCInterfaceImpl( std::shared_ptr<ipc::MTIPCMessageDispatcher> dispatcher, const int connectionId )
            : IPCInterface( dispatcher, connectionId )
        {
            contracts::DclSignals::sendCommandToDcl.connect( [&]( std::shared_ptr<ipc::MTIPCJson> message ) {
                this->send( message );
            } );

            contracts::DclSignals::createAndSendCommandToDcl.connect( [&]( const CPktMgr::HWMsg_t& message ) {
                this->createAndPublishToDcl( message );
            } );
        }

    private:
        void createAndPublishToDcl( const CPktMgr::HWMsg_t& hwMessage )
        {
            std::shared_ptr<ipc::MTIPCJson> message =
                ipc::MTIPCMessageConstructor::constructRequestMessage( contracts::DclIPCMessageType::DCL_IPC_MESSAGE_TYPE_HW_COMMAND );
            contracts::DclContractsHelper::hWMessageToIPCMessage( hwMessage, message );
            send( message );
        }
    };

    DclIPCInterface::DclIPCInterface( std::shared_ptr<ipc::MTIPCMessageDispatcher> dispatcher, const int connectionId )
        : impl( new contracts::DclIPCInterfaceImpl( dispatcher, connectionId ) ) {}

    DclIPCInterface::~DclIPCInterface()
    {
        impl.reset();
    }
}
