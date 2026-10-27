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
    EditZones::EditZones()
    {
        setTitle( "Edit Zones" );
        setName( "Edit Zones" );

        current.setRadioGroupId( 0x986532, juce::dontSendNotification );
        all.setRadioGroupId( 0x986532, juce::dontSendNotification );
        multiple.setRadioGroupId( 0x986532, juce::dontSendNotification );

        separator.setOrientation( ui::KickSeparator::Horizantal );
        separator.setColours( ui::Theme::foregroundColour( 1 ) );
        separator.setThickness( 2 );

        miniMap.setLayout( ui::KickPadMiniMap::Layout::Horizantal );
        miniMap.setActive( -1, true );

        mt::utils::addChildComponents( this, current, all, multiple, separator, miniMap );

        current.onStateChange = [&]() {
            if( current.getToggleState() )
                clearQuick();
        };

        multiple.onStateChange = [&]() {
            if( multiple.getToggleState() )
                clearQuick();
        };

        all.onStateChange = [&]() {
            if( all.getToggleState() )
            {
                selected.clearQuick();
                for( auto i = 0; i < 128; ++i )
                {
                    selected.addIfNotAlreadyThere( i );
                    miniMap.setHighlight( i, true );
                }
            }
        };

        miniMap.onClicked = [&]( juce::String Bank, int Pad ) {
            lastPadPressed = mt::utils::getBankOffset( Bank ) + Pad;
            if( multiple.getToggleState() )
            {
                selected.addIfNotAlreadyThere( lastPadPressed );

                auto isHighlight = miniMap.isHighlight( lastPadPressed );
                miniMap.setHighlight( lastPadPressed, !isHighlight );
            }
            else if( current.getToggleState() )
            {
                selected.clearQuick();
                miniMap.clearAll();

                selected.addIfNotAlreadyThere( lastPadPressed );
                miniMap.setHighlight( lastPadPressed, true );
            }
        };

        current.setToggleState( true, juce::sendNotification );
        miniMap.triggerClick( lastPadPressed );
    }

    void EditZones::clearQuick()
    {
        selected.clearQuick();
        miniMap.clearAll();
        selected.addIfNotAlreadyThere( lastPadPressed );
        miniMap.setHighlight( lastPadPressed, true );
    }

    juce::Array<int>& EditZones::getEditZones()
    {
        return selected;
    }

    juce::String EditZones::getSelectedZonesString()
    {
        juce::String Str;
        for( auto it : selected )
            Str += mt::utils::getPad( it ) + ",";
        return Str.trimCharactersAtEnd( "," );
    }

    void EditZones::bankPressed( int BankIndex )
    {
    }

    void EditZones::padPressed( int PadNo, int State, int Velocity, int Aftertouch )
    {
        int bankOffset = mt::getAudioInterface().getBankOffset();
        int padNo      = bankOffset + PadNo;
        miniMap.triggerClick( padNo );
    }

    void EditZones::resized()
    {
        auto bounds = getLocalBounds().reduced( 10 );
        auto bRow   = bounds.removeFromTop( 44 ).reduced( 0, 5 );
        auto w      = bounds.getWidth() / 3;

        current.setBounds( bRow.removeFromLeft( w ).reduced( 10, 0 ) );
        all.setBounds( bRow.removeFromLeft( w ).reduced( 10, 0 ) );
        multiple.setBounds( bRow.removeFromLeft( w ).reduced( 10, 0 ) );

        separator.setBounds( bounds.removeFromTop( 10 ).reduced( 0, 3 ) );
        miniMap.setBounds( bounds.reduced( 0, 10 ) );
    }

    void EditZones::visibilityChanged()
    {
        if( isVisible() )
        {
        }

        setFocus( isVisible() );

        for( auto it : getChildren() )
            it->setVisible( isVisible() );
    }

}
