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

namespace juce
{
    class String;
}

namespace contracts
{
    class LoggerImpl;
    class MTIPCMessageDispatcher;

    class Logger : public juce::Logger
    {
    public:
        Logger( const juce::String&                          logFileBaseName,
                const juce::String&                          metricsFileBaseName,
                const juce::String&                          processName,
                bool                                         logToServer,
                int                                          connectionId = -1,
                std::shared_ptr<ipc::MTIPCMessageDispatcher> dispatcher   = nullptr );
        ~Logger();

    public:
        static void Info( const juce::String& message, const juce::String& origin = "", const juce::int64 timeInMillSeconds = 0 );
        static void Debug( const juce::String& message, const juce::String& origin = "", const juce::int64 timeInMillSeconds = 0 );
        static void Warning( const juce::String& message, const juce::String& origin = "", const juce::int64 timeInMillSeconds = 0 );
        static void Error( const juce::String& message, const juce::String& origin = "", const juce::int64 timeInMillSeconds = 0 );
        static void Metric( const juce::String& name, const juce::var& value, const juce::String& origin = "", const juce::int64 timeInMillSeconds = 0 );
        static void log( const juce::String& message, const int level, const juce::String& origin = "", const juce::int64 timeInMillSeconds = 0 );
        void        setDispatcher( const int connId, std::shared_ptr<ipc::MTIPCMessageDispatcher> pDisptacter );
        void        logMessage( const juce::String& message );
        void        createLogFile();
        void        createMetricsFile();
        const long  getLogFileSize();
        const long  getLogMetricsSize();

    private:
        static contracts::LoggerImpl* pImpl;
    };
}
