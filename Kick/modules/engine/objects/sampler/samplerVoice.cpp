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
#include <tuple>

//#include "stftPitchShift/StftPitchShift.h"
#include "smbPitchShift/smbPitchShift.h"

namespace mt
{

    //
    SamplerVoice::SamplerVoice()
    {
    }

    SamplerVoice::SamplerVoice( const Params& P )
    {
        setParameters( P );
    };

    SamplerVoice::~SamplerVoice()
    {
        //DBG( "Destructed..." );
    }

    void SamplerVoice::setParameters( const Params& P )
    {
        p = P;
        refresh();
    }

    void SamplerVoice::refresh()
    {
        pannedGain.setParameters( { p.pan, AudIO::Panner<float>::Type::EQUAL_POWER } );

        // Resize temporary buffer to hold processed values.
        for( auto i = 0; i < NUM_TEMP_BUFFERS; i++ )
            tempBuffer[i].resize( p.blockSize, 0.0 );

        // Reset...
        reset();

        isFinished.store( false );

        // Setup sample rates...
        lfo.setSampleRate( p.sampleRate );
        amp_adsr.setSampleRate( p.sampleRate );
        filt_adsr.setSampleRate( p.sampleRate );
        pitch_adsr.setSampleRate( p.sampleRate );
        fade_adsr.setSampleRate( p.sampleRate );

        // Setup LFO...
        if( p.lfo.isEnabled )
        {
            lfo.setFrequency( p.lfo.frequency, true );
            switch( p.lfo.type )
            {
                case LFOType::SINE:
                    lfo.setType( AudIO::LFOType::SINE );
                    break;
                case LFOType::SQUARE:
                    lfo.setType( AudIO::LFOType::SQUARE );
                    break;
                case LFOType::SAWTOOTH_UP:
                    lfo.setType( AudIO::LFOType::SAW_UP );
                    break;
                case LFOType::SAWTOOTH_DOWN:
                    lfo.setType( AudIO::LFOType::SAW_DOWN );
                    break;
                case LFOType::TRIANGLE:
                    lfo.setType( AudIO::LFOType::TRIANGLE );
                    break;
                case LFOType::NOISE:
                    lfo.setType( AudIO::LFOType::NOISE );
                    break;
            }
            lfo.setEnabled( p.lfo.isEnabled );
        }

        // Setup Envelope...
        if( p.envelope.amp.isEnabled )
        {
            AudIO::ADSR<double>::Params params{ p.envelope.amp.adsr.attackTimeInSeconds, p.envelope.amp.adsr.holdTimeInSeconds,
                                                p.envelope.amp.adsr.decayTimeInSeconds, p.envelope.amp.adsr.sustainLevel, p.envelope.amp.adsr.releaseTimeInSeconds };
            amp_adsr.setParameters( params );
            amp_adsr.keyOn();
            amp_adsr.setEnabled( true );
        }
        if( p.envelope.filter.isEnabled )
        {
            AudIO::ADSR<double>::Params params{ p.envelope.filter.adsr.attackTimeInSeconds, p.envelope.filter.adsr.holdTimeInSeconds,
                                                p.envelope.filter.adsr.decayTimeInSeconds, p.envelope.filter.adsr.sustainLevel, p.envelope.filter.adsr.releaseTimeInSeconds };
            filt_adsr.setParameters( params );
            filt_adsr.keyOn();
            filt_adsr.setEnabled( true );
        }
        if( p.envelope.pitch.isEnabled )
        {
            pitch_adsr.setSampleRate( p.sampleRate );
            AudIO::ADSR<double>::Params params{ p.envelope.pitch.adsr.attackTimeInSeconds, p.envelope.pitch.adsr.holdTimeInSeconds,
                                                p.envelope.pitch.adsr.decayTimeInSeconds, p.envelope.pitch.adsr.sustainLevel, p.envelope.pitch.adsr.releaseTimeInSeconds };
            pitch_adsr.setParameters( params );
            pitch_adsr.keyOn();
            pitch_adsr.setEnabled( true );
        }

        {
            AudIO::ADSR<double>::Params params{ 0.01, 0.0, 0.0, 1.0, 0.01 };
            fade_adsr.setParameters( params );
            fade_adsr.keyOn();
            fade_adsr.setEnabled( true );
        }

        // Setup Voice...
        if( p.midiNote != p.rootNote )
        {
            playbackRatio = juce::MidiMessage::getMidiNoteInHertz( p.midiNote ) / juce::MidiMessage::getMidiNoteInHertz( p.rootNote );
        }
        playbackRatio *= p.voice->getSampleRate() / p.sampleRate;
        sourceChannels = p.voice->getNumChannels();
        maxSamples     = p.voice->getNumSamplesPerChannel();

        beginLoop();
        // To be hooked up at correct place...
        //if( p.lfo.isEnabled ) {
        //    pShift[0].reset( new CSmbPitchShift() );
        //    pShift[1].reset( new CSmbPitchShift() );
        //}

        //DBG( "Note PB:" << playbackRatio << " M:" << juce::String( maxSamples ) << " S:" << juce::String( readIndex )
        //    << " E: " << juce::String( endIndex ) << " SP:" << juce::String( samplesLeftToPlay ) );
    }

