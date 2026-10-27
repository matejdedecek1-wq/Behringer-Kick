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

namespace mt
{
    // Time related data types...
    using MTTempo         = AudIO::Tempo;
    using MTTimeSignature = AudIO::TimeSignature;
    using MTTimeBase      = AudIO::TimeBase;
    using MTBarNBeats     = AudIO::BarBeats;

    using MTMeterData  = AudIO::MeterData;
    using MTBus        = AudIO::DawBus;
    using MTInsertInfo = AudIO::ObjectInfo;

    using LFOType = AudIO::LFOType;

    enum ADSRType
    {
        Type_AD,
        Type_ADSR,
        Type_AHD,
        Type_AHDS,
        Type_AHDSR,
    };

    enum EnvelopeType
    {
        AMPLITUDE,
        FILTER,
        PITCH
    };

    constexpr const float OneOverTwo   = (float)( 1.0 / 2.0 );
    constexpr const float OneOverThree = (float)( 1.0 / 3.0 );
    constexpr const float OneOverFour  = (float)( 1.0 / 4.0 );
    constexpr const float OneOverFive  = (float)( 1.0 / 5.0 );

    static float getAttackDecayScaler( ADSRType Type )
    {
        switch( Type )
        {
            case Type_AD: return OneOverTwo;
            case Type_AHD: return OneOverThree;
            case Type_AHDS: // fall-through
            case Type_ADSR: return OneOverFour;
            default: break;
        }
        return OneOverFive; //case AHDSR
    }
    static float getHoldScaler( ADSRType Type )
    {
        switch( Type )
        {
            case Type_AHD: return OneOverThree;
            case Type_AHDS: return OneOverFour;
            default: break;
        }
        return 0.0; //case AD, ADSR,
    }
    static float getReleaseScaler( ADSRType Type )
    {
        switch( Type )
        {
            case Type_ADSR: return OneOverFour;
            case Type_AHDSR: return OneOverFive;
            default: break;
        }
        return 0.0; //case AD, AHD, AHDS
    }

    using MTInsertsMap = std::map<const std::string, MTInsertInfo>;

    constexpr float      minTempo{ MTTempo::minTempo };
    constexpr float      maxTempo{ MTTempo::maxTempo };
    constexpr float      defaultTempo{ MTTempo::defaultTempo };
    constexpr int        defaultLength{ 4 };
    constexpr int        defaultNumerator{ MTTimeSignature::defaultNumerator };
    constexpr int        defaultDenominator{ MTTimeSignature::defaultDenominator };
    constexpr int        defaultPPQN{ MTTimeSignature::defaultPPQN };
    constexpr MTTimeBase defaultTimeBase{ MTTimeSignature::defaultTimebase };
    constexpr MTTimeBase defaultQuantize{ MTTimeSignature::defaultQuantize };

    static float getTimeBaseMultiplier( MTTimeBase Base ) noexcept
    {
        switch( Base )
        {
            case MTTimeBase::_1: return 4.0f;
            case MTTimeBase::_2: return 2.0f;
            case MTTimeBase::_2T: return 1.33333334f;
            case MTTimeBase::_4: return 1.0f;
            case MTTimeBase::_4T: return 0.66666667f;
            case MTTimeBase::_8: return 0.5f;
            case MTTimeBase::_8T: return 0.3333333334f;
            case MTTimeBase::_16: return 0.25f;
            case MTTimeBase::_16T: return 0.1666666667f;
            case MTTimeBase::_32: return 0.125f;
            case MTTimeBase::_32T: return 0.0833333334f;
            case MTTimeBase::_64: return 0.0625f;
            case MTTimeBase::_64T: return 0.0416666667f;
        }
        return 1.f;
    }

    struct MTILoopData
    {
        CachedValue<bool> isLooping;
        CachedValue<int>  loopStart;
        CachedValue<int>  loopEnd;

        explicit MTILoopData( const juce::ValueTree& V, juce::UndoManager* UM = nullptr );
        virtual void forceUpdateOfCachedValues();
    };

    struct MTPlayhead
    {
        std::atomic<unsigned int> loopCount{ 0 };
        double                    normized{ 0.0 }; // Play head position in percentage terms (0.0 - 1.0)
        MTBarNBeats               bnb;

        void clear() noexcept
        {
            bnb.clear();
            loopCount.store( 0 );
            normized = 0.0;
        }

        MTPlayhead& operator=( const MTPlayhead& Oth ) noexcept
        {
            loopCount.store( Oth.loopCount );
            normized = Oth.normized;
            bnb      = Oth.bnb;
            return *this;
        }

        bool operator==( const MTPlayhead& Oth ) noexcept { return normized == Oth.normized && loopCount == Oth.loopCount; }
        bool operator!=( const MTPlayhead& Oth ) noexcept { return !operator==( Oth ); }
    };

} // namespace MT
