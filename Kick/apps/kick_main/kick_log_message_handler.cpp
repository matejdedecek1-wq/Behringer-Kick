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
#include "kick_log_message_handler.h"
#include "contracts/logger/logger_constants.h"
#include "contracts/logger/logger_message_types.h"
#include "contracts/logger/logger_enums.h"
#include "ipc/utils/mt_ipc_json.h"
#include "contracts/logger/logger_keys.h"
#include "contracts/logger/logger.h"
#include "ipc/utils/mt_ipc_keys.h"

namespace kick
{
    namespace main
    {

        KickLogMessageHandler::KickLogMessageHandler()
            : supportedMessageTypes{ contracts::LoggerMessageType::LOGGER_MESSAGE_TYPE_LOG }

        {
        }

        void KickLogMessageHandler::handleEvent( const int                                    messageType,
                                                 std::shared_ptr<ipc::MTIPCJson>              message,
                                                 std::weak_ptr<ipc::MTIPCMessageDispatcher> dispatcher )
        {
            int logType = message->getProperty( contracts::IPC_KEY_LOG_TYPE, contracts::LogType::LOG_TYPE_INVALID );

            if( logType == contracts::LogType::LOG_TYPE_INVALID ) return;

            juce::String      origin    = message->getProperty( ipc::IPC_KEY_MESSAGE_ORIGINATOR, "" ).toString();
            const juce::int64 timestamp = message->getProperty( ipc::IPC_KEY_TIMESTAMP, 0 );

            if( logType == contracts::LogType::LOG_TYPE_LOG )
            {
                int logLevel = message->getProperty( ipc::IPC_KEY_LOG_LEVEL, contracts::LogLevel::LOG_LEVEL_INFO );
                contracts::Logger::log( message->getProperty( ipc::IPC_KEY_LOG_MESSAGE, "" ), logLevel, origin, timestamp );
            }

            if( logType == contracts::LogType::LOG_TYPE_METRIC )
            {
                juce::String metricName  = message->getProperty( ipc::IPC_KEY_METRIC_NAME, "" );
                juce::var    metricValue = message->getProperty( ipc::IPC_KEY_METRIC_VALUE, "" );

                if( metricName.isEmpty() )
                    return;

                contracts::Logger::Metric( metricName, metricValue, origin, timestamp );
            }
        }

    }
}
