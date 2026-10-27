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
    class StepSequencerControl : public juce::Component
    {
    public:
        StepSequencerControl();
        ~StepSequencerControl() = default;

        void resized() override;

    public:
        ui::KickToggleButton trkVolumeButton{ TRANS( "TRK VOLUME" ) };
        // nudge
        ui::KickImageButton leftNudgeButton{ TRANS( "-" ), "Minus", "Minus_Active" };
        ui::KickLabel       nudgeText{ "Nudge" };
        ui::KickImageButton rightNudgeButton{ TRANS( "+" ), "Plus", "Plus_Active" };

        ui::KickLabel       barLabel{ "Bar" }, padLabel{ "Pad" };
        ui::KickImageButton barMinus{ TRANS( "-" ), "Minus", "Minus_Active" };
        ui::KickImageButton barPlus{ TRANS( "+" ), "Plus", "Plus_Active" };
        ui::KickImageButton padMinus{ TRANS( "-" ), "Minus", "Minus_Active" };
        ui::KickImageButton padPlus{ TRANS( "+" ), "Plus", "Plus_Active" };

        ui::KickToggleButton clearpad{ "Clear Pad", "Clear Pad", "Clear Pad" };
        ui::KickToggleButton clearbar{ "Clear Bar", "Clear Bar", "Clear Bar" };
        ui::KickToggleButton padSelect{ "Pad Select", "Pad Select", "Pad Select" };

        int totalBars = 1;
        int rangeLow  = 1;
        int rangeHigh = 16;

    private:
        mt::MTAudioInterface& ae{ mt::getAudioInterface() };
        const mt::MTSelected& aeSel{ mt::getAudioInterface().getSelected() };
        GridData&             gridData{ GridData::Instance() };

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( StepSequencerControl )
    };

    class StepSequencerTopControl : public juce::Component
    {
    public:
        StepSequencerTopControl();
        ~StepSequencerTopControl() = default;

        std::function<void()> pageChanged;
        std::function<void()> barChanged;
        std::function<void()> padChanged;

        void paint( juce::Graphics& g ) override;
        void resized() override;

        void                setTotalBars( int bars );
        void                setMaxPages( int pages );
        void                setPageEnable( bool State ) { page.setEnabled( State ); }
        const juce::String& getTimeDivisionString() { return timeDivValueListBox.getSelected(); }
        int                 getTimeDivision();
        inline int          getPage() { return page.getSelectedIndex(); };
        inline int          getBar() { return barValueListBox.getSelectedIndex(); };

        void setSelectedPad( int index );
        void setSelectedPage( int index );

    public:
        ui::KickToggleButton trkVolumeButton{ TRANS( "TRK VOLUME" ) };
        ui::KickListBox      paramValueListBox{ ui::KickListBox::BoxType::withoutTitleBar, "Parameter", { "Velocity", "Probability", "Ratchet" } };
        ui::KickListBox      timeDivValueListBox{ ui::KickListBox::BoxType::withoutTitleBar, "Time Div", mt::Kick::TimeDivisionStrings, 0, 4 };
        // Pad
        ui::KickListBox padValueListBox{ ui::KickListBox::BoxType::withoutTitleBar, "Pad", { "A01" }, 0, 8 };
        // Bar
        ui::KickListBox barValueListBox{ ui::KickListBox::BoxType::withoutTitleBar, "Bar", { "1" }, 0, 4 };

    private:
        ui::KickListBox page{ ui::KickListBox::BoxType::withoutTitleBar, "Page", { "1" }, 0, 2 };
        ui::KickButton  pageLeft{ "<" };
        ui::KickButton  pageRight{ ">" };

    private:
        //labels
        ui::KickLabel pad{ "PAD" };
        ui::KickLabel parameter{ "PARAMETER" };
        ui::KickLabel timediv{ "TIME DIV" };
        ui::KickLabel bar{ "BAR" };
        ui::KickLabel pageLabel{ "PAGE" };

        std::vector<juce::String> padData, barsData;

        int totalBars = 1;
        int rangeLow  = 1;
        int rangeHigh = 16;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( StepSequencerTopControl )
    };
}
