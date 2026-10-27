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

static sigslot::signal<>         gridNeedsRepaint;
static sigslot::signal<int, int> gridNeedsToMove;

namespace kick_ui
{
    class IGridTool
        : public juce::MouseListener
        , public juce::KeyListener
    {
    public:
        explicit IGridTool( GridComponent& gridComponent, GridData& data, GridToolType Type, juce::Viewport& GridViewPort )
            : gridComp( gridComponent ), gridData( data ), type( Type ), viewPort( GridViewPort )
        {
            gridComp.addKeyListener( this );
        }

        virtual ~IGridTool()
        {
            gridComp.removeKeyListener( this );
        }

        virtual bool keyPressed( const juce::KeyPress& key, juce::Component* /*originatingComponent*/ ) { return false; }
        virtual void paint( juce::Graphics& g ) = 0;

        inline GridToolType getType() { return type; }

        virtual void mouseDown( const juce::MouseEvent& event )
        {
            mDownNoteNum      = gridData.getNoteFromY( event.getMouseDownY() );
            mDownTime         = gridData.getTimeFromX( event.getMouseDownX(), false );
            mDownNoteID       = ae.getNoteID( mDownNoteNum, mDownTime, -1 );
            mDownSelectedNote = ( 0 < ae.getSelectedNoteID( mDownNoteNum, mDownTime, -1 ) );
        }
        inline int getMouseDownNoteNumber() { return mDownNoteNum; }

    protected:
        mt::MTAudioInterface& ae{ mt::getAudioInterface() };
        const mt::MTSelected& aeSelected{ mt::getAudioInterface().getSelected() };
        GridComponent&        gridComp;
        GridData&             gridData;
        juce::Viewport&       viewPort;

        int  mDownTime         = 0;
        int  mDownNoteID       = 0;
        int  mDownNoteNum      = 0;
        bool mDownSelectedNote = false;

    private:
        const GridToolType type;
    };
} // namespace mt
