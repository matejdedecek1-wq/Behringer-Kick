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
    ClipProgramEdit::ClipProgramEdit()
        : ProgEditInterface( mt::MTProgramType::Clip )
    {
        // Add selector buttons...
        mt::utils::addAndMakeVisible( this, programSelector, padSelector );
        mt::utils::setRadioGroupID( (int)RadioGroupId::ClipProgram, programSelector, padSelector );

        mt::utils::addChildComponents( this, programTab, padTab );

        programSelector.setValueColour( ui::Theme::textColour( 1 ) );
        programSelector.setValueFontHeight( 18 );
        padSelector.setValueColour( ui::Theme::textColour( 1 ) );
        padSelector.setValueFontHeight( 18 );

        programSelector.onClick = [&]() {
            if( programSelector.getToggleState() )
            {
                // Control visibility...
                programTab.setVisible( true );
                padTab.setVisible( false );
            }
        };

        padSelector.onClick = [&]() {
            if( padSelector.getToggleState() )
            {
                // Control visibility...
                programTab.setVisible( false );
                padTab.setVisible( true );
            }
        };

        programSelector.triggerClick();

        ui::themeChanged.connect( [&]() {
            programSelector.setToggleButtonText( "Program", "Program" );
            padSelector.setToggleButtonText( "Pad", "Pad" );
        } );
    }

    void ClipProgramEdit::handleCommand( CPktMgr::HWMsg_t& hwMsg )
    {
        if( programSelector.getToggleState() )
            programTab.handleCommand( hwMsg );

        else if( padSelector.getToggleState() )
            padTab.handleCommand( hwMsg );
    }

    void ClipProgramEdit::resized()
    {
        programSelector.setBoundsRelative( .0097f, .8864f, .1653f, .0742f );
        padSelector.setBoundsRelative( .179f, .8864f, .1653f, .0742f );

        programTab.setBoundsRelative( .0f, .0f, 1.0f, .8646f );
        padTab.setBoundsRelative( .0f, .0f, 1.0f, .8646f );
    }

    void ClipProgramEdit::visibilityChanged()
    {
        if( isVisible() )
            refresh();
    }

    void ClipProgramEdit::refresh()
    {
        if( pClip.get() != pIProg )
            pClip.reset( dynamic_cast<mt::MTClipProgram*>( pIProg ) );

        programTab.refresh();
        padTab.refresh();
    }

    ////
    ////
    ////
    ////
    ClipHeader::ClipHeader()
    {
        mt::utils::addAndMakeVisible( this, sample, pad, settings );

        sample.setValueBackgroundColour( ui::Theme::backgroundColour( 13 ) );
        pad.setValueBackgroundColour( ui::Theme::backgroundColour( 13 ) );
        sample.setJustificationType( juce::Justification::centredTop, juce::Justification::centred );
        pad.setJustificationType( juce::Justification::centredTop, juce::Justification::centred );
        sample.setColours( ui::Theme::textColour( 2 ), ui::Theme::textColour( 1 ) );
        pad.setColours( ui::Theme::textColour( 2 ), ui::Theme::textColour( 1 ) );

        ui::themeChanged.connect( [&]() {
            settings.setImages( "Settings_Header" );
        } );
    }

    void ClipHeader::paint( juce::Graphics& g )
    {
        g.fillAll( ui::Theme::backgroundColour( 2 ) );
    }

    void ClipHeader::resized()
    {
        auto b = getLocalBounds();
        settings.setBounds( b.removeFromRight( 54 ).reduced( 5, 16 ) );
        pad.setBounds( b.removeFromRight( 84 ).reduced( 5, 16 ) );
        sample.setBounds( b.removeFromRight( 84 ).reduced( 5, 16 ) );
    }
}
