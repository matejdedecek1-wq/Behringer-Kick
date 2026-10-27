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
    class ClipProgramTab : public juce::Component
    {
    public:
        explicit ClipProgramTab( int& SelectedPad, const std::shared_ptr<mt::MTClipProgram>& PClip );
        ~ClipProgramTab() = default;

        void refresh();
        void handleCommand( CPktMgr::HWMsg_t& );

    private:
        void paint( juce::Graphics& g ) override;
        void resized() override;
        void visibilityChanged() override;

    private:
        int&                                      selectedPad;
        mt::MTAudioInterface&                     ae{ mt::getAudioInterface() };
        const std::shared_ptr<mt::MTClipProgram>& pClip;

        ui::KickSemiSlider              semi;
        ui::KickFineSlider              fine;
        ui::KickListBox                 launchQuantize{ "Launch Quantize", mt::Kick::QuantizeStrings, 0, 2 };
        ui::KickPadButtonGroupComponent pads{ ui::KickPadButton::PadButtonType::assignSample };

        std::vector<juce::String> files;
        ui::KickList              fileList{ "Samples" };

        std::shared_ptr<mt::Kick::ScopeTracker> scope;
    };

}
