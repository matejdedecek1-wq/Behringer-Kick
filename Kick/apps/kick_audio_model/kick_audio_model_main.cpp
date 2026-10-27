/*
 * Copyright (c) 2022 MUSIC Tribe
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

#include "contracts/config/model_ipc_config.h"
#include "contracts/config/ipc_config_constants.h"
#include "contracts/utils/app/ipc_application.h"
#include "ipc/utils/mt_ipc_helpers.h"

//==============================================================================
class Model : public contracts::IPCApplication 
{
public:
    //==============================================================================
    Model()
        : IPCApplication( contracts::MODEL_PROCESS_NAME,
                          contracts::MODEL_PROCESS_NAME,
                          contracts::MODEL_PROCESS_NAME,
                          true, true, true ) {}

    const juce::String getApplicationName() override
    {
        return "KickAudioModel";
    }

    const juce::String getApplicationVersion() override
    {
        return "0.0.1";
    }

    bool moreThanOneInstanceAllowed() override
    {
        return false;
    }

    void addMessageHandlers( std::vector<std::shared_ptr<ipc::MTIIPCMessageHandler>>& handlers ) {}
    juce::var getConfig( const juce::String& commandLine )
    {
        return ( commandLine.isEmpty() ?
                     contracts::ModelIPCConfig().getStandAloneConfig() :
                     ipc::createJsonObjectFromCommandLineString( commandLine ) );
    }
};

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION( Model )
