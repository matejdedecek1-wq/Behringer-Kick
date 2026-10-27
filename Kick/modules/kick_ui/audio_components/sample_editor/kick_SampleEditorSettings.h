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

        class SampleEditorSettings : public juce::Component
        {
        public:
            SampleEditorSettings( SampleEditorData& seData );
            ~SampleEditorSettings() = default;

            bool hasTimeLineUnitsChanged();
            bool hasAutoScrollChanged();
            bool hasCuePreviewChanged();
            bool hasSlicePreciewChanged();

            SampleEditorData::TimeLineUnits getTimeLineUnits() { return tUnits; }
            SampleEditorData::AutoScroll    getAutoscroll() { return aScroll; }
            SampleEditorData::CuePreview    getCuePreview() { return cuePreview; }
            SampleEditorData::SlicePreview  getSlicePreview() { return sPreview; }

        private:
            void resized() override;

            ui::KickLabel cuePlayLabel{ "CUE PLAY MODE" };
            ui::KickLabel cuePreviewLabel{ "CUE PREVIEW" };
            ui::KickLabel slicePreviewLabel{ "SLICE PREVIEW" };
            ui::KickLabel slicePreview1Label{ "SLICE PREVIEW" };
            ui::KickLabel autoScrollLabel{ "AUTO SCROLL" };
            ui::KickLabel timeLineUnitsLabel{ "TIMELINE UNITS" };

            ui::KickToggleButton toggleOneShot{ "Toggle", "One Shot", "Toggle" };
            ui::KickListBox      cuePreviewList{ ui::KickListBox::withoutTitleBar, "", { "Off", "Before", "After" } };
            ui::KickListBox      slicePreviewList{ ui::KickListBox::withoutTitleBar, "", { "Off", "Before", "After" } };
            ui::KickListBox      slicePreview1List{ ui::KickListBox::withoutTitleBar, "", { "Off", "Before", "After" } };
            ui::KickListBox      autoScrollList{ ui::KickListBox::withoutTitleBar, "", { "Off", "Follow", "Page" } };
            ui::KickListBox      timeLineUnitsList{ ui::KickListBox::withoutTitleBar, "", { "Time", "Samples", "Beats" } };

            SampleEditorData&               seData;
            SampleEditorData::TimeLineUnits tUnits{ seData.getTimeLineUnits() };
            SampleEditorData::AutoScroll    aScroll{ seData.getAutoScroll() };
            SampleEditorData::SlicePreview  sPreview{ seData.getSlicePreview() };
            SampleEditorData::CuePreview    cuePreview{ seData.getCuePreview() };
        };

    }
}
