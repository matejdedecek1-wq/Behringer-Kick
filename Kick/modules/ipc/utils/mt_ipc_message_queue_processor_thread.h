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
#include "../utils/mt_ipc_exception.h"
namespace ipc
{
    template<typename T>
    class MTIPCMessageQueueProcessingThread : public juce::Thread
    {
    public:
        MTIPCMessageQueueProcessingThread( juce::String threadName )
            : juce::Thread( threadName ){};

        virtual void run()
        {
            while( !threadShouldExit() )
            {
                message_added.wait( 1 );
                if( q.isEmpty() )
                    continue;
                process_q();
            }
        }

        void push_to_q( std::shared_ptr<T> data, bool lockFree = false )
        {
            {
                if( lockFree )
                {
                    q.push( data );
                }
                else
                {
                    const juce::ScopedWriteLock locker( rwl );
                    q.push( data );
                }
            }
            message_added.signal();
        }

    protected:
        virtual void processMessage( std::shared_ptr<T> message ) = 0;

    private:
        void process_q()
        {
            while( !q.isEmpty() && !threadShouldExit() )
            {
                try
                {
                    const juce::ScopedWriteLock locker( rwl );
                    std::shared_ptr<T>          data = q.pop();
                    if( !data )
                        return;
                    processMessage( data );
                    data.reset();

                } catch( const ipc::MTIPCException& ex )
                {
                    juce::Logger::writeToLog( "Exception Occurred in" + getThreadName() + ". Error Code: " + juce::String( ex.e_code ) + ".Error Message: " + juce::String( ex.e_message ) );
                } catch( const std::exception& ex )
                {
                    juce::Logger::writeToLog( "Exception Occurred in" + getThreadName() + ex.what() );
                } catch( ... )
                {
                    juce::Logger::writeToLog( "Exception Occurred in" + getThreadName() + "Unknown Exception.." );
                }
            }
        }

    private:
        ipc::MTIPCMessageQueue<T> q;
        juce::WaitableEvent       message_added;
        juce::ReadWriteLock       rwl;
    };
}
