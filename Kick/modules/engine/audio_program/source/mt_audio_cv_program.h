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

#include "mt_audio_iprogram.h"
#include "hardware/source/kick_I2c.h"

namespace mt
{
    class MTCVProgram
        : public MTIProgram
        , public MTICvProgramData
    {
    public:
        explicit MTCVProgram( AudIO::DawEngine& Engine, juce::ValueTree& VT, juce::UndoManager* UM = nullptr );
        virtual ~MTCVProgram() = default;

        // CV Program settings...
        inline juce::String getNoteOption() const { return vTree.getProperty( IDs::CVNote, "Last" ); }
        inline int          getPort() const { return vTree.getProperty( IDs::CVPort, 1 ); }
        inline int          getGatePort() const { return vTree.getProperty( IDs::CVGate, 1 ); }
        inline int          getModWheelPort() const { return vTree.getProperty( IDs::CVModWheel, 1 ); }
        inline int          getVelocityPort() const { return vTree.getProperty( IDs::CVVelocity, 100 ); }
        inline int          getStartNote() const { return vTree.getProperty( IDs::CVStartNote, 0 ); }

        // IMidiMsgProcessor
        void noteOn( int Note, int Velocity ) override;
        void noteOff( int Note ) override;
        void noteOffAll() override;
        void modWheel( int Value ) override;

    protected:
    private:
        void refresh();

        void handleNote();
        void setGate( bool On );
        void setNoteVoltage( int Note );
        void setVelocity( int Velocity );

        int findHighestNote();
        int findLowestNote();

    private:
        CV&              cvinterface;
        juce::Array<int> noteOns;
    };

} // namespace MT
