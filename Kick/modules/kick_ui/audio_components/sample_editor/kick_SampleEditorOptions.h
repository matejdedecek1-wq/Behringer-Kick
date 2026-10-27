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
    namespace ac
    {
        class FromBPMMatch : public juce::Component
        {
        public:
            FromBPMMatch( const juce::String, int tuneValue );
            ~FromBPMMatch() = default;

            int getTuneValue() { return tune.getValue(); }

        private:
            ui::KickNumber tune{ "TUNE", -24, 24, 0, 1, true };

            ui::KickNumber  beats{ "BEATS", 1, 999, 1, 1, true };
            ui::KickNumberf bpm{ "BPM", 1.f, 999.f, 80.f, 1.f, true };
            ui::KickLabel   sample{ "" };

            ui::KickButton toSeq{ "TO SEQUENCE" };
            ui::KickButton match{ "MATCH" };
            ui::KickButton close{ "CANCEL" };

            void resized() override;
        };

        class SliceAndCombine : public juce::Component
        {
        public:
            std::function<void( void )> onSliceClicked;
            std::function<void( void )> onCombineClicked;

            SliceAndCombine();
            ~SliceAndCombine() = default;

        private:
            ui::KickButton slice{ "Slice" };
            ui::KickButton combine{ "Combine" };

            void resized() override;
        };

        class SampleEditorOPtions : public juce::Component
        {
        public:
            SampleEditorOPtions( SampleEditorData& data, ui::KickViewport& v, ui::KickViewport& s );

            void removeViewAndScale()
            {
                removeChildComponent( &view );
                removeChildComponent( &scale );
            }

            void addViewAndScale()
            {
                addAndMakeVisible( view );
                addAndMakeVisible( scale );
            }

            virtual void paint( juce::Graphics& g ) override;
            void         resized() override;

        protected:
            juce::Rectangle<int> getActionsRectangle();
            juce::Rectangle<int> getOptionsRectangle();

        protected:
            SampleEditorData& seData;
            ui::KickViewport& view;
            ui::KickViewport& scale;
        };

        class SampleEditorChopOptions : public SampleEditorOPtions
            , public juce::Button::Listener
        {
        public:
            SampleEditorChopOptions( SampleEditorData& seData, ui::KickViewport& view, ui::KickViewport& scale );
            ~SampleEditorChopOptions();
            void paint( juce::Graphics& g ) override;
            void resized() override;
            void buttonClicked( juce::Button* ) {}

        private:
            std::vector<juce::var> getSplitModeBpmOptions();
            void                   setSlices( int newValue );
            void                   setBpmSlices();

        private:
            ui::KickToggleButton manualButton{ "Manual", "Manual", "Manual" };
            ui::KickToggleButton thresholdButton{ "Threshold", "Threshold", "Threshold" };
            ui::KickToggleButton regionsButton{ "Regions", "Regions", "Regions" };
            ui::KickToggleButton bpmButton{ "Bpm", "Bpm", "Bpm" };
            ui::KickToggleButton linkButton{ "Link", "Link", "Link" };
            ui::KickToggleButton scButton{ "S/C", "S/C", "S/C" };

            ui::KickNumber   startNumber{ "START", 0, 174403, 1, 1, true };
            ui::KickNumber   endNumber{ "END", 1, 174404, 1, 1, true };
            ui::KickNumber64 cueNumber{ "CUE", 0, 174404, 0, 1, true };

            ui::KickListBox sliceListBox{ "SLICE", { "All" } };

            //Threshold Mode
            ui::KickNumber thresholdNumber{ "THRESHOLD", 0, 100, 30, 1, true };
            ui::KickNumber minTimeNumber{ "MIN TIME", 0, 1000, 0, 1, true };

            //Regions Mode
            ui::KickNumber regionsNumber{ "REGIONS", 1, 128, 1, 1, true };
            int            regionValue{ 1 };
            int            selectedRegion{ 0 };

            //BPM Mode
            ui::KickNumber  barsNumber{ "BARS", 1, 32, 1, 1, true };
            ui::KickNumber  beatsNumber{ "BEATS", 1, 32, 4, 1, true };
            ui::KickListBox timeDivListBox{ "TIME DIV", 1 };

            bool ignoreModeChanges = false;
            void reset( const juce::int64 );
        };

        ////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////Trim/////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////

        class SampleEditorTrimOptions : public SampleEditorOPtions
            , public juce::Button::Listener
        {
        public:
            SampleEditorTrimOptions( SampleEditorData& seData, ui::KickViewport& view, ui::KickViewport& scale );
            ~SampleEditorTrimOptions();

            void paint( juce::Graphics& g ) override;
            void resized() override;
            void buttonClicked( juce::Button* button ) override {}

        private:
            ui::KickLabel   timingNpitchLabel{ "Timing and Pitch" };
            ui::KickNumber  startNumber{ "START", 0, 174403, 1, 1, true };
            ui::KickNumber  endNumber{ "END", 1, 174404, 1, 1, true };
            ui::KickNumber  semiNumber{ "SEMI", SEMI::MIN, SEMI::MAX, 0, 1, true };
            ui::KickNumber  fineNumber{ "FINE", FINE::MIN, FINE::MAX, 0, 1, true };
            ui::KickNumber  xFadeNumber{ "X-FADE", 0, 1, 0, 1, true };
            ui::KickNumberf tuneNumber{ "TUNE", -24, 24, 0, 1, true };
            ui::KickNumber  bpmNumber{ "BPM", 20, 999, 20, 1, true };

            ui::KickListBox rootNoteListBox{ "ROOT NOTE", { ScaleAndNotes::midiNoteNames }, 0, 4 };

            ui::KickToggleButton repeat{ "Repeat", "Repeat", "Repeat" };
            ui::KickToggleButton loopLock{ "Loop Lock", "Loop Lock", "Loop Lock" };
            ui::KickToggleButton linkSlice{ "Link Slice", "Link Slice", "Link Slice" };
            ui::KickImageButton  cancel1{ "X", "SE_Xfade_Sharp", "SE_Xfade_Sharp_Active", "SE_Xfade_Sharp_Active" };
            ui::KickImageButton  cancel2{ "X", "SE_Xfade_Smooth", "SE_Xfade_Smooth_Active", "SE_Xfade_Smooth_Active" };
            void                 setEnableXfade();
            void                 reset( const juce::int64 );
            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( SampleEditorTrimOptions )
        };

        ////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////Program/////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////

        class SampleEditorProgramOptions : public SampleEditorOPtions
            , public juce::Button::Listener
        {
        public:
            SampleEditorProgramOptions( SampleEditorData& seData, ui::KickViewport& view, ui::KickViewport& scale );
            ~SampleEditorProgramOptions();

            void paint( juce::Graphics& g ) override;
            void resized() override;
            void buttonClicked( juce::Button* button ) override {}

        private:
            ui::KickLabel  timingNpitchLabel{ "Timing and Pitch" };
            ui::KickNumber startNumber{ "START", 0, 174403, 1, 1, true };
            ui::KickNumber endNumber{ "END", 1, 174404, 1, 1, true };
            ui::KickNumber semiNumber{ "SEMI", SEMI::MIN, SEMI::MAX, 0, 1, true };
            ui::KickNumber fineNumber{ "FINE", FINE::MIN, FINE::MAX, 0, 1, true };
            ui::KickNumber tuneNumber{ "TUNE", -24, 24, 0, 1, true };

            ui::KickListBox rootNoteListBox{ "ROOT NOTE", { ScaleAndNotes::midiNoteNames }, 0, 4 };

            ui::KickToggleButton repeat{ "Repeat", "Repeat", "Repeat" };
            ui::KickToggleButton loopLock{ "Loop Lock", "Loop Lock", "Loop Lock" };
            ui::KickToggleButton linkSlice{ "Link Slice", "Link Slice", "Link Slice" };
            ui::KickButton       fromBpm{ "From BPM", "From BPM" };
            void                 reset( const juce::int64 );
            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( SampleEditorProgramOptions )
        };

        ///////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////CHOP OPTIONS/////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////

        class SampleEditorChopOPtions : public SampleEditorOPtions
        {
        public:
            SampleEditorChopOPtions( SampleEditorData& seData, ui::KickViewport& view, ui::KickViewport& scale );
            ~SampleEditorChopOPtions() = default;

            void knobCommand( int knob, int value );
            void handleCommand( CPktMgr::HWMsg_t& );

        private:
            void setSlices( int slices );
            void paint( juce::Graphics& ) override;
            void resized() override;
            void setBpmSlices();
            void setThreshSlices();
            void updateSliceList( int SelectedRegion = 1 );

        private:
            ui::KickListBox modeType{ "MODE" };

            ui::KickListBox      sliceList{ "SLICE", { "All" } };
            ui::KickButton       slicePlus{ "SLICE +", "SLICE +" };
            ui::KickButton       split{ "SPLIT" };
            ui::KickButton       combine{ "COMBINE" };
            ui::KickToggleButton legato{ "LEGATO", "LEGATO", "LEGATO" };
            ui::KickToggleButton playCue{ "PLAY CUE", "PLAY CUE", "PLAY CUE" };

            ui::KickButton repeat{ "REPEAT" };
            ui::KickButton clrAll{ "CLEAR ALL" };

            ui::KickNumber64 startNumber{ "START" };
            ui::KickNumber64 endNumber{ "END" };
            ui::KickNumber64 cueNumber{ "CUE" };
            ui::KickNumber64 xFadeNumber{ "X FADE" };

            ui::KickNumber64 thresholdNumber{ "THRESHOLD", 0, 100, 30 };
            ui::KickNumber64 minSamplesNumber{ "MIN SAMPLES", 0, 1000, 100 };
            ui::KickNumber64 regionsNumber{ "REGIONS", 1, 128, 1 };
            ui::KickNumber64 barsNumber{ "BARS", 1, 32, 1 };
            ui::KickListBox  timeDivList{ "TIME DIV" };

            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( SampleEditorChopOPtions )
        };

        ///////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////TUNE OPTIONS/////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////

        class SampleEditorTuneOPtions : public SampleEditorOPtions
        {
        public:
            SampleEditorTuneOPtions( SampleEditorData& seData, ui::KickViewport& view, ui::KickViewport& scale );
            ~SampleEditorTuneOPtions() = default;

            void handleCommand( CPktMgr::HWMsg_t& );

        private:
            void paint( juce::Graphics& g ) override;
            void resized() override;
            void updateSliceList();

        private:
            ui::KickListBox sliceList{ ui::KickListBox::withTitleBar, "SLICE", {} };
            ui::KickButton  detectBpm{ "DETECT BPM" };
            ui::KickButton  toSeq{ "TO SEQ" };
            ui::KickButton  matchBpm{ "MATCH" };

            ui::KickNumber64 sampleBPM{ "SAMPLE BPM" };
            //ui::KickNumber64 bpm{ "BPM" };
            ui::KickNumber64 semi{ "SEMI" };
            ui::KickNumber64 fine{ "FINE" };

            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( SampleEditorTuneOPtions )
        };
    }
}
