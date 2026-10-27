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
    class SaveFile : public juce::Component
    {
    public:
        SaveFile( const juce::String FileName );
        ~SaveFile() = default;

    private:
        ui::KickLabel fileName{ "Enter Text" };

        void resized() override;
    };

    class KickSongTableDisplay : public juce::Component
    {
    public:
        KickSongTableDisplay();
        ~KickSongTableDisplay() = default;

        std::function<void( int )> onRepeatChanged;
        std::function<void()>      onSequenceChanged;

        void                             refreshStep( const mt::MTSongStepInfo& Step );
        inline const mt::MTSongStepInfo& getStep() { return step; };

    private:
        mt::MTSongStepInfo                   step;
        ui::KickLabel                        seqName{ "" };
        ui::KickNumberWithAttachedValue<int> repeat{ "REPEAT", 1, 128, 1, 1, false };
        ui::KickNumberWithAttachedValue<int> bpm{ "BPM", BPM::MIN, BPM::MAX, BPM::DEFAULT, BPM::STEP, false };
        ui::KickNumberWithAttachedValue<int> bars{ "BARS", 1, 64, 4, 1, false };

        void resized() override;
    };

    class KickSongContent : public juce::Component
    {
    public:
        KickSongContent();

        void        updateSong( const juce::String& Song );
        inline void refreshTable() { tableView.refresh(); };

        mt::MTSongInfo getSongInfo( const juce::String& Song ) { return ae.getSongInfo( Song ); }

        void playOrResume();
        void stopOrPause();
        void refreshSequenceList();
        void seqSongState( bool state );
        void setBpmValue( float BPM );
        void displayState( juce::String& song );

    private:
        struct Header : public juce::Component
        {
            ui::KickLabel sequenceLabel{ "SEQ" };
            ui::KickLabel repeatLabel{ "Repeat" };
            ui::KickLabel barLabel{ "BARS" };
            ui::KickLabel bpmLabel{ "BPM" };

            Header()
            {
                mt::utils::setValueJustification( juce::Justification::centred, sequenceLabel, bpmLabel, barLabel, repeatLabel );
                mt::utils::setValueBackgroundColour( ui::Theme::backgroundColour( 9 ), sequenceLabel, bpmLabel, barLabel, repeatLabel );
                mt::utils::setFontHeight( 17, sequenceLabel, bpmLabel, barLabel, repeatLabel );
                mt::utils::addAndMakeVisible( this, sequenceLabel, repeatLabel, barLabel, bpmLabel );
            }
            void resized() override;
            void paint( juce::Graphics& g ) override
            {
                mt::utils::fillRoundedRect( g, getLocalBounds(), ui::Theme::backgroundColour( 1 ), 5.f, true, true, false, false );
            }
        };

        mt::MTAudioInterface&                            ae;
        std::unique_ptr<ui::KickPadButtonGroupComponent> pads;

        ui::KickTableViewProvider provider;
        ui::KickTableView         tableView{ "SongMode", provider };

        mt::MTSongInfo info;

        juce::String                       songName;
        std::vector<mt::MTISequenceData*>  seqList;
        std::vector<KickSongTableDisplay*> rowComps;

        ui::KickButton convert{ "Convert >SEQ" };
        ui::KickButton clr{ "Clear" };
        ui::KickButton insert{ "Insert" };
        ui::KickButton del{ "Delete" };

        int   selectedPadSeq{ -1 };
        int   selectedId{ -1 };
        bool  isPlaying{ false };
        bool  isSeqSong{ false };
        float bpmValue{ BPM::DEFAULT };

        void resized() override;
        void visibilityChanged() override;
        void setSequenceToPlay( int, int );
        void setPadLED( const int id, int state, int color );
        void paint( juce::Graphics& ) override;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( KickSongContent )
    };

    class KickSongMode : public kick_ui::IScreen
        , public juce::Component
        , public juce::AsyncUpdater
    {
    public:
        KickSongMode();

        bool canClose() { return true; }
        bool requiresControlBar() override { return false; }
        bool requiresStatusBar() override { return true; }
        bool requiresHandleCommand() override { return true; }

        void handleCommand( CPktMgr::HWMsg_t& f_HwMsg ) override;
        void knobCommand( int knob, int value );

    private:
        void handleAsyncUpdate() override;
        void resized() override;
        void visibilityChanged() override;
        void warningPopup();

        void setPlayState( int Type );

    private:
        KickSongContent          content;
        mt::MTAudioInterface&    ae{ mt::getAudioInterface() };
        kick_ui::MetronomePopup& metronome{ kick_ui::MetronomePopup::Instance() };
        kick_ui::KickModeHeader  header{ "Song Editor", "Icon_SongEdit" };
        ui::KickImageButton      exportt{ "Export", "Song_Export", "Song_Export_Active" };
        ui::KickListBox          songListBox{ ui::KickListBox::BoxType::withTitleBar, "Song Name", { "CREATE NEW SONG" }, 0, 3 };
        ui::KickNumberf          bpmNumber{ "BPM", BPM::MIN, BPM::MAX, BPM::DEFAULT, BPM::STEP, true };
        ui::KickToggleButton     seqSongBpm{ "Seq", "Song", "Seq" };
        int                      bpmValue;
        bool                     state{ false };

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( KickSongMode )
    };

}
