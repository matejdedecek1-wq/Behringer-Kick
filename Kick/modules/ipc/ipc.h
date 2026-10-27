/*******************************************************************************
 The block below describes the properties of this module, and is read by
 the Projucer to automatically generate project code that uses it.
 For details about the syntax and how to create or use a module, see the
 JUCE Module Format.txt file.

 BEGIN_JUCE_MODULE_DECLARATION

  ID:                 common
  vendor:             MUSIC Tribe, MY
  version:            0.1.0
  name:               MusicTribe Common definitions
  description:        Classes and interfaces containing common definitions
  website:
  license:            AGPLv3
  dependencies:       juce_core

 END_JUCE_MODULE_DECLARATION

*******************************************************************************/

#pragma once
#ifndef IPC_H_INCLUDE
    #define IPC_H_INCLUDE

    /**
    * This module hosts the ipc and related implementations.
    * IPC is completely async in nature. all messages are sent and sender has to implement the logic to block itself 
    **/
    #include "utils/mt_ipc_json.h"
    #include "utils//mt_ipc_message_constructor.h"
    #include "utils/mt_ipc_exception.h"
    #include "utils/mt_ipc_error_codes.h"
    #include "utils/mt_ipc_message_queue.h"
    #include "utils/mt_ipc_timer.h"
    #include "connection/mt_ipc_connection.h"
    #include "connection/mt_ipc_server_connection.h"
    #include "connection/mt_ipc_connection_manager.h"
    #include "connection/mt_ipc_socket_server.h"
    #include "dispatcher/mt_ipc_message_dispatcher.h"
    #include "process/mt_ipc_child_process_executor.h"
    #include "process/mti_ipc_process_manager_callback.h"
    #include "process/mt_ipc_process_manager.h"
    #include "processor/mti_ipc_message_handler.h"
    #include "processor/mt_ipc_handler_store.h"
    #include "processor/mt_ipc_message_processor.h"
    #include "subsystem/mt_ipc_base_subsystem.h"
    #include "subsystem/mt_ipc_delayed_connector.h"
    #include "subsystem/mt_ipc_subsystem.h"
#endif
