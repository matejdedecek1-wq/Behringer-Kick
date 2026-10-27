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

struct AppSeqDisplayChangeListener
{
    std::function<void()> onName;
};

class AppSequenceDisplay : public juce::Component
{
public:
    explicit AppSequenceDisplay( int Index );
    ~AppSequenceDisplay() = default;

    std::function<void( int )> onSeqChange = []( int ) {};

    int getSeqNo( void ) const noexcept { return seqNo; };
    int getTempoValue( void ) const noexcept { return bpm.getValue(); }

    void setSequence( int Index );
    void linkRotated( int ID, int Value );
    void updateData();

private:
    int                   seqNo{ -1 };
    mt::MTAudioInterface& ae{ mt::getAudioInterface() };

    std::shared_ptr<mt::Kick::ScopeTracker> scope;

    ui::Kick2LineNameLabel name{ "" };
    ui::KickTempo          bpm{ "BPM", (int)BPM::MIN, (int)BPM::MAX, (int)BPM::DEFAULT };
    ui::KickNumberT<int>   bars{ "BARS", BAR::MIN, BAR::MAX, BAR::DEFAULT };
    ui::KickNumberT<int>   start{ "LOOP START", BAR::MIN, BAR::MAX, BAR::DEFAULT };
    ui::KickNumberT<int>   end{ "LOOP END", BAR::MIN, BAR::MAX, BAR::DEFAULT };
    ui::KickImageButton    loop{ "Loop", "Loop_normal", "Loop_active" };
    ui::KickToggleButton   seqGlobal{ "SEQ/GLO", "GLOBAL", "SEQ" };
    ui::KickRotary         transpose{ "TRANSPOSE", juce::Slider::TextBoxBelow, true };

    void resized() override;
    void mouseDown( const juce::MouseEvent& e ) override;
    void visibilityChanged() override;

    mt::MTISequenceData* pSeq{ nullptr };

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( AppSequenceDisplay )
};

class AppSequenceComponent : public juce::Component
{
public:
    enum enBpmOptions
    {
        EN_SEQUENCE = 0,
        EN_MASTER
    };

    std::function<void()> onSeqEdit          = []() {};

    AppSequenceComponent( int value, juce::String name );
    ~AppSequenceComponent();

    inline void updateData() { seq.updateData(); };

    void setSequence( int SequenceNo );
    int  getSeqNumber( void ) const noexcept { return seq.getSeqNo(); }
    int  getTempoValue( void ) const noexcept { return seq.getTempoValue(); }

    static bool sequenceCreated;
    static bool isSequenceCreated( void ) { return ( sequenceCreated ); }
    inline void linkRotated( int ID, int Value ) { seq.linkRotated( ID, Value ); };

private:
    ui::KickSerialNumber srNo{ "", "ID_sequence" };
    AppSequenceDisplay   seq{ -1 };
    ui::KickImageButton  options{ "Options", "editOptions" };

    void resized() override;
    void visibilityChanged() override;

    void mouseUp( const juce::MouseEvent& e ) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( AppSequenceComponent )
};
