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
#include "kick_ui/dialogs/kick_AppStatusbar.h"

namespace kick_ui
{
    class Sampler : public juce::Component
        , public kick_ui::IScreen
    {
    public:
        enum Buttonstate
        {
            SAMPLE,
            SLICE,
            PADTAP,
            PADHOLD,
        };

        enum PadButtonstate
        {
            PAD_PRESSED,
            PAD_RELEASED,
        };

        Sampler();

        bool canClose() override { return true; }
        bool requiresControlBar() override { return false; }
        bool requiresStatusBar() override { return true; }
        bool requiresHandleCommand() override { return true; }

        void handleCommand( CPktMgr::HWMsg_t& ) override;

        static juce::String getValidFileName( const std::string& );

    private:
        mt::MTAudioInterface& ae{ mt::getAudioInterface() };

        kick_ui::KickModeHeader header{ "Sampler", "Icon_Sampler" };
        ui::KickImageButton     clear{ "Clear", "Song_Clear", "Song_Clear_Active" };
        ui::KickImageButton     exportt{ "Export", "Song_Export", "Song_Export_Active" };

        SamplerRecorder     recorder;
        ui::KickIOComponent options;

        //Sampler Actions
        ui::KickToggleButton sample{ TRANS( "Sample" ), "Sample", "Sample" };
        ui::KickToggleButton slice{ TRANS( "Slice" ), "Slice", "Slice" };
        ui::KickToggleButton padTap{ TRANS( "Pad Tap" ), "Pad Tap", "Pad Tap" };
        ui::KickToggleButton padHold{ TRANS( "Pad Hold" ), "Pad Hold", "Pad Hold" };
        ui::KickButton       cancel{ "Cancel", "Cancel" };
        ui::KickButton       slicePlus{ TRANS( "Slice +" ), "Slice +" };

        ui::KickButton       save{ "Save", " Save" };
        ui::KickToggleButton play{ "Play", "Stop", "Play" };
        ui::KickButton       discard{ "Discard", "Discard" };
        ui::KickButton       edit{ "Edit", "Edit" };

        SamplerRecorder::RecordMode mode = SamplerRecorder::RecordMode::recordModeNone;
        std::list<double>           samplePoints;
        std::map<int, double>       startSamplePoints;
        std::map<int, double>       endSamplePoints;
        static int                  lastPressedPad;
        Buttonstate                 buttonState{ SAMPLE };
        bool                        isSliceRecorded{ false };

        void         recordModeChanged( SamplerRecorder::RecordMode newMode );
        juce::String saveSample( const juce::String& sampleName );

        void paint( juce::Graphics& g );
        void resized() override;
        void visibilityChanged() override;
        void refreshPadsLED();

        void         addSlicesToPads( const juce::String& );
        juce::String saveAndLoadRecordedSample( const juce::String& fileName, int PadIndex, int SelProgram, int SliceSaveType );
        void         openSaveFilePopUp( bool isEditing = false );
        void         addSlicesToSample( const juce::String& );
        void         addSlicesToSample( int Program, const juce::String& SampleFile, bool IsSliceToPad );
        void         addAssetSlice( const juce::String& SampleFile, int LoopStart, int LoopEnd, int Index );
    };
}
