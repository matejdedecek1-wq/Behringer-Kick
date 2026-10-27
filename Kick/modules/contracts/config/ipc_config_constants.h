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
namespace contracts
{

#if defined( _WIN32 ) || defined( _WIN64 )
    static const juce::String BINARY_EXTENSION( ".exe" );
#else
    static const juce::String BINARY_EXTENSION( "" );
#endif

    static const int          ENGINE_HOST_ID( 1 );
    static const juce::String ENGINE_UUID( juce::Uuid().toString() );
    static const juce::String ENGINE_EXE_NAME( "KickAudioEngine" + BINARY_EXTENSION );
    static const juce::String ENGINE_PROCESS_NAME( "KickAudioEngine" );
    static const int          ENGINE_SERVER_PORT( 50301 );


    static const int          MODEL_HOST_ID( 2 );
    static const juce::String MODEL_UUID( juce::Uuid().toString() );
    static const juce::String MODEL_EXE_NAME( "KickAudioModel" + BINARY_EXTENSION );
    static const juce::String MODEL_PROCESS_NAME( "KickAudioModel" );
    static const int          MODEL_SERVER_PORT( 50401 );

    static const int          DCL_HOST_ID( 3 );
    static const juce::String DCL_UUID( juce::Uuid().toString() );
    static const juce::String DCL_EXE_NAME( "KickDeviceControl" + BINARY_EXTENSION );
    static const int          DCL_SERVER_PORT( 50201 );
    static const juce::String DCL_PROCESS_NAME( "KickDeviceControl" );

    static const int          GUI_HOST_ID( 4 );
    static const juce::String GUI_UUID( juce::Uuid().toString() );
    static const juce::String GUI_EXE_NAME( "KickUserInterface" + BINARY_EXTENSION );
    static const juce::String GUI_PROCESS_NAME( "KickUserInterface" );

    
    static const int          MAIN_HOST_ID( 5 );
    static const juce::String MAIN_UUID( juce::Uuid().toString() );
    static const juce::String MAIN_PROCESS_NAME( "Kick Main" );
    static const int          MAIN_SERVER_PORT( 50101 );

    static const int CONNECTION_ID_MIN( 1000 );
    static const int CONNECTION_ID_MAX( 1000000 );

    // format <Client>_<Server>_CONNECTION_ID.
    static const int          DCL_MAIN_CONNECTION_ID( juce::Random().nextInt( juce::Range<int>( CONNECTION_ID_MIN, CONNECTION_ID_MAX ) ) );
    static const juce::String DCL_MAIN_CONNECTION_NAME( DCL_PROCESS_NAME + " to " + MAIN_PROCESS_NAME );
    static const int          ENGINE_MAIN_CONNECTION_ID( juce::Random().nextInt( juce::Range<int>( CONNECTION_ID_MIN, CONNECTION_ID_MAX ) ) );
    static const juce::String ENGINE_MAIN_CONNECTION_NAME( ENGINE_PROCESS_NAME + " to " + MAIN_PROCESS_NAME );
    static const int          GUI_MAIN_CONNECTION_ID( juce::Random().nextInt( juce::Range<int>( CONNECTION_ID_MIN, CONNECTION_ID_MAX ) ) );
    static const juce::String GUI_MAIN_CONNECTION_NAME( GUI_PROCESS_NAME + " to " + MAIN_PROCESS_NAME );
    static const int          MODEL_MAIN_CONNECTION_ID( juce::Random().nextInt( juce::Range<int>( CONNECTION_ID_MIN, CONNECTION_ID_MAX ) ) );
    static const juce::String MODEL_MAIN_CONNECTION_NAME( MODEL_PROCESS_NAME + " to " + MAIN_PROCESS_NAME );

    static const int          GUI_DCL_CONNECTION_ID( juce::Random().nextInt( juce::Range<int>( CONNECTION_ID_MIN, CONNECTION_ID_MAX ) ) );
    static const juce::String GUI_DCL_CONNECTION_NAME( GUI_PROCESS_NAME + " to " + DCL_PROCESS_NAME );

    static const int          GUI_MODEL_CONNECTION_ID( juce::Random().nextInt( juce::Range<int>( CONNECTION_ID_MIN, CONNECTION_ID_MAX ) ) );
    static const juce::String GUI_MODEL_CONNECTION_NAME( GUI_PROCESS_NAME + " to " + MODEL_PROCESS_NAME );
    static const int          DCL_MODEL_CONNECTION_ID( juce::Random().nextInt( juce::Range<int>( CONNECTION_ID_MIN, CONNECTION_ID_MAX ) ) );
    static const juce::String DCL_MODEL_CONNECTION_NAME( DCL_PROCESS_NAME + " to " + MODEL_PROCESS_NAME );

    static const int          MODEL_ENGINE_CONNECTION_ID( juce::Random().nextInt( juce::Range<int>( CONNECTION_ID_MIN, CONNECTION_ID_MAX ) ) );
    static const juce::String MODEL_ENGINE_CONNECTION_NAME( MODEL_PROCESS_NAME + " to " + ENGINE_PROCESS_NAME );


    static const int DCL_LOGGER_CONNECTION_ID( DCL_MAIN_CONNECTION_ID );
    static const int GUI_LOGGER_CONNECTION_ID( GUI_MAIN_CONNECTION_ID );
    static const int ENGINE_LOGGER_CONNECTION_ID( ENGINE_MAIN_CONNECTION_ID );
    static const int MODEL_LOGGER_CONNECTION_ID( MODEL_MAIN_CONNECTION_ID );

    static const juce::String BIND_ADRESS_OR_URL( "127.0.0.1" );

    static bool      DELAY_CLIENT_START( false);
    static const int CLIENT_START_DELAY( 30000 );

    static const juce::String BASE_PATH =
        juce::File::getSpecialLocation( juce::File::currentExecutableFile ).getParentDirectory().getFullPathName()
        + juce::File::getSeparatorString();



    static const int STANDALONE_DCL_MAIN_CONNECTION_ID(100 );
    static const int STANDALONE_ENGINE_MAIN_CONNECTION_ID(101 );
    static const int STANDALONE_GUI_MAIN_CONNECTION_ID(102 );
    static const int STANDALONE_GUI_DCL_CONNECTION_ID( 103 );
    static const int STANDALONE_MODEL_ENGINE_CONNECTION_ID( 104 );
    static const int STANDALONE_GUI_MODEL_CONNECTION_ID( 105 );
    static const int STANDALONE_DCL_MODEL_CONNECTION_ID( 106 );
    static const int STANDALONE_MODEL_MAIN_CONNECTION_ID( 107 );
}
