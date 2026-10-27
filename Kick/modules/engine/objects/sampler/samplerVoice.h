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

namespace stftpitchshift
{
    class StftPitchShift;
}
class CSmbPitchShift;

namespace mt
{

#define NUM_TEMP_BUFFERS ( 2 )
    //#define NUM_TEMP_BUFFERS (4)

    struct SamplerVoice
    {
        struct Params
        {
            AFile::Ptr         voice;
            int                midiNote, rootNote;
            unsigned long long startSample{ 0 }, lengthInSamples{ 0 }, blockSize{ 480 };
            double             gain{ 1.0 }, pan{ 0.0 }, sampleRate{ 48000 }, pitchFactor{ 1.0 };
            bool               isMute{ false }, isLooping{ false }, isReverse{ false };
            MTLFOData          lfo;
            MTEnvelopeData     envelope;
        };

        SamplerVoice();
        SamplerVoice( const Params& P );
        ~SamplerVoice();

        void setParameters( const Params& P );
        void refresh();

        void beginLoop();
        void reset();
        void stop();

        void process( AudIO::Buffer<float>& buffer, unsigned int blockSize );

        bool isAnyADSRRunning();

        inline int getNote() { return p.midiNote; }

        inline void setLoop( bool Loop ) { isLooping.store( Loop, std::memory_order_relaxed ); }
        inline void setMute( bool Mute ) { isMuted.store( Mute, std::memory_order_relaxed ); }

        Params p;

        AudIO::Panner<float>       pannedGain;
        double                     playbackRatio{ 1.0 };
        juce::LagrangeInterpolator resampler[2];
        std::atomic<bool>          isFinished{ true }, isMuted{ false }, isLooping{ false };
        unsigned long long         readIndex, endIndex, maxSamples, samplesLeftToPlay = 0;
        int                        sourceChannels{ 2 };
        AudIO::LFO<double>         lfo;
        AudIO::ADSR<double>        amp_adsr;
        AudIO::ADSR<double>        filt_adsr;
        AudIO::ADSR<double>        pitch_adsr;
        AudIO::ADSR<double>        fade_adsr;
        std::vector<float>         tempBuffer[NUM_TEMP_BUFFERS];
        //std::unique_ptr<stftpitchshift::StftPitchShift> pitchShifter[2];
        //std::vector<double> factors;
        std::unique_ptr<CSmbPitchShift> pShift[2];
    };

    double calculatePitchFactor( int semi, int cents );
}
