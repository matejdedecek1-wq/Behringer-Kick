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

    class AppSequenceOptionsMenu : public ui::KickMenuComponent
    {
    public:
        AppSequenceOptionsMenu();
        ~AppSequenceOptionsMenu();
        int getSelectedId() { return selectedId; }

    private:
        int selectedId = -1;
    };

    class AppSequenceOptions : public juce::Component

    {
    public:
        //==============================================================================
        AppSequenceOptions();
        ~AppSequenceOptions() = default;

        std::function<void()> onSeqMasterChanged;
        std::function<void()> onClose;
        std::function<void()> refreshSequence;
        void                  handleCommand( CPktMgr::HWMsg_t& HwMsg );

    private:
        ui::KickSerialNumber   srNo{ "SerialNumber", "ID_sequence" };
        AppSequenceDisplay     seq{ -1 };
        AppSequenceOptionsMenu opts;
        ui::KickImageButton    close{ "Close", "Edit_Close" };
        mt::MTAudioInterface&  ae{ mt::getAudioInterface() };

        AppSequenceComponent::enBpmOptions enumBpmState{ AppSequenceComponent::EN_SEQUENCE };
        bool                               isDialogClosed{ true };

        void visibilityChanged() override;
        void paint( juce::Graphics& g ) override;
        void resized() override;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( AppSequenceOptions )
    };
}
