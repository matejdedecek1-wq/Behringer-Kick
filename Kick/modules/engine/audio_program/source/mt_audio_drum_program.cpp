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

#include "mt_audio_drum_program.h"

namespace mt
{
    MTDrumProgram::MTDrumProgram( AudIO::DawEngine& Engine, juce::ValueTree& VT, juce::UndoManager* UM )
        : MTCommonProgram( VT, UM ), engine( Engine )
    {
        auto pProg = std::make_unique<DrumProgram>( Engine, getName().toStdString(), this );
        if( pProg )
        {
            drum = pProg.get();
            Engine.createObject( std::move( pProg ) );
            drum->initAudioRouting();

            auto outBusId = engine.getBusManager()->getBusId( getOutput().toStdString() );
            drum->connectOutputTo( outBusId );
            drum->setGainsForSend( getSendGains() );

            onNameChanged = [&]() {
                drum->setName( getName().toStdString() );
            };

            onOutChanged = [&]() {
                auto outBusId = engine.getBusManager()->getBusId( getOutput().toStdString() );
                drum->connectOutputTo( outBusId );
            };

            onSendGainChanged = [&]( int Channel, float Value ) {
                drum->setGainForSend( (uint32_t)Channel, Value );
            };

            // Inserts...
            auto vt = VT.getOrCreateChildWithName( IDs::INSERTS, nullptr );
            MTIInsertsInterface::setup( vt, UM, drum );
            rebuildSampleMap();
        }
    };

    MTDrumProgram ::~MTDrumProgram()
    {
        if( drum )
        {
            engine.removeObject( drum->getId() );
        }
    }

    MTIDrum* MTDrumProgram::getDrumClip( int Note )
    {
        return dynamic_cast<MTIDrum*>( getSample( Note ) );
    }

    ///
    /// Crreators
    ///
    std::unique_ptr<MTISample> MTDrumProgram::iSampleCreator( int Note )
    {
        auto vt       = juce::ValueTree{ IDs::CLIP };
        auto clip     = std::make_unique<MTIDrum>( vt, um );
        clip->minNote = Note;
        clip->maxNote = Note;
        clip->keyNote = Note;
        return clip;
    }

    std::unique_ptr<MTISample> MTDrumProgram::iSampleCreator( juce::ValueTree& VT, juce::UndoManager* UM )
    {
        return VT.hasType( IDs::CLIP ) ? ( std::make_unique<MTIDrum>( VT, UM ) ) : nullptr;
    }

    LayerPlayType MTDrumProgram::getNoteLayerPlay( int Note )
    {
        if( auto p = getDrumClip( Note ) )
            return p->layerPlay.get();
        return LayerPlayType::CYCLE;
    }

    SamplePlayType MTDrumProgram::getNoteSamplePlay( int Note )
    {
        if( auto p = getDrumClip( Note ) )
            return p->samplePlay.get();
        return SamplePlayType::ONE_SHOT;
    }

    void MTDrumProgram::setNoteLayerPlay( int Note, LayerPlayType Value )
    {
        if( auto p = getDrumClip( Note ) )
            p->layerPlay = Value;
    }
    void MTDrumProgram::setNoteSamplePlay( int Note, SamplePlayType Value )
    {
        if( auto p = getDrumClip( Note ) )
            p->samplePlay = Value;
    }
} // namespace MT
