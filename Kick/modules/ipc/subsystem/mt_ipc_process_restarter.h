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
#include <memory>
#include <functional>
#include "../process/mt_ipc_process_manager.h"
#include "../utils/mt_ipc_timer.h"
#include "../utils/mt_ipc_constants.h"

namespace ipc
{
    class MTIPCProcessManager;
    class MTIPCTimer;

    class MTIPCProcessRestarter
    {
    public:
        MTIPCProcessRestarter( const int                                 processId,
                               std::shared_ptr<ipc::MTIPCProcessManager> processManager,
                               std::function<void( int, bool )>          onRestartCompleted )
            : processId( processId )
            , processManager( processManager )
            , restartSuccessTimer( new ipc::MTIPCTimer( ipc::IPC_DELAYED_RESTART_DELAY_IN_MILLI_SECONDS, std::bind( &MTIPCProcessRestarter::onRestartSuccessTimerTimeout, this ) ) )
            , onRestartCompleted( onRestartCompleted )
        {}
        void restart()
        {
            if( attempts >= 3 )
            {
                attempts = 0;
                if( onRestartCompleted )
                    onRestartCompleted( processId, false );
                return;
            }
            if( restartSuccessTimer->isRunning() )
            {
                restartSuccessTimer->stop();
            }
            if( processManager )
            {
                attempts++;
                processManager->startProcess( processId );
                restartSuccessTimer->start();
            }
        }

        void stopRestartAttempt()
        {
            if( restartSuccessTimer->isRunning() )
                restartSuccessTimer->stop();
        }

        const int processIdentifier()
        {
            return processId;
        }

        void onRestartSuccessTimerTimeout()
        {
            attempts = 0;
            onRestartCompleted( processId, true );
        }

    private:
        const int                                 processId;
        int                                       attempts{ 0 };
        std::unique_ptr<ipc::MTIPCTimer>          restartSuccessTimer;
        std::function<void( int, bool )>          onRestartCompleted;
        std::shared_ptr<ipc::MTIPCProcessManager> processManager;
    };
}
