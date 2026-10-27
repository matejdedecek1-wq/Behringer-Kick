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

#include "dywapitchtrack.h"
#include "src/dywapitchtrack.h"
#include "src/dywapitchtrack.cpp"

namespace PitchTracking
{
    double detect( const float* Samples, int NumSamples, float SampleRate )
    {
        dywapitchtracker pitchTracker;
        pitchTracker.init( SampleRate );

        //const auto numSamplesPerLoop = pitchTracker.neededSampleCount( 60 );
        //const auto numSamplesPerLoop = 2048;
        const auto numSamplesPerLoop = 8192;

        juce::Array<double> pitches;
        auto                startSample = 0;
        while( startSample < NumSamples )
        {
            auto samplesRemaining   = ( NumSamples - startSample );
            auto numSamplesThisTime = samplesRemaining > numSamplesPerLoop ? numSamplesPerLoop : samplesRemaining;
            if( 16 > numSamplesThisTime )
                break;

            //auto found = dywapitch_computepitch( &pitchTracker, Samples, startSample, numSamplesThisTime );
            auto thePitch = pitchTracker.computePitch( Samples, startSample, numSamplesThisTime );
            startSample += numSamplesThisTime;

            if( ( 0.0 < thePitch ) && ( 0 < pitchTracker.getPitchConfidence() ) )
                pitches.add( thePitch );
        }

        if( !pitches.isEmpty() )
        {
            pitches.sort();
            double avgPitFreq = 0.0;
            for( auto& it : pitches )
            {
                avgPitFreq += it;
                DBG( "Pitch: " << it );
            }
            avgPitFreq /= pitches.size();

            auto midPitFreq = pitches[pitches.size() / 2];
            DBG( "Pitch Avg: " << avgPitFreq << "Pitch Middle: " << midPitFreq << " Note Avg: " << pitchTracker.getMidiNoteNameFromFreq( avgPitFreq ) << ", Note Middle: " << pitchTracker.getMidiNoteNameFromFreq( midPitFreq ) );
            return midPitFreq;
        }
        return 0.0;
    }

    static double detectPartial( const juce::AudioSampleBuffer& Buffer, int StartSample, int NumSamples, double SampleRate )
    {
        /*
        juce::Array<float> samples;
        samples.ensureStorageAllocated( NumSamples );

        auto numChans = Buffer.getNumChannels();
        auto readers  = Buffer.getArrayOfReadPointers();
        for( int i = 0; i < NumSamples; i++ )
        {
            float value0 = ( 1 == numChans ) ? readers[0][StartSample + i] : ( readers[0][StartSample + i] + readers[1][StartSample + i] / 2.f );
            samples.set( i, value0 );
        }

        dywapitchtracker dywaPitchTracker;
        dywapitch_inittracking( &dywaPitchTracker );
        auto       pitchResult     = dywapitch_computepitch( &dywaPitchTracker, (const float*)samples.getRawDataPointer(), 0, NumSamples );
        const auto sampleRateRatio = ( SampleRate / 44100.0 );
        return pitchResult * sampleRateRatio;
        */
    }

    double detect( const juce::AudioSampleBuffer& Buffer, int StartSample, int NumSamples, double SampleRate )
    {
        /*
        auto                startSample       = 0;
        const auto          sampleRateRatio   = ( SampleRate / 44100.0 );
        const auto          numSamplesPerLoop = dywapitch_neededsamplecount( (int)( 50 / sampleRateRatio ) );
        juce::Array<double> pitchFound;
        while( ( startSample + numSamplesPerLoop ) < NumSamples )
        {
            juce::AudioSampleBuffer workingBuffer( (float* const*)Buffer.getArrayOfReadPointers(), Buffer.getNumChannels(), ( StartSample + numSamplesPerLoop ), numSamplesPerLoop );
            auto                    found = detectPartial( workingBuffer, 0, numSamplesPerLoop, SampleRate );
            startSample += numSamplesPerLoop / 2;
            pitchFound.add( found * sampleRateRatio );
        }

        if( !pitchFound.isEmpty() )
        {
            pitchFound.sort();
            return pitchFound[pitchFound.size() / 2];
        }
        */
        return 0.0;
    }
}
