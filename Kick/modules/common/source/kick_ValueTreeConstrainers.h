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

#include "kick_Constants.h"

///
/// Variant Converters
///

namespace juce
{
    template<>
    struct VariantConverter<Colour>
    {
        static Colour fromVar( const var& v ) { return Colour::fromString( v.toString() ); }
        static var    toVar( const Colour& c ) { return c.toString(); }
    };
}

///
/// Value Constrainer...
///

template<typename Type, int StartValue, int EndValue>
struct RangeConstrainer
{
    static Type constrain( const Type& v )
    {
        return juce::Range( (Type)StartValue, (Type)EndValue ).clipValue( v );
    }
};

///
/// Constrainer Wrapper
///

template<typename Type, typename Constrainer>
struct ConstrainerWrapper
{
    ConstrainerWrapper() = default;

    template<typename OtherType>
    ConstrainerWrapper( const OtherType& other )
    {
        value = Constrainer::constrain( other );
    }

    ConstrainerWrapper& operator=( const ConstrainerWrapper& other ) noexcept
    {
        value = Constrainer::constrain( other.value );
        return *this;
    }

    bool operator==( const ConstrainerWrapper& other ) const { return value == other.value; }
    bool operator!=( const ConstrainerWrapper& other ) const { return !operator==( other ); }

    operator juce::var() const noexcept { return ( value ); }
    operator Type() const noexcept { return ( value ); }

    Type value = Type();
};

///
/// Various Constrainers.
///
using MidiConstrainer          = ConstrainerWrapper<int, RangeConstrainer<int, MIDI::MIN, MIDI::MAX>>;
using MidiChannelConstrainer   = ConstrainerWrapper<int, RangeConstrainer<int, MIDI_CH::MIN, MIDI_CH::MAX>>;
using MidiTransposeConstrainer = ConstrainerWrapper<int, RangeConstrainer<int, TRANSPOSE::MIN, TRANSPOSE::MAX>>;
using LengthConstrainer        = ConstrainerWrapper<int, RangeConstrainer<int, BAR::MIN, BAR::MAX>>;
using SemiConstrainer          = ConstrainerWrapper<int, RangeConstrainer<int, SEMI::MIN, SEMI::MAX>>;
using FineConstrainer          = ConstrainerWrapper<int, RangeConstrainer<int, FINE::MIN, FINE::MAX>>;
using PolyphonyConstrainer     = ConstrainerWrapper<int, RangeConstrainer<int, POLYPHONY::MIN, POLYPHONY::MAX>>;
using MuteGroupConstrainer     = ConstrainerWrapper<int, RangeConstrainer<int, MUTEGROUP::MIN, MUTEGROUP::MAX>>;
using RatchetConstrainer       = ConstrainerWrapper<int, RangeConstrainer<int, RATCHET::MIN, RATCHET::MAX>>;
using BPMConstrainer           = ConstrainerWrapper<double, RangeConstrainer<double, (int)BPM::MIN, (int)BPM::MAX>>;
using GainConstrainer          = ConstrainerWrapper<double, RangeConstrainer<double, (int)GAIN::MIN, (int)GAIN::MAX>>;
using PanConstrainer           = ConstrainerWrapper<double, RangeConstrainer<double, (int)PAN::MIN, (int)PAN::MAX>>;
