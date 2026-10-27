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
constexpr int   padButtonSpacer = 12;
constexpr float borderThickness = 1.0f;

// Group Numbers.
const static inline std::vector<juce::String> groupNumbers = {
    "TrackMute_None",
    "TrackMute_1",
    "TrackMute_2",
    "TrackMute_3",
    "TrackMute_4",
    "TrackMute_5",
    "TrackMute_6",
    "TrackMute_7",
    "TrackMute_8",
    "TrackMute_9",
    "TrackMute_10",
    "TrackMute_11",
    "TrackMute_12",
    "TrackMute_13",
    "TrackMute_14",
    "TrackMute_15",
    "TrackMute_16"
};
const static inline std::vector<juce::String> groupActiveNumbers = {
    "TrackMute_None_Active",
    "TrackMute_1_Active",
    "TrackMute_2_Active",
    "TrackMute_3_Active",
    "TrackMute_4_Active",
    "TrackMute_5_Active",
    "TrackMute_6_Active",
    "TrackMute_7_Active",
    "TrackMute_8_Active",
    "TrackMute_9_Active",
    "TrackMute_10_Active",
    "TrackMute_11_Active",
    "TrackMute_12_Active",
    "TrackMute_13_Active",
    "TrackMute_14_Active",
    "TrackMute_15_Active",
    "TrackMute_16_Active"
};

namespace kick_ui
{
    class KickTrackMuteTab;
    class KickTrackMute : public kick_ui::IScreen
        , public juce::Component
    {
    public:
        KickTrackMute();

        bool canClose() { return true; }
        bool requiresControlBar() override { return false; }
        bool requiresStatusBar() override { return true; }
        bool requiresHandleCommand() override { return true; }

        void handleCommand( CPktMgr::HWMsg_t& ) override;

    private:
        AppStatusbar&             statusBar{ AppStatusbar::Instance() };
        KickModeHeader            header{ "Track Mute", "Icon_TrackMute" };
        HeaderBarSequenceSelector hdrComp;

        std::unique_ptr<KickTrackMuteTab> tab;

        ui::KickToggleButton trackMute{ "Track Mute", "Track Mute", "Track Mute" };
        ui::KickToggleButton trackGroup{ "Track Group", "Track Group", "Track Group" };
        ui::KickLabel        sequenceLabel{ "SEQUENCE" };

        juce::Rectangle<int> rect = { 10, 86, 342, 396 };

        void paint( juce::Graphics& g ) override;
        void resized() override;
        void visibilityChanged() override;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( KickTrackMute )
    };

    class KickTrackMuteTab : public juce::Component
    {
    public:
        KickTrackMuteTab();
        void showMiniMap();
        void showGroups();
        void changeBank( juce::String bank );
        void refreshData();
        void refreshAllLEDs();
        void assignKnobToPad( int knobId, int value );
        void knobCommand( int knob, int value );

    private:
        mt::MTAudioInterface& ae{ mt::getAudioInterface() };
        bool                  isMiniMap{ true };

        int                                              tracksMax{ 0 };
        ui::KickPadMiniMap                               miniMap;
        ui::KickListWithImage                            groups{ "OPTIONS", groupNumbers, groupActiveNumbers, 0, 2 };
        std::vector<mt::MTITrackData*>                   list;
        std::unique_ptr<ui::KickPadButtonGroupComponent> pads;

        ui::KickToggleButton soloBtn{ "Solo", "Solo", "Solo" };
        ui::KickButton       clearSoloBtn{ "Clear Solos" };
        ui::KickToggleButton instantAndTD{ "Instant", "Time Division", "Instant" };
        ui::KickListBox      timeDivList{ ui::KickListBox::withoutTitleBar, "TIME DIVISION", { "1/16", "1/8", "1/4", "1/2", "1" }, 2 };

        void displayPadState( const int Id, const juce::String& text, const bool state, int muteGroup );
        void resized() override;
        void visibilityChanged() override;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( KickTrackMuteTab )
    };

    class TrackMuteSettings : public juce::Component
    {
    public:
        TrackMuteSettings();

    private:
        ui::KickLabel        label1{ "Instant Track Mute" };
        ui::KickLabel        label2{ "Play Track Mute and Solo Events" };
        ui::KickLabel        label3{ "Record Track Mute and Solo Events" };
        ui::KickToggleButton instantTrkMute{ "Off", "On", "Off" };
        ui::KickToggleButton playTrkMuteSoloEvents{ "Off", "On", "Off" };
        ui::KickToggleButton recordTrkMuteSoloEvents{ "Off", "On", "Off" };

        void resized() override;
    };
}
