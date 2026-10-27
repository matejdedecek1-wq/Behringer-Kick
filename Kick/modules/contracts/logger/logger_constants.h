
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

namespace contracts
{
    const char* const IPC_CLIENT_NAME_NOT_DEFINED( "Name Not Defined" );
    const char* const SUBSYSTEM_LOG_FILE_SUFFIX( "Log.log" );
    const char* const LOG_LEVEL_INFO_STRING( "[INFO]" );
    const char* const LOG_LEVEL_DEBUG_STRING( "[DEBUG]" );
    const char* const LOG_LEVEL_ERROR_STRING( "[ERROR]" );
    const char* const LOG_LEVEL_WARNING_STRING( "[WARNING]" );
    const char* const LOG_LEVEL_TIME_FORMAT_STRING( "%Y-%m-%d %H:%M:%S" );
    const char* const LOGGER_FILE_EXTN( ".log" );
    const char* const METRICS_FILE_EXTN( ".csv" );
    const char* const LOGS_DIR( "logs" );
    const char* const METRICS_DIR( "metrics" );
    const char* const LOG_FILE_NAME_TIME_FORMAT( "%Y%m%d%H%M%S" );
}
