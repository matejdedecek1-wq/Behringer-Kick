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
    ClipProgram::ClipProgram( AudIO::Design& D, const juce::String& Name, MTIProgram* PClip )
        : ISynthObject( D, POLYPHONY::MAX )
    {
        setName( Name.toStdString() );
        setProgram( PClip );

        // Pre-reseve Memory...
        for( auto i = 0; i < getPolyphony(); ++i )
        {
            auto& pSlice = slices.emplace_back( std::make_unique<ASoundSlice>() );
            voices.emplace_back( std::make_unique<AVoice>( master, pSlice.get() ) );
        }
        ISynthObject::addVoices( voices );
    }

    ClipProgram::~ClipProgram()
    {
        setBypass( true );
        voices.clear();
        slices.clear();
    }

    void ClipProgram::setProgram( MTIProgram* PClip )
    {
        // call base class method...
        ISynthObject::setProgram( PClip );

        noteOffAll();
        pClip = dynamic_cast<MTClipProgram*>( PClip );
        setBypass( ( pClip == nullptr ) );
    }

    void ClipProgram::noteOn( int Note, int Velocity )
    {
        // Return if no audio synth available or invalid midi length
        if( !pClip || !CommonUtils::isValidMidiNote( Note ) ) return;

        // Get clip note.
        if( auto clipNote = pClip->getClip( Note ) )
        {
            // Stop clips in same mute group.
            pClip->forSamplesInSameMuteGroup( Note, clipNote->muteGroup.get(), [&]( int Note ) {
                noteOff( Note );
            } );

            // Note muted,
            if( pClip->isNoteEffectivelyMuted( Note ) && !pClip->isNoteSolo( Note ) )
                return;

            //DBG( "AA" << Note << " " << Velocity );
            if( const auto clipSound = clipNote->getSourceSlice() )
            {
                if( auto pVoice = isPlayback( Note ) )
                {
                    if( ClipPlayType::CLIP_TOGGLE == clipNote->padPlay.get() )
                    {
                        pVoice->noteOff();
                    }
                    pVoice->reset();
                    pVoice->noteOn( Note, Velocity );
                }
                else
                {
                    // Start playback.
                    if( auto pVoice = getFreeVoice( Note, 1 ) )
                    {
                        pVoice->setSlice( clipSound );
                        pVoice->noteOn( Note, Velocity );
                        addVoiceForPlayback( pVoice );
                    }
                }
            }
        }
    }

};
