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
        class SetPropertiesMessageHandler : public kick::ae::AudioEngineMessageHandler
        {
        public:
            SetPropertiesMessageHandler( std::shared_ptr<contracts::AudioInterface> interface )
                : AudioEngineMessageHandler( interface, { contracts::AudioEngineMessageTypes::AUDIO_ENGINE_MESSAGE_SET_PROPERTIES } )
            {
            }

            virtual juce::var processRequest( std::shared_ptr<ipc::MTIPCJson>              message,
                                              std::shared_ptr<ipc::MTIPCMessageDispatcher> dispatcher,
                                              std::shared_ptr<contracts::AudioInterface>   audioInterface )
            {
                const int objectId       = message->getProperty( contracts::AUDIO_ENGINE_IPC_KEY_OBJECT_ID );
                juce::var properties     = message->getProperty( contracts::AUDIO_ENGINE_IPC_KEY_OBJECT_PROPERTIES );

                juce::Array<juce::var>* props = properties.getArray();
                juce::Array<juce::var>  setProps;
                for( auto& prop : *props )
                {
                    juce::var            propertyId     = properties.getProperty( contracts::AUDIO_ENGINE_IPC_KEY_PROPERTY_ID, -1 );
                    juce::var            requestedValue = properties.getProperty( contracts::AUDIO_ENGINE_IPC_KEY_PROPERTY_VALUE, -1 );
                    const juce::var      setValue       = audioInterface->setProperty( objectId,
                                                                                       propertyId,
                                                                                       requestedValue );
                    juce::DynamicObject* pProperty      = new juce::DynamicObject();

                    pProperty->setProperty( contracts::AUDIO_ENGINE_IPC_KEY_SET_VALUE, setValue );
                    pProperty->setProperty( contracts::AUDIO_ENGINE_IPC_KEY_REQUESTED_VALUE, requestedValue );
                    pProperty->setProperty( contracts::AUDIO_ENGINE_IPC_KEY_PROPERTY_ID, propertyId );
                    setProps.add( pProperty );
                }

                juce::DynamicObject* pObj = new juce::DynamicObject();
                pObj->setProperty( contracts::AUDIO_ENGINE_IPC_KEY_OBJECT_PROPERTIES, setProps );
                return pObj;
            }
        };
    }
}
