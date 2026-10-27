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
    enum CustomButtons
    {
        mix = 0,
        program,
        ins1,
        ins2,
        ins3,
        ins4,
        midi
    };

    class Content : public juce::Component
    {
    public:
        Content();
        ~Content();

    private:
        void resized() override;
        void paint( juce::Graphics& g ) override;
    };

    class BottomContent : public juce::Component
    {
    public:
        BottomContent();
        ~BottomContent() = default;
        ui::KickListBox level{ ui::KickListBox::withoutTitleBar, "Level", {} };

    private:
        void                 paint( juce::Graphics& g ) override;
        void                 resized() override;
        ui::KickLabel        parameter{ "Parameter" };
        ui::KickToggleButton momentary{ "MOMENTARY", "MOMENTARY", "MOMENTARY" };
    };

    class TopContent : public juce::Component
    {
    public:
        std::function<void()>           isPad;
        std::function<void( int )>      onBankClick;
        std::function<void( int )>      onPadClick;
        std::function<void( int, int )> onPadChange;
        TopContent();
        ~TopContent() = default;
        void                 padContent();
        void                 click( mt::MTProgramType types );
        ui::KickToggleButton type{ "PROGRAM", "PAD", "PROGRAM" };
        void                 padSelect( int index );

    private:
        void            resized() override;
        void            paint( juce::Graphics& g ) override;
        ui::KickLabel   typeLabel{ "TYPE" };
        ui::KickListBox bank{ ui::KickListBox::withTitleBar, "BANK", mt::Kick::padBankNames };
        ui::KickListBox pad{ ui::KickListBox::withTitleBar, "PAD", mt::Kick::midiOutChannels };
    };

    class MiddleContent : public juce::Component
    {
    public:
        std::function<void( CustomButtons )> onButtonClick;
        MiddleContent();
        ~MiddleContent() = default;
        void updateButtons( mt::MTProgramType type );

    private:
        void                 resized() override;
        void                 paint( juce::Graphics& g ) override;
        void                 visibilityChanged() override;
        void                 onClick( CustomButtons button );
        ui::KickLabel        customLabel;
        ui::KickToggleButton midi{ "MIDI CC", "MIDI CC", "MIDI CC" };
        ui::KickToggleButton mix{ "Mix", "Mix", "Mix" };
        ui::KickToggleButton program{ "Program", "Program", "Program" };
        ui::KickToggleButton ins1{ "Ins 1", "Ins 1", "Ins 1" };
        ui::KickToggleButton ins2{ "Ins 2", "Ins 2", "Ins 2" };
        ui::KickToggleButton ins3{ "Ins 3", "Ins 3", "Ins 3" };
        ui::KickToggleButton ins4{ "Ins 4", "Ins 4", "Ins 4" };
    };

    class CustomTab : public juce::Component
    {
    public:
        CustomTab( KnobGrid& knob );
        ~CustomTab() = default;
        void          resized() override;
        TopContent    topControls;
        MiddleContent middleControls;
        void          type( CustomButtons buttonName );
        void          setSelectedKnob( int index ) { knobnum = index; }
        void          setIndex( int index );
        void          setInsertsList( int insertIndex );
        void          setListInfo( mt::MTProgramType type, CustomButtons buttonName, bool topState );

    private:
        KnobGrid&     knob;
        BottomContent bottomControls;
        int           knobnum{ -1 }, indexNum{ -1 };
        void          setParameter( int knobnum, juce::String parameter );
    };

}
