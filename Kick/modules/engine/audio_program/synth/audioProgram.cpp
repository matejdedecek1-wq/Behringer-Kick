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
    AudioProgram::AudioProgram( AudIO::Design& D, const juce::String& Name, MTIProgram* PClip )
        : ISynthObject( D, 1 )
    {
        setName( Name.toStdString() );
        setProgram( PClip );

        // Pre-reseve Memory...
        for( auto i = 0; i < getPolyphony(); ++i )
            voices.emplace_back( std::make_unique<AVoice>( master ) );

        ISynthObject::addVoices( voices );
    }

    AudioProgram::~AudioProgram()
    {
        setBypass( true );
        voices.clear();
    }

    void AudioProgram::setProgram( MTIProgram* PAudio )
    {
        // call base class method...
        ISynthObject::setProgram( PAudio );

        noteOffAll();
        pAudio = dynamic_cast<MTAudioProgram*>( PAudio );
        setBypass( ( pAudio == nullptr ) );
    }

    void AudioProgram::noteOn( int Note, int Velocity )
    {
        if( !pAudio || pAudio->isNoteEffectivelyMuted( Note ) )
            return;

        auto& assetMgr = getAudioInterface().getAssetManager();
        auto  slice    = assetMgr.getAssetSlice( "" ); // Source

        noteOff( Note );
        if( auto voice = getFreeVoice( Note, 1 ) )
        {
            voice->setSlice( slice );
            voice->noteOn( 0, 127 );
            addVoiceForPlayback( voice );
        }
    }

};
