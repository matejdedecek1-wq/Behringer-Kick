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
#include <exception>
#include <string>

namespace ipc
{
    const char* const EMPTY_MESSAGE_ERROR_CODE( "1001" );
    const char* const JSON_SET_VALUE_ERROR_CODE( "1002" );
    const char* const JSON_GET_VALUE_ERROR_CODE( "1003" );
    const char* const JSON_SAVE_ERROR_CODE( "1004" );
    const char* const IPC_ERROR_CODE( "1005" );
    const char* const MESSAGE_QUEUE_ERROR_CODE( "1006" );
    const char* const TRASNPORT_ERROR_CODE( "1007" );
    const char* const EMPTY_FILE_NAME_ERROR_CODE( "1008" );
    const char* const BUILD_UNSUPPORTED_TRANSPORT_ERROR_CODE( "1009" );
    const char* const BUILD_TRANSPORT_FAILED_ERROR_CODE( "1010" );
    const char* const CHANNEL_ERROR_CODE( "1011" );
    const char* const CONNECTION_MANAGER_ERROR_CODE( "1012" );
    const char* const TRASNPORT_READ_TIMEOUT_ERROR_CODE( "1013" );
    const char* const MESSAGE_DISPATCHER_ERROR_CODE( "1014" );
    const char* const IPC_CONNECTION_ERROR_CODE( "1015" );

    // messages
    const char* const EMPTY_FILE_NAME_ERROR_MESSAGE( "The input filename is Empty.." );
    const char* const EMPTY_MESSAGE_ERROR_MESSAGE( "The input filename is Empty.." );
    const char* const JSON_SET_VALUE_ERROR_MESSAGE( "Error while setting value to json.." );
    const char* const JSON_GET_VALUE_ERROR_MESSAGE( "Error while getting value from json.." );
    const char* const JSON_SAVE_ERROR_MESSAGE( "Error while saving json" );
    const char* const IPC_SEND_ERROR_MESSAGE( "Error while sending with peer" );
    const char* const IPC_DISCONNECT_ERROR_MESSAGE( "Error while disconnecting from peer" );
    const char* const IPC_CONNECT_ERROR_MESSAGE( "Error while connecting to peer" );
    const char* const MESSAGE_QUEUE_POP_ERROR_MESSAGE( "Error while poping from the queue..." );
    const char* const TRASNPORT_EMPTY_PIPENAME_ERROR_MESSAGE( "Pipe name is empty.." );
    const char* const TRASNPORT_PIPE_CONNECTION_ERROR_MESSAGE( "Pipe connection Failed" );
    const char* const TRASNPORT_PIPE_DISCONNECTION_ERROR_MESSAGE( "Pipe disconnection Failed" );
    const char* const TRASNPORT_PIPE_WRITE_ERROR_MESSAGE( "Pipe Write Failed" );
    const char* const TRASNPORT_PIPE_READ_ERROR_MESSAGE( "Pipe Read Failed" );
    const char* const TRASNPORT_PIPE_READ_TIMEOUT_ERROR_MESSAGE( "Pipe Read Timeout..." );
    const char* const TRASNPORT_PIPE_CREATION_ERROR_MESSAGE( "Failed to Create Object of Pipe Transport" );
    const char* const BUILD_UNSUPPORTED_TRANSPORT_ERROR_MESSAGE( "Unsupported transport requested to be build.." );
    const char* const BUILD_TRANSPORT_FAILED_ERROR_MESSAGE( "Unable to build transport.." );
    const char* const CHANNEL_READ_ERROR_MESSAGE( "Unable to read from Channel" );
    const char* const CONNECTION_MANAGER_CHANNEL_CREATION_ERROR_MESSAGE( "Channel Created Failed by connection Manager." );
    const char* const IPC_PIPE_CONNECTION_FAILED_ERROR_MESSAGE( "Unable to create or connect to Pipe" );
    const char* const IPC_SOCKET_CONNECTION_FAILED_ERROR_MESSAGE( "Unable to connect to Socket" );
    const char* const IPC_REQUEST_FAILED_ERROR_MESSAGE( "NO Response received for request" );
}
