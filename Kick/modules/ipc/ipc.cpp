/*
 * Copyright (c) 2021 MUSIC Tribe
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

#ifdef IPC_H_INCLUDE
/* When you add this cpp file to your project, you mustn't include it in a file where you've
     already included any other headers - just put it inside a file on its own, possibly with your config
     flags preceding it, but don't include anything else. That also includes avoiding any automatic prefix
     header files that the compiler may be using.
  */
    #error "Incorrect use of JUCE cpp file"
#endif

// Module include files...
// #include "hardware.h"

#include "ipc.h"
#include "utils/mt_ipc_json.cpp"
#include "utils//mt_ipc_message_constructor.cpp"
#include "connection/mt_ipc_connection.cpp"
#include "connection/mt_ipc_server_connection.cpp"
#include "connection/mt_ipc_connection_manager.cpp"
#include "connection/mt_ipc_socket_server.cpp"
#include "dispatcher/mt_ipc_message_dispatcher.cpp"
#include "process/mt_ipc_child_process_executor.cpp"
#include "process/mt_ipc_process_manager.cpp"
#include "processor/mt_ipc_message_processor.cpp"
#include "processor/mt_ipc_handler_store.cpp"
#include "subsystem/mt_ipc_base_subsystem.cpp"
#include "subsystem/mt_ipc_delayed_connector.cpp"
#include "subsystem/mt_ipc_subsystem.cpp"

