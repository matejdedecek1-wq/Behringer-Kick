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

#include "audio_voice.h"

namespace mt
{
    /// @brief
    KeygroupProgram::KeygroupProgram( AudIO::Design& D, const juce::String& Name, MTIProgram* PKeys )
        : ISynthObject( D, POLYPHONY::MAX )
    {
        setName( Name.toStdString() );
        setProgram( PKeys );

        // Pre-reseve Memory...
        for( auto i = 0; i < getPolyphony(); ++i )
        {
            voices.emplace_back( std::make_unique<AVoice>( master ) );
        }
        ISynthObject::addVoices( voices );
    }

    KeygroupProgram::~KeygroupProgram()
    {
        setBypass( true );
    }

    void KeygroupProgram::setProgram( MTIProgram* PKeys )
    {
        // call base class method...
        ISynthObject::setProgram( PKeys );

        noteOffAll();
        pKeys = dynamic_cast<MTKeygroupProgram*>( PKeys );
        setBypass( ( pKeys == nullptr ) );
    }

    void KeygroupProgram::noteOn( int Note, int Velocity )
    {
        if( !pKeys || !CommonUtils::isValidMidiNote( Note ) )
            return;

        if( pKeys->isNoteEffectivelyMuted( Note ) ) return;

        //if( !pKeys->isSourceAssigned( Note ) ) return;

        return;
        /*
        //DBG( "AA" << Note << " " << Velocity );
        auto vt = pKeys->getSampleVT( Note );
        if( vt.isValid() )
        {
            // No polyphony on the same note number...
            auto voice = isNotePlaying( Note );
            if( nullptr == voice )
                voice = findFreeVoice( Note );

            if( nullptr != voice )
            {
                auto noteData = pKeys->getSampleForNote( Note );

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
                    SynthVoice::Params* VoiceParameters = &voice->p;

                    auto vel  = CommonUtils::midiToGain( Velocity );
                    auto gain = ( noteData.gain * noteData.layer[noteData.idx].gain );
                    gain      = LayerPlayType::VELOCITY == noteData.layerPlay ? ( gain ) : ( gain * vel );

                    VoiceParameters->voice           = sounds[noteData.idx];
                    VoiceParameters->midiNote        = Note;
                    VoiceParameters->rootNote        = noteData.keyNote;
                    VoiceParameters->startSample     = noteData.layer[noteData.idx].offset;
                    VoiceParameters->lengthInSamples = sounds[noteData.idx]->getNumSamplesPerChannel();
                    VoiceParameters->gain            = gain;
                    VoiceParameters->pan             = noteData.pan + noteData.layer[noteData.idx].pan;
                    VoiceParameters->sampleRate      = (float)sampleRate();
                    VoiceParameters->lfo             = noteData.lfo;
                    VoiceParameters->envelope        = noteData.env;

                    VoiceParameters->pitchFactor = AudIO::utils::calculatePitchFactor( noteData.semi, noteData.fine );

                    // Queue this voice for processing...
                    addVoiceForPlayback( voice, Note );
                    //if( added ) DBG( "NoteAdded" );
                }

                // Simultaneous plays...
                for( auto i = 0; i < MAX_SIMUL_ACTIONS; ++i )
                {
                    if( -1 != noteData.sPlay[i] )
                        noteOn( noteData.sPlay[i], Velocity );
                    if( -1 != noteData.sMute[i] )
                        noteMute( noteData.sMute[i], true );
                }
            }
        }*/
    }

};
