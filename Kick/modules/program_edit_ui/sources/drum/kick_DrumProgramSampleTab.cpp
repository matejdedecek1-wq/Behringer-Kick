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
    DPSampleTab::DPSampleTab( std::vector<int>& Pads, mt::MTDrumProgram*& PDrum )
        : pads( Pads ), pDrum( PDrum ), KickGrid( 4, 4 )
    {
        setRowTitles( { "WAVEFORM", "LAYER ASSIGN", "TIMING & PITCH", "CONTROL" } );

        addComponent( wav, 0, 0, 4 );

        for( auto i = 0; i < 4; ++i )
        {
            auto name = "Layer " + juce::String( i );
            layer[i].setName( name );
            layer[i].setTitle( name );
            layer[i].showTitle( true );
            layer[i].setID( i );
            layer[i].setValueFontHeight( 18.f );
            layer[i].setRadioGroupId( 563365 );
            layer[i].addListener( this );

            addComponent( layer[i], 1, i );
        }

        addComponent( sStart, 2, 0 );
        addComponent( sEnd, 2, 1 );
        addComponent( lStart, 2, 2 );
        addComponent( lEnd, 2, 3 );

        addComponent( semi, 3, 0 );
        addComponent( fine, 3, 1 );
        addComponent( loop, 3, 2 );
        addComponent( reverse, 3, 3 );

        sStart.onValueChanged = [&]( juce::int64 value ) {
            if( pDrum && pads.size() )
            {
                if( value <= sEnd.getValue() )
                {
                    pDrum->setLayerLoopStart( pads[0], selectedLayer, value );
                    lStart.setValue( value, juce::dontSendNotification );
                }
                else
                    sStart.setValue( sEnd.getValue(), juce::dontSendNotification );
            }
        };

        sEnd.onValueChanged = [&]( juce::int64 value ) {
            if( pDrum && pads.size() )
            {
                if( value >= sStart.getValue() )
                {
                    pDrum->setLayerLoopEnd( pads[0], selectedLayer, value );
                    lEnd.setValue( value, juce::dontSendNotification );
                }
                else
                    sEnd.setValue( sStart.getValue(), juce::dontSendNotification );
            }
        };

        lStart.onValueChanged = [&]( juce::int64 value ) {
            if( pDrum && pads.size() )
            {
                if( value <= lEnd.getValue() )
                {
                    pDrum->setLayerLoopStart( pads[0], selectedLayer, value );
                    sStart.setValue( value, juce::dontSendNotification );
                }
                else
                    lStart.setValue( lEnd.getValue(), juce::dontSendNotification );
            }
        };

        lEnd.onValueChanged = [&]( juce::int64 value ) {
            if( pDrum && pads.size() )
            {
                if( value >= lStart.getValue() )
                {
                    pDrum->setLayerLoopEnd( pads[0], selectedLayer, value );
                    sEnd.setValue( value, juce::dontSendNotification );
                }
                else
                    lEnd.setValue( lStart.getValue(), juce::dontSendNotification );
            }
        };

        semi.onValueChanged = [&]( juce::int64 value ) {
            if( pDrum )
                pDrum->setLayerSemi( pads[0], selectedLayer, value );
        };

        fine.onValueChanged = [&]( juce::int64 value ) {
            if( pDrum )
                pDrum->setLayerFine( pads[0], selectedLayer, value );
        };

        loop.onClick = [&]() {
            if( pDrum )
                pDrum->setLayerLoop( pads[0], selectedLayer, loop.getToggleState() );
        };

        reverse.onClick = [&]() {
            if( pDrum )
                pDrum->setLayerReverse( pads[0], selectedLayer, reverse.getToggleState() );
        };

        semi.setRange( SEMI::MIN, SEMI::MAX, SEMI::STEP );
        fine.setRange( FINE::MIN, FINE::MAX, FINE::STEP );
        semi.setCentered( true );
        fine.setCentered( true );

        semi.setDoubleClickReturnValue( true, 0, 0 );
        fine.setDoubleClickReturnValue( true, 0, 0 );

        layer[0].setToggleState( true, juce::dontSendNotification );
    }

    void DPSampleTab::buttonStateChanged( juce::Button* B )
    {
        if( B->getToggleState() )
        {
            auto selLayer = dynamic_cast<ui::KickToggleButton*>( B )->getID();
            if( selLayer != selectedLayer )
            {
                selectedLayer = selLayer;
                refresh();
            }
        }
    }

    void DPSampleTab::refresh()
    {
        if( pDrum )
        {
            for( auto i = 0; i < 4; ++i )
            {
                auto path = pDrum->getLayerSource( pads[0], i );
                auto src  = juce::File( path ).getFileNameWithoutExtension();
                layer[i].setToggleButtonText( src, src );
            }
        }
        refreshPads();
    }

    void DPSampleTab::refreshPads()
    {
        if( pDrum && pads.size() )
        {
            auto source = pDrum->getLayerSource( pads[0], selectedLayer );
            int  min{ 0 };
            int  max{ 1 };
            if( auto sPtr = mt::getAudioSoundCache().getOrLoad( source ) )
            {
                max            = sPtr->getNumSamplesPerChannel();

                wav.setSource( sPtr->getData(), sPtr->getSampleRate() );

                sStart.setValueBounds( min, max, min );
                sEnd.setValueBounds( min, max, max );
                lStart.setValueBounds( min, max, min );
                lEnd.setValueBounds( min, max, max );
            }
            else
                wav.setSource( {}, 0 );
            mt::utils::setEnabled( source.isNotEmpty(), sStart, sEnd, lStart, lEnd, semi, fine, loop, reverse );

            sStart.setValue( pDrum->getLayerLoopStart( pads[0], selectedLayer ), juce::dontSendNotification );
            lStart.setValue( pDrum->getLayerLoopStart( pads[0], selectedLayer ), juce::dontSendNotification );
            auto loopEndValue = pDrum->getLayerLoopEnd( pads[0], selectedLayer );
            sEnd.setValue( loopEndValue == -1 ? ( source.isNotEmpty() ? max : 0 ) : loopEndValue, juce::dontSendNotification );
            lEnd.setValue( loopEndValue == -1 ? ( source.isNotEmpty() ? max : 0 ) : loopEndValue, juce::dontSendNotification );
            semi.setValue( pDrum->getLayerSemi( pads[0], selectedLayer ), juce::dontSendNotification );
            fine.setValue( pDrum->getLayerFine( pads[0], selectedLayer ), juce::dontSendNotification );
            loop.setToggleState( pDrum->isLayerLooping( pads[0], selectedLayer ), juce::dontSendNotification );
            reverse.setToggleState( pDrum->isLayerReversed( pads[0], selectedLayer ), juce::dontSendNotification );
        }
    }

    void DPSampleTab::handleCommand( CPktMgr::HWMsg_t& f_HwMsg )
    {
        if( contracts::DclContractsHelper::isPadButtonMessage( f_HwMsg ) )
        {
            auto padNum = contracts::DclContractsHelper::getPadNumber( f_HwMsg );
            pads[0]     = padNum + mt::getAudioInterface().getBankOffset();
            refresh();
        }
    }

    void DPSampleTab::visibilityChanged()
    {
        auto visible = isVisible();
        if( visible )
            refresh();
    }

}
