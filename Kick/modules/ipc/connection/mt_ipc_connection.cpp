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
#include <juce_events/juce_events.h>

#include "mt_ipc_connection.h"
#include "mti_ipc_connection_callback.h"
#include "../utils/mt_ipc_json.h"
#include "../utils/mt_ipc_constants.h"
#include "../utils/mt_ipc_keys.h"
#include "../utils/mt_ipc_enums.h"
#include "../utils/mt_ipc_message_types.h"
#include "../utils/mt_ipc_exception.h"
#include "../utils/mt_ipc_error_codes.h"

namespace ipc
{

	/// Private implementation class, Locks need to be added in case synchronization is necessary, Right now the assumption is that MessageDispatcher will call this.
	class MTIPCConnectionPimpl : public juce::InterprocessConnection
	{
	public:
		MTIPCConnectionPimpl(const int connectionId, const std::shared_ptr<juce::var> config, std::function<void()> connectedCB, std::function<void()> disConnectedCB,
			std::function<void(std::shared_ptr<ipc::MTIPCJson>)> messageCB, bool callbacksOnMessageThread = true)
			: juce::InterprocessConnection(callbacksOnMessageThread), connectionId(connectionId), connectionConfig(config), connectedCB(connectedCB), disConnectedCB(disConnectedCB), messageCB(messageCB)
		{
            connName = config ? config->getProperty( ipc::IPC_KEY_CONNECTION_NAME, ipc::IPC_CLIENT_NAME_NOT_DEFINED ).toString() :
                                juce::String( ipc::IPC_CLIENT_NAME_NOT_DEFINED );
		}

		~MTIPCConnectionPimpl()
		{
			disconnect();
		}

		bool connect()
		{
			if (!connectionConfig) return false;

			if (isConnected()) return true; // return true if it already connected as the connection parameters are not going to change.

			int transport = connectionConfig->getProperty(ipc::IPC_KEY_TRANSPORT, ipc::MTIPCTransportType::IPC_TRANSPORT_TYPE_INVALID);
			bool connected = false;

			switch (transport)
			{
			case ipc::MTIPCTransportType::IPC_TRANSPORT_TYPE_PIPE:
			{
				juce::String pipename = connectionConfig->getProperty(ipc::IPC_KEY_PIPE_NAME, "");

				if (pipename.isNotEmpty())
				{
					connected = createPipe(pipename, -1, true);
					if (!connected)
					{
						connected = connectToPipe(pipename, -1);
					}
				}
			}
			break;
			case ipc::MTIPCTransportType::IPC_TRANSPORT_TYPE_SOCKET:
			{
				juce::String url = connectionConfig->getProperty(ipc::IPC_KEY_URL, "");
				int          port = connectionConfig->getProperty(ipc::IPC_KEY_PORT, -1);
                if( url.isEmpty() || port <= 0 )
                {
                    return false;
                }

				int maxTimeout    = connectionConfig->getProperty( ipc::IPC_KEY_CONNECTION_TIMEOUT, IPC_MAX_CONNECTION_TIMEOUT );
                int maxIterations = maxTimeout < IPC_DEFAULT_CONNECTION_TIMEOUT ? 1:  maxTimeout / IPC_DEFAULT_CONNECTION_TIMEOUT;

                for( int i = 0; i < maxIterations; i++ )
                {
                    connected = connectToSocket( url, port, IPC_DEFAULT_CONNECTION_TIMEOUT );
                    if( connected )
                        break;
                }
			}
			break;
			default:
				break;
			}
			return connected;
		}

		void disconnect()
		{
			juce::InterprocessConnection::disconnect();
		}

		bool publish(std::shared_ptr<ipc::MTIPCJson> message)
		{
			if (!isConnected() || !message) return false;

			juce::String json = juce::JSON::toString(message->getRoot());
			return sendMessage(juce::MemoryBlock(json.getCharPointer(), json.length()));
		}

		const std::shared_ptr<juce::var> config()
		{
			return connectionConfig;
		}

		virtual void connectionMade() override 
		{
            try
            {
                connectedCB();
            }
            catch( ... )
            {
                // log exception
            }
			
		}

