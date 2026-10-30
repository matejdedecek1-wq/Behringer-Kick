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

#include "kick_HardwareProbe.h"

namespace mt::hw
{
    namespace
    {
        constexpr Feature allFeatures[] = { Feature::wifi, Feature::ethernet, Feature::cv, Feature::update, Feature::clock, Feature::resources };

        /// KICK_HARDWARE, parsed once. isSet false: the variable is unset, use the probe.
        struct Override
        {
            bool              isSet{ false };
            juce::StringArray enabled;
        };

        const Override& getOverride()
        {
            static const Override o = [] {
                Override   r;
                const auto spec = juce::SystemStats::getEnvironmentVariable( "KICK_HARDWARE", {} ).trim();
                if( spec.isEmpty() )
                    return r;

                r.isSet = true;
                if( spec != "none" )
                {
                    r.enabled.addTokens( spec, ", ", {} );
                    r.enabled.trim();
                    r.enabled.removeEmptyStrings();
                    for( const auto& name : r.enabled )
                    {
                        bool known = false;
                        for( auto f : allFeatures )
                            known = known || name == getName( f );
                        if( !known )
                            juce::Logger::writeToLog( "Hardware: KICK_HARDWARE names an unknown feature '" + name + "'" );
                    }
                }
                juce::StringArray on, off;
                for( auto f : allFeatures )
                    ( r.enabled.contains( getName( f ) ) ? on : off ).add( getName( f ) );
                juce::Logger::writeToLog( "Hardware: KICK_HARDWARE=" + spec + ": available [" + on.joinIntoString( "," ) + "], unavailable [" + off.joinIntoString( "," ) + "]" );
                return r;
            }();
            return o;
        }

#if( JUCE_LINUX || __linux__ )
        bool isOnPath( const juce::String& Program )
        {
            juce::StringArray dirs;
            dirs.addTokens( juce::SystemStats::getEnvironmentVariable( "PATH", "/usr/sbin:/usr/bin:/sbin:/bin" ), ":", {} );
            for( const auto& d : dirs )
                if( d.isNotEmpty() && juce::File( d ).getChildFile( Program ).existsAsFile() )
                    return true;
            return false;
        }
#endif

        bool probe( Feature F )
        {
#if( JUCE_LINUX || __linux__ )
            switch( F )
            {
                case Feature::wifi:      return juce::File( "/sys/class/net/wlan0" ).exists();
                case Feature::ethernet:  return juce::File( "/sys/class/net/eth0" ).exists();
                case Feature::cv:        return juce::File( "/dev/i2c-3" ).exists();
                case Feature::update:    return isOnPath( "swupdate" );
                case Feature::clock:     return true;
                case Feature::resources: return true;
            }
            return false;
#else
            juce::ignoreUnused( F );
            return false;
#endif
        }
    } // namespace

    juce::String getName( Feature F )
    {
        switch( F )
        {
            case Feature::wifi:      return "wifi";
            case Feature::ethernet:  return "ethernet";
            case Feature::cv:        return "cv";
            case Feature::update:    return "update";
            case Feature::clock:     return "clock";
            case Feature::resources: return "resources";
        }
        return "unknown";
    }

    bool isAvailable( Feature F )
    {
        const auto& o = getOverride();
        if( o.isSet )
            return o.enabled.contains( getName( F ) );
        return probe( F );
    }
} // namespace mt::hw
