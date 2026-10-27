/*
 * Copyright (c) 2023 MUSIC Tribe
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

    class GridHeader : public juce::Component
    {
        SIGNALS : sigslot::signal<GridToolType> toolSelectionChanged;

        std::function<void( void )> onCutClick;
        std::function<void( void )> onCopyClick;
        std::function<void( void )> onPasteClick;

    public:
        GridHeader();
        virtual ~GridHeader() = default;

        ui::KickListBox tracks{ ui::KickListBox::withTitleBar, "TRACK", {} };
        ui::KickNumber  bars{ "BARS", BAR::MIN, BAR::MAX, BAR::DEFAULT, BAR::STEP };

    private:
        void resized();
        void visibilityChanged() override;

    private:
        ui::KickImageButton                  magnify{ "Magnify", "Magnify", "Magnify" };
        ui::KickImageButton                  pencil{ "Pencil", "Pencil_Normal", "Pencil_Active" };
        ui::KickImageButton                  erase{ "Erase", "Erase_Normal", "Erase_Active" };
        ui::KickImageButton                  select{ "Select", "Select_Normal", "Select_Active" };
        ui::KickImageButton                  copy{ "Copy", "Copy Button", "Copy Button_Active" };
        ui::KickImageButton                  cut{ "Cut", "Cut_Normal", "Cut_Active" };
        ui::KickImageButton                  paste{ "Paste", "Copy_Normal", "Copy_Activ" };
        ui::KickImageButtonWithAttachedValue play{ TRANS( "Play" ), "Play_Grid", "Stop_Active" };

        bool isCutClicked{ false };
        bool isCopyClicked{ false };
        int  selectedNotesSize{ 0 };
    };

} //namespace kick_ui
