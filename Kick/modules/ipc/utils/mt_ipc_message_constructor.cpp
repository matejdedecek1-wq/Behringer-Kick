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

#include "mt_ipc_message_constructor.h"
#include "../utils/mt_ipc_message_types.h"
#include "../utils/mt_ipc_enums.h"

juce::String ipc::MTIPCMessageConstructor::messageOriginator;
namespace ipc
{
	void MTIPCMessageConstructor::setMessageOriginator(const juce::String originator)
	{
		messageOriginator = originator;
	}

	std::shared_ptr<ipc::MTIPCJson> MTIPCMessageConstructor::constructHeaderMessage(int messageType)
	{
		std::shared_ptr<ipc::MTIPCJson> message(new ipc::MTIPCJson());
        updateHeaderIntoMessage( message );

		message->setProperty(ipc::IPC_KEY_MESSAGE_TYPE, messageType);
		message->setProperty(ipc::IPC_KEY_TRANSACTION_TYPE, ipc::MTIPCTransactionType::IPC_TRANSACTION_TYPE_NA);
		return message;
	}

	std::shared_ptr<ipc::MTIPCJson> MTIPCMessageConstructor::constructRequestMessage(int messageType, const juce::HashMap<juce::String, juce::var>& dotSeparatedKeyToValuePairs)
	{
		std::shared_ptr<ipc::MTIPCJson> message(new ipc::MTIPCJson());
		updateHeaderIntoMessage(message);
		message->setProperty(ipc::IPC_KEY_MESSAGE_TYPE, messageType);
		message->setProperty(ipc::IPC_KEY_TRANSACTION_TYPE, ipc::MTIPCTransactionType::IPC_TRANSACTION_TYPE_REQUEST);
		addDataToMessage(message, dotSeparatedKeyToValuePairs);
		return message;
	}

    std::shared_ptr<ipc::MTIPCJson> MTIPCMessageConstructor::constructEventMessage( int messageType, const juce::HashMap<juce::String, juce::var>& dotSeparatedKeyToValuePairs )
    {
        std::shared_ptr<ipc::MTIPCJson> message( new ipc::MTIPCJson() );
        updateHeaderIntoMessage( message );
        message->setProperty( ipc::IPC_KEY_MESSAGE_TYPE, messageType );
        message->setProperty( ipc::IPC_KEY_TRANSACTION_TYPE, ipc::MTIPCTransactionType::IPC_TRANSACTION_TYPE_EVENT );
        addDataToMessage( message, dotSeparatedKeyToValuePairs );
        return message;
    }

	std::shared_ptr<ipc::MTIPCJson> MTIPCMessageConstructor::constructResponseMessage(int messageType, const juce::String& requestId, const juce::HashMap<juce::String, juce::var>& dotSeparatedKeyToValuePairs)
	{
		std::shared_ptr<ipc::MTIPCJson> message(new ipc::MTIPCJson());
		updateHeaderIntoMessage(message);
		message->setProperty(ipc::IPC_KEY_MESSAGE_TYPE, messageType);
		message->setProperty(ipc::IPC_KEY_TRANSACTION_TYPE, ipc::MTIPCTransactionType::IPC_TRANSACTION_TYPE_RESPONSE);
		message->setProperty(ipc::IPC_KEY_REQUEST_MESSAGE_ID, requestId);
		addDataToMessage(message, dotSeparatedKeyToValuePairs);
		return message;
	}

	std::shared_ptr<ipc::MTIPCJson> MTIPCMessageConstructor::constructMessage(const juce::HashMap<juce::String, juce::var>& dotSeparatedKeyToValuePairs)
	{
		std::shared_ptr<ipc::MTIPCJson> message(new ipc::MTIPCJson());
        updateHeaderIntoMessage( message );
        addDataToMessage(message, dotSeparatedKeyToValuePairs);
        return message;
	}

	void MTIPCMessageConstructor::updateHeaderIntoMessage( std::shared_ptr<ipc::MTIPCJson> message )
	{
		message->setProperty(ipc::IPC_KEY_MESSAGE_ID, juce::Uuid().toString());
		message->setProperty(ipc::IPC_KEY_TIMESTAMP, juce::Time().getCurrentTime().toMilliseconds());
		message->setProperty(ipc::IPC_KEY_MESSAGE_ORIGINATOR, messageOriginator);
	}

    void MTIPCMessageConstructor::addDataToMessage( std::shared_ptr<ipc::MTIPCJson> message, const juce::HashMap<juce::String, juce::var>& dotSeparatedKeyToValuePairs )
    {
        if( dotSeparatedKeyToValuePairs.size() <= 0 ) return;

        for( auto itr = dotSeparatedKeyToValuePairs.begin(); itr != dotSeparatedKeyToValuePairs.end(); itr.next() )
        {
            message->setProperty( itr.getKey(), itr.getValue() );
        }   
    }
}
