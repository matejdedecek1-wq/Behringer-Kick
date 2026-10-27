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

#include "contracts/audioengine/audio_engine_message_types.h"
#include "kick_ae_message_handler.h"
#include "kick_audio_interface.h"
#include "ipc/utils/mt_ipc_json.h"

namespace kick
{
    namespace ae
    {
        class DisconnectMessageHandler : public kick::ae::AudioEngineMessageHandler
        {
        public:
            DisconnectMessageHandler( std::shared_ptr<contracts::AudioInterface> interface )
                : AudioEngineMessageHandler( interface, { contracts::AudioEngineMessageTypes::AUDIO_ENGINE_MESSAGE_DISCONNECT } )
            {
            }

            virtual juce::var processRequest( std::shared_ptr<ipc::MTIPCJson>              message,
                                              std::shared_ptr<ipc::MTIPCMessageDispatcher> dispatcher,
                                              std::shared_ptr<contracts::AudioInterface>   audioInterface )
            {
                juce::var root = message->getRoot();

                bool disconnectionStatus = false;

                if( root.hasProperty( contracts::AUDIO_ENGINE_IPC_KEY_OBJECT_ID_CONNECTED ) )
                {
                    disconnectionStatus = audioInterface->disconnect( root.getProperty( contracts::AUDIO_ENGINE_IPC_KEY_OBJECT_ID, -1 ),
                                                                      root.getProperty( contracts::AUDIO_ENGINE_IPC_KEY_OBJECT_ID_CONNECTED, -1 ) );
                }
                else
                {
                    disconnectionStatus = audioInterface->disconnectAll( root.getProperty( contracts::AUDIO_ENGINE_IPC_KEY_OBJECT_ID, -1 ) );
                }

                juce::DynamicObject* response = new juce::DynamicObject();
                response->setProperty( contracts::AUDIO_ENGINE_IPC_KEY_DISCONNECION_STATUS, disconnectionStatus );
                return response;
            };
        };
    }
}
