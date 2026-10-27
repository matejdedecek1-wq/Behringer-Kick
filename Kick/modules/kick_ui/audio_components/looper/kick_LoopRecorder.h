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
    ///////////////////////////////////////////////////////////////////////
    // Looper Playhead
    ///////////////////////////////////////////////////////////////////////
    class LooperPlayHead
        : public juce::Component
    {
    public:
        LooperPlayHead();
        ~LooperPlayHead() = default;

        inline double getPlayHeadPosition() const { return normPos; }
        inline void   reDraw() { repaint(); };

    private:
        void          paint( juce::Graphics& g ) override;
        const double& normPos{ mt::getAudioInterface().getLooperProgress() };
    };

    ///////////////////////////////////////////////////////////////////////
    // Looper Recorder
    ///////////////////////////////////////////////////////////////////////
    class LoopRecorder
        : public juce::Component
        , private juce::Timer
        , private mt::MTAudioInterfaceListener
    {
    public:
        ///
        /// Signals
        ///
        sigslot::signal<bool> recordingStarted;

        explicit LoopRecorder( ui::KickIOComponent& Opts );
        ~LoopRecorder();

        void play( bool start );
        void stop();
        void record( bool Overdub );
        void clearRecording();
        void exportRecording( juce::String& FilePath );

        // Handle button interactions...
        void handleCommand( CPktMgr::HWMsg_t& );

    private:
        void setupCallbackHandlers();

        void   setThumbnail();
        void   setRecordingLength( int Bars );
        double getRecordingLength( int Bars );

        void onSequenceSelectionChange( const mt::MTSelected& Selected ) override;

        void timerCallback() override;
        void resized() override;
        void paint( juce::Graphics& g ) override;

        void getDisplayText( juce::String& markerText, int bar, int beat, int tick, int numDigits );
        void paintBeats( juce::Graphics& g, int numBeats, float beatWidth, const juce::Rectangle<int>& displayRect );
        void drawMarkingText( juce::Graphics& g, juce::String& value, float xpos, const juce::Rectangle<int>& displayRect, juce::Colour = ui::Theme::textColour( 3 ) );
        void visibilityChanged() override;

        mt::MTAudioInterface& ae{ mt::getAudioInterface() };
        mt::KickSettings&     settings{ mt::getSettings() };
        const mt::MTPlayhead& playHead{ ae.getPlayhead() };
        ui::KickIOComponent&  options;

    public:
        mt::MTTempoMgr*   tempoMgr{ nullptr };
        juce::ProgressBar progressBar{ ae.getLooperProgress() };

        ui::KickNumber bars{ "LENGTH", BAR::MIN, BAR::LOOP_RECORDER_MAX, BAR::DEFAULT, BAR::STEP, false };

        ui::KickImageButton reverseB{ "Reverse", "Looper_reverse", "Looper_reverse_active" };
        ui::KickImageButton replaceB{ "Replace", "Looper_repeat", "Looper_repeat_active" };
        ui::KickImageButton playB{ "Play", "Looper_play", "Looper_play_active" };
        ui::KickImageButton recordB{ "Record", "Looper_rec", "Looper_rec_active" };

        ui::KickToggleButton syncB{ "SEQ", "Sync", "Sync" };
        ui::KickToggleButton stopB{ "PLAY", "STOP", "PLAY" };

        ui::KickRotary outGain{ "GAIN", juce::Slider::TextBoxBelow, true };
        ui::KickRotary outPan{ "PAN", juce::Slider::TextBoxBelow, true };

        juce::Rectangle<int> scaleArea;
        ui::WaveRenderer     wav;

        LooperPlayHead ph;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( LoopRecorder )
    };
}
