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

#include "audio_engine_signals.h"
#include "audio_engine_object_properties.h"
#include "ipc/utils/mt_ipc_json.h"


namespace contracts
{
    sigslot::signal<std::shared_ptr<ipc::MTIPCJson>> AudioEngineSignals::SendRequestToAudioEngine = {};

    sigslot::signal<const juce::String& /*requestid*/,
                    const int /*request messagetype*/,
                    const int /*objectId*/,
                    juce::var /*response data*/> AudioEngineSignals::ResponseReceivedFromAudioEngine = {};

    sigslot::signal<const juce::String& /*requestid*/,
                    const int /*objectId*/,
                    const contracts::AudioEngineObjectProperties& /*properties*/> AudioEngineSignals::ObjectPropertiesReceived = {};
}
