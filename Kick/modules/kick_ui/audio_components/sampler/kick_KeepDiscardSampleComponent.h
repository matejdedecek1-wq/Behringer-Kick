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
    class KeepDiscardSampleComponent : public juce::Component
    {
    public:
        KeepDiscardSampleComponent( int ButtonState = 0 );
        ~KeepDiscardSampleComponent() = default;

        juce::String getSampleName() { return sampleName.getText(); }
        int          getPadIndex() { return assignPad.getSelectedIndex(); }
        int          getSelectedProgram() { return programs.getSelectedIndex(); }
        int          getSliceSaveTypeIndex() { return createNewProgram.getSelectedIndex(); }

    private:
        ui::KickLabel        sampleName{ "Name Of Sample", "New Sample", true };
        ui::KickListBox      programs{ ui::KickListBox::withoutTitleBar, "", {} };
        ui::KickListBox      assignPad{ ui::KickListBox::withoutTitleBar, "", {} };
        ui::KickNoteSelector rootNote{ "Root Note" };
        ui::KickListBox      createNewProgram{ ui::KickListBox::withoutTitleBar, "", { "Off", "With Non-Destructive Slices", "With Pad Parameters" } };
        ui::KickLabel        programLabel{ "PROGRAM" }, padLabel{ "ASSIGN PAD" }, rootNoteLabel{ "ROOT NOTE" }, createNewPgmLabel{ "CREATE NEW PROGRAM" };

        std::vector<juce::String> padData;
        std::vector<juce::String> progList;
        mt::MTAudioInterface&     ae{ mt::getAudioInterface() };

        int buttonState{ 0 }; //SAMPLE

        void resized() override;
    };
}
