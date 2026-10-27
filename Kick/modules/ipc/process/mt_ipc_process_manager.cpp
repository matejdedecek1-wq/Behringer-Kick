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
#include "mt_ipc_process_manager.h"
#include "mt_ipc_child_process_executor.h"
#include "../utils/mt_ipc_constants.h"
#include "../utils/mt_ipc_keys.h"
#include "mti_ipc_process_manager_callback.h"

const juce::String DEFAULT_PROCESS_NAME( "" );
namespace ipc
{
    class MTIPCProcessStarterThread : public juce::Thread
    {
    public:
        MTIPCProcessStarterThread( const int processId, std::function<bool( int )> processStartMethod,
                                   std::function<void( int )> exitNotifier )
            : juce::Thread( "Process_Starter_Thread " + juce::String( processId ) ), processIdentifier( processId ),
            processStartMethod( processStartMethod ), exitNotifier( exitNotifier )
        {
        }

        void run()
        {
            if( !processStartMethod ) return;

            processStartMethod( processIdentifier );
            juce::MessageManager::callAsync( [&]() { exitNotifier( processIdentifier ); } );
        }

    private:
        int                        processIdentifier;
        std::function<bool( int )> processStartMethod;
        std::function<void( int )> exitNotifier;
    };


	class MTIPCProcessManagerData
	{
	public:
		~MTIPCProcessManagerData()
		{
			executors.clear();
		}

	public:

		juce::HashMap<int, std::shared_ptr<ipc::MTIPCChildProcessExecutor>> executors;
		juce::HashMap<int, std::shared_ptr<juce::var>> configurations;
		std::shared_ptr<ipc::MTIIPCProcessManagerCallback> callback;
        juce::HashMap<int, std::shared_ptr<ipc::MTIPCProcessStarterThread>> starters;

	};

	class MTIPCProcessMonitorThread : public juce::Thread
	{
	public:
		MTIPCProcessMonitorThread(std::shared_ptr< MTIPCProcessManagerData> data)
			: juce::Thread("ProcessManagerThread"), data(data)
		{
		}
		~MTIPCProcessMonitorThread()
		{

		}

		virtual void run() override
		{

			int count = 0;
			int sleepDuration = 100;
			int timeoutInmilliSeconds = (ipc::IPC_PROCESS_MONITOR_THREAD_TIMEOUT_IN_MILLI_SECONDS) / sleepDuration;

			while (!threadShouldExit())
			{
                juce::Thread::sleep( sleepDuration );
				count++;
				if (count == timeoutInmilliSeconds)
				{
					count = 0;

					for (auto itr = data->executors.begin(); itr != data->executors.end(); itr.next())
					{
						if (itr.getValue()->isRunning())
							continue;
						int pid = itr.getKey();
						int exitcode = itr.getValue()->exitCode();
						data->executors.remove(pid);
                        juce::String processName( "" );
                        if( data->configurations.contains(pid) )
                            processName = data->configurations[pid]->getProperty( ipc::IPC_KEY_PROCESS_NAME, "" );
						itr.reset();
						if (data->callback)
							juce::MessageManager::callAsync([&, pid, exitcode, processName]() { data->callback->processExited(pid, exitcode, processName); });
					}
				}
			}
		}


	private:
		std::shared_ptr< MTIPCProcessManagerData> data;
	};

   
	MTIPCProcessManager::MTIPCProcessManager(std::shared_ptr<ipc::MTIIPCProcessManagerCallback> callback)
		: data(new MTIPCProcessManagerData())
		, processMonitorThread(nullptr)
	{
		data->callback = callback;
	}

	MTIPCProcessManager::~MTIPCProcessManager()
	{
		data.reset();
        processMonitorThread->stopThread( 1500 );
		processMonitorThread.reset();
	}

    void MTIPCProcessManager::starterThreadExitNotifier(const int pid)
    {
        if( data->starters.contains( pid ) )
        {
            std::shared_ptr<ipc::MTIPCProcessStarterThread> th = data->starters[pid];
            th->stopThread(1000);
            th->waitForThreadToExit( 1000 );
            data->starters.remove( pid );
        }
    }

