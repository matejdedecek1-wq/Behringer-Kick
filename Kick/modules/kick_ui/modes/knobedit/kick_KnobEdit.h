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
    class KnobEdit : public kick_ui::IScreen
        , public juce::Component
        , private juce::Timer
    {
    public:
        KnobEdit();
        ~KnobEdit();

        bool canClose() override { return true; }
        bool requiresControlBar() override { return false; }
        bool requiresStatusBar() override { return false; }
        bool requiresHandleCommand() override { return true; }

        void         resized() override;
        juce::String getProgramType( mt::MTProgramType type );
        void         visibilityChanged() override;
        void         knobCommand( int knobnum, int value ) override;
        void         handleCommand( CPktMgr::HWMsg_t& ) override;

        void knobLongPress();
        void timerCallback() override;

    private:
        int                     timeinms = 5000;
        mt::KickKnobControl&    knobControl{ mt::getSettings().knobControl };
        kick_ui::KickModeHeader header{ "Q-Link Edit", "Icon_qlink" };
        ui::KickListBox         tracks{ ui::KickListBox::withoutTitleBar, "TRACK", {} };
        ui::KickLabel           label{ "" };
        ui::KickLabel           trackLabel{ "TRACK" };
        ui::KickToggleButton    custom{ "Custom", "Custom", "Custom" };
        ui::KickToggleButton    screen{ "Screen", "Screen", "Screen" };
        KnobGrid                knob;
        Content                 content;
        CustomTab               customContent{ knob };
        juce::Component*        comp{ nullptr };

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( KnobEdit )
    };

}
