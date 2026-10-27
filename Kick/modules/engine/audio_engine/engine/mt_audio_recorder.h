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

#pragma once

namespace mt
{
    struct MTLooper
        : public Looper
        , public MTIInsertsInterface
    {
        explicit MTLooper( AudIO::Design& D )
            : Looper( D ) {}

        MTIInsertsInterface* getInsertsInterface()
        {
            return dynamic_cast<MTIInsertsInterface*>( this );
        };
    };

    //
    class MTAudioRecorderManager
    {
    public:
        explicit MTAudioRecorderManager( AudIO::DawEngine& Engine );
        virtual ~MTAudioRecorderManager();

        void initRecorder();
        void closeRecorder();
        void enableRecorder( bool Enable );
        void setup( juce::ValueTree VT, juce::UndoManager* UM );

        void setupRecorder( double timeInSeconds, float Gain, float Pan, bool Looping, bool Overdub );
        void setMonitorOn( bool Monitor );
        void setRecorderLoop( bool Looping );
        void setRecorderOverdub( bool Overdub );
        void setRecorderPan( float Pan );
        void setRecorderGain( float Gain );
        void setRecorderGainDB( float GainDB );
        void setRecorderThreshold( float Gain );
        void setRecorderThresholdDB( float DB );
        void setRecorderDirection( bool ForwardDirection );
        void setRecorderMode( int Mode );

        void startRecorder( int Mode, bool ForwardDirection, bool FromBeginging );
        void startRecorder( bool FromBeginging );
        void stopRecorder();
        void clearRecording();
        void saveRecording( const std::string& FilePath );
        void startRecorderPreview( double fromStartTimeInSeconds );
        void stopRecorderPreview();

        inline int   getRecorderMode() { return _sampler ? _sampler->getRecordingMode() : 0; }
        inline float getRecorderGain() { return _sampler ? _sampler->getGain() : 0.f; }
        inline float getRecorderProgress() { return _sampler ? _sampler->readProgress() : 0.f; }
        inline float getRecorderThreshold() { return _sampler ? _sampler->getThreshold() : 0.f; }
        inline float getRecorderThresholdDB() { return _sampler ? _sampler->getThresholdDB() : -100.f; }

        inline bool isRecorderEnabled() { return _sampler ? _sampler->isEnabled() : false; }
        inline bool isRecorderMonitorOn() { return _sampler ? _sampler->isMonitorOn() : false; }
        inline bool isRecorderLooping() { return _sampler ? _sampler->isLooping() : false; }
        inline bool isRecorderOverdubbing() { return _sampler ? _sampler->isOverdubbing() : false; }
        inline bool isRecorderInForwardDirection() { return _sampler ? _sampler->isForward() : true; }
        inline bool isRecorderRunning() { return _sampler ? _sampler->isRecording() : false; }
        inline bool isRecorderPreviewing() { return _sampler ? _sampler->isPreviewing() : false; }

        inline MTMeterData getRecorderInputMeter() { return _sampler ? _sampler->readInputMeter() : MTMeterData{}; }
        inline MTMeterData getRecorderOutputMeter() { return _sampler ? _sampler->readOutputMeter() : MTMeterData{}; }

        juce::AudioBuffer<float>& getRecorderAudioBuffer();

        void setRecorderListener( MTRecorderListener& Listener );

        MTLooper* getLooper() noexcept { return _looper; }

    private:
        AudIO::DawEngine&        _engine;
        SampleRecorder*          _sampler{ nullptr };
        MTLooper*                _looper{ nullptr };
        juce::AudioBuffer<float> _recBufEmpty;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( MTAudioRecorderManager )
    };
} // namespace MT
