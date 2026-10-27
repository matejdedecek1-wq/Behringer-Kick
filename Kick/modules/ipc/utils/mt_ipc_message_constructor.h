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

#include <juce_core/juce_core.h>
#include "../utils/mt_ipc_json.h"
#include "../utils/mt_ipc_constants.h"
#include "../utils/mt_ipc_keys.h"

namespace ipc
{
    class MTIPCMessageConstructor
    {
    public:
        static void                           setMessageOriginator( const juce::String originator );
        static std::shared_ptr<ipc::MTIPCJson> constructHeaderMessage( int messageType );
        static std::shared_ptr<ipc::MTIPCJson> constructResponseMessage(int messageType, const juce::String& requestId, 
            const juce::HashMap<juce::String, juce::var>& dotSeparatedKeyToValuePairs = juce::HashMap<juce::String, juce::var>());
        static std::shared_ptr<ipc::MTIPCJson> constructRequestMessage(int messageType, 
            const juce::HashMap<juce::String, juce::var>& dotSeparatedKeyToValuePairs = juce::HashMap<juce::String, juce::var>());
        static std::shared_ptr<ipc::MTIPCJson> constructEventMessage(int messageType, 
            const juce::HashMap<juce::String, juce::var>& dotSeparatedKeyToValuePairs = juce::HashMap<juce::String, juce::var>());
        static std::shared_ptr<ipc::MTIPCJson> constructMessage( const juce::HashMap<juce::String, juce::var>& dotSeparatedKeyToValuePairs );
        static void                           updateHeaderIntoMessage( std::shared_ptr<ipc::MTIPCJson> message );
        static void                           addDataToMessage( std::shared_ptr<ipc::MTIPCJson> message, const juce::HashMap<juce::String, juce::var>& dotSeparatedKeyToValuePairs );

    private:
        static juce::String messageOriginator;
    };
}
