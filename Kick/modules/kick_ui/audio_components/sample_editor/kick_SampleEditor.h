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
    static sigslot::signal<const juce::String&, SampleEditorMode> sigSampleEditLoadFileForEditing;
    static sigslot::signal<double>                                sigSampleEditAddRegionAt;
    static sigslot::signal<const int, std::list<juce::var>&>      sampleProcessAction;

    class SampleEditor : public kick_ui::IScreen
        , public juce::Component
        , public SampleEditorInterface
        , public ui::KickViewport::Listener
    {
    public:
        ~SampleEditor() = default;

        bool requiresControlBar() override { return false; }
        bool requiresStatusBar() override { return true; }
        bool requiresHandleCommand() override { return true; }

        bool canClose() override { return true; }

        void handleCommand( CPktMgr::HWMsg_t& ) override;

        void updateOnViewPositionChange( ui::KickViewport* pViewport ) override;

        //SampleEditorInterface
        void loadSample( const juce::String& samplePath );
        void addRegionAt( double time );
        void setMode( SampleEditorMode mode );
        void showEditor();
        SampleEditor();

    private:
        mt::MTAudioInterface& ae{ mt::getAudioInterface() };
        const mt::MTSelected& aeSel{ ae.getSelected() };

        ui::KickViewport scaleViewport;
        ui::KickViewport seViewport;
        KickModeHeader   header{ "Sample Editor", "sEdit_Icon" };

        mt::ac::SampleEditorData        seData;
        mt::ac::SEScaleComponent        scale{ seData };
        mt::ac::SEViewComponent         editor{ seData, &seViewport };
        mt::ac::SampleEditorTuneOPtions tuneComp{ seData, seViewport, scaleViewport };
        mt::ac::SampleEditorChopOPtions chopComp{ seData, seViewport, scaleViewport };

        ui::KickListBox sampleListBox{ ui::KickListBox::BoxType::withTitleBar, "SAMPLES LIST", {} };

        std::vector<juce::String> samples;
        std::vector<juce::String> fileNames;
        juce::String              path;
        juce::ValueTree           vTree;

        ui::KickToggleButton tuneCompButton{ "Tune", "Tune", "Tune" };
        ui::KickToggleButton chopCompButton{ "Chop", "Chop", "Chop" };
        ui::KickButton       assignButton{ "Assign" };
        ui::KickButton       processButton{ "Process" };
        ui::KickImageButton  plusButton{ "PLUS", "Plus", "Plus", "Plus_Active" };
        ui::KickImageButton  minusButton{ "MINUS", "Minus", "Minus", "Minus_Active" };
        ui::KickLabel        zoomLabel{ "Zoom" };

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( SampleEditor )
        void updateSampleList();
        void resized() override;
        void visibilityChanged() override;
    };

    class ProcessSlice : public juce::Component
    {
    public:
        ProcessSlice( int index );
        ~ProcessSlice() = default;

        int                   getSelectedProcess() { return selectedID; }
        std::list<juce::var>& getParams() { return params; }

    private:
        const static inline std::vector<juce::String> processtype = {
            "Discard", "Delete", "Silence", "Extract", "Normalize", "Reverse", "Fade In", "Fade Out", "Pitch Shift", "Time Stretch", "Gain Change", "Copy", "Bit Reduce", "Stereo-Mono"
        };
        ui::KickRotary                         rotary{ "", juce::Slider::TextBoxRight, true };
        ui::KickLabel                          data;
        ui::KickListBox                        process{ "FUNCTION", processtype };
        ui::KickLabel                          sampleName{ "Sample Name" };
        ui::KickLabel                          sampleText{ "SET SAMPLE NAME" };
        ui::Kick2LineNameLabel                 text{ "" };
        ui::KickNumberWithAttachedValue<int>   beat{ "BEAT" };
        ui::KickNumberWithAttachedValue<int>   ratio{ "RATIO" };
        ui::KickNumberWithAttachedValue<float> originalTempo{ "ORIGINAL TEMPO" };
        ui::KickNumberWithAttachedValue<float> newTempo{ "NEW TEMPO" };
        ui::KickToggleOptionsWithAttachedValue linear{ "", { "Linear", "Log", "Exp" } };
        ui::KickToggleOptionsWithAttachedValue left{ "", { "Left", "Sum", "Right" } };
        ui::KickImageButton                    pointing{ "ARROW", "sEdit_Arrow" };
        ui::KickImageButton                    image{ "" };
        ui::KickImageButton                    warningIcon{ "Warning", "sEdit_Warning" };
        void                                   paint( juce::Graphics& g ) override{};
        void                                   resized() override;
        void                                   close( int );
        std::list<juce::var>                   params;
        int                                    selectedID{ -1 };
    };

    class SampleProcess : public kick_ui::IScreen
        , public juce::Component
    {
    public:
        SampleProcess();
        ~SampleProcess();
        bool requiresControlBar() override { return false; }
        bool requiresStatusBar() override { return false; }

        bool canClose() override { return true; }
        int  getSelectedId() { return selectedId; }
        void paint( juce::Graphics& g ) override;
        void resized() override;
        void openProcessPopup( int index );

    private:
        kick_ui::KickModeHeader header{ "Select Function", "sEdit_Icon" };
        int                     selectedId = -1;
        ui::KickImageButton     discard{ "Discard", "sEdit_discard" };
        ui::KickImageButton     deletebtn{ "Delete", "sEdit_delete" };
        ui::KickImageButton     silence{ "Silence", "sEdit_silence" };
        ui::KickImageButton     extract{ "Extract", "sEdit_extract" };
        ui::KickImageButton     normalize{ "Normalize", "sEdit_normalize" };
        ui::KickImageButton     reverse{ "Reverse", "sEdit_reverse" };
        ui::KickImageButton     fadeIn{ "Fade In", "sEdit_fadein" };
        ui::KickImageButton     fadeOut{ "Fade Out", "sEdit_fadeout" };
        ui::KickImageButton     pitchshift{ "Pitch Shift", "sEdit_pitchshift" };
        ui::KickImageButton     timeStretch{ "Time Stretch", "sEdit_timestretch" };
        ui::KickImageButton     gainChange{ "Gain Change", "sEdit_gainchange" };
        ui::KickImageButton     copy{ "copy", "sEdit_copy" };
        ui::KickImageButton     bitReduce{ "Bit Reduce", "sEdit_bitreduce" };
        ui::KickImageButton     stereoMono{ "Stereo-Mono", "sEdit_steromono" };
        ui::KickLabel           slice{ "Slice" };
        ui::KickLabel           sample{ "Sample" };
        ui::KickButton          cancel{ "Cancel", "Cancel" };
    };

    class AssignSampleToPad : public juce::Component
    {
    public:
        AssignSampleToPad();
        ~AssignSampleToPad() = default;

        int getAssingType() { return assignToList.getSelectedIndex(); }
        int getSelectedPadId() { return padsList.getSelectedIndex(); }
        int getSliceType() { return sliceTypeList.getSelectedIndex(); }
        int getSelectedProgram()
        {
            auto& ae = mt::getAudioInterface();
            return ae.getProgramIndexByName( programList.getSelected() );
        }
        bool isCropSampleEnable() { return cropSampleBtn.getToggleState(); }

    private:
        ui::KickLabel assignTo{ "ASSIGN TO" };
        ui::KickLabel pad{ "PAD" };
        ui::KickLabel sliceType{ "SLICE TYPE" };
        ui::KickLabel cropSample{ "CROP SAMPLE" };
        ui::KickLabel program{ "PROGRAM" };

        ui::KickListBox assignToList{ ui::KickListBox::withoutTitleBar, "", {} };
        ui::KickListBox padsList{ ui::KickListBox::withoutTitleBar, "", {}, 0, 4 };
        ui::KickListBox sliceTypeList{ ui::KickListBox::withoutTitleBar, "", {} };
        ui::KickListBox programList{ ui::KickListBox::withoutTitleBar, "", {}, 0, 2 };

        ui::KickToggleButton cropSampleBtn{ "DISABLED", "ENABLED", "DISABLED" };

        void resized() override;
        void visibilityChanged() override;
    };
}
