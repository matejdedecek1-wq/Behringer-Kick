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
    MasterDrumProgramEdit::MasterDrumProgramEdit( std::vector<int>& Pads, mt::MTDrumProgram*& PDrum )
        : pads( Pads ), pDrum( PDrum ), KickGrid( 4, 4 )
    {
        setRowTitles( { "MASTER", "", "PAD", "LAYER CONTROL" } );

        addComponent( masterLevel, 0, 0 );
        addComponent( masterPan, 0, 1 );
        addComponent( masterSemi, 0, 2 );
        addComponent( masterFine, 0, 3 );

        addComponent( masterPoly, 1, 0 );

        addComponent( padLevel, 2, 0 );
        addComponent( padPan, 2, 1 );
        addComponent( padSemi, 2, 2 );
        addComponent( padFine, 2, 3 );

        addComponent( layerPlay, 3, 0 );
        addComponent( samplePlay, 3, 1 );
        addComponent( muteGroup, 3, 3 );

        layerPlay.setColours( ui::Theme::textColour( 1 ), ui::Theme::textColour( 1 ) );
        samplePlay.setColours( ui::Theme::textColour( 1 ), ui::Theme::textColour( 1 ) );
        muteGroup.setColours( ui::Theme::textColour( 2 ), ui::Theme::textColour( 1 ) );

        masterPoly.setRange( POLYPHONY ::MIN, POLYPHONY::MAX, POLYPHONY::STEP );

        masterLevel.setRange( MIDI::MIN, MIDI::MAX, MIDI::STEP );
        masterPan.setRange( PAN::MIN, PAN::MAX, PAN::STEP );
        padLevel.setRange( MIDI::MIN, MIDI::MAX, MIDI::STEP );
        padPan.setRange( PAN::MIN, PAN::MAX, PAN::STEP );

        masterSemi.setRange( SEMI::MIN, SEMI::MAX, SEMI::STEP );
        padSemi.setRange( SEMI::MIN, SEMI::MAX, SEMI::STEP );
        masterFine.setRange( FINE::MIN, FINE::MAX, FINE::STEP );
        padFine.setRange( FINE::MIN, FINE::MAX, FINE::STEP );

        masterPan.setCentered( true );
        masterSemi.setCentered( true );
        masterFine.setCentered( true );
        padPan.setCentered( true );
        padSemi.setCentered( true );
        padFine.setCentered( true );

        masterLevel.setDoubleClickReturnValue( true, 127, 0 );
        masterPan.setDoubleClickReturnValue( true, 0.f, 0 );
        masterSemi.setDoubleClickReturnValue( true, 0, 0 );
        masterFine.setDoubleClickReturnValue( true, 0, 0 );
        masterPoly.setDoubleClickReturnValue( true, 8, 0 );

        padLevel.setDoubleClickReturnValue( true, 127, 0 );
        padPan.setDoubleClickReturnValue( true, 0.f, 0 );
        padSemi.setDoubleClickReturnValue( true, 0, 0 );
        padFine.setDoubleClickReturnValue( true, 0, 0 );

        masterLevel.onValueChanged = [&]( int Value ) {
            if( pDrum )
                pDrum->setLevel( Value );
        };

        masterPan.onValueChanged = [&]( double Value ) {
            if( pDrum )
                pDrum->setPan( Value );
        };

        masterSemi.onValueChanged = [&]( int Value ) {
            if( pDrum )
                pDrum->setSemi( Value );
        };

        masterFine.onValueChanged = [&]( int Value ) {
            if( pDrum )
                pDrum->setFine( Value );
        };

        masterPoly.onValueChanged = [&]( int Value ) {
            if( pDrum )
                pDrum->setPolyphony( Value );
        };

        padLevel.onValueChanged = [&]( int Value ) {
            if( pDrum )
                pDrum->setNoteGain( pads[0], CommonUtils::midiToGain( Value ) );
        };

        padPan.onValueChanged = [&]( double Value ) {
            if( pDrum )
                pDrum->setNotePan( pads[0], Value );
        };

        padSemi.onValueChanged = [&]( int Value ) {
            if( pDrum )
                pDrum->setNoteSemi( pads[0], Value );
        };

        padFine.onValueChanged = [&]( int Value ) {
            if( pDrum )
                pDrum->setNoteFine( pads[0], Value );
        };

        muteGroup.onValueChanged = [&]( juce::int64 Value ) {
            if( pDrum )
                pDrum->setNoteMuteGroup( pads[0], Value );
        };

        layerPlay.onSelectionChanged = [&]( const int Index, const juce::String& Value ) {
            if( pDrum )
                pDrum->setNoteLayerPlay( pads[0], toEnum<mt::LayerPlayType>( Index ) );
        };

        samplePlay.onSelectionChanged = [&]( const int Index, const juce::String& Value ) {
            if( pDrum )
                pDrum->setNoteSamplePlay( pads[0], toEnum<mt::SamplePlayType>( Index ) );
        };
    }

    void MasterDrumProgramEdit::refresh()
    {
        auto levelHandler = [&]() { masterLevel.setValue( pDrum->getLevel(), juce::dontSendNotification ); };
        auto panHandler   = [&]() { masterPan.setValue( pDrum->getPan(), juce::dontSendNotification ); };
        auto semiHandler  = [&]() { masterSemi.setValue( pDrum->getSemi(), juce::dontSendNotification ); };
        auto fineHandler  = [&]() { masterFine.setValue( pDrum->getFine(), juce::dontSendNotification ); };
        auto polyHandler  = [&]() { masterPoly.setValue( pDrum->getPolyphony(), juce::dontSendNotification ); };

        scope.reset();
        if( pDrum )
        {
            scope = std::make_shared<mt::Kick::ScopeTracker>();

            pDrum->level.onValueChange.connect( levelHandler, scope );
            pDrum->pan.onValueChange.connect( panHandler, scope );
            pDrum->semi.onValueChange.connect( semiHandler, scope );
            pDrum->fine.onValueChange.connect( fineHandler, scope );
            pDrum->polyphony.onValueChange.connect( polyHandler, scope );
        }

        if( pDrum )
        {
            pDrum->level.onValueChange();
            pDrum->pan.onValueChange();
            pDrum->semi.onValueChange();
            pDrum->fine.onValueChange();
            pDrum->polyphony.onValueChange();
        }

        refreshPads();
    }

    void MasterDrumProgramEdit::refreshPads()
    {
        if( pDrum && pads.size() )
        {
            padLevel.setValue( CommonUtils::gainToMidi( pDrum->getNoteGain( pads[0] ) ), juce::dontSendNotification );
            padPan.setValue( pDrum->getNotePan( pads[0] ), juce::dontSendNotification );
            padSemi.setValue( pDrum->getNoteSemi( pads[0] ), juce::dontSendNotification );
            padFine.setValue( pDrum->getNoteFine( pads[0] ), juce::dontSendNotification );
            muteGroup.setValue( pDrum->getNoteMuteGroup( pads[0] ), juce::dontSendNotification );
            samplePlay.setSelected( (int)pDrum->getNoteSamplePlay( pads[0] ), juce::dontSendNotification );
            layerPlay.setSelected( (int)pDrum->getNoteLayerPlay( pads[0] ), juce::dontSendNotification );
        }
    }

    void MasterDrumProgramEdit::handleCommand( CPktMgr::HWMsg_t& f_HwMsg )
    {
        if( contracts::DclContractsHelper::isPadButtonMessage( f_HwMsg ) )
        {
            auto padNum = contracts::DclContractsHelper::getPadNumber( f_HwMsg );
            pads[0]     = padNum + mt::getAudioInterface().getBankOffset();
            refresh();
        }
    }

    void MasterDrumProgramEdit::visibilityChanged()
    {
        auto visible = isVisible();
        if( visible )
            refresh();
    }

}
