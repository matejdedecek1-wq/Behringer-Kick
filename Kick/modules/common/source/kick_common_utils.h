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

template<typename EnumType>
constexpr decltype( auto ) toInt( EnumType enumValue )
{
    static_assert( std::is_enum<EnumType>::value, "Enum type required" );
    using EnumValueType = std::underlying_type_t<EnumType>;
    return static_cast<EnumValueType>( enumValue );
}

template<typename EnumType, typename IntegralType = int>
constexpr EnumType toEnum( IntegralType value )
{
    static_assert( std::is_enum<EnumType>::value, "Enum type required" );
    static_assert( std::is_integral<IntegralType>::value, "Integer required" );
    return static_cast<EnumType>( value );
}

class CommonUtils
{
public:
#define ID_LENGTH ( 8 )

    static juce::String createID() noexcept;

    static bool isValidMidiNote( int Note ) { return ( 0 <= Note && Note <= 127 ); }
    static bool isInValidMidiNote( int Note ) { return !isValidMidiNote( Note ); }

    static float dbToGain( float db ) noexcept;
    static float gainToDb( float gain ) noexcept;

    static float midiToDb( int Midivalue ) noexcept;
    static int   dbToMidi( float db ) noexcept;

    static float midiToGain( int Midivalue ) noexcept; // Midi 0-127
    static int   gainToMidi( float Gain ) noexcept; // Gain 0.0-1.0

    static int getDefaultMuteGroupForClipProgram( int Note ) noexcept;

    // For Each; Utility functions...
    template<typename Type, typename UnaryFunction>
    static inline void forEachItem( const std::list<Type>& items, const UnaryFunction& fn )
    {
        for( auto f : items ) fn( f );
    }

    template<typename Type, typename UnaryFunction>
    static inline void forEachItem( const std::vector<Type>& items, const UnaryFunction& fn )
    {
        for( auto f : items ) fn( f );
    }

    // math
    static float getPixelFromDb( const juce::NormalisableRange<double>& norRange, float displayRange, float dbValue, bool vertical = false );
    static float getDbFromPixel( const juce::NormalisableRange<double>& norRange, float displayRange, float pixel, bool vertical = false );

    template<typename T>
    static T convertToPrecision( T Value, int precision = 3 )
    {
        T                 tValue;
        std::stringstream o;
        o << std::setprecision( precision ) << std::fixed << Value;
        tValue = (T)strtod( o.str().c_str(), nullptr );
        return tValue;
    }

    // time signature
    static std::vector<juce::String> getTimeDivAsString();
    static std::vector<juce::String> getDefaultTimeDivisions();
    static std::vector<juce::String> getTripletTimeDivisions();
    static bool                      isValidTimeSig( int num, int den );
    static int                       timeDivStrToQl( const juce::String& timeDiv );

    static juce::String getQLinkMode( const juce::ValueTree& vt );
    static void         setQLinkMode( juce::ValueTree& vt, const juce::String& value );
};
