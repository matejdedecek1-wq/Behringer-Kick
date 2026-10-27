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
    ////////////////////////////    Header Bar - TrackSelector
    //////////////////////////////////////////////////////////////////////////////////////////
    HeaderBarTrackSelector::HeaderBarTrackSelector()
    {
        mt::utils::addAndMakeVisible( this, tracks );
        tracks.setJustificationType( juce::Justification::centredLeft );
    }

    void HeaderBarTrackSelector::resized()
    {
        auto b = getLocalBounds();
        auto w = b.getWidth();
        tracks.setBounds( b );
    }

    void HeaderBarTrackSelector::visibilityChanged()
    {
        if( isVisible() )
        {
            auto& ae  = mt::getAudioInterface();
            auto  sel = ae.getSelected();
            tracks.setListOptions( ae.getTrackList( sel.sequence ), sel.track );
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////    Header Bar - SequenceSelector
    //////////////////////////////////////////////////////////////////////////////////////////
    HeaderBarSequenceSelector::HeaderBarSequenceSelector()
    {
        mt::utils::addAndMakeVisible( this, icon, sequence );
        sequence.setJustificationType( juce::Justification::centredLeft );
        sequence.setTitleColour( ui::Theme::backgroundColour( 8 ) );
    }

    void HeaderBarSequenceSelector::resized()
    {
        auto b = getLocalBounds();
        sequence.setBoundsRelative( 0.0f, 0.0f, 1.0f, 1.0f );
    }

    void HeaderBarSequenceSelector::visibilityChanged()
    {
        if( isVisible() )
        {
            auto& ae  = mt::getAudioInterface();
            auto  sel = ae.getSelected();
            sequence.setListOptions( ae.getSequenceList(), sel.sequence );
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////    Header Bar - Program Selector
    //////////////////////////////////////////////////////////////////////////////////////////

    HeaderBarProgramSelector::HeaderBarProgramSelector()
        : selector( std::make_unique<KickProgramSelector>( "SELECT PROGRAM" ) )
    {
        mt::utils::addAndMakeVisible( this, icon, title );
        title.setValueJustification( juce::Justification::centredLeft );
        title.setValueBackgroundColour( ui::Theme::backgroundColour( 3 ) );
        title.setValueFontHeight( 20 );
        title.setPadding( 10 );
        title.setEditable( false, true );
        title.showDropdown( true );

        auto clicked = [&]() {
            selector->refreshContent();
            mt::utils::showOptions(
                selector->getName(), selector.get(), [&]( int r ) {
                    if( r == UI::APPLY )
                    {
                        title.setText( selector->getSelectedProgram(), juce::dontSendNotification );
                        if( onSelectionChanged )
                            onSelectionChanged( selector->getSelectedProgram(), selector->getSelectedProgramType() );
                    }
                },
                ui::layout::screen, ui::Theme::backgroundColour( 5 ), ui::Theme::titleColour( 1 ), ui::Theme::backgroundColour( 2 ), ui::Theme::backgroundColour( 5 ) );
            selector->table->refresh();
            selector->updateRowSelection( false );
        };
        title.onEditorShow = clicked;
    }

    void HeaderBarProgramSelector::resized()
    {
        auto b = getLocalBounds();
        title.setBounds( b.removeFromTop( 40 ).withTrimmedTop( 5 ) );
    }

    void HeaderBarProgramSelector::visibilityChanged()
    {
        if( isVisible() )
        {
            auto&        ae          = mt::getAudioInterface();
            auto         sel         = ae.getSelected();
            juce::String programName = ae.getProgramNameByIndex( ae.getSelectedProgram() );
            selector->setValues( programName, ae.getProgramType( programName ) );
            selector->table->selectRow( 0 );
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////    Header Bar - Program & Track Selector
    //////////////////////////////////////////////////////////////////////////////////////////
    HeaderBarProgramAndTrackSelector::HeaderBarProgramAndTrackSelector()
    {
        mt::utils::addAndMakeVisible( this, icon, programs, tracks );
        programs.setJustificationType( juce::Justification::centredLeft );
        tracks.setJustificationType( juce::Justification::centredLeft );
        programs.setBackGroundColours( ui::Theme::backgroundColour( 13 ), ui::Theme::backgroundColour( 2 ) );
        tracks.setBackGroundColours( ui::Theme::backgroundColour( 13 ), ui::Theme::backgroundColour( 2 ) );
    }

    void HeaderBarProgramAndTrackSelector::resized()
    {
        auto b = getLocalBounds();
        auto w = b.getWidth();
        programs.setBounds( b.removeFromLeft( (int)( w * 0.54 ) ) );
        b.removeFromLeft( 10 );
        tracks.setBounds( b );
    }

    void HeaderBarProgramAndTrackSelector::visibilityChanged()
    {
        if( isVisible() )
        {
            auto& ae  = mt::getAudioInterface();
            auto  sel = ae.getSelected();

            tracks.setListOptions( ae.getTrackList( sel.sequence ), sel.track );
            programs.setListOptions( ae.getProgramList(), sel.program );
        }
    }
} // namespace
