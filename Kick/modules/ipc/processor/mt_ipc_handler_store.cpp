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

#include "mt_ipc_handler_store.h"
#include "mti_ipc_message_handler.h"
#include "juce_core/juce_core.h"

namespace ipc
{

    class MTIPCHandlerStoreData
    {
    public:
        juce::HashMap<int, std::shared_ptr<ipc::MTIIPCMessageHandler>> handlers;
        std::shared_ptr<ipc::MTIIPCMessageHandler>                     defaultHandler;
    };

    MTIPCHandlerStore::MTIPCHandlerStore()
        : data(new ipc::MTIPCHandlerStoreData())
    {
    }

    MTIPCHandlerStore::MTIPCHandlerStore( const std::vector<std::shared_ptr<ipc::MTIIPCMessageHandler>>& handlers )
        : data( new ipc::MTIPCHandlerStoreData() )
    {
        registerHandlers( handlers );
    }

    MTIPCHandlerStore::~MTIPCHandlerStore()
    {
        data.reset();
    }

    std::shared_ptr<ipc::MTIIPCMessageHandler> MTIPCHandlerStore::handler( int messageType )
    {
        return data->handlers.contains( messageType ) ? data->handlers[messageType] : data->defaultHandler ? data->defaultHandler : nullptr;
    }

    std::shared_ptr<ipc::MTIIPCMessageHandler> MTIPCHandlerStore::defaultHandler()
    {
        return data->defaultHandler;
    }

    void MTIPCHandlerStore::registerHandler( std::shared_ptr<ipc::MTIIPCMessageHandler> handler )
    {
        const std::vector<int>& types = handler->messageTypes();
        if( handler->isDefaultHandler() )
        {
            data->defaultHandler = handler;
        }
        for( const int& type : types )
        {
            data->handlers.set(type, handler);
        }
    }

    void MTIPCHandlerStore::registerHandlers( const std::vector<std::shared_ptr<ipc::MTIIPCMessageHandler>>& handlers )
    {
        for( auto handler : handlers )
        {
            registerHandler( handler );
        }
    }

    void MTIPCHandlerStore::removeHandler( int messageType )
    {
        if( data->handlers.contains( messageType ) )
            data->handlers.remove( messageType );
    }

    void MTIPCHandlerStore::removeHandler( std::shared_ptr<ipc::MTIIPCMessageHandler> handler )
    {
        auto types = handler->messageTypes();
        for( auto type : types )
        {
            removeHandler( type );
        }
        if( handler == data->defaultHandler )
            data->defaultHandler.reset();
    }

    void MTIPCHandlerStore::setDefaultHandler( std::shared_ptr<ipc::MTIIPCMessageHandler> handler )
    {
        data->defaultHandler = handler;
    }
}
