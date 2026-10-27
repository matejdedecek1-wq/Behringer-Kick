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
    class SaveSampleDialog : public juce::Component
        , public juce::Button::Listener
    {
    public:
        SaveSampleDialog();
        ~SaveSampleDialog() = default;

        juce::String getSampleName() { return sampleName.getText(); }

    private:
        ui::KickNameLabel sampleName{ "", "New Sample" };
        ui::KickListBox   saveOptions{ "", { "None" } };
        ui::KickButton    ok{ "", "Ok" };
        ui::KickButton    cancel{ "", "Cancel" };

        void resized() override;
        void buttonClicked( juce::Button* ) override;
    };

}
