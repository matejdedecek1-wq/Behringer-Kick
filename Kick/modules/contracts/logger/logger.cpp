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
#include "ipc/dispatcher/mt_ipc_message_dispatcher.h"
#include "ipc/utils/mt_ipc_message_constructor.h"
#include "logger_constants.h"
#include "logger_keys.h"
#include "logger_message_types.h"
#include "logger_enums.h"
#include "logger_helpers.h"
#include "logger.h"

namespace contracts
{
    // this class creates a log file at startup. Call createLogFile and createMetricsFile to create the files.
    // This will not monitor the file for deletion which needs to be implemented.

    class LoggerImpl
    {
    public:
        LoggerImpl( const juce::String&                          logFileBaseName,
                    const juce::String&                          metricsFileBaseName,
                    const juce::String&                          processName,
                    bool                                         logToServer  = false,
                    int                                          connectionId = -1,
                    std::shared_ptr<ipc::MTIPCMessageDispatcher> dispatcher   = nullptr )
            : connectionId( connectionId ), dispatcherWkptr( dispatcher ), processName( processName ), logFileName( logFileBaseName ), metricsFileName( metricsFileBaseName )
        {
            logMethod = logToServer ? std::bind( &LoggerImpl ::logMessagePrivate, this, std::placeholders::_1, std::placeholders::_2,
                                                 std::placeholders::_3, std::placeholders::_4 ) :
                                      std::bind( &LoggerImpl ::logMessageToFile, this, std::placeholders::_1, std::placeholders::_2,
                                                 std::placeholders::_3, std::placeholders::_4 );

            metricsMethod = logToServer ? std::bind( &LoggerImpl ::logMetricsPrivate, this, std::placeholders::_1, std::placeholders::_2,
                                                     std::placeholders::_3, std::placeholders::_4 ) :
                                          std::bind( &LoggerImpl ::logMetricsToFile, this, std::placeholders::_1, std::placeholders::_2,
                                                     std::placeholders::_3, std::placeholders::_4 );

            createMetricsFile();
            createLogFile();
        }

        ~LoggerImpl()
        {
            pLogFile.reset();
            pMetricsFile.reset();
        }

    public:
        void createLogFile()
        {
            juce::String filePath =
                juce::File::getSpecialLocation( juce::File::currentExecutableFile ).getParentDirectory().getFullPathName()
                + juce::File::getSeparatorString() + contracts::LOGS_DIR + juce::File::getSeparatorString() + logFileName + "_" + juce::Time::getCurrentTime().formatted( LOG_FILE_NAME_TIME_FORMAT ) + LOGGER_FILE_EXTN;

            {
                const juce::ScopedLock lk( logCS );
                pLogFile.reset( new juce::File( filePath ) );
                if( !pLogFile->exists() )
                    pLogFile->create();
            }
        }

        void createMetricsFile()
        {
            juce::String filePath =
                juce::File::getSpecialLocation( juce::File::currentExecutableFile ).getParentDirectory().getFullPathName()
                + juce::File::getSeparatorString() + contracts::METRICS_DIR + juce::File::getSeparatorString() + metricsFileName + "_" + juce::Time::getCurrentTime().formatted( LOG_FILE_NAME_TIME_FORMAT ) + METRICS_FILE_EXTN;

            {
                const juce::ScopedLock lk( metricCS );
                pMetricsFile.reset( new juce::File( filePath ) );
                if( !pMetricsFile->exists() )
                    pMetricsFile->create();
            }
        }

        const long getLogFileSize()
        {
            return pLogFile->getSize();
        }

        const long getMetricsFileSize()
        {
            return pMetricsFile->getSize();
        }

        void setDispatcher( const int connId, std::shared_ptr<ipc::MTIPCMessageDispatcher> pDisptacter )
        {
            this->connectionId    = connId;
            this->dispatcherWkptr = pDisptacter;
        }

