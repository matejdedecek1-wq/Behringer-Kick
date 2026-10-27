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
    ClipSampler::ClipSampler( MTIProgram* PClip )
        : KickVoiceSampler( MAX_POLYPHONY_PER_OBJECT )
        , prog( dynamic_cast<MTClipProgram*>( PClip ) )
    {
        setBypass( ( prog == nullptr ) );
    }

    ClipSampler::~ClipSampler()
    {
        setBypass( true );
    }

    void ClipSampler::setProgram( MTIProgram* PClip )
    {
        prog = dynamic_cast<MTClipProgram*>( PClip );

        stopAllNotes();
        setBypass( ( prog == nullptr ) );
    }

    void ClipSampler::addNote( int Note, int Velocity )
    {
        if( !prog ) return;
        if( !CommonUtils::isValidMidiNote( Note ) ) return;
        if( !prog->isSourceAssigned( Note ) ) return;

        //DBG( "AA" << Note << " " << Velocity );
        auto vt = prog->getClip( Note );
        if( vt.isValid() )
        {
            int    _semi    = prog->getSemi();
            int    _fine    = prog->getFine();
            double _gain    = ( (float)prog->getLevel() / 127.f );
            double _pan     = prog->getPan();
            auto   clipNote = prog->toClipData( vt );

            if( isPlaying( Note ) )
            {
                if( CLIP_TOGGLE == clipNote.padPlay )
                {
                    stopNote( Note );
                }

                restartNote( Note );
            }
            else
            {
                auto idx = findFreeVoice( Note );
                if( juce::isPositiveAndBelow( idx, MAX_POLYPHONY_PER_OBJECT ) )
                {
                    // Stop clips in same mute group.
                    auto notes = prog->getNotesInSameMuteGroup( Note );
                    for( auto it : notes )
                        stopNote( it );

                    // Enqueue Note for playback
                    auto                  voice           = getVoiceAtIndex( idx );
                    SamplerVoice::Params* VoiceParameters = &voice->p;

                    auto         sound      = AudioFileManager::Instance().GetOrLoad( clipNote.source );
                    unsigned int numSamples = sound ? (unsigned int)sound->getNumSamplesPerChannel() : 0;

                    VoiceParameters->voice    = AudioFileManager::Instance().GetOrLoad( clipNote.source );
                    VoiceParameters->midiNote = Note;
                    VoiceParameters->rootNote = Note;

                    VoiceParameters->startSample     = clipNote.loopStart;
                    VoiceParameters->lengthInSamples = ( 0 == clipNote.loopEnd ) ? numSamples : juce::jmin( numSamples, clipNote.loopEnd );
                    VoiceParameters->sampleRate      = (float)_sampleRate;

                    _gain *= ( clipNote.gain * ( (float)Velocity / 127.f ) );
                    _pan += clipNote.pan;
                    _gain                 = juce::jlimit( 0.0, 1.0, _gain );
                    _pan                  = juce::jlimit( -1.0, 1.0, _pan );
                    VoiceParameters->gain = _gain;
                    VoiceParameters->pan  = _pan;

                    _semi *= clipNote.semi;
                    _fine *= clipNote.fine;
                    VoiceParameters->pitchFactor = calculatePitchFactor( _semi, _fine );

                    VoiceParameters->isMute    = clipNote.isMute;
                    VoiceParameters->isLooping = clipNote.padPlay == CLIP_TOGGLE;
                    VoiceParameters->isReverse = clipNote.reverse;

                    //VoiceParameters->lfo = pNoteData->lfo;
                    //VoiceParameters->envelope = pNoteData->env;

                    // Queue this voice for processing...
                    addVoiceForPlayback( idx, Note );
                    //if( added ) DBG( "NoteAdded" );
                }
            }
        }
    }

};
