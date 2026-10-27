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

#include "memory"
#include "kick_heart_beat_data.h"
#include "juce_core/juce_core.h"

const int HEART_BEAT_MONITOR_TIMEOUT_IN_MS( 2000 );
const int HEART_BEAT_MONITOR_EXIT_WAIT_IN_MS( 500 );

namespace kick
{
    namespace main
    {
        class KickHeartBeatData;
        class KickHearBeatMonitor : public juce::Thread
        {
        public:
            KickHearBeatMonitor( std::shared_ptr<kick::main::KickHeartBeatData> heartBeatData,
                                 std::function<void( const int )>               onHeartBeatNotReceived )
                : juce::Thread( "HeartBeatMonitorThread" )
                , heartBeatData( heartBeatData )
                , onHeartBeatNotReceived( onHeartBeatNotReceived )
            {

            }
            void startMonitoring()
            {
                previousCheckTimePoint = juce::Time::currentTimeMillis();
                startThread();
            }

            void stopMonintoring()
            {
                stopThread( HEART_BEAT_MONITOR_EXIT_WAIT_IN_MS );
            }

            virtual void run() override
            {
                int count         = 0;
                int sleepDuration = 100;
                int totalCount    = ( HEART_BEAT_MONITOR_TIMEOUT_IN_MS ) / sleepDuration;
                while( !threadShouldExit() )
                {
                    juce::Thread::sleep( sleepDuration );
                    count++;
                    if( count == totalCount )
                    {
                        count = 0;
                        juce::int64 currentCheckTimePoint = juce::Time::currentTimeMillis();
                        heartBeatData->verify( previousCheckTimePoint, currentCheckTimePoint );
                        previousCheckTimePoint = juce::Time::currentTimeMillis();
                    }
                }
            }

        private:
            std::shared_ptr<kick::main::KickHeartBeatData> heartBeatData;
            std::function<void( const int )>               onHeartBeatNotReceived;
            juce::int64                                    previousCheckTimePoint{ 0 };
        };
    }
}
