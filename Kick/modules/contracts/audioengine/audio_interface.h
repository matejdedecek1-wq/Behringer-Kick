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

namespace contracts
{
    class AudioInterface
    {
    public:
        virtual const int createObject( const int objectType ) = 0;
        // destroy object;
        // fill up the destroyed object ids,
        // ex.. a sample object may have slice objects which are its clildren, which should be destroyed when sample is destroyed
        // update all those destroyed ids.
        virtual void destroyObject( const int objectId, juce::Array<juce::var>& destroyedObjectIds ) = 0;
        // get properties from object and return.
        virtual const contracts::AudioEngineObjectProperties& getProperties( const int objectId )   = 0;
        virtual const int                                     getObjectStatus( const int objectId ) = 0;
        // destroy all objects in the current pipeline and fill up the destroyed object ids
        // ex.. a sample object may have slice objects which are its clildren, which should be destroyed when sample is destroyed
        // update all those destroyed ids.
        virtual void      clear( juce::Array<juce::var>& destroyedObjectIds )                                                                     = 0;
        virtual bool      connect( const int objectId, juce::Array<juce::var> connectedObjectIds, juce::Array<juce::var>& successfulConnections ) = 0;
        virtual bool      disconnectAll( const int objectId )                                                                                     = 0;
        virtual const int disconnect( const int firstObjectId, const int secondObjectId )                                                         = 0;
        virtual juce::var setProperty( const int ObjectId, const int propertyId, const juce::var& val )                                           = 0;
        virtual juce::var execute( const int objectId, const int operationId )                                                                    = 0;
    };
}
