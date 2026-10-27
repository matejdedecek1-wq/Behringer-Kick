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
    ProgramMixer::ProgramMixer()
        : kick_ui::IRowView( "Program View" )
    {
        setupProvider();
        setMargin( 0, 6, 2, 0 );
    }

    void ProgramMixer::setupProvider()
    {
        provider.numRows = [&]() -> int {
            return ae.getNumPrograms();
        };

        provider.cellComponent = [&]( int Row, Component* Existing ) -> juce::Component* {
            auto numProgs = ae.getNumPrograms();
            if( juce::isPositiveAndBelow( Row, numProgs ) )
            {
                auto comp = dynamic_cast<ProgramMixerDisplay*>( Existing );
                if( comp == nullptr )
                {
                    comp                = new ProgramMixerDisplay();
                    comp->onSoloChanged = [&]() {
                        refresh();
                    };
                }
                comp->setProgram( Row );
                return comp;
            }
            return nullptr;
        };

        juce::MessageManager::callAsync( []() {
            auto insertList = mt::getAudioInterface().getAvailableInsertsList();
            //  ui::Inserts::setInsertsList( insertList );
        } );

        _initialized = true;
    }

    void ProgramMixer::populate( void )
    {
        if( !isVisible() || !_initialized.load() )
            return;

        refresh();
    }

    void ProgramMixer::onProgramListChanged()
    {
        juce::MessageManager::callAsync( [&]() {
            if( ae.isProjectLoaded() )
                populate();
        } );
    }

    void ProgramMixer::knobCommand( int knob, int value )
    {
        auto& rowComp       = getRowComponents();
        auto  comp1         = dynamic_cast<ProgramMixerDisplay*>( rowComp[0] );
        auto  elementBounds = comp1->getElementBounds( ( knob - 1 ) % 4 + 1 );
        auto  numRows       = getNumRows();
        auto  rows          = getRowNumbers( 3, ( ( knob - 1 ) / 4 ) + 1, elementBounds.getX(), elementBounds.getWidth(), 50 );

        std::vector<ProgramMixerDisplay*> entry;

        for( auto i = 0; i < rows.size(); i++ )
        {
            int rowIndex = rows[i];
            if( rowIndex < numRows )
            {
                auto comp = dynamic_cast<ProgramMixerDisplay*>( rowComp[rowIndex] );
                entry.push_back( comp );
            }
        }

        switch( knob )
        {
            case contracts::DeviceControlContracts::EN_KNOB_1:
            case contracts::DeviceControlContracts::EN_KNOB_2:
            case contracts::DeviceControlContracts::EN_KNOB_3:
            case contracts::DeviceControlContracts::EN_KNOB_4:
                if( entry.size() > 0 && entry[0] ) entry[0]->linkRotated( ( knob - contracts::DeviceControlContracts::EN_KNOB_1 ), value );
                break;

            case contracts::DeviceControlContracts::EN_KNOB_5:
            case contracts::DeviceControlContracts::EN_KNOB_6:
            case contracts::DeviceControlContracts::EN_KNOB_7:
            case contracts::DeviceControlContracts::EN_KNOB_8:
                if( entry.size() > 1 && entry[1] ) entry[1]->linkRotated( ( knob - contracts::DeviceControlContracts::EN_KNOB_5 ), value );
                break;

            case contracts::DeviceControlContracts::EN_KNOB_9:
            case contracts::DeviceControlContracts::EN_KNOB_10:
            case contracts::DeviceControlContracts::EN_KNOB_11:
            case contracts::DeviceControlContracts::EN_KNOB_12:
                if( entry.size() > 2 && entry[2] ) entry[2]->linkRotated( ( knob - contracts::DeviceControlContracts::EN_KNOB_9 ), value );
                break;
        }
    }

    //==============================================================================
    void ProgramMixer::visibilityChanged()
    {
        IRowView::visibilityChanged();

        if( isVisible() )
        {
            ae.addListener( this );
            selectRow( ae.getSelectedProgram() );
        }
        else
            ae.removeListener( this );
    }

    void ProgramMixer::meterUpdate()
    {
        auto& comps = getRowComponents();
        for( auto& it : comps )
        {
            if( auto r = dynamic_cast<ProgramMixerDisplay*>( it ) )
            {
                r->meterUpdate();
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////    Row content - ProgramMixerDisplay
    //////////////////////////////////////////////////////////////////////////////////////////
    ProgramMixerDisplay::ProgramMixerDisplay()
    {
        setInterceptsMouseClicks( true, true );

        std::vector<juce::String> busNames;
        auto&                     outBusNames = ae.getOutputBusNames();
        for( auto& n : outBusNames )
            busNames.push_back( n );
        auto& busMgr   = ae.getBusManager();
        auto  subMixes = busMgr.getNumSubmixes();
        for( auto subMix = 0; subMix < subMixes; ++subMix )
            busNames.push_back( busMgr.getSubmixBusName( subMix ) );
        ioListBox.setListOptions( busNames );
        programName.setEditable( false );

        mt::utils::addChildComponents( this, srNo, programName, programHeader, programInserts, monitor, solo, mute, levelMeter1, levelMeter2, ioListBox, levelSlider, panSlider );

        srNo.setFontProperties( 36 );
        srNo.setColours( ui::Theme::textColour( 0 ), ui::Theme::backgroundColour( 6 ), ui::Theme::backgroundColour( 12 ) );
        ioListBox.setJustificationType( juce::Justification::centredTop, juce::Justification::centredLeft );
        ioListBox.setBackGroundColours( ui::Theme::textColour( 4 ), ui::Theme::backgroundColour( 9 ) );
        ioListBox.setColours( ui::Theme::textColour( 2 ), ui::Theme::textColour( 1 ) );
        ioListBox.setTitleFontHeight( 14 );

        programName.setValueFontHeight( 18 );
        programName.setPadding( 10 );
        programName.setValueBackgroundColour( ui::Theme::textColour( 4 ) );
        programHeader.setValueBackgroundColour( ui::Theme::backgroundColour( 9 ) );
        programHeader.setValueColour( ui::Theme::textColour( 1 ) );
        programHeader.setValueJustification( juce::Justification::topLeft );

        programInserts.setValueColour( ui::Theme::textColour( 1 ) );
        programInserts.setValueFontHeight( 18 );

        ioListBox.onSelectionChanged = [&]( int, const juce::String& Sel ) { if( pProg ) pProg->output = Sel; };
        mute.onClick                 = [&]() { if( pProg ) pProg->mute = mute.getToggleState(); };
        levelSlider.onValueChanged   = [&]( double Value ) { if( pProg ) pProg->level = (int)Value; };
        panSlider.onValueChanged     = [&]( double Value ) { if( pProg ) pProg->pan = Value; };

        solo.onClick = [&]() {
            if( pProg )
            {
                pProg->solo = solo.getToggleState();
                if( onSoloChanged )
                    onSoloChanged();
            }
        };

        programInserts.onClick = [&]() {
            kick_ui::signals::onScreenData( Screen::Inserts, (void*)( dynamic_cast<mt::MTIInsertsInterface*>( pProg ) ) );
            kick_ui::signals::sigScreenOpen( kick_ui::Screen::Inserts );
        };

        ui::themeChanged.connect( [&]() {
            monitor.setImages( "Monitor_normal", "Monitor_active" );
            solo.setImages( "Solo_normal", "Solo_active" );
            mute.setImages( "Mute_normal", "Mute_active" );
            updateData();
        } );
    };

    void ProgramMixerDisplay::setProgram( int Prog )
    {
        progNo = Prog;
        pProg  = ae.getProgram( progNo );

        auto nameValueHandler  = [&]() { programName.setText( pProg ? pProg->name.get() : "", juce::dontSendNotification ); };
        auto ioValueHandler    = [&]() { ioListBox.setSelected( pProg ? pProg->output.get() : "", juce::dontSendNotification ); };
        auto muteValueHandler  = [&]() { mute.setToggleState( pProg ? pProg->mute.get() : false, juce::dontSendNotification ); };
        auto soloValueHandler  = [&]() { solo.setToggleState( pProg ? pProg->solo.get() : false, juce::dontSendNotification ); };
        auto panValueHandler   = [&]() { panSlider.setValue( pProg ? (double)pProg->pan.get() : 0.0, juce::dontSendNotification ); };
        auto levelValueHandler = [&]() { levelSlider.setValue( pProg ? (double)pProg->level.get() : 0.0, juce::dontSendNotification ); };

        scope.reset();
        if( pProg )
        {
            scope = std::make_shared<mt::Kick::ScopeTracker>();

            pProg->name.onValueChange.connect( nameValueHandler, scope );
            pProg->output.onValueChange.connect( ioValueHandler, scope );
            pProg->mute.onValueChange.connect( muteValueHandler, scope );
            pProg->solo.onValueChange.connect( soloValueHandler, scope );
            pProg->pan.onValueChange.connect( panValueHandler, scope );
            pProg->level.onValueChange.connect( levelValueHandler, scope );
        }

        updateData();
    }

    void ProgramMixerDisplay::updateData()
    {
        auto srNoValueHandler = [&]() { srNo.setText( juce::String( progNo + 1 ) ); };
        auto iconValueHandler = [&]() { srNo.setImage( getEditProgramIcon( pProg ? pProg->type.get() : mt::MTProgramType::Unknown ) ); };

        // Trigger data update
        srNoValueHandler();
        iconValueHandler();
        if( pProg )
        {
            pProg->name.onValueChange();
            pProg->output.onValueChange();
            pProg->mute.onValueChange();
            pProg->solo.onValueChange();
            pProg->pan.onValueChange();
            pProg->level.onValueChange();
        }
    }

    void ProgramMixerDisplay::clear()
    {
        progNo = -1;
        pProg  = nullptr;
        srNo.setImage( getEditProgramIcon( mt::MTProgramType::Unknown ) );
        programName.setText( "", juce::dontSendNotification );
        panSlider.setValue( 0.0, juce::dontSendNotification );
        solo.setToggleState( false, juce::dontSendNotification );
        mute.setToggleState( false, juce::dontSendNotification );
    }

    void ProgramMixerDisplay::meterUpdate()
    {
        if( pProg )
        {
            auto& in  = pProg->readInMeter();
            auto& out = pProg->readOutMeter();
            levelMeter1.setSignalStrength( in[0].rms, in[1].rms, in[0].peak, in[0].peak );
            levelMeter2.setSignalStrength( out[0].rms, out[1].rms, out[1].peak, out[1].peak );
        }
    }

    void ProgramMixerDisplay::resized()
    {
        srNo.setBoundsRelative( .001f, .0f, .044f, .982f );
        programHeader.setBoundsRelative( .065f, .0734f, .1603f, .2201f );
        programName.setBoundsRelative( .065f, .267f, .1603f, .404f );
        levelMeter1.setBoundsRelative( .0651f, .68f, .1603f, .2935f );
        levelMeter2.setBoundsRelative( .4346f, .68f, .1607f, .2935f );
        programInserts.setBoundsRelative( .2594f, .3035f, .099f, .3928f );
        ioListBox.setBoundsRelative( .4346f, .0892f, .1653f, .5714f );
        levelSlider.setBoundsRelative( .6198f, .0892f, .1653f, .8214f );
        panSlider.setBoundsRelative( .8049f, .0892f, .0792f, .8214f );
        solo.setBoundsRelative( .9108f, .0892f, .0594f, .3928f );
        mute.setBoundsRelative( .9108f, .5178f, .0594f, .3928f );
    }

    void ProgramMixerDisplay::mouseUp( const juce::MouseEvent& e )
    {
        if( e.mouseWasClicked() )
            mt::getAudioInterface().setSelectedProgram( progNo );
    }

    void ProgramMixerDisplay::visibilityChanged()
    {
        bool visible = isVisible();

        for( auto child : getChildren() )
            child->setVisible( visible );

        if( visible )
            updateData();

        if( pProg )
            pProg->enableOutputMetering( visible );
    }

    void ProgramMixerDisplay::linkRotated( int ID, int value )
    {
        switch( ID )
        {
            case 0: ioListBox.knobRotated( value ); break;
            case 1: levelSlider.knobRotated( value ); break;
            case 2: panSlider.knobRotated( value ); break;
            case 3:
                mute.setToggleState( value > 0, juce::dontSendNotification );
                solo.setToggleState( value <= 0, juce::dontSendNotification );
                break;
        }
    }

    juce::Rectangle<int> ProgramMixerDisplay::getElementBounds( int state )
    {
        switch( state )
        {
            case 1: return ioListBox.getBounds(); break;
            case 2: return levelSlider.getBounds(); break;
            case 3: return panSlider.getBounds(); break;
            case 4: return mute.getBounds(); break;
            default: return ioListBox.getBounds(); break;
        }
    }
}
