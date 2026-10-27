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
#include "ipc/utils/mt_ipc_json.h"
#include "ipc/utils/mt_ipc_keys.h"
#include "ipc_config_constants.h"
#include "ipc_config_inteface.h"
#include "dcl_ipc_config.h"
#include "gui_ipc_config.h"
#include "engine_ipc_config.h"
#include "model_ipc_config.h"

namespace contracts
{
    class KickIPCConfig : public IIPCConfig
    {
    public:
        juce::var getConfig()
        {
            juce::DynamicObject* pProcessConfig = new juce::DynamicObject();
            juce::DynamicObject* pConfig        = new juce::DynamicObject();

            juce::Array<juce::var> children;
            for( auto cfg : configs )
            {
                juce::var c = cfg->getConfig();
                if( c.isUndefined() )
                    continue;
                children.add( c );
            }
            if( children.size() > 0 )
                pProcessConfig->setProperty( ipc::IPC_KEY_CHILDREN, children );

            pConfig->setProperty( ipc::IPC_KEY_PROCESS_CONFIG, pProcessConfig );
            pConfig->setProperty( ipc::IPC_KEY_CONNECTION_CONFIG, prepareConnectionConfig() );

             juce::Array<juce::var> connectedHosts{
                prepareConnectedHostConfig( GUI_MAIN_CONNECTION_ID, GUI_HOST_ID ),
                prepareConnectedHostConfig( DCL_MAIN_CONNECTION_ID, DCL_HOST_ID ),
                prepareConnectedHostConfig( ENGINE_MAIN_CONNECTION_ID, ENGINE_HOST_ID ),
                prepareConnectedHostConfig( MODEL_MAIN_CONNECTION_ID, MODEL_HOST_ID )
            };

            pConfig->setProperty( ipc::IPC_KEY_CONNECTED_HOSTS, connectedHosts );
            pConfig->setProperty( ipc::IPC_KEY_HOST_ID, MAIN_HOST_ID );

            return pConfig;
        }

        juce::var prepareConnectionConfig()
        {
            juce::Array<juce::var> validConnections{
                prepareConnectionValidationConfig( DCL_MAIN_CONNECTION_ID, DCL_UUID ),
                prepareConnectionValidationConfig( ENGINE_MAIN_CONNECTION_ID, ENGINE_UUID ),
                prepareConnectionValidationConfig( GUI_MAIN_CONNECTION_ID, GUI_UUID ),
                prepareConnectionValidationConfig( MODEL_MAIN_CONNECTION_ID, MODEL_UUID ),
            };

            juce::var            serverConfig      = prepareServerConfig( BIND_ADRESS_OR_URL, MAIN_SERVER_PORT, true, validConnections );
            juce::DynamicObject* pConnectionConfig = new juce::DynamicObject();
            pConnectionConfig->setProperty( ipc::IPC_KEY_UUID, MAIN_UUID );
            pConnectionConfig->setProperty( ipc::IPC_KEY_SERVER, serverConfig );
            return pConnectionConfig;
        }


        juce::var getStandAloneConfig()
        {
            juce::var serverConfig = prepareServerConfig(BIND_ADRESS_OR_URL, MAIN_SERVER_PORT);

            juce::DynamicObject* pConnectionConfig = new juce::DynamicObject();
            pConnectionConfig->setProperty( ipc::IPC_KEY_UUID, "571f03f662c345fd8d1c003ab46f629a" );
            pConnectionConfig->setProperty( ipc::IPC_KEY_SERVER, serverConfig );
            juce::DynamicObject* pObj = new juce::DynamicObject();
            pObj->setProperty( ipc::IPC_KEY_CONNECTION_CONFIG, pConnectionConfig );

             juce::Array<juce::var> connectedHosts{
                prepareConnectedHostConfig( STANDALONE_GUI_MAIN_CONNECTION_ID, GUI_HOST_ID ),
                prepareConnectedHostConfig( STANDALONE_DCL_MAIN_CONNECTION_ID, DCL_HOST_ID ),
                prepareConnectedHostConfig( STANDALONE_ENGINE_MAIN_CONNECTION_ID, ENGINE_HOST_ID )
            };

            pObj->setProperty( ipc::IPC_KEY_CONNECTED_HOSTS, connectedHosts );

            return pObj;
        }

    private:
        juce::Array<std::shared_ptr<IIPCConfig>> configs{
            std::shared_ptr<IIPCConfig>( new contracts::ModelIPCConfig()),
            std::shared_ptr<IIPCConfig>( new contracts::EngineIPCConfig() ),
            std::shared_ptr<IIPCConfig>( new contracts::DclIPCConfig() ),
            std::shared_ptr<IIPCConfig>( new contracts::GUIIPCConfig() )
        };
    };
}