        void logMessage( const juce::String& message, int level, const juce::String& origin = "", const juce::int64 timeInMilliseconds = 0 )
        {
            logMethod( message, level, origin, timeInMilliseconds );
        }

        void logMetric( const juce::String& name, const juce::var& value, const juce::String& origin = "", const juce::int64 timeInMilliseconds = 0 )
        {
            metricsMethod( name, value, origin, timeInMilliseconds );
        }

    private:
        void logMessageToFile( const juce::String& message, int level, const juce::String& origin = "", const juce::int64 timeInMilliseconds = 0 )
        {
            if( !pLogFile ) return;

            juce::String name     = origin.isEmpty() ? processName : origin;
            juce::String time     = contracts::getFormattedTime( timeInMilliseconds );
            juce::String logLevel = contracts::logLevelString( level );
            juce::String msg      = "[" + time + "] [" + name + "] " + logLevel + " " + message + juce::newLine;
            {
                const juce::ScopedLock lk( logCS );
                pLogFile->appendText( msg );
            }
        }

        void logMetricsToFile( const juce::String& metricName, const juce::var& metricValue, const juce::String& origin = "", const juce::int64 timeInMilliseconds = 0 )
        {
            if( !pMetricsFile ) return;

            juce::String name = origin.isEmpty() ? processName : origin;
            juce::String time = contracts::getFormattedTime( timeInMilliseconds );
            juce::String msg  = time + "," + name + ",METRIC," + metricName + "," + metricValue.toString() + juce::newLine;
            {
                const juce::ScopedLock lk( metricCS );
                pMetricsFile->appendText( msg );
            }
        }

        void logMessagePrivate( const juce::String& message, const int level, const juce::String& origin = "", const juce::int64 timeInMilliseconds = 0 )
        {
            std::shared_ptr<ipc::MTIPCMessageDispatcher> dispatcher = dispatcherWkptr.lock();
            if( canDispatchMessage( connectionId, dispatcher ) )
            {
                std::shared_ptr<ipc::MTIPCJson> msg = ipc::MTIPCMessageConstructor::constructEventMessage( contracts::LoggerMessageType::LOGGER_MESSAGE_TYPE_LOG );
                msg->setProperty(contracts::IPC_KEY_LOG_TYPE, contracts::LogType::LOG_TYPE_LOG);
                msg->setProperty( contracts::IPC_KEY_LOG_LEVEL, level );
                msg->setProperty( contracts::IPC_KEY_LOG_MESSAGE, message );
                dispatcher->publish( connectionId, msg );
            }
            else
                logMessageToFile( message, level, origin, timeInMilliseconds );
        }
        // timeInMilliseconds is used only in case of loging to a file, if the value is 0 then the current time will be considered.
        void logMetricsPrivate( const juce::String& name, const juce::var& value, const juce::String& origin = "", const juce::int64 timeInMilliseconds = 0 )
        {
            std::shared_ptr<ipc::MTIPCMessageDispatcher> dispatcher = dispatcherWkptr.lock();
            if( canDispatchMessage( connectionId, dispatcher ) )
            {
                std::shared_ptr<ipc::MTIPCJson> msg = ipc::MTIPCMessageConstructor::constructEventMessage( contracts::LoggerMessageType::LOGGER_MESSAGE_TYPE_LOG );
                msg->setProperty( contracts::IPC_KEY_LOG_TYPE, contracts::LogType::LOG_TYPE_METRIC );
                msg->setProperty( contracts::IPC_KEY_METRIC_NAME, name );
                msg->setProperty( contracts::IPC_KEY_METRIC_VALUE, value );
                dispatcher->publish( connectionId, msg );
            }
            else
                logMetricsToFile( name, value, origin, timeInMilliseconds );
        }

        inline bool canDispatchMessage( const int id, std::shared_ptr<ipc::MTIPCMessageDispatcher> dispatcher )
        {
            return ( dispatcher && dispatcher->canDispatch( id ) );
        }

