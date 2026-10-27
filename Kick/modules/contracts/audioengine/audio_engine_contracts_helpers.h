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

#include <memory>
#include "juce_core/juce_core.h"

namespace ipc
{
    class MTIPCJson;
}

namespace contracts
{
    class AudioEngineObjectProperties;
    class AudioEngineContractsHeleper
    {
    public:
        static std::shared_ptr<ipc::MTIPCJson> makeAllocateMessage( const int objectType );
        static std::shared_ptr<ipc::MTIPCJson> makeDeallocateMessage( const int objectId );
        static std::shared_ptr<ipc::MTIPCJson> makeClearMessage();
        static std::shared_ptr<ipc::MTIPCJson> makeConnectMessage( const int objectId1, const int objectId2 );
        static std::shared_ptr<ipc::MTIPCJson> makeConnectMessage( const int objectId, juce::Array<int> toObjectIds );
        static std::shared_ptr<ipc::MTIPCJson> makeConnectMessage( juce::HashMap<int, juce::Array<int>> connectionsMap );
        static std::shared_ptr<ipc::MTIPCJson> makeDisconnectMessage( const int objectId );
        static std::shared_ptr<ipc::MTIPCJson> makeDisconnectMessage( const int objectId1, const int objectId2 );
        static std::shared_ptr<ipc::MTIPCJson> makeSetPropertiesMessage( const int                            objectId,
                                                                         const juce::HashMap<int, juce::var>& properties );
        static std::shared_ptr<ipc::MTIPCJson> makeSetPropertyMessage( const int        objectId,
                                                                       const int        propertyId,
                                                                       const juce::var& value );
        static std::shared_ptr<ipc::MTIPCJson> makeGetPropertiesMessage( const int objectId );
        static std::shared_ptr<ipc::MTIPCJson> makeGetStatusMessage( const int objectId );
        static std::shared_ptr<ipc::MTIPCJson> makeResponseMessage( const int           objectId,
                                                                    const juce::String& requestMessageId,
                                                                    const int           requestMessageType,
                                                                    const int           responseCode,
                                                                    const juce::var&    response );
    };
}
