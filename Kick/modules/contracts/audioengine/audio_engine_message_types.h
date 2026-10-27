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

#include "../utils/contracts_marcros.h"

namespace contracts
{
    CONTRACTS_DEFINE_INT_ENUM_CLASS( AudioEngineMessageTypes,
                                     AUDIO_ENGINE_IPC_MESSAGE_TYPE_INVALID{ 200 },
                                     // queries
                                     AUDIO_ENGINE_MESSAGE_GET_PROPERTIES{ 201 },
                                     AUDIO_ENGINE_MESSAGE_GET_OBJECT_STATUS{ 202 },
                                     // commands  
                                     AUDIO_ENGINE_MESSAGE_ALLOCATE{ 203 },
                                     AUDIO_ENGINE_MESSAGE_DEALLOCATE{ 204 },
                                     AUDIO_ENGINE_MESSAGE_CLEAR{ 205 },
                                     AUDIO_ENGINE_MESSAGE_CONNECT{ 206 },
                                     AUDIO_ENGINE_MESSAGE_DISCONNECT{ 207 },
                                     AUDIO_ENGINE_MESSAGE_SET_PROPERTY{ 208 },
                                     AUDIO_ENGINE_MESSAGE_SET_PROPERTIES{ 209 },
                                     // responses
                                     AUDIO_ENGINE_RESPONSE_MESSAGE{ 210 } );
}
