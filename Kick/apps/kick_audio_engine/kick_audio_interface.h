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

#include "juce_core/juce_core.h"
#include "contracts/audioengine/audio_engine_object_properties.h"
#include "contracts/audioengine/audio_engine_response_code.h"
#include "contracts/audioengine/audio_interface.h"

namespace kick
{
    namespace ae
    {
        class KickAudioInterface : public contracts::AudioInterface
        {
        public:
            const int createObject( const int objectType )
            {
                // create the object;
                return juce::Random().nextInt();
            };

            void destroyObject( const int objectId, juce::Array<juce::var>& destroyedObjectIds )
            {
                // destroy object;
                // fill up the destroyed object ids,
                // ex.. a sample object may have slice objects which are its clildren, which should be destroyed when sample is destroyed
                // update all those destroyed ids.
            }

            const contracts::AudioEngineObjectProperties& getProperties( const int objectId )
            {
                return dummy; // get properties from object and return. now it returns dummy, which needs to be removed.
            }

            const int getObjectStatus( const int objectId )
            {
                const int status = 0; // get status from object
                return status;
            }

            void clear( juce::Array<juce::var>& destroyedObjectIds )
            {
                // destroy all objects in the current pipeline and fill up the destroyed object ids
                // ex.. a sample object may have slice objects which are its clildren, which should be destroyed when sample is destroyed
                // update all those destroyed ids.
            }

            bool connect( const int objectId, juce::Array<juce::var> connectedObjectIds, juce::Array<juce::var>& successfulConnections )
            {
                const int code = contracts::AudioEngineResponseCode::AUDIO_ENGINE_RESPONSE_CODE_SUCCESS;
                // connect objects;
                return code;
            }

            bool disconnectAll( const int objectId )
            {
                const int code = contracts::AudioEngineResponseCode::AUDIO_ENGINE_RESPONSE_CODE_SUCCESS;
                // connect objects;
                return code;
            }

            const int disconnect( const int firstObjectId, const int secondObjectId )
            {
                const int code = contracts::AudioEngineResponseCode::AUDIO_ENGINE_RESPONSE_CODE_SUCCESS;
                // connect objects;
                return code;
            }

            juce::var setProperty( const int ObjeectId, const int propertyId, const juce::var& val )
            {
                // set the property to object, the object should return the acutual value after setting the property.
                return juce::var();
            }

            juce::var execute( const int objectId, const int operationId )
            {
                return juce::var();
            }

        private:
            contracts::AudioEngineObjectProperties dummy; // remove this once the necessary object code is written.
        };
    }
}
