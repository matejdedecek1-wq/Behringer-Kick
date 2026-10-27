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

#include "connection_id_store.h"

namespace contracts
{
    int ConnectionIdStore::PARENT_CONNECTION_ID = -1;
    int ConnectionIdStore::LOGGER_CONNECTION_ID = -1;
    int ConnectionIdStore::MAIN_CONNECTION_ID   = -1;
    int ConnectionIdStore::DCL_CONNECTION_ID    = -1;
    int ConnectionIdStore::ENGINE_CONNECTION_ID = -1;
    int ConnectionIdStore::GUI_CONNECTION_ID    = -1;
    int ConnectionIdStore::MODEL_CONNECTION_ID    = -1;

    void ConnectionIdStore::initialize( const juce::var& config )
    {
        auto cfg = config.getProperty( contracts::CONFIG_KEY_CONNECTION_IDS, juce::var() );

        PARENT_CONNECTION_ID = cfg.getProperty( contracts::CONFIG_KEY_PARENT_CONNECTION_ID, -1 );
        LOGGER_CONNECTION_ID = cfg.getProperty( contracts::CONFIG_KEY_LOGGER_CONNECTION_ID, -1 );
        MAIN_CONNECTION_ID   = cfg.getProperty( contracts::CONFIG_KEY_MAIN_CONNECTION_ID, -1 );
        DCL_CONNECTION_ID    = cfg.getProperty( contracts::CONFIG_KEY_DCL_CONNECTION_ID, -1 );
        ENGINE_CONNECTION_ID = cfg.getProperty( contracts::CONFIG_KEY_ENGINE_CONNECTION_ID, -1 );
        GUI_CONNECTION_ID    = cfg.getProperty( contracts::CONFIG_KEY_GUI_CONNECTION_ID, -1 );
        MODEL_CONNECTION_ID  = cfg.getProperty( contracts::CONFIG_KEY_MODEL_CONNECTION_ID, -1 );
    }

}