    void SamplerVoice::beginLoop()
    {
        samplesLeftToPlay = playbackRatio > 0 ? ( 1 + (int)( p.lengthInSamples / playbackRatio ) ) : 0;

        readIndex = p.startSample;
        endIndex  = std::min( maxSamples, ( readIndex + samplesLeftToPlay ) );
    }

    void SamplerVoice::reset()
    {
        readIndex     = p.startSample;
        playbackRatio = p.pitchFactor;

        resampler[0].reset();
        resampler[1].reset();

        lfo.reset();
        amp_adsr.reset();
        filt_adsr.reset();
        pitch_adsr.reset();

        isMuted.store( p.isMute );

        //auto semi = []() -> double { return std::pow( 2, 0 / 12 ); };
        //auto cent = []() -> double { return std::pow( 2, 0 / 1200 ); };
        //factors.clear();
        //factors.push_back( semi() );
        //factors.push_back( cent() );
        //pitchShifter[0] = std::make_unique<stftpitchshift::StftPitchShift>( (size_t)128, (size_t)4, p.sampleRate );
        //pitchShifter[1] = std::make_unique<stftpitchshift::StftPitchShift>( (size_t)128, (size_t)4, p.sampleRate );
    }

    void SamplerVoice::stop()
    {
        amp_adsr.keyOff();
        filt_adsr.keyOff();
        pitch_adsr.keyOff();
        fade_adsr.keyOff();
        //DBG( "Voice-Finished..." );
    }

    bool SamplerVoice::isAnyADSRRunning()
    {
        return amp_adsr.isActive() || filt_adsr.isActive() || pitch_adsr.isActive() || fade_adsr.isActive();
    }

