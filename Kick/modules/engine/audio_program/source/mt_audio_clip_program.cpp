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

#include "mt_audio_clip_program.h"

namespace mt
{
    MTClipProgram::MTClipProgram( AudIO::DawEngine& Engine, juce::ValueTree& VT, juce::UndoManager* UM )
        : MTIProgram( VT, UM )
        , MTIClipProgramData( VT, UM )
        , engine( Engine )
    {
        rebuildSampleMap();

        auto pProg = std::make_unique<ClipProgram>( Engine, getName().toStdString(), this );
        if( pProg )
        {
            pClip = pProg.get();
            Engine.createObject( std::move( pProg ) );
            pClip->initAudioRouting();

            auto outBusId = engine.getBusManager()->getBusId( getOutput().toStdString() );
            pClip->connectOutputTo( outBusId );
            pClip->setGainsForSend( getSendGains() );

            onNameChanged = [&]() {
                pClip->setName( getName().toStdString() );
            };

            onOutChanged = [&]() {
                auto outBusId = engine.getBusManager()->getBusId( getOutput().toStdString() );
                pClip->connectOutputTo( outBusId );
            };

            onSendGainChanged = [&]( int Channel, float Value ) {
                pClip->setGainForSend( (uint32_t)Channel, Value );
            };

            // Inserts...
            auto vt = VT.getOrCreateChildWithName( IDs::INSERTS, nullptr );
            MTIInsertsInterface::setup( vt, UM, pClip );
        }
    };

    MTClipProgram::~MTClipProgram()
    {
        if( pClip )
        {
            engine.removeObject( pClip->getId() );
        }
    }

    void MTClipProgram::refresh()
    {
        MTIProgram::refresh();
    }

    ///
    /// Crreators
    ///
    std::unique_ptr<MTISample> MTClipProgram::iSampleCreator( int Note )
    {
        auto vt   = juce::ValueTree{ IDs::CLIP };
        auto clip = std::make_unique<MTIClip>( vt, um );
        clip->muteGroup.setDefault( CommonUtils::getDefaultMuteGroupForClipProgram( Note ) );
        clip->minNote = Note;
        clip->maxNote = Note;
        clip->keyNote = Note;
        return clip;
    }

    std::unique_ptr<MTISample> MTClipProgram::iSampleCreator( juce::ValueTree& VT, juce::UndoManager* UM )
    {
        return VT.hasType( IDs::CLIP ) ? ( std::make_unique<MTIClip>( VT, UM ) ) : nullptr;
    }

    MTIClip* MTClipProgram::getClip( int Note )
    {
        return dynamic_cast<MTIClip*>( getSample( Note ) );
    }

    //////
    ////// Program parameter API's
    //////
    float MTClipProgram::getQuantize() { return launchQuantize.get(); }
    void  MTClipProgram::setQuantize( float Quantize ) { launchQuantize = Quantize; }

    //////
    ////// Clip parameter API's
    //////
    void MTClipProgram::setClipSamplePlay( int Note, ClipPlayType Type )
    {
        if( auto p = getClip( Note ) )
            p->padPlay = Type;
    }

    ClipPlayType MTClipProgram::getClipSamplePlay( int Note )
    {
        if( auto p = getClip( Note ) )
            return p->padPlay.get();
        return ClipPlayType::CLIP_ONE_SHOT;
    }

} // namespace MT
