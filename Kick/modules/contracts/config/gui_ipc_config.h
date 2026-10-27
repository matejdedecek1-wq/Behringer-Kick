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

#include "ipc_config_inteface.h"
#include "ipc_config_constants.h"
#include "ipc/utils/mt_ipc_keys.h"
#include "ipc/utils/mt_ipc_enums.h"
#include "ipc/utils/mt_ipc_constants.h"
#include "ipc_config_helpers.h"
namespace contracts
{
    class GUIIPCConfig : public contracts::IIPCConfig
    {
    public:
        virtual juce::var getConfig() override
        {
            juce::Array<juce::var> clients{
                prepareClientConfig( ipc::MTIPCTransportType::IPC_TRANSPORT_TYPE_SOCKET, GUI_MAIN_CONNECTION_ID,
                                     MAIN_SERVER_PORT, BIND_ADRESS_OR_URL, ipc::IPC_MAX_CONNECTION_TIMEOUT,
                                     GUI_MAIN_CONNECTION_NAME ),

                prepareClientConfig( ipc::MTIPCTransportType::IPC_TRANSPORT_TYPE_SOCKET, GUI_MODEL_CONNECTION_ID,
                                     MODEL_SERVER_PORT, BIND_ADRESS_OR_URL, ipc::IPC_MAX_CONNECTION_TIMEOUT,
                                     GUI_MODEL_CONNECTION_NAME ),

                prepareClientConfig( ipc::MTIPCTransportType::IPC_TRANSPORT_TYPE_SOCKET, GUI_DCL_CONNECTION_ID,
                                     DCL_SERVER_PORT, BIND_ADRESS_OR_URL, ipc::IPC_MAX_CONNECTION_TIMEOUT,
                                     GUI_DCL_CONNECTION_NAME )

            };

            juce::DynamicObject* pConnectionConfig = new juce::DynamicObject();
            pConnectionConfig->setProperty( ipc::IPC_KEY_UUID, GUI_UUID );
            pConnectionConfig->setProperty( ipc::IPC_KEY_DELAY_STARTING_CLIENT, DELAY_CLIENT_START );
            pConnectionConfig->setProperty( ipc::IPC_KEY_CLIENT_START_DELAY, CLIENT_START_DELAY );
            pConnectionConfig->setProperty( ipc::IPC_KEY_CLIENTS, clients );

            juce::Array<juce::var> connectedHosts{
                prepareConnectedHostConfig( GUI_DCL_CONNECTION_ID, DCL_HOST_ID ),
                prepareConnectedHostConfig( GUI_MAIN_CONNECTION_ID, MAIN_HOST_ID ),
                prepareConnectedHostConfig( GUI_MODEL_CONNECTION_ID, MODEL_HOST_ID )
            };

            juce::DynamicObject* pArguments = new juce::DynamicObject();
            pArguments->setProperty( ipc::IPC_KEY_CONNECTION_CONFIG, pConnectionConfig );
            pArguments->setProperty( ipc::IPC_KEY_HOST_ID, GUI_HOST_ID );
            pArguments->setProperty( contracts::CONFIG_KEY_CONNECTION_IDS, prepareConnectionIdConfig(
                                                                               GUI_MAIN_CONNECTION_ID, GUI_LOGGER_CONNECTION_ID,
                                                                               GUI_MAIN_CONNECTION_ID,
                                                                               GUI_DCL_CONNECTION_ID, -1, -1, GUI_MODEL_CONNECTION_ID) );

           
            pArguments->setProperty( ipc::IPC_KEY_CONNECTED_HOSTS, connectedHosts );

            return prepareChildConfig( GUI_HOST_ID, BASE_PATH + GUI_EXE_NAME, GUI_PROCESS_NAME, pArguments );
        }

        juce::var getStandAloneConfig()
        {
            juce::Array<juce::var> clients{
                prepareClientConfig( ipc::MTIPCTransportType::IPC_TRANSPORT_TYPE_SOCKET, STANDALONE_GUI_MAIN_CONNECTION_ID,
                                     MAIN_SERVER_PORT, BIND_ADRESS_OR_URL, ipc::IPC_DEFAULT_CONNECTION_TIMEOUT,
                                     GUI_MAIN_CONNECTION_NAME ),

                prepareClientConfig( ipc::MTIPCTransportType::IPC_TRANSPORT_TYPE_SOCKET, STANDALONE_GUI_MODEL_CONNECTION_ID,
                                     ENGINE_SERVER_PORT, BIND_ADRESS_OR_URL, ipc::IPC_DEFAULT_CONNECTION_TIMEOUT,
                                     GUI_MODEL_CONNECTION_NAME ),

                prepareClientConfig( ipc::MTIPCTransportType::IPC_TRANSPORT_TYPE_SOCKET, STANDALONE_GUI_DCL_CONNECTION_ID,
                                     DCL_SERVER_PORT, BIND_ADRESS_OR_URL, ipc::IPC_DEFAULT_CONNECTION_TIMEOUT,
                                     GUI_DCL_CONNECTION_NAME ),
            };

            juce::DynamicObject* pConnectionConfig = new juce::DynamicObject();
            pConnectionConfig->setProperty( ipc::IPC_KEY_UUID, "9923c2d5a5ed460680dafca091e95b51" );
            pConnectionConfig->setProperty( ipc::IPC_KEY_DELAY_STARTING_CLIENT, false );
            pConnectionConfig->setProperty( ipc::IPC_KEY_CLIENT_START_DELAY, CLIENT_START_DELAY );
            pConnectionConfig->setProperty( ipc::IPC_KEY_CLIENTS, clients );

            juce::Array<juce::var> connectedHosts{
                prepareConnectedHostConfig( STANDALONE_GUI_DCL_CONNECTION_ID, DCL_HOST_ID ),
                prepareConnectedHostConfig( STANDALONE_GUI_MAIN_CONNECTION_ID, MAIN_HOST_ID ),
                prepareConnectedHostConfig( STANDALONE_GUI_MODEL_CONNECTION_ID, ENGINE_HOST_ID )
            };

            juce::DynamicObject* pObj = new juce::DynamicObject();
            pObj->setProperty( ipc::IPC_KEY_CONNECTION_CONFIG, pConnectionConfig );
            pObj->setProperty( ipc::IPC_KEY_HOST_ID, GUI_HOST_ID );
            pObj->setProperty( contracts::CONFIG_KEY_CONNECTION_IDS, prepareConnectionIdConfig(
                                                                         STANDALONE_GUI_MAIN_CONNECTION_ID, STANDALONE_GUI_MAIN_CONNECTION_ID,
                                                                         STANDALONE_GUI_MAIN_CONNECTION_ID,
                                                                         STANDALONE_GUI_DCL_CONNECTION_ID, -1, -1, STANDALONE_GUI_MODEL_CONNECTION_ID) );

            pObj->setProperty( ipc::IPC_KEY_CONNECTED_HOSTS, connectedHosts );

            return pObj;
        }
    };
}
