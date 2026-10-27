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
    static juce::AudioSampleBuffer mutedSample{ 2, 1 };

    ///
    /// ASound ASliceRenderer
    ///

    ASliceRenderer::ASliceRenderer()
    {
        mutedSample.clear();
        setupAdvanceFunction();
    }

    void ASliceRenderer::setSlice( ASoundSlice* Slice )
    {
        finished.store( true );
        slice = Slice;
        reset();
    }

    void ASliceRenderer::reset() noexcept
    {
        inL = inR = nullptr;
        if( slice && slice->source )
        {
            inL = (float*)slice->source->getData().getReadPointer( 0 );
            inR = (float*)slice->source->getData().getReadPointer( 1 );
        }
        setupAdvanceFunction();

        curPos = getStartPosition();
        setMute( false );
        start();
    }

    void ASliceRenderer::start() noexcept
    {
        finished.store( ( slice && inL && inR ) ? false : true );
    }

    void ASliceRenderer::setPosition( double Position )
    {
        if( !slice )
        {
            curPos = 0.0;
            return;
        }

        const auto loopStart = static_cast<double>( slice->loopStart() );
        const auto loopEnd   = static_cast<double>( slice->loopEnd() );
        const auto isForward = slice->isForward();
        const auto isLooping = slice->isLooping();

        if( !isLooping )
        {
            auto end = isForward ? ( Position > loopEnd ) : ( Position < loopStart );
            if( !finished.load() )
                finished.store( end );
            curPos = std::clamp( Position, loopStart, loopEnd );
        }
        else
        {
            if( isForward )
                curPos = Position <= loopEnd ? Position : ( loopStart + ( Position - loopEnd ) );
            else
                curPos = Position >= loopStart ? Position : ( loopEnd - ( loopStart - Position ) );
        }
    }

    void ASliceRenderer::advance( double PitchRatio, int NumSamples )
    {
        auto newPos = curPos + ( NumSamples * PitchRatio );
        setPosition( newPos );
    }

    juce::AudioSampleBuffer& ASliceRenderer::getNext( double PitchRatio )
    {
        advanceOne( PitchRatio );

        const auto muted = finished.load() || isMuted() || slice->isMuted();
        return muted ? mutedSample : interpolateSample();
    }

    void ASliceRenderer::getNextBlock( juce::AudioSampleBuffer& Block, double PitchRatio )
    {
        if( finished.load() )
            return;

        const auto numSamples = Block.getNumSamples();
        const auto muted      = isMuted() || slice->isMuted();
        if( muted )
        {
            advance( PitchRatio, numSamples );
            Block.clear();
            return;
        }

        auto out0 = Block.getWritePointer( 0 );
        auto out1 = Block.getWritePointer( 1 );

        if( out1 )
            for( auto i = 0; i < numSamples; ++i )
            {
                advanceOne( PitchRatio );
                auto& sample = finished.load() ? mutedSample : interpolateSample();
                // Copy to Bock.
                *out0++ = sample.getSample( 0, 0 );
                *out1++ = sample.getSample( 1, 0 );
            }
        else
            for( auto i = 0; i < numSamples; ++i )
            {
                advanceOne( PitchRatio );
                auto& sample = finished.load() ? mutedSample : interpolateSample();
                // Copy to Bock.
                *out0++ = sample.getSample( 0, 0 );
            }
    }

    void ASliceRenderer::setupAdvanceFunction()
    {
        auto moveForward = [&]( double PitchRatio ) {
            advanceForward( PitchRatio );
        };

        auto moveForwardLooping = [&]( double PitchRatio ) {
            advanceForwardLooping( PitchRatio );
        };

        auto moveReverse = [&]( double PitchRatio ) {
            advanceReverse( PitchRatio );
        };

        auto moveReverseLooping = [&]( double PitchRatio ) {
            advanceReverseLooping( PitchRatio );
        };

        if( slice )
        {
            if( slice->isForward() )
            {
                if( slice->isLooping() )
                    advanceOne = moveForwardLooping;
                else
                    advanceOne = moveForward;
            }
            else
            {
                if( slice->isLooping() )
                    advanceOne = moveReverseLooping;
                else
                    advanceOne = moveReverse;
            }
        }
        else
            advanceOne = moveForward;
    }

    ///##
    ///## Implementation: Advance One
    ///##

#define CALC_DIR_CONSTANTS( PitchRatio )                                                 \
    const auto newPos    = curPos + ( slice->getEffectiveResamplerRate() * PitchRatio ); \
    const auto loopEnd   = slice->loopEnd() - 1;                                         \
    const auto loopStart = slice->loopStart();

#define CALC_FORWARD_INTERPOLATOR_COORDINATES()                               \
    {                                                                         \
        const int iPos = static_cast<int>( curPos );                          \
        alpha          = curPos - iPos;                                       \
        m0             = ( iPos < loopEnd ) ? iPos : loopEnd;                 \
        m1             = ( ( iPos + 1 ) < loopEnd ) ? ( iPos + 1 ) : loopEnd; \
    }

#define CALC_REVERSE_INTERPOLATOR_COORDINATES()                                   \
    {                                                                             \
        const int iPos = static_cast<int>( curPos );                              \
        alpha          = curPos - iPos;                                           \
        m0             = ( iPos > loopStart ) ? iPos : loopStart;                 \
        m1             = ( ( iPos - 1 ) > loopStart ) ? ( iPos - 1 ) : loopStart; \
    }

    void ASliceRenderer::advanceForward( double PitchRatio )
    {
        CALC_DIR_CONSTANTS( PitchRatio );

        const auto outOfBounds = ( newPos > loopEnd );
        // Has Finished?
        finished.store( outOfBounds ? true : finished.load() );
        // Update position
        curPos = outOfBounds ? loopEnd : newPos;
        // Compute interpolater co-ordinates.
        CALC_FORWARD_INTERPOLATOR_COORDINATES();
    }

    void ASliceRenderer::advanceForwardLooping( double PitchRatio )
    {
        CALC_DIR_CONSTANTS( PitchRatio );

        const auto outOfBounds = ( newPos > loopEnd );
        // Update position considering looping
        curPos = outOfBounds ? ( loopStart + ( newPos - loopEnd ) ) : newPos;
        // Compute interpolater co-ordinates.
        CALC_FORWARD_INTERPOLATOR_COORDINATES();
    }

    void ASliceRenderer::advanceReverse( double PitchRatio )
    {
        CALC_DIR_CONSTANTS( PitchRatio );

        const auto outOfBounds = ( newPos < loopStart );
        // Has Finished?
        finished.store( outOfBounds ? true : finished.load() );
        // Update position
        curPos = outOfBounds ? loopStart : newPos;
        // Compute interpolater co-ordinates.
        CALC_REVERSE_INTERPOLATOR_COORDINATES();
    }

    void ASliceRenderer::advanceReverseLooping( double PitchRatio )
    {
        CALC_DIR_CONSTANTS( PitchRatio );

        const auto outOfBounds = ( newPos < loopStart );
        // Update position considering looping
        curPos = ( newPos < loopStart ) ? ( loopEnd - ( loopStart - newPos ) ) : newPos;
        // Compute interpolater co-ordinates.
        CALC_REVERSE_INTERPOLATOR_COORDINATES();
    }

#undef CALC_DIR_CONSTANTS
#undef CALC_FORWARD_INTERPOLATOR_COORDINATES
#undef CALC_REVERSE_INTERPOLATOR_COORDINATES

} // namespace mt
