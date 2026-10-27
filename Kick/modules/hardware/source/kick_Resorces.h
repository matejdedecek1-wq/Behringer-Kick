/*
 * Copyright (c) 2021 MUSIC Tribe
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
#include <string>
#if( JUCE_LINUX || __linux__ )
    #include <iostream>
    #include <sys/sysinfo.h>
    #include <chrono>
    #include <cstdio>
    #include <cstring>
    #include <thread>
    #include <fstream>
    #include <sstream>
    #include <unistd.h>
#endif
namespace mt
{

    struct sysSizeInfo
    {
        bool   status;
        double avaSize;
        double usedSize;
        double totalSize;
        int    perSize;
    };
    struct sysRamInfo
    {
        double totalRam;
        double usedRam;
        double freeRam;
        int    perRam;
    };
    class SystemResource
    {
    public:
        SystemResource() = default;

        void                   ramInfo( sysRamInfo& info );
        void                   sizeInfo( std::string& path, sysSizeInfo& info );
        void                   cpuInfo( double& cpuUsage );
        void                   folderSize( std::string folderPath );
        static SystemResource& getResourceInterface()
        {
            static SystemResource _sysResource;
            return _sysResource;
        }
    };
}
