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

// module include files...
#include "pitchShifter.h"
#include "smbPitchShift/smbPitchShift.h"

namespace mt
{
    /// @brief
    PitchSampler::PitchSampler()
        : AudIO::Object( 0, 2, kParamMax )
    {
        pShift[0].reset( new CSmbPitchShift() );
        pShift[1].reset( new CSmbPitchShift() );
    }
    PitchSampler::~PitchSampler() {}

    void PitchSampler::setParameters( const Params& P )
    {
        auto semi = []( int Semi ) -> float { return (float)std::pow( 2.f, Semi / 12.f ); };
        auto cent = []( int Cents ) -> float { return (float)std::pow( 2.f, Cents / 1200.f ); };

        pitchFactor = semi( P.semitones ) * cent( P.cents );
    }

    void PitchSampler::prepareToPlay( uint32_t samplerate, uint32_t blocksize )
    {
        AudIO::Object::prepareToPlay( samplerate, blocksize );
        pShift[0]->reset();
        pShift[1]->reset();
    }

    void PitchSampler::process( AudIO::Buffer<float>& buffer )
    {
        if( AudIO::utils::isEqual<float>( 1.0f, pitchFactor ) )
        {
            buffer.copyInputsToOutputs();
        }
        else
        {
            buffer.clearOutputs();
            if( pShift[0] ) pShift[0]->smbPitchShift( 0.5f, _blockSize, 256, 32, (float)_sampleRate, (float*)buffer.getInputBuffer( 0 ), buffer.getOutputBuffer( 0 ) );
            if( pShift[1] ) pShift[1]->smbPitchShift( 0.5f, _blockSize, 256, 32, (float)_sampleRate, (float*)buffer.getInputBuffer( 1 ), buffer.getOutputBuffer( 1 ) );
        }
    }
};
