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
#include <juce_core/juce_core.h>

namespace ipc
{
    const char* const IPC_KEY_TRANSPORT( "transport" );
    const char* const IPC_KEY_PIPE_NAME( "pipename" );
    const char* const IPC_KEY_URL( "url" );
    const char* const IPC_KEY_PORT( "port" );
    const char* const IPC_KEY_BIND_ADDRESS( "bind_address" );
    const char* const IPC_KEY_CONNECTION_TIMEOUT( "connection_timeout" );
    const char* const IPC_KEY_MESSAGE_ID( "message_id" );
    const char* const IPC_KEY_MESSAGE_TYPE( "message_type" );
    const char* const IPC_KEY_TRANSACTION_TYPE( "transaction_type" );
    const char* const IPC_KEY_CONNECTION_ID( "connection_id" );
    const char* const IPC_KEY_TIMESTAMP( "timestamp" );
    const char* const IPC_KEY_REGISTERATION_STATUS( "registration_status" );
    const char* const IPC_KEY_SERVER( "server" );
    const char* const IPC_KEY_CLIENTS( "clients" );
    const char* const IPC_KEY_CONNECTIONS( "connections" );
    const char* const IPC_KEY_UUID( "uuid" );
    const char* const IPC_KEY_ARGUMENTS( "arguments" );
    const char* const IPC_KEY_PROCESS_PATH( "process_path" );
    const char* const IPC_KEY_PROCESS_NAME( "process_name" );
    const char* const IPC_KEY_HOST_ID( "host_id" );
    const char* const IPC_KEY_CHILDREN( "children" );
    const char* const IPC_KEY_CONNECTION_CONFIG( "connection_config" );
    const char* const IPC_KEY_PROCESS_CONFIG( "process_config" );
    const char* const IPC_KEY_MESSAGE_ORIGINATOR( "originator" );
    const char* const IPC_KEY_DELAY_STARTING_CLIENT( "delay_starting_client" );
    const char* const IPC_KEY_CLIENT_START_DELAY( "client_start_delay" );
    const char* const IPC_KEY_REQUEST_MESSAGE_ID( "request_message_id" );
    const char* const IPC_KEY_LOG_TYPE( "log_type" );// valid log types - LOG, METRIC.
    const char* const IPC_KEY_LOG_LEVEL( "log_level" ); // valid log levels - INFO, DEBUG, WARN,ERROR
    const char* const IPC_KEY_LOG_MESSAGE( "log_message" );
    const char* const IPC_KEY_METRIC_NAME( "metric_name" );
    const char* const IPC_KEY_METRIC_VALUE( "metric_value" );
    const char* const IPC_KEY_CLIENT_ENABLE_VALIDATION( "enable_client_validation" );
    const char* const IPC_KEY_CONNECTION_NAMES( "connection_names" );
    const char* const IPC_KEY_CONNECTION_NAME( "connection_name" );
    const char* const IPC_KEY_UN_CONNECTED_CONNECTION_IDS( "un_connected_connection_ids" );
    const char* const IPC_KEY_CONNECTION_STATUS( "connection_status" );
    const char* const IPC_KEY_CONNECTED_HOST( "connection_host" );
    const char* const IPC_KEY_CONNECTED_HOSTS( "connection_hosts" );
}
