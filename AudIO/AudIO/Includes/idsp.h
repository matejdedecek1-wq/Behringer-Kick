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
#ifndef AUDIO_IDSP_H_INCLUDE
    #define AUDIO_IDSP_H_INCLUDE

    #include <atomic>
    #include "AudIOBuffer.h"

namespace AudIO
{
    template<typename T>
    class IDSP
    {
    public:
        IDSP()          = default;
        virtual ~IDSP() = default;

        void setEnabled( bool Enabled ) noexcept { isEnabled_ = Enabled; }
        void setBypass( bool Bypass ) noexcept { isBypassed_ = Bypass; }
        void setSampleRate( double SampleRate ) noexcept
        {
            sampleRate_ = static_cast<T>( SampleRate );
            recalculate();
        }

        // Getter functions...
        inline bool isEnabled() const noexcept { return isEnabled_.load(); }
        inline bool isBypassed() const noexcept { return isBypassed_.load(); }
        inline bool skipProcess() const noexcept { return ( !this->isEnabled_ || this->isBypassed_ ); }
        inline T    sampleRate() const noexcept { return sampleRate_; }

        inline T operator()() noexcept { return process(); }
        inline T getNext() noexcept { return process(); }

        //! DSP objects needs to implement these following public methods...
        virtual void reset() noexcept                            = 0;
        virtual T    advance( unsigned int NumSamples ) noexcept = 0;

        virtual T    process() noexcept = 0;
        virtual void process( Buffer<float>& Buffer ) noexcept {};

    protected:
        //! DSP objects needs to implement these following protected method...
        virtual void recalculate() noexcept = 0;

    private:
        std::atomic<bool> isEnabled_{ false };
        std::atomic<bool> isBypassed_{ false };
        std::atomic<T>    sampleRate_{ 0 };
    };
}

#endif // AUDIO_IDSP_H_INCLUDE
