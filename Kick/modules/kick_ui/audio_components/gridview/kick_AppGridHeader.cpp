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
#include "contracts/dcl/dcl_contracts_helper.h"
#include "contracts/dcl/dcl_signals.h"

namespace kick_ui
{
    GridHeader::GridHeader()
    {
        tracks.setJustificationType( juce::Justification::centredLeft );

        auto noHighlight = ui::Theme::highlightColour( 0 );
        auto highlight   = ui::Theme::highlightColour( 1 );

        pencil.setRadioGroupId( (int)RadioGroupId::AppGridHeader, juce::dontSendNotification );
        erase.setRadioGroupId( (int)RadioGroupId::AppGridHeader, juce::dontSendNotification );
        select.setRadioGroupId( (int)RadioGroupId::AppGridHeader, juce::dontSendNotification );
        magnify.setRadioGroupId( (int)RadioGroupId::AppGridHeader, juce::dontSendNotification );

        mt::utils::addChildComponents( this, tracks, pencil, erase, select, copy, play, paste, cut, bars, magnify );
        mt::utils::setTitleColour( ui::Theme::textColour( 1 ), tracks, bars );
        mt::utils::setTitleFontHeight( 16, tracks, bars );
        bars.setTitleProportion( .4f );

        pencil.onClick  = [&]() { if( pencil.getToggleState() ) toolSelectionChanged( GridToolType::GridPencilTool ); };
        erase.onClick   = [&]() { if( erase.getToggleState() ) toolSelectionChanged( GridToolType::GridEraseTool ); };
        select.onClick  = [&]() { if( select.getToggleState() ) toolSelectionChanged( GridToolType::GridSelectTool ); };
        magnify.onClick = [&]() { if( magnify.getToggleState() ) toolSelectionChanged( GridToolType::GridMagnifyTool ); };
        copy.onClick    = [&]() {
            auto& ae          = mt::getAudioInterface();
            selectedNotesSize = ae.getSelectedNoteList().size();
            if( selectedNotesSize > 0 )
            {
                if( onCopyClick ) onCopyClick();
                isCopyClicked = true;
                mt::utils::makeHidden( cut );
                mt::utils::makeVisible( copy, paste );
            }
            else
                copy.setToggleState( false, juce::dontSendNotification );
            resized();
        };

        cut.onClick = [&]() {
            auto& ae          = mt::getAudioInterface();
            selectedNotesSize = ae.getSelectedNoteList().size();
            if( selectedNotesSize > 0 )
            {
                if( onCutClick ) onCutClick();
                isCutClicked = true;
                mt::utils::makeHidden( copy );
                mt::utils::makeVisible( cut, paste );
            }
            else
                cut.setToggleState( false, juce::dontSendNotification );
            resized();
        };

        paste.onClick = [&]() {
            if( selectedNotesSize > 0 )
            {
                if( onPasteClick ) onPasteClick();
                isCutClicked  = false;
                isCopyClicked = false;
                mt::utils::makeHidden( paste );
                mt::utils::makeVisible( copy, cut );
            }
            paste.setToggleState( false, juce::dontSendNotification );
            copy.setToggleState( false, juce::dontSendNotification );
            cut.setToggleState( false, juce::dontSendNotification );
            resized();
        };

        pencil.setToggleState( true, juce::sendNotification );

        mt::signals::playStateChanged.connect( [&]( mt::TransportState State ) {
            bool state = ( State == mt::TransportState::Playing );
            juce::MessageManager::callAsync( [this, state] {
                play.setToggleState( state, juce::dontSendNotification );
            } );
        } );

        play.onClick = [&]() {
            auto& ae = mt::getAudioInterface();
            play.getToggleState() ? ae.setSequencePlay( ae.getSelectedSequence(), false, true ) : ae.setSequencePause();
        };

        ui::themeChanged.connect( [&]() {
            pencil.setImages( "Pencil_Normal", "Pencil_Active" );
            erase.setImages( "Erase_Normal", "Erase_Active" );
            select.setImages( "Select", "Select_Normal", "Select_Active" );
            copy.setImages( "Copy Button", "Copy Button_Active" );
            cut.setImages( "Cut_Normal", "Cut_Active" );
            paste.setImages( "Copy_Normal", "Copy_Activ" );
            play.setImages( "Play_Grid", "Stop_Active" );
        } );
    }

    void GridHeader::resized()
    {
        tracks.setBoundsRelative( .0f, .21f, .2666f, 0.5789f );
        bars.setBoundsRelative( .283f, 0.21f, .123f, 0.5789f );
        play.setBoundsRelative( .423f, 0.21f, .073f, 0.5789f );
        magnify.setBoundsRelative( .503f, 0.21f, .073f, 0.5789f );
        select.setBoundsRelative( .583f, 0.21f, .073f, 0.5789f );
        pencil.setBoundsRelative( .663f, 0.21f, .073f, 0.5789f );
        erase.setBoundsRelative( .743f, 0.21f, .073f, 0.5789f );
        if( isCutClicked )
        {
            cut.setBoundsRelative( .823f, 0.21f, .073f, 0.5789f );
            paste.setBoundsRelative( .903f, 0.21f, .073f, 0.5789f );
        }
        else if( isCopyClicked )
        {
            copy.setBoundsRelative( .823f, 0.21f, .073f, 0.5789f );
            paste.setBoundsRelative( .903f, 0.21f, .073f, 0.5789f );
        }
        else
        {
            cut.setBoundsRelative( .823f, 0.21f, .073f, 0.5789f );
            copy.setBoundsRelative( .903f, 0.21f, .073f, 0.5789f );
        }
    }

    void GridHeader::visibilityChanged()
    {
        auto visible = isVisible();
        if( isVisible() )
        {
            auto& ae  = mt::getAudioInterface();
            auto& sel = ae.getSelected();
            tracks.setListOptions( ae.getTrackList( sel.sequence ), sel.track );
        }

        for( auto it : getChildren() )
            it->setVisible( isVisible() );
    }
} // namespace
