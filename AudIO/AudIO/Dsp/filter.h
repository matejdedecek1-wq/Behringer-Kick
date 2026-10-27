/*
 * Copyright (c) 2022 MUSIC Tribe
 *
 * This file is part of AudIO.
 *
 * AudIO is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Affero General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * AudIO is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with AudIO. If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#pragma once
#ifndef AUDIO_FILTER_H_INCLUDE
    #define AUDIO_FILTER_H_INCLUDE

    #include <cmath>

    #include "../Includes/idsp.h"
    #include "../Includes/Utils.h"

namespace AudIO
{
    template<typename T>
    class Filter : public IDSP<T>
    {
    public:
        enum class Type
        { //! Filter types...
            LOW_PASS,
            HIGH_PASS,
            BAND_PASS,
        };

        enum class Slope
        { //! Filter slope...
            _12DB,
            _24DB,
        };

        struct Params
        { //! Filter parameters...
            double cutoff;
            double resonance;
            double depth;
            Type   type;
        };

        Filter( Type Typ = Type::LOW_PASS ) noexcept
            : type_( Typ ) { recalculate(); };
        Filter( const Params& P ) noexcept { setParameters( P ); }

        Type getType() noexcept { return type_; }
        void setType( Type Typ ) noexcept { type_ = Typ; }

        void setParameters( const Params& P ) noexcept { recalculate(); }

        inline void setCutoff( double newCutoff )
        {
            cutoff_ = utils::clamp<double>( newCutoff, 0.01, 0.99 );
            recalculate();
        };
        inline void setCutoffModulate( double newCutoff )
        {
            cutoffMod_ = utils::clamp<double>( newCutoff, 0.01, 0.99 );
            recalculate();
        };
        inline void setResonance( double newResonance )
        {
            resonance_ = newResonance;
            recalculate();
        };

        void reset() noexcept override {}

        T advance( unsigned int NumSamples ) noexcept override
        {
            for( auto i = ( NumSamples - 1 ); 0 < i; --i )
                nextSample();
            return nextSample();
        }

        T process( T inputValue ) noexcept
        {
            if( this->isBypassed_ || !this->isEnabled_ || utils::isEqual<T>( (T)0.0, inputValue ) )
                return inputValue;

            auto calcCutoff = getCalculatedCutoff();
            buf0_ += calcCutoff * ( inputValue - buf0_ + feedbackAmount_ * ( buf0_ - buf1_ ) );
            buf1_ += calcCutoff * ( buf0_ - buf1_ );
            if( Slope::_12DB == slope_ )
            {
                switch( type_ )
                {
                    case Type::LOW_PASS: return buf1_;
                    case Type::HIGH_PASS: return inputValue - buf1_;
                    case Type::BAND_PASS: return buf0_ - buf1_;
                    default: return 0.0;
                }
            }
            else // if( Slope::_24DB == slope_ ){
            {
                buf2_ += calcCutoff * ( buf1_ - buf2_ );
                buf3_ += calcCutoff * ( buf2_ - buf3_ );
                switch( type_ )
                {
                    case Type::LOW_PASS: return buf3_;
                    case Type::HIGH_PASS: return inputValue - buf3_;
                    case Type::BAND_PASS: return buf0_ - buf3_;
                    default: return 0.0;
                }
            }
        }

        void process( Buffer<float>& Buffer ) noexcept override
        {
            auto bSize  = Buffer.getBlockSize();
            auto nChans = Buffer.getNumInputChannels();

            for( uint32_t channel = 0; channel < nChans; channel++ )
            {
                auto in  = Buffer.getInputBuffer( channel );
                auto out = Buffer.getOutputBuffer( channel );

                if( in && out )
                {
                    for( uint32_t sample = 0; sample < bSize; ++sample )
                        *out++ = process( *in++ );
                }
            }
        }

    private:
        Type  type_{ Type::LOW_PASS };
        Slope slope_{ Slope::_12DB };
        T     cutoff_{ 0.99 }, resonance_{ 0 }, feedbackAmount_, cutoffMod_;
        T     buf0_{ 0 }, buf1_{ 0 }, buf2_{ 0 }, buf3_{ 0 };

        void recalculate() noexcept override
        {
            feedbackAmount_ = resonance_ + resonance_ / ( 1.0 - getCalculatedCutoff() );
        }

        inline double getCalculatedCutoff() const
        {
            return utils::clamp<double>( ( cutoff_ + cutoffMod_ ), 0.01, 0.99 );
        };

        T nextSample() noexcept override {}
        T processNext() noexcept override {}
        T getNext() noexcept override {}
    };

    //
}; // Namespace AudIO
#endif // Header guard
