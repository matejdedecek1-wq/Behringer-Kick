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

#include "juce_core/juce_core.h"
#include "juce_events/juce_events.h"
#include "mt_ipc_delayed_connector.h"
#include "../utils/mt_ipc_constants.h"
#include "../connection/mt_ipc_connection_manager.h"
#include "../utils/mt_ipc_timer.h"

namespace ipc
{
    class MTIPCDelayedConnectorData
    {
    public:
        std::shared_ptr<ipc::MTIPCConnectionManager> cm{ nullptr };
        int                                          connectionId;
        int                                          attempts{ 0 };
        std::function<void( int, bool )>             connectionAttemptCompletedCB;
        std::unique_ptr<ipc::MTIPCTimer>             timer{ nullptr };
    };

    MTIPCDelayedConnector::MTIPCDelayedConnector(std::shared_ptr<ipc::MTIPCConnectionManager> cm,
        const int connectionId, std::function<void(int, bool)> connectionAttemptCompletedCB)
        : data(new MTIPCDelayedConnectorData())
    {
        data->cm = cm;
        data->connectionId = connectionId;
        data->connectionAttemptCompletedCB = connectionAttemptCompletedCB;
        data->attempts = 0;
        data->timer.reset(new ipc::MTIPCTimer(ipc::IPC_DELAYED_CONNECT_DELAY_IN_MILLI_SECONDS, std::bind(&MTIPCDelayedConnector::onTimeout, this)));
    }
    MTIPCDelayedConnector::~MTIPCDelayedConnector()
    {
        data->timer->stop();
        data.reset();
    }

    int  MTIPCDelayedConnector::connectionIdentifier() { return data->connectionId; }
    bool MTIPCDelayedConnector::isConnected() { return data->cm ? data->cm->isConnected(data->connectionId) : false; }
    void MTIPCDelayedConnector::connect() { data->timer->start(); }
    void MTIPCDelayedConnector::stopConnectAttempt() { data->timer->stop(); }

    void MTIPCDelayedConnector::onTimeout()
    {
        if (!data->cm) return;
        bool connected = false;
        connected = data->cm->connect(data->connectionId);
            
        if (connected)
        {
            data->attempts = 0;
            juce::MessageManager::callAsync([&, connected]() { data->connectionAttemptCompletedCB(data->connectionId, connected); });
        }

        else if (data->attempts < 3)
        {
            data->attempts++;
            juce::MessageManager::callAsync([&]() {connect(); });
        }
        else
        {
            // log connection failure message
            data->attempts = 0;
            juce::MessageManager::callAsync([&, connected]() { data->connectionAttemptCompletedCB(data->connectionId, connected); });
        }
    }
}
