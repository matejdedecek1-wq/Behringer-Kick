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


#include "juce_core/juce_core.h"
#include "logger_enums.h"
#include "logger_constants.h"

namespace contracts
{
    static juce::String logLevelString( const int logLevel )
    {
        switch( logLevel )
        {
            case contracts::LogLevel::LOG_LEVEL_INFO:
                return contracts::LOG_LEVEL_INFO_STRING;

            case contracts::LogLevel::LOG_LEVEL_DEBUG:
                return contracts::LOG_LEVEL_DEBUG_STRING;

            case contracts::LogLevel::LOG_LEVEL_ERROR:
                return contracts::LOG_LEVEL_ERROR_STRING;

            case contracts::LogLevel::LOG_LEVEL_WARN:
                return contracts::LOG_LEVEL_WARNING_STRING;

            default:
                return " ";
        }
    }

    static juce::String getFormattedTime( const juce::int64 timeInMilliSeconds = 0 )
    {
        return timeInMilliSeconds > 0 ?
                   juce::Time( timeInMilliSeconds ).formatted( contracts::LOG_FILE_NAME_TIME_FORMAT ) :
                   juce::Time::getCurrentTime().formatted( contracts::LOG_LEVEL_TIME_FORMAT_STRING );
    }
}

