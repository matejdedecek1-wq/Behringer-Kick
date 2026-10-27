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

    class ProgEditInterface : public juce::Component
    {
    public:
        std::function<void()> onProgramChanged;

        explicit ProgEditInterface( mt::MTProgramType Type )
            : type( Type ) {}

        mt::MTProgramType getType() const { return type; }
        juce::String      getProgramName() { return pIProg ? pIProg->getName() : ""; }
        virtual void      setProgram( int Index )
        {
            idx    = Index;
            pIProg = ae.getProgram( idx );

            if( onProgramChanged )
                onProgramChanged();

            refresh();
        }

        virtual void refresh()                                   = 0;
        virtual void setupHeader( kick_ui::KickModeHeader& Hdr ) = 0;
        virtual void handleCommand( CPktMgr::HWMsg_t& ) {}

    protected:
        mt::MTAudioInterface& ae{ mt::getAudioInterface() };
        const mt::MTSelected& aeSel{ ae.getSelected() };
        juce::ValueTree       state;
        mt::MTIProgram*       pIProg{ nullptr };

    private:
        int                     idx{ -1 };
        const mt::MTProgramType type;
    };

}
