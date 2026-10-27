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


#include "mt_ipc_connection.h"
#include "string"

namespace juce
{
    class String;
    class var;
}


namespace ipc
{
	class MTIPCJson;
	class MTIPCServerConnection : public ipc::MTIPCConnection
	{
	public:
		MTIPCServerConnection(const juce::String& uuid, const int  connectionId, const std::shared_ptr<juce::var> config,
			std::shared_ptr<ipc::MTIIPCConnectionCallback> pCallback);
	protected:
		virtual void onDisconnected() override;
		virtual void onConnected() override;
		virtual void onMessage(std::shared_ptr<ipc::MTIPCJson> message);
        virtual inline bool isConnected() override { return MTIPCConnection::isConnected() && isRegistered; }        
	private:
		bool isRegistered{ false };
		const juce::String& uuid;
	};
}
