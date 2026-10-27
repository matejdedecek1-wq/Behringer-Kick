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

namespace mt
{

    /// Audio Looper interface API's..
    class MTAudioManager;
    struct MTLooperInterface
    {
        std::unique_ptr<MTAudioManager>& manager;
        explicit MTLooperInterface( std::unique_ptr<MTAudioManager>& Manager )
            : manager( Manager ) {}

        /// Setters...
        void setLooperEnable( bool Enable ) noexcept;

        void setLooperLength( double timeInSeconds ) noexcept;
        void setLooperMonitorOn( bool Monitor ) noexcept;

        void setLooperOverdub( bool Overdub ) noexcept;
        void setLooperRecord( bool Record ) noexcept;
        void setLooperPlay() noexcept;

        void setLooperLooping( bool Looping ) noexcept;

        void setLooperMonitorGain( float Gain ) noexcept;
        void setLooperPan( float Pan ) noexcept;
        void setLooperGain( float Gain ) noexcept;
        void setLooperGainDB( float GainDB ) noexcept;
        void setLooperThreshold( float Gain ) noexcept;
        void setLooperThresholdDB( float DB ) noexcept;
        void setLooperDirection( Recorder::Direction ForwardDirection ) noexcept;
        void setLooperRecordingMode( Recorder::Mode Mode ) noexcept;

        /// Getters...
        float          getLooperSampleRate() const noexcept;
        float          getLooperLength() const noexcept;
        float          getLooperMonitorGain() const noexcept;
        float          getLooperPan() const noexcept;
        float          getLooperGain() const noexcept;
        float          getLooperThreshold() const noexcept;
        float          getLooperThresholdDB() const noexcept;
        Recorder::Mode getLooperRecordingMode() const noexcept;

        bool isLooperEnabled() const noexcept;
        bool isLooperEmpty() const noexcept;
        bool isLooperMonitorOn() const noexcept;
        bool isLooperLooping() const noexcept;
        bool isLooperOverdubbing() const noexcept;
        bool isLooperRecording() const noexcept;
        bool isLooperPlay() const noexcept;
        bool isLooperForward() const noexcept;
        bool isLooperReverse() const noexcept;
        bool isLooperRunning() const noexcept;

        /// Metering
        MTMeterData getLooperInputMeter() noexcept;
        MTMeterData getLooperOutputMeter() noexcept;
        /// Buffer
        juce::AudioSampleBuffer& getLooperSampleBuffer() const noexcept;
        // Inserts
        MTIInsertsInterface* getLooperInsertsInterface();

        /// Playback controls...
        void    looperStart( bool FromBeginning, bool SkipThresholdDetection ) noexcept;
        void    looperPlayback( double timeInSeconds = 0.0 ) noexcept;
        void    looperStop() noexcept;
        void    looperClear() noexcept;
        void    looperUndo() noexcept;
        void    looperRedo() noexcept;
        void    looperSave( const juce::String& FilePath ) noexcept;
        double& getLooperProgress() noexcept;
    };

} // namespace mt
