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

    //////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////    Header Bar - Track Selector
    //////////////////////////////////////////////////////////////////////////////////////////
    class HeaderBarTrackSelector : public juce::Component
    {
    public:
        ui::KickListBox tracks{ ui::KickListBox::withoutTitleBar, "Track", {} };

        HeaderBarTrackSelector();

    private:
        void resized();
        void visibilityChanged() override;
    };

    //////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////    Header Bar - Sequence Selector
    //////////////////////////////////////////////////////////////////////////////////////////
    class HeaderBarSequenceSelector : public juce::Component
    {
    public:
        ui::KickImageButton icon{ "Program" };
        ui::KickListBox     sequence{ ui::KickListBox::withoutTitleBar, "", {} };

        HeaderBarSequenceSelector();

    private:
        void resized();
        void visibilityChanged() override;
    };

    //////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////    Header Bar - Program Selector
    //////////////////////////////////////////////////////////////////////////////////////////
    class HeaderBarProgramSelector : public juce::Component
    {
    public:
        std::function<void( const juce::String& Program, const mt::MTProgramType Type )> onSelectionChanged;

        ui::KickImageButton                  icon{ "Program" };
        ui::KickLabel                        title{ "Program" };
        std::unique_ptr<KickProgramSelector> selector;

        HeaderBarProgramSelector();

    private:
        void resized();
        void visibilityChanged() override;
    };

    //////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////    Header Bar - Program Selector
    //////////////////////////////////////////////////////////////////////////////////////////
    class HeaderBarProgramAndTrackSelector : public juce::Component
    {
    public:
        ui::KickImageButton icon{ "Program" };
        ui::KickListBox     programs{ ui::KickListBox::withoutTitleBar, "PROGRAM", {} };
        ui::KickListBox     tracks{ ui::KickListBox::withoutTitleBar, "TRACK", {} };
        HeaderBarProgramAndTrackSelector();

    private:
        void resized();
        void visibilityChanged() override;
    };

}
