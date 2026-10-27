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

#include "kick_exit_message_handler.h"
#include "contracts/main/system_message_types.h"
#include "juce_events/juce_events.h"

namespace kick
{
    namespace main
    {
        KickExitMessageHandler::KickExitMessageHandler()
            : supportedTypes{ contracts::SystemIPCMessageType::SYSTEM_IPC_MESSAGE_TYPE_EXIT }

        {
        }

        const std::vector<int>& KickExitMessageHandler::messageTypes() { return supportedTypes; }

        void KickExitMessageHandler::handleRequest( const int                                  messageType,
                                                    std::shared_ptr<ipc::MTIPCJson>            message,
                                                    std::weak_ptr<ipc::MTIPCMessageDispatcher> dispatcher )
        {
            juce::Logger::writeToLog( "___________Received Exit Message____________" );
            juce::MessageManager::callAsync( juce::JUCEApplicationBase::getInstance()->quit );
        }
    }
}
