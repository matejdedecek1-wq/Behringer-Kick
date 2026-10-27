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
    class ASliceRenderer
    {
    public:
        ASliceRenderer();
        ~ASliceRenderer() = default;

        void         setSlice( ASoundSlice* Slice );
        ASoundSlice* getSlice() noexcept { return slice; }

        // Duration.
        double getLength() { return slice ? slice->getSliceLength() : 0.0; };

        // Mute.
        bool isMuted() const noexcept { return mute.load(); }
        void setMute( bool State ) noexcept { mute.store( State ); }

        // Positional.
        bool hasFinished() const noexcept { return finished.load(); }
        bool isRunning() const noexcept { return !finished.load(); }

        inline double getPosition() { return curPos; }
        void          setPosition( double Position );
        void          advance( double PitchRatio, int NumSamples = 1 );

        // Control
        void reset() noexcept;
        void start() noexcept;
        void stop() noexcept { finished.store( true ); }

        // Samples.
        juce::AudioSampleBuffer& getNext( double PitchRatio = 1.0 );
        void                     getNextBlock( juce::AudioSampleBuffer& Block, double PitchRatio = 1.0 );

    private:
        std::function<void( double )> advanceOne;

        void setupAdvanceFunction();

        void advanceForward( double PitchRatio );
        void advanceForwardLooping( double PitchRatio );
        void advanceReverse( double PitchRatio );
        void advanceReverseLooping( double PitchRatio );

        inline double getStartPosition()
        {
            return !slice ? 0.0 : ( slice->isForward() ? slice->loopStart() : slice->loopEnd() );
        }

        inline juce::AudioSampleBuffer& interpolateSample()
        {
            *pL = slice->gL * linearInterpolator( alpha, inL[m0], inL[m1] );
            *pR = slice->gR * linearInterpolator( alpha, inR[m0], inR[m1] );
            return next;
        }

    private:
        ASoundSlice* slice{ nullptr };

        std::atomic<bool> finished{ true };
        std::atomic<bool> mute{ false };

        juce::AudioSampleBuffer next{ 2, 1 };
        float* const            pL{ next.getWritePointer( 0 ) };
        float* const            pR{ next.getWritePointer( 1 ) };

        float* inL{ nullptr };
        float* inR{ nullptr };

        double curPos{ 0.0 };
        double alpha{ 0 };
        int    m0{ 0 };
        int    m1{ 0 };
    };

} // namespace mt
