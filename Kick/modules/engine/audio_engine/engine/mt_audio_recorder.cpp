/*
 * Copyright (c) 2024 MUSIC Tribe
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

#include "mt_audio_recorder.h"

namespace mt
{
    MTAudioRecorderManager::MTAudioRecorderManager( AudIO::DawEngine& Engine )
        : _engine( Engine )
    {
    }

    MTAudioRecorderManager::~MTAudioRecorderManager()
    {
        closeRecorder();
    }

    // Audio Recorder...
    void MTAudioRecorderManager::initRecorder()
    {
        if( auto pRecorder = std::make_unique<SampleRecorder>( _engine ) )
        {
            _sampler = pRecorder.get();
            _engine.addMeter( _engine.createObject( std::move( pRecorder ) ) );
            _sampler->initAudioRouting();
            _sampler->connectInputTo( AudIO::DawRouting::getDefaultStereoInID() );
            _sampler->setEnable( true );
            assert( _sampler != nullptr );
        }

        if( auto pRecorder = std::make_unique<MTLooper>( _engine ) )
        {
            _looper = pRecorder.get();
            _engine.addMeter( _engine.createObject( std::move( pRecorder ) ) );
            _looper->initAudioRouting();
            _looper->connectInputTo( AudIO::DawRouting::getDefaultStereoInID() );
            _looper->setEnable( true );
            assert( _looper != nullptr );
        }
    }

    void MTAudioRecorderManager::closeRecorder()
    {
        if( _sampler )
        {
            _engine.removeObject( _sampler->getId() );
            _sampler = nullptr;
        }
        if( _looper )
        {
            _engine.removeObject( _looper->getId() );
            _looper = nullptr;
        }
    }

    void MTAudioRecorderManager::enableRecorder( bool Enable )
    {
        if( _sampler ) _sampler->setEnable( Enable );
    }

    void MTAudioRecorderManager::setup( juce::ValueTree VT, juce::UndoManager* UM )
    {
        // Setup inserts on looper.
        auto looperNode = VT.getOrCreateChildWithName( IDs::LOOPER, nullptr );
        auto looperVT   = looperNode.getOrCreateChildWithName( IDs::INSERTS, nullptr );
        _looper->setup( looperVT, UM, dynamic_cast<AudIO::DawInserts*>( _looper ) );

        // Setup inserts on Sampler.
        auto samplerNode = VT.getOrCreateChildWithName( IDs::SAMPLER, nullptr );
        auto samplerVT   = looperNode.getOrCreateChildWithName( IDs::SAMPLER, nullptr );
    }

    void MTAudioRecorderManager::setupRecorder( double timeInSeconds, float Gain, float Pan, bool Looping, bool Overdub )
    {
        if( _sampler )
        {
            _sampler->reset();
            _sampler->setGainAndPan( Gain, Pan, true );
            _sampler->setLooping( Looping );
            _sampler->setOverdub( Overdub );
            _sampler->setLength( timeInSeconds );
        }
    }
    void MTAudioRecorderManager::setMonitorOn( bool Monitor )
    {
        if( _sampler ) _sampler->setMonitorOn( Monitor );
    }

    void MTAudioRecorderManager::setRecorderLoop( bool Looping )
    {
        if( _sampler ) _sampler->setLooping( Looping );
    }

    void MTAudioRecorderManager::setRecorderOverdub( bool Overdub )
    {
        if( _sampler ) _sampler->setOverdub( Overdub );
    }

    void MTAudioRecorderManager::setRecorderPan( float Pan )
    {
        if( _sampler ) _sampler->setPan( Pan );
    }

    void MTAudioRecorderManager::setRecorderGain( float Gain )
    {
        if( _sampler ) _sampler->setGain( Gain );
    }

    void MTAudioRecorderManager::setRecorderGainDB( float GainDB )
    {
        if( _sampler ) _sampler->setGainDB( GainDB );
    }

    void MTAudioRecorderManager::setRecorderThreshold( float Gain )
    {
        if( _sampler ) _sampler->setThreshold( Gain );
    }

    void MTAudioRecorderManager::setRecorderThresholdDB( float DB )
    {
        if( _sampler ) _sampler->setThresholdDB( DB );
    }

    void MTAudioRecorderManager::setRecorderDirection( bool ForwardDirection )
    {
        if( _sampler ) _sampler->setDirection( ForwardDirection ? SampleRecorder::FORWARD : SampleRecorder::REVERSE );
    }

    void MTAudioRecorderManager::setRecorderMode( int Mode )
    {
        if( _sampler ) _sampler->setRecordingMode( (SampleRecorder::Mode)Mode );
    }

    void MTAudioRecorderManager::startRecorder( int Mode, bool ForwardDirection, bool FromBeginging )
    {
        setRecorderDirection( ForwardDirection );
        setRecorderMode( Mode );
        startRecorder( FromBeginging );
    }

    void MTAudioRecorderManager::startRecorder( bool FromBeginging )
    {
        if( _sampler ) _sampler->startRecording( FromBeginging );
    }

    void MTAudioRecorderManager::stopRecorder()
    {
        if( _sampler ) _sampler->stopRecording();
    }

    void MTAudioRecorderManager::clearRecording()
    {
        if( _sampler ) _sampler->reset();
    }

    void MTAudioRecorderManager::saveRecording( const std::string& FilePath )
    {
        if( _sampler ) _sampler->saveRecording( FilePath );
    }

    void MTAudioRecorderManager::startRecorderPreview( double fromStartTimeInSeconds )
    {
        if( _sampler ) _sampler->startPreview( fromStartTimeInSeconds );
    }

    void MTAudioRecorderManager::stopRecorderPreview()
    {
        if( _sampler ) _sampler->stopPreview();
    }

    juce::AudioBuffer<float>& MTAudioRecorderManager::getRecorderAudioBuffer()
    {
        return _sampler ? _sampler->getAudioBuffer() : _recBufEmpty;
    }

    void MTAudioRecorderManager::setRecorderListener( MTRecorderListener& Listener )
    {
        if( _sampler )
        {
            _sampler->onRecorderStarted        = Listener.onRecorderStarted;
            _sampler->onRecorderStopped        = Listener.onRecorderStopped;
            _sampler->onRecorderPaused         = Listener.onRecorderPaused;
            _sampler->onRecorderResumed        = Listener.onRecorderResumed;
            _sampler->onRecorderLengthChanged  = Listener.onRecorderLengthChanged;
            _sampler->onRecorderPreviewStarted = Listener.onRecorderPreviewStarted;
            _sampler->onRecorderPreviewStopped = Listener.onRecorderPreviewStopped;
        }
    }

}
