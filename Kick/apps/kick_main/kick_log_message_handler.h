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

#include "ipc/processor/mti_ipc_message_handler.h"

namespace kick
{
    namespace main
    {
        class KickLogMessageHandler : public ipc::MTIIPCMessageHandler
        {
        public:
            KickLogMessageHandler();

            bool isDefaultHandler() { return false; };

            const std::vector<int>& messageTypes() { return supportedMessageTypes; } // the message types handled by this processor.

            void handleEvent( const int                       messageType,
                              std::shared_ptr<ipc::MTIPCJson> message,
                              std::weak_ptr<ipc::MTIPCMessageDispatcher>
                                  dispatcher ) override;
        private:
            std::vector<int> supportedMessageTypes;
        };
    }
}
