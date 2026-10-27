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

namespace kick_ui
{
    class AppTrackOptionsMenu : public ui::KickMenuComponent
    {
    public:
        AppTrackOptionsMenu();
        ~AppTrackOptionsMenu();
        int getSelectedId() { return selectedId; }

    private:
        int selectedId = -1;
    };

    class AppTrackOptions : public juce::Component
    {
    public:
        //==============================================================================
        AppTrackOptions();
        ~AppTrackOptions() = default;

        std::function<void()> onSoloChanged;
        std::function<void()> onClose;
        void                  handleCommand( CPktMgr::HWMsg_t& HwMsg );

        ui::KickImageButton close{ "Close", "Edit_Close" };

    private:
        ui::KickSerialNumber  srNo{ "SerialNumber", "" };
        AppTrackDisplay       track;
        AppTrackOptionsMenu   opts;
        mt::MTAudioInterface& ae{ mt::getAudioInterface() };

        bool isDialogClosed{ true };

        void visibilityChanged() override;
        void paint( juce::Graphics& g ) override;
        void resized() override;

        void openWarningPopUp( const juce::String& );

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( AppTrackOptions )
    };
}
