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

#include "mt_ipc_child_process_executor.h"
#include "juce_core/juce_core.h"
#include "../utils/mt_ipc_constants.h"
#include "../utils/mt_ipc_keys.h"

namespace ipc
{
    class MTIPCChildProcessExecutorImpl : public juce::ChildProcess
    {
        public:
        MTIPCChildProcessExecutorImpl( std::shared_ptr<juce::var> config )
            : config( config ) {}

        ~MTIPCChildProcessExecutorImpl()
        {
            config.reset();
            if(isRunning())
                kill();
        }
        bool startChildProcess()
        {
            juce::String props = juce::JSON::toString( config->getProperty( ipc::IPC_KEY_ARGUMENTS, "" ) );
            juce::String path  = config->getProperty( ipc::IPC_KEY_PROCESS_PATH, "" );
            if( path.isEmpty() )
                return false;

            juce::StringArray arguments{ path, props};
            return start( arguments );

        }

    private:
        std::shared_ptr<juce::var> config;
    };

    MTIPCChildProcessExecutor::MTIPCChildProcessExecutor( std::shared_ptr<juce::var> config )
        : impl( new MTIPCChildProcessExecutorImpl(config) )
    {
    }

    MTIPCChildProcessExecutor::~MTIPCChildProcessExecutor()
    {
        impl = nullptr;
    }

    bool MTIPCChildProcessExecutor::start( )
    {
        if( isRunning() ) return true;

        return impl->startChildProcess();
    }

    bool MTIPCChildProcessExecutor::stop()
    {
        if( isRunning() )
            return impl->kill();
        return true;
    }

    bool MTIPCChildProcessExecutor::join()
    {
        if(isRunning()) 
            return impl->waitForProcessToFinish(5000);
        return true;
    }

    int MTIPCChildProcessExecutor::exitCode()
    {
        return impl->getExitCode();
    }

    bool MTIPCChildProcessExecutor::isRunning()
    {
        return impl->isRunning();
    }

}
