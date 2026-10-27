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

namespace mt
{
    /// @brief
    KeygroupSampler::KeygroupSampler( MTIProgram* Prog )
        : KickVoiceSampler( MAX_POLYPHONY_PER_OBJECT )
        , prog( dynamic_cast<MTKeygroupProgram*>( Prog ) )
    {
        setBypass( ( prog == nullptr ) );
    }

    KeygroupSampler::~KeygroupSampler()
    {
        setBypass( true );
    }

    void KeygroupSampler::setProgram( MTIProgram* Prog )
    {
        prog = dynamic_cast<MTKeygroupProgram*>( Prog );

        stopAllNotes();
        setBypass( ( prog == nullptr ) );
    }

    void KeygroupSampler::addNote( int Note, int Velocity )
    {
        if( !prog ) return;
        if( !CommonUtils::isValidMidiNote( Note ) ) return;
        if( !prog->isSourceAssigned( Note ) ) return;

        //DBG( "AA" << Note << " " << Velocity );
        auto vt = prog->getSample( Note );
        if( vt.isValid() )
        {
            // No polyphony on the same note number...
            auto idx   = isVoicePlaying( Note );
            bool reset = ( 0 <= idx );

            // If, `Note` is not playing already, find a free voice.
            if( !reset )
                idx = findFreeVoice( Note );

            if( juce::isPositiveAndBelow( idx, MAX_POLYPHONY_PER_OBJECT ) )
            {
                auto noteData = prog->getSampleForNote( Note );

                std::vector<AFile::Ptr> sounds;
                std::vector<int>        validLayers;
                for( auto i = 0; i < MAX_LAYERS; ++i )
                {
                    auto vPtr = AudioFileManager::Instance().GetOrLoad( noteData.layer[i].source );
                    sounds.push_back( vPtr );
                    if( vPtr )
                    {
                        validLayers.push_back( i );
                    }
                } // for

                switch( noteData.layerPlay )
                {
                    break;
                    case LayerPlayType::RANDOM:
                        noteData.idx = AudIO::utils::random<int>( 0, ( (int)validLayers.size() - 1 ) );
                        break;
                    case LayerPlayType::CYCLE:
                    {
                        auto loc = std::find( begin( validLayers ), end( validLayers ), noteData.idx );
                        if( ++loc != validLayers.end() ) noteData.idx = *loc;
                        else noteData.idx = validLayers[0];
                    }
                    break;
                    case LayerPlayType::VELOCITY:
                    {
                        noteData.idx = -1;
                        for( auto i = 0; i < validLayers.size(); ++i )
                        {
                            auto l = validLayers[i];
                            if( noteData.layer[l].velocityStart <= Velocity && Velocity < noteData.layer[l].velocityEnd )
                            {
                                noteData.idx = l;
                                break;
                            }
                        }
                    }
                }; // switch

                if( ( -1 < noteData.idx && noteData.idx < MAX_LAYERS ) && ( sounds[noteData.idx] ) )
                {
                    auto                  voice           = getVoiceAtIndex( idx );
                    SamplerVoice::Params* VoiceParameters = &voice->p;

                    auto vel  = CommonUtils::midiToGain( Velocity );
                    auto gain = ( noteData.gain * noteData.layer[noteData.idx].gain );
                    gain      = VELOCITY == noteData.layerPlay ? ( gain ) : ( gain * vel );

                    VoiceParameters->voice           = sounds[noteData.idx];
                    VoiceParameters->midiNote        = Note;
                    VoiceParameters->rootNote        = noteData.keyNote;
                    VoiceParameters->startSample     = noteData.layer[noteData.idx].offset;
                    VoiceParameters->lengthInSamples = sounds[noteData.idx]->getNumSamplesPerChannel();
                    VoiceParameters->gain            = noteData.gain * noteData.layer[noteData.idx].gain;
                    VoiceParameters->pan             = noteData.pan + noteData.layer[noteData.idx].pan;
                    VoiceParameters->sampleRate      = (float)_sampleRate;
                    VoiceParameters->lfo             = noteData.lfo;
                    VoiceParameters->envelope        = noteData.env;

                    VoiceParameters->pitchFactor = calculatePitchFactor( noteData.semi, noteData.fine );

                    // Queue this voice for processing...
                    addVoiceForPlayback( idx, Note );
                    //if( added ) DBG( "NoteAdded" );
                }

                // Simultaneous plays...
                for( auto i = 0; i < MAX_SIMUL_ACTIONS; ++i )
                {
                    if( -1 != noteData.sPlay[i] )
                        addNote( noteData.sPlay[i], Velocity );
                    if( -1 != noteData.sMute[i] )
                        muteNote( noteData.sMute[i], true );
                }
            }
        }
    }

};
