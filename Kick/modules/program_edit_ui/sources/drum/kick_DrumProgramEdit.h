/*
 * Copyright (c) 2024 MUSIC Tribe
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

#define DRUM_NAV_BUTTONS ( 5 )

    class DrumHeader : public juce::Component

    {
    public:
        DrumHeader();

        ui::KickImageButton flattenPads{ "FLATTEN PADS", "PgmEdit_FP" };
        ui::KickImageButton editZone{ "EDIT ZONE", "PgmEdit_EZ" };
        ui::KickImageButton convertToKG{ "CONVERT TO KG", "PgmEdit_ConvertKG" };

        ui::KickListBox pad{ "PAD" };

    private:
        void resized() override;
        void paint( juce::Graphics& ) override;
    };

    class DrumProgramEdit : public ProgEditInterface
        , public juce::Button::Listener
    {
    public:
        DrumProgramEdit();

        void refresh() override;
        void setupHeader( KickModeHeader& Hdr ) override{};
        void handleCommand( CPktMgr::HWMsg_t& ) override;

        juce::Component* getHeaderComponent() { return &hdrComp; }
        DrumHeader       hdrComp;

        enum ViewType
        {
            firstTime,
            secondTime
        };

    private:
        void resized() override;
        void visibilityChanged() override;
        void buttonClicked( juce::Button* button ) override;

    private:
        ui::KickToggleButton mainButton{ "Main", "Main", "Main" };
        ui::KickToggleButton samplesButton{ "Samples", "Samples", "Samples" };
        ui::KickToggleButton layerMixButton{ "Layer Mix", "Layer Mix", "Layer Mix" };
        ui::KickToggleButton envelopesButton{ "Envelopes", "Envelopes", "Envelopes" };
        ui::KickToggleButton lfoButton{ "LFO", "LFO", "LFO" };

        int                                           sIdx{ 0 };
        mt::MTDrumProgram*                            pDrum{ nullptr };
        std::vector<int>                              pads;
        std::vector<std::unique_ptr<juce::Component>> comps;
        int                                           tIndex{ 0 }, padTab{ 1 };
        bool                                          isDefault{ false }, isMain{ true }, ispadFx{ false };

        std::unique_ptr<juce::Drawable> mainImage;
    };
}