		virtual void connectionLost() override
		{
            try
            {
                disConnectedCB();
            } catch( ... )
            {
                // log exception
            }
		}

		virtual void messageReceived(const juce::MemoryBlock& block) override
		{
            try
            {
                std::shared_ptr<ipc::MTIPCJson> message( new ipc::MTIPCJson( block.toString() ) );
                messageCB( message );
                message.reset();
            }
            catch( ... )
            {
                // log exception.
            }
			
		}

		void setConnectionIdentifier(const int id)
		{
			connectionId = id;
		}

		const int connectionIdentifier()
		{
			return connectionId;
		}

        const juce::String& connectionName()
        {
            return connName;
        }

        void setConnectionName( const juce::String& name )
        {
            connName = name;
        }

	private:
		const std::shared_ptr<juce::var>                      connectionConfig{ nullptr };
		std::function<void()>                                 connectedCB;
		std::function<void()>                                 disConnectedCB;
		std::function<void(std::shared_ptr<ipc::MTIPCJson>)> messageCB;
		int                                                   connectionId{ -1 };
        juce::String                                          connName;
	};

	/// Facade class

	MTIPCConnection::MTIPCConnection(const int connectionId, std::shared_ptr<ipc::MTIIPCConnectionCallback> pCallback, bool callbacksOnMessageThread)
		: callback(pCallback), impl(new MTIPCConnectionPimpl(connectionId,
			nullptr,
			std::bind(&ipc::MTIPCConnection::onConnected, this),
			std::bind(&ipc::MTIPCConnection::onDisconnected, this),
			std::bind(&ipc::MTIPCConnection::onMessage, this, std::placeholders::_1), callbacksOnMessageThread))
	{
	}

	MTIPCConnection::MTIPCConnection(const int connectionId, std::shared_ptr<juce::var> config, std::shared_ptr<ipc::MTIIPCConnectionCallback> pCallback, bool callbacksOnMessageThread )
		: callback(pCallback), impl(new MTIPCConnectionPimpl(connectionId,
			config,
			std::bind(&ipc::MTIPCConnection::onConnected, this),
			std::bind(&ipc::MTIPCConnection::onDisconnected, this),
			std::bind(&ipc::MTIPCConnection::onMessage, this, std::placeholders::_1), callbacksOnMessageThread))
	{
	}

	MTIPCConnection::~MTIPCConnection()
	{
		callback = nullptr;
		impl = nullptr;
	}

	bool MTIPCConnection::connect()
	{
		return impl->connect();
	}

	void MTIPCConnection::disconnect()
	{
		impl->disconnect();
	}

	bool MTIPCConnection::publish(std::shared_ptr<ipc::MTIPCJson> message)
	{
		return  impl->publish(message);
	}

	bool MTIPCConnection::isConnected()
	{
		return impl->isConnected();
	}

	const std::shared_ptr<juce::var> MTIPCConnection::config()
	{
		return impl->config();
	}

	juce::InterprocessConnection* MTIPCConnection::getConnectionObject()
	{
		return impl.get();
	}

	void MTIPCConnection::setConnectionIdentifier(const int identifier)
	{
		impl->setConnectionIdentifier(identifier);
	}

	const int MTIPCConnection::connectionIdentifier()
	{
		return impl->connectionIdentifier();
	}

    const juce::String& MTIPCConnection::connectionName()
    {
        return impl->connectionName();
    }

    void MTIPCConnection::setConnectionName( const juce::String& name )
    {
        impl->setConnectionName( name );
    }

    void MTIPCConnection::onConnected()
    {
        if (callback)
        {
            callback->onConnected(connectionIdentifier(), connectionName());
        }
    }

    void MTIPCConnection::onDisconnected()
    {
        if (callback)
        {           
			callback->onDisconnected( connectionIdentifier(), connectionName() );
        }
    }

    void MTIPCConnection::onMessage( std::shared_ptr<ipc::MTIPCJson> message )
    {
        if (callback)
        {
           callback->onMessage(connectionIdentifier(), message);
        }
            
    }

	void MTIPCConnection::setCallback(std::shared_ptr<ipc::MTIIPCConnectionCallback> pCallback)
	{
		callback = pCallback;
	}
}
