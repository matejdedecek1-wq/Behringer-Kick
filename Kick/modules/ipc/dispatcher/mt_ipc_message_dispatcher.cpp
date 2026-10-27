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

#include <juce_core/juce_core.h>
#include "mt_ipc_message_dispatcher.h"
#include "../utils/mt_ipc_message_queue.h"
#include "../connection/mt_ipc_connection_manager.h"
#include "../utils/mt_ipc_json.h"
#include "../utils/mt_ipc_message_queue_processor_thread.h"

namespace ipc
{
    const int DISPATCH_TYPE_BROADCAST( 0 );
    const int DISPATCH_TYPE_PUBLISH( 1 );
    const int DISPATCH_TYPE_MULTICAST( 2 );

    class MTIPCDispatcherData
    {
    public:
        MTIPCDispatcherData( std::shared_ptr<ipc::MTIPCJson> message )
            : dispatchType( DISPATCH_TYPE_BROADCAST ), message( message )
        {}

        MTIPCDispatcherData( std::shared_ptr<ipc::MTIPCJson> message, int connectionId )
            : dispatchType( DISPATCH_TYPE_PUBLISH ), message( message ), connectionIds{ connectionId }
        {}

        MTIPCDispatcherData( std::shared_ptr<ipc::MTIPCJson> message, const std::vector<int>& connectionIds )
            : dispatchType( DISPATCH_TYPE_MULTICAST ), message( message ), connectionIds{ connectionIds }
        {}

    public:
        int                            dispatchType{ DISPATCH_TYPE_BROADCAST };
        std::vector<int>               connectionIds;
        std::shared_ptr<ipc::MTIPCJson> message;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class MTIPCMessageDispatcherImpl : public ipc::MTIPCMessageQueueProcessingThread<ipc::MTIPCDispatcherData>
    {
    public:
        MTIPCMessageDispatcherImpl( std::shared_ptr<ipc::MTIPCConnectionManager> connectionManager )
            : ipc::MTIPCMessageQueueProcessingThread<ipc::MTIPCDispatcherData>( "Message_Dispatcher_Thread" ), cm( connectionManager )
        {
            startThread();
        }

        bool canDispatch( const int connectionId )
        {
            if( cm )
            {
                return cm->isConnected( connectionId );
            }
            return false;
        }

    protected:
        virtual void processMessage( std::shared_ptr<ipc::MTIPCDispatcherData> message )
        {
            try
            {
                if( message->dispatchType == DISPATCH_TYPE_BROADCAST )
                {
                    cm->broadcast( message->message );
                }
                else if( message->dispatchType == DISPATCH_TYPE_PUBLISH )
                {
                    cm->publish( message->connectionIds[0], message->message );
                }
                else if( message->dispatchType == DISPATCH_TYPE_MULTICAST )
                {
                    cm->multicast( message->connectionIds, message->message );
                }
                else
                {
                    juce::Logger::writeToLog( "Unsupported Message" );
                }
            }
            catch( ... )
            {
                // log exception.
            }
            
        }

    private:
        std::shared_ptr<ipc::MTIPCConnectionManager> cm;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MTIPCMessageDispatcher::MTIPCMessageDispatcher( std::shared_ptr<ipc::MTIPCConnectionManager> connectionManager )
        : impl( new MTIPCMessageDispatcherImpl( connectionManager ) )
    {
        start();
    }

    MTIPCMessageDispatcher::~MTIPCMessageDispatcher()
    {
        stop();
        impl = nullptr;
    }

    void MTIPCMessageDispatcher::start()
    {
        impl->startThread();
    }

    void MTIPCMessageDispatcher::stop()
    {
        if( impl->isThreadRunning() )
            impl->stopThread( 5000 );
    }

    bool MTIPCMessageDispatcher::canDispatch( const int connectionId )
    {
        return impl->canDispatch( connectionId );
    }

    void MTIPCMessageDispatcher::publish( const int connectionId, std::shared_ptr<ipc::MTIPCJson> message )
    {
        impl->push_to_q( std::shared_ptr<ipc::MTIPCDispatcherData>( new ipc::MTIPCDispatcherData( message, connectionId ) ), false );
    }

    void MTIPCMessageDispatcher::broadcast( std::shared_ptr<ipc::MTIPCJson> message )
    {
        impl->push_to_q( std::shared_ptr<ipc::MTIPCDispatcherData>( new ipc::MTIPCDispatcherData( message ) ), false );
    }

    void MTIPCMessageDispatcher::multicast( std::vector<int>& connectionIds, std::shared_ptr<ipc::MTIPCJson> message )
    {
        impl->push_to_q( std::shared_ptr<ipc::MTIPCDispatcherData>( new ipc::MTIPCDispatcherData( message, connectionIds ) ), false );
    }
}
