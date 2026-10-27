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

#include "signal/signal.hpp"
#include "memory"
#include "juce_core/juce_core.h"


namespace ipc
{
    class MTIPCJson;
}

namespace juce
{
    class var;
    class String;
}

namespace contracts
{
    class AudioEngineObjectProperties;
    class AudioEngineSignals
    {
    public:
        static sigslot::signal<std::shared_ptr<ipc::MTIPCJson>> SendRequestToAudioEngine;

        static sigslot::signal<const juce::String& /*requestid*/,
                               const int /*request messagetype*/,
                               const int /*objectId*/,
                               juce::var /*response data*/ > ResponseReceivedFromAudioEngine;

        static sigslot::signal<const juce::String& /*requestid*/,
                               const int /*objectId*/,
                               const contracts::AudioEngineObjectProperties& /*properties*/> ObjectPropertiesReceived;

    };
}
