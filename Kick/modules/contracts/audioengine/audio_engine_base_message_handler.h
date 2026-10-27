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

#include "ipc/processor/mti_ipc_message_handler.h"
#include "audio_engine_contracts_helpers.h"
#include "audio_engine_message_types.h"
#include "audio_engine_signals.h"
#include "audio_engine_object_properties.h"
#include "audio_engine_ipc_keys.h"
#include "ipc/utils/mt_ipc_keys.h"
#include "ipc/utils/mt_ipc_json.h"


namespace contracts
{
    class AudioEngineBaseMessageHandler : public ipc::MTIIPCMessageHandler
    {
    public:
        const std::vector<int>& messageTypes() override { return supportedMessageTypes; };

        void handleResponse( const int                                  messageType,
                             std::shared_ptr<ipc::MTIPCJson>            message,
                             std::weak_ptr<ipc::MTIPCMessageDispatcher> dispatcher ) override
        {
            juce::var    root               = message->getRoot();
            const int    requestMessageType = root.getProperty( contracts::AUDIO_ENGINE_IPC_KEY_REQUEST_MESSAGE_TYPE, -1 );
            juce::String requestId          = root.getProperty( ipc::IPC_KEY_REQUEST_MESSAGE_ID, "" ).toString();
            const int    objectId           = root.getProperty( contracts::AUDIO_ENGINE_IPC_KEY_OBJECT_ID, -1 );
            juce::var    response           = root.getProperty( contracts::AUDIO_ENGINE_IPC_KEY_RESPONSE_DATA, juce::var() );

            if( requestMessageType == contracts::AudioEngineMessageTypes::AUDIO_ENGINE_MESSAGE_GET_PROPERTIES )
            {
                contracts::AudioEngineObjectProperties props;
                props.parseJsonVar(response);
                contracts::AudioEngineSignals::ObjectPropertiesReceived( requestId, objectId, props );
            }
            else
            {
                contracts::AudioEngineSignals::ResponseReceivedFromAudioEngine( requestId, requestMessageType, objectId, response );
            }

        };

    private:
        std::vector<int> supportedMessageTypes{ contracts::AudioEngineMessageTypes::AUDIO_ENGINE_RESPONSE_MESSAGE};
    
    };
}
