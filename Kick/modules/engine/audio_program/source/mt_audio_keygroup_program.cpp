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

#include "mt_audio_keygroup_program.h"

namespace mt
{

    MTKeygroupProgram::MTKeygroupProgram( AudIO::DawEngine& Engine, juce::ValueTree& VT, juce::UndoManager* UM )
        : MTCommonProgram( VT, UM ), engine( Engine )
    {
        auto pProg = std::make_unique<KeygroupProgram>( Engine, getName().toStdString(), this );
        if( pProg )
        {
            key = pProg.get();
            Engine.createObject( std::move( pProg ) );
            key->initAudioRouting();

            auto outBusId = engine.getBusManager()->getBusId( getOutput().toStdString() );
            key->connectOutputTo( outBusId );
            key->setGainsForSend( getSendGains() );

            onNameChanged = [&]() {
                key->setName( getName().toStdString() );
            };

            onOutChanged = [&]() {
                auto outBusId = engine.getBusManager()->getBusId( getOutput().toStdString() );
                key->connectOutputTo( outBusId );
            };

            onSendGainChanged = [&]( int Channel, float Value ) {
                key->setGainForSend( (uint32_t)Channel, Value );
            };

            // Inserts...
            auto vt = VT.getOrCreateChildWithName( IDs::INSERTS, nullptr );
            MTIInsertsInterface::setup( vt, UM, key );
            rebuildSampleMap();
        }
    };

    MTKeygroupProgram::~MTKeygroupProgram()
    {
        if( key )
        {
            engine.removeObject( key->getId() );
        }
    }

    MTIKeys* MTKeygroupProgram::getKeysClip( int Note )
    {
        return dynamic_cast<MTIKeys*>( getSample( Note ) );
    }

    ///
    /// Crreators
    ///
    std::unique_ptr<MTISample> MTKeygroupProgram::iSampleCreator( int Note )
    {
        auto vt       = juce::ValueTree{ IDs::CLIP };
        auto clip     = std::make_unique<MTIKeys>( vt, um );
        clip->minNote = Note;
        clip->maxNote = Note;
        clip->keyNote = Note;
        return clip;
    }

    std::unique_ptr<MTISample> MTKeygroupProgram::iSampleCreator( juce::ValueTree& VT, juce::UndoManager* UM )
    {
        return VT.hasType( IDs::CLIP ) ? ( std::make_unique<MTIKeys>( VT, UM ) ) : nullptr;
    }

    LayerPlayType MTKeygroupProgram::getNoteLayerPlay( int Note )
    {
        if( auto p = getKeysClip( Note ) )
            return p->layerPlay.get();
        return LayerPlayType::CYCLE;
    }

    SamplePlayType MTKeygroupProgram::getNoteSamplePlay( int Note )
    {
        if( auto p = getKeysClip( Note ) )
            return p->samplePlay.get();
        return SamplePlayType::ONE_SHOT;
    }

    void MTKeygroupProgram::setNoteLayerPlay( int Note, LayerPlayType Value )
    {
        if( auto p = getKeysClip( Note ) )
            p->layerPlay = Value;
    }
    void MTKeygroupProgram::setNoteSamplePlay( int Note, SamplePlayType Value )
    {
        if( auto p = getKeysClip( Note ) )
            p->samplePlay = Value;
    }

} // namespace MT