    void SamplerVoice::process( AudIO::Buffer<float>& buffer, unsigned int blockSize )
    {
        if( isFinished.load() )
            return;

        auto numSamplesToProcess = std::min( blockSize, (unsigned int)samplesLeftToPlay );
        if( isMuted.load() )
        {
            // Update indexes...
            readIndex += numSamplesToProcess;
            samplesLeftToPlay -= numSamplesToProcess;

            if( ( endIndex <= readIndex ) || ( 0 >= samplesLeftToPlay ) )
            {
                isFinished.store( true );
                reset();
            }
        }
        else
        {
            if( p.isLooping && ( 0 >= ( endIndex - readIndex ) ) )
                beginLoop();

            auto numSamplesToRead = endIndex - readIndex;
            if( 0 < numSamplesToRead && 0 < samplesLeftToPlay )
            {
                // Resample...
                auto samplesProcessed  = 0;
                auto channelsToProcess = juce::jmin( sourceChannels, NUM_TEMP_BUFFERS );
                for( auto ch = 0; ch < channelsToProcess; ++ch )
                {
                    std::fill( tempBuffer[ch].begin(), tempBuffer[ch].end(), 0.f );
                    samplesProcessed = resampler[ch].processAdding( playbackRatio, &p.voice->samples[ch][readIndex], &tempBuffer[ch][0],
                                                                    numSamplesToProcess, (int)( numSamplesToRead ), 0, ( ch ? pannedGain.getLeft() : pannedGain.getRight() ) );
                }

                // Duplicate on second channel; if source is mono
                if( 1 == channelsToProcess )
                    std::copy( tempBuffer[0].cbegin(), tempBuffer[0].cend(), tempBuffer[1].begin() );

                // Update indexes...
                readIndex += samplesProcessed;
                samplesLeftToPlay -= numSamplesToProcess;

                if( ( 0 >= samplesLeftToPlay ) || ( endIndex <= readIndex ) )
                {
                    stop();
                }
            }
            else
            {
                if( !isAnyADSRRunning() )
                {
                    isFinished.store( true );
                    reset();
                }
            }

            // Apply transpose...
            // To be hooked up at correct place...
            //pitchShifter[0]->shiftpitch( blockSize, tempBuffer[0].data(), tempBuffer[2].data(), factors );
            //pitchShifter[1]->shiftpitch( blockSize, tempBuffer[1].data(), tempBuffer[3].data(), factors );
            //if( pShift[0] ) pShift[0]->smbPitchShift( 0.5f, blockSize, 256, 32, (float)p.sampleRate, tempBuffer[0].data(), tempBuffer[0].data() );
            //if( pShift[1] ) pShift[1]->smbPitchShift( 0.5f, blockSize, 256, 32, (float)p.sampleRate, tempBuffer[1].data(), tempBuffer[1].data() );

            // Copy processed output to audio buffer...
            auto ch0 = buffer.getOutputBuffer( 0 );
            auto ch1 = buffer.getOutputBuffer( 1 );
            for( unsigned int sample = 0; sample < blockSize; ++sample )
            {
                auto chGain     = p.gain;
                auto sample_pan = p.pan;

                // LFO Modulation...
                if( p.lfo.isEnabled )
                {
                    auto _lfo = lfo.processNext();
                    // Amplitude Modulation...
                    if( p.lfo.modulateAmp )
                    {
                        auto factor = ( 1 + ( _lfo * (float)p.lfo.modulateAmp / 127.0 ) ) / 2;
                        chGain *= (float)factor;
                    }

                    if( p.lfo.modulatePan )
                    {
                        sample_pan += (float)( _lfo * ( (float)p.lfo.modulatePan / 127.0 ) );
                    }
                }

                // Envelopes...
                // Amplitude...
                if( p.envelope.amp.isEnabled )
                {
                    auto ampAdsr = amp_adsr.processNext();
                    auto factor  = ampAdsr * ( (float)p.envelope.amp.velocityMod / 127.0 );

                    if( p.envelope.amp.ampMod )
                    {
                        chGain *= (float)( factor * ( (float)p.envelope.amp.ampMod / 127.0 ) );
                    }
                    if( p.envelope.amp.panMod )
                    {
                        sample_pan *= (float)( factor * ( (float)p.envelope.amp.panMod / 127.0 ) );
                    }
                }

                // Fade-In or Fade-Out
                {
                    chGain *= (float)fade_adsr.processNext();
                }

                // Apply overall pan.
                {
                    AudIO::Panner<float> panner( sample_pan );
                    auto                 s = panner.panSamples( tempBuffer[0][sample], tempBuffer[1][sample], chGain );

                    if( ch0 )
                    {
                        *ch0++ += std::get<0>( s );
                    }
                    if( ch1 )
                    {
                        *ch1++ += std::get<1>( s );
                    }

                    //if( 0 == sample ) DBG( "P" << juce::String( sample_pan ) << " LR:" << juce::String( panner.getLeft() ) << "::" << juce::String( panner.getRight() ) );
                    //DBG( " L:" << juce::String( ch0->readSample( sample ) ) << " R:" << juce::String( ch1->readSample( sample ) ) );
                }
            } // Copy samples to buffer...
        } // If NOT Muted...
    } // Process()

    double calculatePitchFactor( int semi, int cents )
    {
        int cents_ = semi * 100 + cents;
        return pow( 2, ( cents_ / 1200 ) );
    }

}
