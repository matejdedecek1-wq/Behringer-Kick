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


#include "../utils/contracts_marcros.h"
namespace contracts
{
    CONTRACTS_DEFINE_INT_ENUM_CLASS( LogType, LOG_TYPE_INVALID{ 0 }, LOG_TYPE_LOG{ 1 }, LOG_TYPE_METRIC{ 2 } );

    CONTRACTS_DEFINE_INT_ENUM_CLASS( LogLevel, LOG_LEVEL_INFO{ 1 }, LOG_LEVEL_DEBUG{ 2 }, LOG_LEVEL_WARN{ 3 }, LOG_LEVEL_ERROR{ 4 } );
}
