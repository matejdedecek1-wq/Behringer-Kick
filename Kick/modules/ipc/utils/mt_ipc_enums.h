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


#include "mt_ipc_macros.h"
// Trasaction type event is just a record of past event.
// Transaction type request is similar to command.
namespace ipc
{
    MT_IPC_DEFINE_INT_ENUM_CLASS( MTIPCTransactionType, IPC_TRANSACTION_TYPE_NA{ 0 }, IPC_TRANSACTION_TYPE_REQUEST{ 10 }, IPC_TRANSACTION_TYPE_RESPONSE{ 20 }, IPC_TRANSACTION_TYPE_EVENT{ 30 } );

    MT_IPC_DEFINE_INT_ENUM_CLASS( MTIPCRegistrationStatus, IPC_MESSAGE_REGISTERATION_SUCCESS{ 1 }, IPC_MESSAGE_REGISTERATION_FAILED{ 2 } );

    MT_IPC_DEFINE_INT_ENUM_CLASS( MTIPCTransportType, IPC_TRANSPORT_TYPE_INVALID{ 0 }, IPC_TRANSPORT_TYPE_SOCKET{ 1 }, IPC_TRANSPORT_TYPE_PIPE{ 2 } );

    MT_IPC_DEFINE_INT_ENUM_CLASS( MTIPCConnectionStatus, IPC_CONNECTION_STATUS_CONNECTED{ 1 }, IPC_CONNECTION_STATUS_NOT_CONNECTED{ 2 });
}
