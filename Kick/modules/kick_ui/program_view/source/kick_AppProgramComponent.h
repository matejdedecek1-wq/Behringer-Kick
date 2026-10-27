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

class AppProgramDisplay : public juce::Component
    , private juce::Timer
    , public juce::AsyncUpdater

{
public:
    std::function<void( int )> onProgChange;

    explicit AppProgramDisplay( int Index );
    ~AppProgramDisplay() = default;

    void                     setProgram( int Prog );
    inline int               getProgNumber( void ) const { return ( progNumber ); };
    inline mt::MTProgramType getProgramType() const { return progType; }

private:
    void updateMidiPorts();
    void refreshDisplay();
    void resized() override;
    void timerCallback() override;
    void handleAsyncUpdate() override;
    void mouseDown( const juce::MouseEvent& e ) override;

private:
    mt::MTAudioInterface&            ae{ mt::getAudioInterface() };
    const std::vector<juce::String>& _midiPorts{ mt::getAudioInterface().getMidiDeviceNames( false ) };
    juce::String                     selMidiPort{ "" };
    mt::MTIProgram*                  MTIProg{ nullptr };

    int progNumber;

    mt::MTProgramType progType;
    ui::KickNameLabel name{ "" };

    ui::KickImageButton route{ "ROUTE", "ROUTE", "ROUTE_active" };
    ui::KickImageButton assign{ "ASSIGN", "ASSIGN", "ASSIGN_active" };
    ui::KickImageButton edit{ "EDIT" };
    ui::KickButton      sampler{ "SAMPLER" };

    ui::KickListBox progCH{ "PROGRAM CH", ui::KickListBox::generateOptions( 1, 127, true ), 0, 1 };
    ui::KickListBox bankMsb{ "BANK MSB", ui::KickListBox::generateOptions( 1, 127, true ), 0, 1 };
    ui::KickListBox bankLsb{ "BANK LSB", ui::KickListBox::generateOptions( 1, 127, true ), 0, 1 };
    ui::KickListBox midiCh{ "MIDI CH", ui::KickListBox::generateOptions( 1, 16, false ), 0, 1 };
    ui::KickListBox midiPort{ ui::KickListBox::withTitleBar, "MIDI OUT", _midiPorts, -1, 2 };

    ui::KickListBox note{ ui::KickListBox::withTitleBar, "NOTE", mt::Kick::CVNoteOptions, 0, 1 };
    ui::KickListBox cvPort{ ui::KickListBox::withTitleBar, "CV PORT", mt::Kick::CVPortNames, 0, 2 };
    ui::KickListBox gatePort{ ui::KickListBox::withTitleBar, "GATE", mt::Kick::CVPortNames, 0, 2 };
    ui::KickListBox modWheel{ ui::KickListBox::withTitleBar, "WHEEL", mt::Kick::CVPortNames, 0, 2 };
    ui::KickListBox velocity{ ui::KickListBox::withTitleBar, "VELOCITY", mt::Kick::CVPortNames, 0, 2 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( AppProgramDisplay )
};

class AppProgramComponent : public juce::Component
{
public:
    explicit AppProgramComponent( int Index );
    ~AppProgramComponent();

    void       setProgram( int Prog );
    inline int getProgNumber( void ) const { return prg.getProgNumber(); }

    inline void           updateData() { prg.triggerAsyncUpdate(); };
    std::function<void()> onPrgEdit;

private:
    void resized() override;
    void visibilityChanged() override;

private:
    ui::KickSerialNumber srNo{ "", "ID_Program" };
    AppProgramDisplay    prg{ -1 };
    ui::KickImageButton  options{ "Options", "editOptions" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( AppProgramComponent )
};
