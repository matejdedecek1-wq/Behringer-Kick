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

#include "juce_core/juce_core.h"
#include "mti_ipc_message_handler.h"
#include "mt_ipc_message_processor.h"
#include "mt_ipc_handler_store.h"
#include "../utils/mt_ipc_message_queue.h"
#include "../utils/mt_ipc_json.h"
#include "../utils/mt_ipc_message_queue_processor_thread.h"
#include "../utils/mt_ipc_constants.h"
#include "../utils/mt_ipc_keys.h"
#include "../utils/mt_ipc_message_types.h"
#include "../utils/mt_ipc_enums.h"


namespace ipc
{
    class MTIPCMessageProcessorData
    {
    public:
        ~MTIPCMessageProcessorData()
        {
        }
    public:
        std::weak_ptr<ipc::MTIPCHandlerStore>      handlerStore;
        std::weak_ptr<ipc::MTIPCMessageDispatcher> dispatcher;
    };

    class MTIPCMessageProcessorImpl : public ipc::MTIPCMessageQueueProcessingThread<ipc::MTIPCJson>
    {
    public:
        MTIPCMessageProcessorImpl( std::function<void( std::shared_ptr<ipc::MTIPCJson> )> callback )
            : ipc::MTIPCMessageQueueProcessingThread<ipc::MTIPCJson>( "Message_Processor_Thread" ), callback( callback )
        {
        }
    protected:
        virtual void processMessage( std::shared_ptr<ipc::MTIPCJson> message )
        {
            callback(message);
        }
        
    private:
        std::function<void( std::shared_ptr<ipc::MTIPCJson> )> callback;
    };

    MTIPCMessageProcessor::MTIPCMessageProcessor( std::shared_ptr<ipc::MTIPCMessageDispatcher> dispatcher,
        std::shared_ptr<ipc::MTIPCHandlerStore> store )
        : data( new ipc::MTIPCMessageProcessorData() )
        , impl( new ipc::MTIPCMessageProcessorImpl( std::bind(&MTIPCMessageProcessor::processMessage, this, std::placeholders::_1)))
    {
        data->dispatcher = dispatcher;
        data->handlerStore = store;
        start();
    }
    MTIPCMessageProcessor::~MTIPCMessageProcessor()
    {
        stop();
        data = nullptr;
        impl = nullptr;
        
    }

    void MTIPCMessageProcessor::start()
    {
        impl->startThread();
    }

    void MTIPCMessageProcessor::stop()
    {
        if(impl->isThreadRunning())
            impl->stopThread(5000);
    }

    void MTIPCMessageProcessor::process( const int connectionId, std::shared_ptr<ipc::MTIPCJson> message )
    {
        message->setProperty(ipc::IPC_KEY_CONNECTION_ID, connectionId);
        impl->push_to_q(message);
    }

    void MTIPCMessageProcessor::processMessage( std::shared_ptr<ipc::MTIPCJson> message )
    {
        try
        {
            int messageType = message->getProperty( ipc::IPC_KEY_MESSAGE_TYPE, ipc::MTIPCMessageType::IPC_MESSAGE_TYPE_INVALID );
       
            int transactionType = message->getProperty(ipc::IPC_KEY_TRANSACTION_TYPE, ipc::MTIPCTransactionType::IPC_TRANSACTION_TYPE_NA);

            auto store = data->handlerStore.lock();

            std::shared_ptr<MTIIPCMessageHandler> handler = store->handler(messageType);
         
            if(!handler) return;
            if(transactionType == ipc::MTIPCTransactionType::IPC_TRANSACTION_TYPE_REQUEST)
                handler->handleRequest( messageType, message, data->dispatcher );
            if (transactionType == ipc::MTIPCTransactionType::IPC_TRANSACTION_TYPE_RESPONSE)
                handler->handleResponse( messageType, message, data->dispatcher );
            if( transactionType == ipc::MTIPCTransactionType::IPC_TRANSACTION_TYPE_EVENT )
                handler->handleEvent( messageType, message, data->dispatcher );
            
        }
        catch( ... )
        {
            // log exception.
        }
    }
}