    private:
        int                                        connectionId;
        std::weak_ptr<ipc::MTIPCMessageDispatcher> dispatcherWkptr;

        std::unique_ptr<juce::File> pLogFile{ nullptr };
        std::unique_ptr<juce::File> pMetricsFile{ nullptr };

        juce::String          processName;
        juce::String          logFileName;
        juce::String          metricsFileName;
        juce::CriticalSection logCS;
        juce::CriticalSection metricCS;

        std::function<void( const juce::String&, const juce::var&, const juce::String&, const juce::int64 )> metricsMethod;
        std::function<void( const juce::String&, const int level, const juce::String&, const juce::int64 )>  logMethod;
    };

    contracts::LoggerImpl* contracts::Logger::pImpl = nullptr;

    Logger::Logger( const juce::String&                          logFileBaseName,
                    const juce::String&                          metricsFileBaseName,
                    const juce::String&                          processName,
                    bool                                         logToServer,
                    int                                          connectionId,
                    std::shared_ptr<ipc::MTIPCMessageDispatcher> dispatcher )
    {
        if( !pImpl )
            pImpl = new contracts::LoggerImpl( logFileBaseName, metricsFileBaseName, processName, logToServer, connectionId, dispatcher );
    }

    Logger::~Logger()
    {
        delete pImpl;
        pImpl = nullptr;
    }

    void Logger::Info( const juce::String& message, const juce::String& origin, const juce::int64 timeInMillSeconds )
    {
        pImpl->logMessage( message, contracts::LogLevel::LOG_LEVEL_INFO, origin, timeInMillSeconds );
    }

    void Logger::Debug( const juce::String& message, const juce::String& origin, const juce::int64 timeInMillSeconds )
    {
        pImpl->logMessage( message, contracts::LogLevel::LOG_LEVEL_DEBUG, origin, timeInMillSeconds );
    }

    void Logger::Warning( const juce::String& message, const juce::String& origin, const juce::int64 timeInMillSeconds )
    {
        pImpl->logMessage( message, contracts::LogLevel::LOG_LEVEL_WARN, origin, timeInMillSeconds );
    }

    void Logger::Error( const juce::String& message, const juce::String& origin, const juce::int64 timeInMillSeconds )
    {
        pImpl->logMessage( message, contracts::LogLevel::LOG_LEVEL_ERROR, origin, timeInMillSeconds );
    }

    void Logger::Metric( const juce::String& name, const juce::var& value, const juce::String& origin, const juce::int64 timeInMillSeconds )
    {
        pImpl->logMetric( name, value, origin, timeInMillSeconds );
    }

    void Logger::log( const juce::String& message, const int level, const juce::String& origin, const juce::int64 timeInMillSeconds )
    {
        switch( level )
        {
            case contracts:: LogLevel::LOG_LEVEL_DEBUG:
                Debug( message, origin, timeInMillSeconds );
                break;

            case contracts::LogLevel::LOG_LEVEL_WARN:
                Warning( message, origin, timeInMillSeconds );
                break;

            case contracts::LogLevel::LOG_LEVEL_ERROR:
                Error( message, origin, timeInMillSeconds );
                break;
            default:
                Info( message, origin, timeInMillSeconds );
                break;
        }
    }

    void Logger::setDispatcher( const int connId, std::shared_ptr<ipc::MTIPCMessageDispatcher> pDisptacter )
    {
        pImpl->setDispatcher( connId, pDisptacter );
    }

    void Logger::logMessage( const juce::String& message )
    {
        pImpl->logMessage( message, contracts::LogLevel::LOG_LEVEL_INFO );
    }

    void Logger::createLogFile()
    {
        pImpl->createLogFile();
    }

    void Logger::createMetricsFile()
    {
        pImpl->createMetricsFile();
    }

    const long Logger::getLogFileSize()
    {
        return pImpl->getLogFileSize();
    }

    const long Logger::getLogMetricsSize()
    {
        return pImpl->getMetricsFileSize();
    }
}
