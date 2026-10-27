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

#include "system_ipc_helpers.h"
#include "ipc/utils/mt_ipc_json.h"
#include "ipc/utils/mt_ipc_message_constructor.h"
#include "system_message_types.h"

namespace contracts
{
    std::shared_ptr<ipc::MTIPCJson> SystemIPCHelpers::prepareExitSystemMessage()
    {
        return ipc::MTIPCMessageConstructor::constructRequestMessage( contracts::SystemIPCMessageType::SYSTEM_IPC_MESSAGE_TYPE_EXIT );
    }

    std::shared_ptr<ipc::MTIPCJson> SystemIPCHelpers::prepareHeartBeatMessage()
    {
        return ipc::MTIPCMessageConstructor::constructEventMessage( contracts::SystemIPCMessageType::SYSTEM_IPC_MESSAGE_TYPE_HEART_BEAT );
    }
}
