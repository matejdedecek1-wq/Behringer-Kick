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

#include "mt_audio_cv_program.h"

#if( JUCE_LINUX || __linux__ )
    #include <iostream>
    #include <fstream>
    #include <string>
    #include <stdexcept>
    #include <fcntl.h>
    #include <unistd.h>
#endif

namespace mt
{
    MTCVProgram::MTCVProgram( AudIO::DawEngine& Engine, juce::ValueTree& VT, juce::UndoManager* UM )
        : MTIProgram( VT, UM )
        , MTICvProgramData( VT, UM )
        , cvinterface( CV::getCVInterface() ) {};

    void MTCVProgram::refresh()
    {
        MTIProgram::refresh();
    }

    void MTCVProgram::modWheel( int Value )
    {
        //DBG( "ModWheel Port " << getModWheelPort() << " Value=" << Value );
        cvinterface.setstepVoltage( getModWheelPort(), Value );
    }

    void MTCVProgram::noteOn( int Note, int Velocity )
    {
        // DBG( "NoteON -> " << Note << " Velocity=" << Velocity << " gatePort=" << getPort() );

        // Remove start offset
        auto note = Note - getStartNote();

        // Track note on's
        noteOns.removeAllInstancesOf( note );
        noteOns.add( note );

        // Handle Gate
        setGate( !noteOns.isEmpty() );

        // Handle Note
        handleNote();

        // Handle Velocity
        setVelocity( Velocity );
    }

    void MTCVProgram::noteOff( int Note )
    {
        // DBG( "NoteOff ->" << Note << "CVPort()=" << getGatePort() );

        // Remove start offset
        auto note = Note - getStartNote();

        // Remove note on's
        noteOns.removeAllInstancesOf( note );

        // Handle Gate.
        setGate( !noteOns.isEmpty() );

        // Handle Note.
        handleNote();
    }

    void MTCVProgram::noteOffAll()
    {
        noteOns.clearQuick();
        setGate( false );
        setNoteVoltage( -1 );
        setVelocity( -1 );
    }

    inline void MTCVProgram::handleNote()
    {
        if( noteOns.isEmpty() )
            return;

        auto mode = getNoteOption();
        if( 0 == mode.compareIgnoreCase( "Last" ) )
        {
            setNoteVoltage( noteOns.getLast() );
        }
        else if( 0 == mode.compareIgnoreCase( "Highest" ) )
        {
            setNoteVoltage( findHighestNote() );
        }
        else // if( 0 == mode.compareIgnoreCase( "Lowest" ) )
        {
            setNoteVoltage( findLowestNote() );
        }
    }

    void MTCVProgram::setGate( bool On )
    {
        if( On )
            cvinterface.setVoltageOn( getGatePort() );
        else
            cvinterface.setVoltageOff( getGatePort() );
    }

    void MTCVProgram::setNoteVoltage( int Note )
    {
        if( CommonUtils::isValidMidiNote( Note ) )
            cvinterface.setVoltage( getPort(), (uint8_t)Note );
        else
            cvinterface.setVoltageOff( getPort() );
    }

    void MTCVProgram::setVelocity( int Velocity )
    {
        if( Velocity > 0 )
            cvinterface.setstepVoltage( getVelocityPort(), Velocity );
        else
            cvinterface.setstepVoltage( getVelocityPort(), 0 );
    }

    inline int MTCVProgram::findHighestNote()
    {
        int highestNote = -1;
        for( auto i : noteOns )
        {
            if( i > highestNote )
                highestNote = i;
        }
        return highestNote;
    }

    inline int MTCVProgram::findLowestNote()
    {
        int lowestNote = 128;
        for( auto i : noteOns )
        {
            if( i < lowestNote )
                lowestNote = i;
        }
        return lowestNote;
    }

} // namespace MT
