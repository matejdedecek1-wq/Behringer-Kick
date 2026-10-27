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
namespace kick_ui
{
#define loopRecorderMargin                10
#define loopRecorderrecorderOptionsHeight 104
#define loopRecorderMargin                10
#define loopRecorderScaleHeight           20

    class SamplerRecorder
        : public juce::Component
        , private juce::Timer
        , private juce::AsyncUpdater
    {
    public:
        enum RecordChannels
        {
            L_MONO,
            R_MONO,
            LR_MONO,
            L_STEREO,
            R_STEREO,
            LR_STEREO,
        };

        enum class RecordMode
        {
            recordModeNone,
            recordModeRecord,
            recordModeRecording,
            recordModeStopped
        };

        SamplerRecorder();
        ~SamplerRecorder();

        void              paint( juce::Graphics& g ) override;
        void              resized() override;
        void              setNumChannels( int channels );
        const juce::File& getRecordFile() const { return recordFile; }
        void              timerCallback();
        void              visibilityChanged() override;

        void   recordedTimeChanged( double totalRecordTime );
        void   magnitudeChanged( const std::vector<float>& magnitude ) {}
        void   stopRecording( bool cancel = false );
        double getCurrentRecordTime();

        void           setRecordChannels( RecordChannels Chans );
        RecordChannels getRecordChannels();

        void  setRecordMode( RecordMode newMode );
        void  setMonitorOn( bool on );
        float getRecordThresholdDB();
        void  setRecordThresholdDB( float threshDb );
        void  clearRecording();
        void  setGain( float );
        void  setGainDB( float );
        bool  isRecording();
        bool  isMonitorOn();
        void  startRecording();
        void  knobCommand( int knob, int value );

    private:
        float gain      = -20.0;
        bool  recording = false;

    public:
        SIGNALS : sigslot::signal<mt::MTMeterData&>  signalStrengthChanged;
        sigslot::signal<SamplerRecorder::RecordMode> recordModeChanged;
        sigslot::signal<bool>                        recordingStarted;
        sigslot::signal<bool>                        recordingPreviewStarted;

    private:
        juce::Rectangle<float>    getRecorderRect() const;
        juce::Rectangle<float>    getMaxLengthRect() const;
        void                      setThumbnail();
        juce::Rectangle<int>      getThumbnailRectangle();
        void                      handleAsyncUpdate() override;
        void                      paintBeats( juce::Graphics& g, int numBeats, float beatWidth, const juce::Rectangle<int>& displayRect );
        juce::Rectangle<int>      getScaleRectangle();
        void                      getDisplayText( juce::String& markerText, int bar, int beat, int tick, int numDigits );
        void                      drawMarkingText( juce::Graphics& g, juce::String& markerText, float xpos, const juce::Rectangle<int>& displayRect );
        juce::AudioFormatManager  formatManager;
        juce::AudioThumbnailCache thumbnailCache{ 100 };
        juce::AudioThumbnail      thumbnail{ 512, formatManager, thumbnailCache };

    private:
        mt::MTAudioInterface& ae{ mt::getAudioInterface() };

        ui::KickTimerControl timerControl, maxLengthTimerControl;
        ui::KickImageButton  recordButton{ "recordButton", "sampler_Rec" };
        ui::KickLabel        recordLabel{ "Waiting for Signal" };
        ui::KickLabel        maxLengthLabel{ "Max Length" };
        ui::KickImageButton  statusImage{ "Arm", "sampler_Icon-wait" };
        double               _progress{ 0.0 };

        ui::KickProgressBar progressBar;

        double                 _recordLength{ 0 };
        mt::MTRecorderListener recorderListner;

        int            numChannels = 1;
        RecordChannels recChans{ LR_STEREO };
        juce::File     recordFile;
        RecordMode     mode = RecordMode::recordModeNone;
        bool           waveFormTimer{ false };
    };
}
