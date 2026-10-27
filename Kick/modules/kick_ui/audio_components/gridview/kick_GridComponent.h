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
    class IGridTool;

    ///////////////////////////////////////////////////////////////////////
    // Grid Notes
    ///////////////////////////////////////////////////////////////////////
    class GridNotes : public juce::Component
    {
    public:
        GridNotes();

    private:
        void paint( juce::Graphics& g ) override;

    private:
        mt::MTAudioInterface& ae{ mt::getAudioInterface() };
        const mt::MTSelected& aeSel{ ae.getSelected() };
        GridData&             gridData{ GridData::Instance() };
    };

    ///////////////////////////////////////////////////////////////////////
    // Grid Component
    ///////////////////////////////////////////////////////////////////////
    class GridComponent : public juce::Component
        , public juce::AsyncUpdater
    {
    public:
        GridComponent( juce::Viewport& ViewPort );
        ~GridComponent();

        void noteSelectionChanged( int note );
        void setPosition( int bar, int beat, int tick );

        void mouseUp( const juce::MouseEvent& event ) override;
        void mouseMove( const juce::MouseEvent& event ) override;
        void mouseDrag( const juce::MouseEvent& event ) override;
        void mouseDown( const juce::MouseEvent& event ) override;
        void mouseDoubleClick( const juce::MouseEvent& event ) override;

        int                 getMinimumWidth() { return minimumWidth; }
        void                toolTypeChanged( GridToolType toolType );
        void                setGridViewType( GridViewType newGridViewType ) { viewType = newGridViewType; }
        void                insertTimeSig( int bar, int num, int den );
        void                deleteTimeSig( int bar );
        float               playHeadXPos = 0;
        bool                isPlaying{ false };
        void                setEditNoteStart( bool state ) { noteStartEdit = state; }
        void                setEditNoteEnd( bool state ) { noteEndEdit = state; }
        void                setTranspose( bool state ) { transpose = state; }
        bool                getEditNoteStart() { return noteStartEdit; }
        bool                getEditNoteEnd() { return noteEndEdit; }
        bool                getTranspose() { return transpose; }
        int                 getMouseDownNote();
        juce::TooltipWindow infoBar;

    private:
        void paint( juce::Graphics& g ) override;
        void resized() override;
        void visibilityChanged() override;
        void handleAsyncUpdate() override;

    private:
        //to convert to multi dimensional array for faster lookup
        mt::MTAudioInterface& ai{ mt::getAudioInterface() };
        const mt::MTSelected& aiSel{ ai.getSelected() };
        GridData&             gridData{ GridData::Instance() };
        GridNotes             notes;
        IGridTool*            activeTool{ nullptr };
        juce::Viewport&       viewPort;

        int noteSelected = 0;
        int minimumWidth = 0;
        int currentBar   = 0;
        int currentBeat  = 0;
        int currentTick  = 0;

        std::map<GridToolType, IGridTool*> tools;

        GridViewType viewType;

        bool noteStartEdit{ false };
        bool noteEndEdit{ false };
        bool transpose{ false };
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( GridComponent )
    };
} //namespace mt
