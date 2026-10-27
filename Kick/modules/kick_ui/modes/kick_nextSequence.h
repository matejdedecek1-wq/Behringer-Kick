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
    struct SequenceData
    {
        juce::String srNo{ "" };
        juce::String name{ "" };
        int          length{ 0 };
        int          bpm{ 0 };
        int          bars{ 1 };
        int          repeat{ 1 };
        int          index{ 0 };
    };

    class ObservableInt
    {
    public:
        std::function<void( int )> onValueChanged;
        ObservableInt( int initialValue = 0 )
            : value( initialValue ) {}

        ObservableInt& operator=( int newValue )
        {
            value = newValue;
            if( onValueChanged )
                onValueChanged( newValue );
            return *this;
        }

        operator int() const { return value; }

    private:
        int                        value;
        std::function<void( int )> onValueChange;
    };

    class KickNextSequenceDisplay : public juce::Component
    {
    public:
        KickNextSequenceDisplay( SequenceData data );
        ~KickNextSequenceDisplay() {}

        void updateData( SequenceData data );

    private:
        ui::KickLabel                        srNo{ "" };
        ui::KickLabel                        seqName{ "" };
        ui::KickNumberWithAttachedValue<int> length{ "LENGTH", 1, 120, 4, 1, false };
        ui::KickNumberWithAttachedValue<int> bpm{ "BPM", 20, 240, 4, 1, false };
        SequenceData                         seqData;

        void resized() override;
    };

    class KickNextSequenceContent : public juce::Component
        , public mt::MTAudioInterfaceListener
    {
    public:
        KickNextSequenceContent();
        ~KickNextSequenceContent();

        void onPlay();
        void onStop();
        void handleCommand( CPktMgr::HWMsg_t& );
        void enableRowSelection( bool State );

    private:
        mt::MTAudioInterface&                            ae;
        ui::KickTableViewProvider                        provider;
        std::unique_ptr<ui::KickTableView>               tableView;
        std::unique_ptr<ui::KickPadButtonGroupComponent> pads;
        SequenceData                                     seqData;

        ui::KickToggleButton nextBar{ "NextT Bar", "Next Bar", "Next Bar" };
        ui::KickToggleButton sudden{ "Sudden", "Sudden", "Sudden" };
        ui::KickToggleButton clear{ "Clear", "Clear", " Clear" };
        ui::KickToggleButton hold{ "Hold", "Hold", "Hold" };

        ui::KickLabel srNo{ "#" }, sequence{ "Sequence" }, length{ "Length" }, bpm{ "BPM" };

        int playingSequence{ 0 };

        ObservableInt nextSequence{ -1 };

        bool isPlaying{ false };

        void onSequenceSelectionChange( const mt::MTSelected& Selected ) override;

        void paint( juce::Graphics& g ) override;
        void resized() override;
        void visibilityChanged() override;

        void updateData();
        void selectSequence( int Index );

        void changePlayingPad( int OldPlayingPad, int NewPlayingPad );
        void clearNextSequence();
        void startPlaying();
        void stopPlaying();
        void restoreData();

        void setPadLED( const int id, int state, int color );
        void setPadBlinking( const int id, int state, int OnColorID, int OffColorID = contracts::DeviceControlContracts::PadColourIds::PadColourTrans );

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( KickNextSequenceContent )
    };

    class KickNextSequenceMode : public kick_ui::IScreen
        , public juce::Component
    {
    public:
        KickNextSequenceMode();

        bool requiresControlBar() override { return false; }
        bool requiresStatusBar() override { return true; }
        bool requiresHandleCommand() override { return true; }

        void handleCommand( CPktMgr::HWMsg_t& f_HwMsg ) override;

        void knobCommand( int knob, int value );

        bool canClose() { return true; }

    private:
        kick_ui::KickModeHeader  header{ "Next Sequence", "Icon_NextSeq" };
        KickNextSequenceContent  content;
        mt::MTAudioInterface&    ae{ mt::getAudioInterface() };
        void                     resized() override;
        kick_ui::MetronomePopup& metronome{ kick_ui::MetronomePopup::Instance() };

        void visibilityChanged() override;
        void setPlayState( int );

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( KickNextSequenceMode )
    };
}
