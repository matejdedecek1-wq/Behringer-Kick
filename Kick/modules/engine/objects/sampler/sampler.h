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
    class Sampler : public AudIO::Object
    {
    public:
        enum
        {
            kParamMax,
        };

        Sampler();
        virtual ~Sampler(){};

        void load( const std::string& file )
        {
            voice.addIfNotAlreadyThere( AudioFileManager::Instance().GetOrLoad( file ) );
        }

        void process( AudIO::Buffer<float>& buffer ) override
        {
            /*
            auto ch0 = buffer.getOutputBuffer( 0 );
            auto ch1 = buffer.getOutputBuffer( 1 );

            auto maxSamples = voice->getNumSamplesPerChannel();
            if( readIndex > maxSamples ){
                for( uint32_t sample = 0; sample < _blockSize; ++sample ){
                    if( ch0 ) ch0->writeSample( sample, 0.f );
                    if( ch1 ) ch1->writeSample( sample, 0.f );
                }
                //stop();
                readIndex = 0;
            } else{
                for( uint32_t sample = 0; sample < _blockSize; ++sample ){
                    if( readIndex < maxSamples ){
                        if( ch0 ) ch0->writeSample( sample, (voice->samples[0][readIndex]) );
                        if( ch1 ) ch1->writeSample( sample, (voice->samples[1][readIndex]) );
                    }
                    ++readIndex;
                }
            }
            */

            auto addNote = [&]( int idx ) {
                SamplerVoice::Params VoiceParameters;
                VoiceParameters.voice           = voice[idx];
                VoiceParameters.midiNote        = note++;
                VoiceParameters.rootNote        = 72;
                VoiceParameters.startSample     = 0;
                VoiceParameters.lengthInSamples = voice[idx]->getNumSamplesPerChannel();
                VoiceParameters.gain            = 0.75;
                VoiceParameters.pan             = 0.0;
                VoiceParameters.sampleRate      = (float)_sampleRate;

                array.add( new SamplerVoice( VoiceParameters ) );
                //if (added) DBG("NoteAdded");
                if( note > 72 )
                    note = 72;
            };

            buffer.clearOutputs();
            if( 0 == array.size() && 0 != voice.size() )
            {
                if( 1 == voice.size() )
                    addNote( 0 );
                else
                {
                    auto r = juce::Random().nextInt( voice.size() );
                    for( auto v = 0; v < r; ++v )
                    {
                        auto idx = juce::Random().nextInt( ( voice.size() - 1 ) );
                        addNote( idx );
                    }
                }
            }
            //if( 0 != array.size() ){
            // Process...
            {
                for( auto i = 0; i < array.size(); ++i )
                {
                    auto sv = array[i];
                    sv->process( buffer, _blockSize );
                    if( sv->isFinished )
                    {
                        array.removeObject( sv );
                    }
                }
            }
        };

    private:
        unsigned int                   readIndex{ 0 };
        juce::Array<AFile::Ptr>        voice;
        juce::OwnedArray<SamplerVoice> array;
        int                            note{ 72 };
    };
    REGISTER_OBJECT( "AudIOAudioSampler", Sampler );
};
