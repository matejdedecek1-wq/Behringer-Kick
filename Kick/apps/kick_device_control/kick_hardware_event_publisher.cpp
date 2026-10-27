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

#include "ipc/utils/mt_ipc_message_constructor.h"
#include "contracts/dcl/dcl_contracts_helper.h"
#include "kick_hardware_event_publisher.h"
#include "ipc/dispatcher/mt_ipc_message_dispatcher.h"
#include "contracts/dcl/dcl_ipc_message_types.h"
#include "contracts/utils/keyboard/keyboard_signals.h"
#include "input_events/input_events.h"


namespace kick
{
    namespace dcl
    {
        KickHardwareEventPublisher::KickHardwareEventPublisher( std::shared_ptr<ipc::MTIPCMessageDispatcher> dispatcher )
            : dispatcher( dispatcher )
        {
            Controls::InputEvents::dispatchHardwareEvent.connect( [&]( CPktMgr::HWMsg_t& f_HwMsg ) {
                publish( f_HwMsg );
            } );
        }

        void KickHardwareEventPublisher::publish( const CPktMgr::HWMsg_t& f_HwMsg )
        {
            std::shared_ptr<ipc::MTIPCMessageDispatcher> pDispatcher = dispatcher.lock();
            if( !pDispatcher ) return;

            try
            {
                std::shared_ptr<ipc::MTIPCJson> message =
                    ipc::MTIPCMessageConstructor::constructEventMessage( contracts::DclIPCMessageType::DCL_IPC_MESSAGE_TYPE_HW_EVENT );
                contracts::DclContractsHelper::hWMessageToIPCMessage( f_HwMsg, message );
                pDispatcher->broadcast(message);

            } catch( ... )
            {
                // log exception.
            }
        }
    }
}
