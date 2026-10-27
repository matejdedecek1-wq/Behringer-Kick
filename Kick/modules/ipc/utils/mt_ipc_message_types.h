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
// Todo: The need for this enum needs to be considered later as this enum would impose a restriction on the possible values compared to integer
// and this would also necessitate the types to be declared in this file. 

namespace ipc
{
    MT_IPC_DEFINE_INT_ENUM_CLASS( MTIPCMessageType,
                                  IPC_MESSAGE_TYPE_INVALID{ 0 },
                                  IPC_MESSAGE_TYPE_REGISTER{ 1 },
                                  IPC_MESSAGE_TYPE_REGISTERATION_STATUS{ 2 },
                                  IPC_MESSAGE_TYPE_CONNECTION_STATUS{ 3 });
}
