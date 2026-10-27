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

namespace contracts
{
    const char* const AUDIO_ENGINE_IPC_KEY_QUERY_TYPE( "query_type" );
    const char* const AUDIO_ENGINE_IPC_KEY_COMMAND_TYPE( "command_type" );
    const char* const AUDIO_ENGINE_IPC_KEY_ASYNC_EVENT_TYPE( "async_event_type" );
    const char* const AUDIO_ENGINE_IPC_KEY_OBJECT_PROPERTIES( "object_properties" );
    const char* const AUDIO_ENGINE_IPC_KEY_OBJECT_STATUS( "object_status" );
    const char* const AUDIO_ENGINE_IPC_KEY_OBJECT_TYPE( "object_type" );
    const char* const AUDIO_ENGINE_IPC_KEY_OBJECT_ID( "object_id" );
    const char* const AUDIO_ENGINE_IPC_KEY_OBJECT_ID_CONNECTED( "connected_object_id" );
    const char* const AUDIO_ENGINE_IPC_KEY_ALLOCATION_STATUS( "allocation_status" );
    const char* const AUDIO_ENGINE_IPC_KEY_OBJECT_CONNECTIONS( "object_connections" );
    const char* const AUDIO_ENGINE_IPC_KEY_METERING( "metering" );
    const char* const AUDIO_ENGINE_IPC_KEY_REQUESTED_VALUE( "requested_value" );
    const char* const AUDIO_ENGINE_IPC_KEY_REQUEST_MESSAGE_TYPE( "request_message_type" );
    const char* const AUDIO_ENGINE_IPC_KEY_SET_VALUE( "set_value" );
    const char* const AUDIO_ENGINE_IPC_KEY_RESPONSE_DATA( "response_data" );
    const char* const AUDIO_ENGINE_IPC_KEY_RESPONSE_CODE( "response_code" );
    const char* const AUDIO_ENGINE_IPC_KEY_DESTROYED_OBJECTS( "destroyed_objects" );
    const char* const AUDIO_ENGINE_IPC_KEY_DISCONNECION_STATUS( "disconnection_status" );
    const char* const AUDIO_ENGINE_IPC_KEY_PROPERTY_ID( "property_id" );
    const char* const AUDIO_ENGINE_IPC_KEY_PROPERTY_VALUE( "property_value" );
}
