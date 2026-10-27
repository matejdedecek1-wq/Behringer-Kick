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
    const char* const IPC_KEY_LOG_TYPE( "log_type" ); // valid log types - LOG, METRIC.
    const char* const IPC_KEY_LOG_LEVEL( "log_level" ); // valid log levels - INFO, DEBUG, WARN,ERROR
    const char* const IPC_KEY_LOG_MESSAGE( "log_message" );
    const char* const IPC_KEY_METRIC_NAME( "metric_name" );
    const char* const IPC_KEY_METRIC_VALUE( "metric_value" );
}
