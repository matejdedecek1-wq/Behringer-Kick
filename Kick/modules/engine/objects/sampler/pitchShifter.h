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


class CSmbPitchShift;

namespace mt {

    class PitchSampler : public AudIO::Object {
    public:
        enum {
            kParamMax,
        };

        struct Params {
            int semitones{ 0 };
            int cents{ 0 };
        };

        PitchSampler();
        virtual ~PitchSampler();

        void setParameters( const Params& P );

    private:

        void prepareToPlay( uint32_t samplerate, uint32_t blocksize ) override;
        void process( AudIO::Buffer<float>& buffer ) override;

        std::unique_ptr<CSmbPitchShift> pShift[2];
        float pitchFactor{ 1.0f };
    };

    REGISTER_OBJECT( "AudIOPitchShifter", PitchSampler );
};