	void MTIPCProcessManager::initialize(std::shared_ptr<juce::var> config)
	{
		juce::var               processes = config->getProperty(ipc::IPC_KEY_CHILDREN, juce::Array<juce::var>());
		juce::Array<juce::var>* arr = processes.getArray();
		for (juce::var& processConfig : *arr)
		{
			int id = processConfig.getProperty(ipc::IPC_KEY_HOST_ID, -1);
            if( id >= 0 )
            {
                data->configurations.set( id, std::make_shared<juce::var>( processConfig ) );
            }
		}
		processMonitorThread.reset(new ipc::MTIPCProcessMonitorThread(data));
	}

	void MTIPCProcessManager::start()
	{

		for (auto itr = data->configurations.begin(); itr != data->configurations.end(); itr.next())
		{
            startProcess( itr.getKey() );
        }
		processMonitorThread->startThread();
	}

	void MTIPCProcessManager::stop()
	{
		if(processMonitorThread && processMonitorThread->isThreadRunning())
			processMonitorThread->stopThread(1500);
		for (auto executor : data->executors)
		{
			executor->stop();
		}

	}

	void MTIPCProcessManager::wait()
	{
		for (auto executor : data->executors)
		{
			executor->join();
		}
	}

	int MTIPCProcessManager::exitCode( const int processIdentifier )
	{
		return data->executors[processIdentifier]->exitCode();
	}

	bool MTIPCProcessManager::isRunning( const int processIdentifier )
	{
		return data->executors[processIdentifier]->isRunning();
	}

    void MTIPCProcessManager::startProcessInThread( const int pid )
    {
        std::shared_ptr<ipc::MTIPCProcessStarterThread> th( new ipc::MTIPCProcessStarterThread(
            pid, std::bind( &MTIPCProcessManager::startProcess, this, std::placeholders::_1 ),
            std::bind( &MTIPCProcessManager::starterThreadExitNotifier, this, std::placeholders::_1 ) ) );

        data->starters.set( pid, th );
        th->startThread();
    }

	bool MTIPCProcessManager::startProcess( const int processIdentifier )
	{
		std::shared_ptr<juce::var> processConfig = data->configurations[processIdentifier];
		std::shared_ptr< ipc::MTIPCChildProcessExecutor> exe(new ipc::MTIPCChildProcessExecutor(processConfig));		
		bool started = exe->start();

       

        if( started )
        {
            data->executors.set( processIdentifier, exe );
        }

        juce::String processName( "" );
        if( data->configurations.contains( processIdentifier ) )
            processName = data->configurations[processIdentifier]->getProperty( ipc::IPC_KEY_PROCESS_NAME, "" );
        juce::Logger::writeToLog( started ? processName + " Started..." : "Failed to Start " + processName);


        if( data->callback )
        {
            started ?
                juce::MessageManager::callAsync(
                    [&, processIdentifier, processName]() { data->callback->processStarted( processIdentifier, processName ); } ) :
                juce::MessageManager::callAsync(
                    [&, processIdentifier, processName]() { data->callback->failedToStartProcess( processIdentifier, processName ); } );
        }
		
		return started;
	}

	bool MTIPCProcessManager::stopProcess( const int processIdentifier )
	{
		bool stopped = data->executors[processIdentifier]->stop();
        if( stopped && data->callback )
		{
            juce::String processName( DEFAULT_PROCESS_NAME );
            if( data->configurations.contains( processIdentifier ) )
                processName = data->configurations[processIdentifier]->getProperty( ipc::IPC_KEY_PROCESS_NAME, DEFAULT_PROCESS_NAME );

			juce::MessageManager::callAsync(
                [&, processIdentifier, processName]() { data->callback->processExited( processIdentifier, data->executors[processIdentifier]->exitCode(), processName ); } );
		}
		return stopped;
	}

	bool MTIPCProcessManager::waitForProcessToExit(const int processIdentifier)
	{
		return data->executors[processIdentifier]->join();
	}

    const juce::String& MTIPCProcessManager::processName( const int processIdentifier )
    {

        if( data->configurations.contains( processIdentifier ) )
            return data->configurations[processIdentifier]->getProperty( ipc::IPC_KEY_PROCESS_NAME, DEFAULT_PROCESS_NAME );
        return DEFAULT_PROCESS_NAME;
    }
}
