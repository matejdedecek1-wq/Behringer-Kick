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

#include "memory"
#include "ipc/processor/mti_ipc_message_handler.h"
#include "contracts/audioengine/audio_engine_contracts_helpers.h"
#include "contracts/audioengine/audio_engine_ipc_keys.h"
#include "contracts/audioengine/audio_interface.h"
#include "ipc/utils/mt_ipc_json.h"
#include "ipc/utils/mt_ipc_keys.h"
#include "ipc/dispatcher/mt_ipc_message_dispatcher.h"
#include "contracts/audioengine/audio_engine_response_code.h"

namespace kick
{
    namespace ae
    {
        class AudioInterface;
        class AudioEngineMessageHandler : public ipc::MTIIPCMessageHandler
        {
        public:
            AudioEngineMessageHandler( std::shared_ptr<contracts::AudioInterface> audioInterface, std::vector<int> supportedMessageTypes )
                : iface( audioInterface ), supportedMessageTypes( supportedMessageTypes )
            {
            }

            virtual const std::vector<int>& messageTypes()
            {
                return supportedMessageTypes;
            }

            virtual void handleRequest( const int                                  messageType,
                                        std::shared_ptr<ipc::MTIPCJson>            message,
                                        std::weak_ptr<ipc::MTIPCMessageDispatcher> dispatcher )
            {
                auto pDispatcher = dispatcher.lock();

                auto      audioIface        = iface.lock();

                juce::var response     = juce::var::undefined();

                int       responseCode = responseCode = contracts::AudioEngineResponseCode::AUDIO_ENGINE_RESPONSE_CODE_SUCCESS;

                if( audioIface )
                {
                    try
                    {
                        response = processRequest( message, pDispatcher, audioIface );
                    }
                    catch( ... )
                    {
                        juce::Logger::writeToLog( "Exception Occurred while handling message with Id " + juce::String( messageType ) );
                        response = juce::var::undefined();
                    }
                }
                else
                {
                    response = juce::var::undefined();
                }

                disptachResponse( response,
                                  response.isUndefined() ?
                                      contracts::AudioEngineResponseCode::AUDIO_ENGINE_RESPONSE_CODE_FAILURE :
                                      contracts::AudioEngineResponseCode::AUDIO_ENGINE_RESPONSE_CODE_SUCCESS,
                                  message, pDispatcher );
            };

        protected:
            virtual juce::var processRequest( std::shared_ptr<ipc::MTIPCJson>              message,
                                              std::shared_ptr<ipc::MTIPCMessageDispatcher> dispatcher,
                                              std::shared_ptr<contracts::AudioInterface>   audioInterface ) = 0;


            void disptachResponse( const juce::var&                             response,
                                   const int                                    responseCode,
                                   std::shared_ptr<ipc::MTIPCJson>              incommingMessage,
                                   std::shared_ptr<ipc::MTIPCMessageDispatcher> dispatcher )
            {

                if( !dispatcher )
                {
                    juce::Logger::writeToLog( "Unable to dispatch messages. Invalid dispatcher." );
                    return;
                }

                int          objectId             = incommingMessage->getProperty( contracts::AUDIO_ENGINE_IPC_KEY_OBJECT_ID, -1);
                juce::String requestMessageId     = incommingMessage->getProperty( ipc::IPC_KEY_MESSAGE_ID );
                int          requestMessageType   = incommingMessage->getProperty( ipc::IPC_KEY_MESSAGE_TYPE );
                int          responseConnectionId = incommingMessage->getProperty( ipc::IPC_KEY_CONNECTION_ID );
                auto         message =
                    contracts::AudioEngineContractsHeleper::makeResponseMessage( objectId, requestMessageId, requestMessageType, responseCode, response );
                dispatcher->publish( responseConnectionId, message );
            }

        private:
            std::weak_ptr<contracts::AudioInterface> iface;
            std::vector<int>                        supportedMessageTypes;
        };
    }
}
