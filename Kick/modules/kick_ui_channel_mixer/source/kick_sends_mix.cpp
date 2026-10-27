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
    SendsMixer::SendsMixer()
        : kick_ui::IRowView( "Program View" )
    {
        setupProvider();
        setMargin( 0, 6, 2, 0 );
        ui::themeChanged.connect( [&]() {
            refresh();
        } );
    }

    void SendsMixer::setupProvider()
    {
        provider.numRows = [&]() -> int {
            return ae.getNumPrograms();
        };

        provider.cellComponent = [&]( int Row, Component* Existing ) -> juce::Component* {
            auto numProgs = ae.getNumPrograms();
            if( juce::isPositiveAndBelow( Row, numProgs ) )
            {
                auto comp = dynamic_cast<SendsMixerDisplay*>( Existing );
                if( comp == nullptr )
                {
                    comp = new SendsMixerDisplay();
                }
                comp->setNumber( Row );
                return comp;
            }
            return nullptr;
        };
        _initialized = true;
    }

    void SendsMixer::populate( void )
    {
        if( !isVisible() || !_initialized.load() )
            return;

        refresh();
    }

    void SendsMixer::onProgramListChanged()
    {
        if( ae.isProjectLoaded() )
            populate();
    }

    void SendsMixer::meterUpdate()
    {
        auto& comps = getRowComponents();
        for( auto& it : comps )
        {
            if( auto r = dynamic_cast<SendsMixerDisplay*>( it ) )
            {
                r->meterUpdate();
            }
        }
    }

    void SendsMixer::knobCommand( int knob, int value )
    {
        auto& rowComp       = getRowComponents();
        auto  comp1         = dynamic_cast<SendsMixerDisplay*>( rowComp[0] );
        auto  elementBounds = comp1->getElementBounds( ( knob - 1 ) % 4 + 1 );
        auto  numRows       = getNumRows();
        auto  rows          = getRowNumbers( 3, ( ( knob - 1 ) / 4 ) + 1, elementBounds.getX(), elementBounds.getWidth(), 50 );

        std::vector<SendsMixerDisplay*> entry;

        for( auto i = 0; i < rows.size(); i++ )
        {
            int rowIndex = rows[i];
            if( rowIndex < numRows )
            {
                auto comp = dynamic_cast<SendsMixerDisplay*>( rowComp[rowIndex] );
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
    void SendsMixer::visibilityChanged()
    {
        IRowView::visibilityChanged();

        if( isVisible() )
            ae.addListener( this );
        else
            ae.removeListener( this );
    }

    //////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////    Row content - SendsMixerDisplay
    //////////////////////////////////////////////////////////////////////////////////////////
    SendsMixerDisplay::SendsMixerDisplay()
    {
        programName.setEditable( false );

        mt::utils::addChildComponents( this, srNo, icon, programName, programHeader, sends1, sends2, sends3, sends4, levelMeter );
        mt::utils::setValueFontHeight( 18, programName );

        srNo.setFontProperties( 36 );
        srNo.setColours( ui::Theme::textColour( 0 ), ui::Theme::backgroundColour( 6 ), ui::Theme::backgroundColour( 6 ) );
        icon.toFront( true );

        programName.setValueBackgroundColour( ui::Theme::textColour( 4 ) );
        programName.setPadding( 10 );
        programHeader.setValueJustification( juce::Justification::topLeft );
        programHeader.setValueBackgroundColour( ui::Theme::backgroundColour( 9 ) );
        programHeader.setValueColour( ui::Theme::textColour( 1 ) );

        sends1.onValueChanged = [&]( double Value ) { if( pProg ) pProg->send1 = CommonUtils::midiToGain( Value ); };
        sends2.onValueChanged = [&]( double Value ) { if( pProg ) pProg->send2 = CommonUtils::midiToGain( Value ); };
        sends3.onValueChanged = [&]( double Value ) { if( pProg ) pProg->send3 = CommonUtils::midiToGain( Value ); };
        sends4.onValueChanged = [&]( double Value ) { if( pProg ) pProg->send4 = CommonUtils::midiToGain( Value ); };

        ui::themeChanged.connect( [&]() { updateData(); } );
    };

    void SendsMixerDisplay::setNumber( int ProgNo )
    {
        no    = ProgNo;
        pProg = ae.getProgram( no );

        auto nameValueHandler = [&]() { programName.setText( pProg ? pProg->name.get() : "", juce::dontSendNotification ); };
        auto send1Handler     = [&]() { sends1.setValue( pProg ? CommonUtils::gainToMidi( (double)pProg->send1.get() ) : 0.0, juce::dontSendNotification ); };
        auto send2Handler     = [&]() { sends2.setValue( pProg ? CommonUtils::gainToMidi( (double)pProg->send2.get() ) : 0.0, juce::dontSendNotification ); };
        auto send3Handler     = [&]() { sends3.setValue( pProg ? CommonUtils::gainToMidi( (double)pProg->send3.get() ) : 0.0, juce::dontSendNotification ); };
        auto send4Handler     = [&]() { sends4.setValue( pProg ? CommonUtils::gainToMidi( (double)pProg->send4.get() ) : 0.0, juce::dontSendNotification ); };

        scope.reset();
        if( pProg )
        {
            scope = std::make_shared<mt::Kick::ScopeTracker>();

            pProg->name.onValueChange.connect( nameValueHandler, scope );
            pProg->send1.onValueChange.connect( send1Handler, scope );
            pProg->send2.onValueChange.connect( send2Handler, scope );
            pProg->send3.onValueChange.connect( send3Handler, scope );
            pProg->send4.onValueChange.connect( send4Handler, scope );
        }

        updateData();
    }

    void SendsMixerDisplay::updateData()
    {
        setProgramIcon( icon, mt::MTProgramType::Unknown );

        auto srNoValueHandler = [&]() { srNo.setText( juce::String( no + 1 ) ); };
        auto iconValueHandler = [&]() { srNo.setImage( getEditProgramIcon( pProg ? pProg->type.get() : mt::MTProgramType::Unknown ) ); };

        // Trigger data update
        srNoValueHandler();
        iconValueHandler();
        if( pProg )
        {
            pProg->name.onValueChange();
            pProg->send1.onValueChange();
            pProg->send2.onValueChange();
            pProg->send3.onValueChange();
            pProg->send4.onValueChange();
        }
    }

    void SendsMixerDisplay::clear()
    {
        no    = -1;
        pProg = nullptr;
        programName.setText( "", juce::dontSendNotification );
        sends1.setValue( 0.0, juce::dontSendNotification );
        sends2.setValue( 0.0, juce::dontSendNotification );
        sends3.setValue( 0.0, juce::dontSendNotification );
        sends4.setValue( 0.0, juce::dontSendNotification );
    }

    void SendsMixerDisplay::meterUpdate()
    {
        if( pProg )
        {
            auto& out = pProg->readOutMeter();
            levelMeter.setSignalStrength( out[0].rms, out[1].rms, out[0].peak, out[1].peak );
        }
    }

    void SendsMixerDisplay::resized()
    {
        srNo.setBoundsRelative( .001f, .0f, .044f, .982f );
        icon.setBoundsRelative( .001f, .491f, .044f, .491f );
        programHeader.setBoundsRelative( .065f, .0734f, .1603f, .2201f );
        programName.setBoundsRelative( .065f, .267f, .1603f, .404f );
        levelMeter.setBoundsRelative( .0651f, .68f, .1603f, .2935f );
        sends1.setBoundsRelative( .2495f, .0892f, .1653f, .8214f );
        sends2.setBoundsRelative( .4346f, .0892f, .1653f, .8214f );
        sends3.setBoundsRelative( .6198f, .0892f, .1653f, .8214f );
        sends4.setBoundsRelative( .8049f, .0892f, .1653f, .8214f );
    }

    void SendsMixerDisplay::mouseUp( const juce::MouseEvent& e )
    {
        if( e.mouseWasClicked() )
            ae.setSelectedProgram( no );
    }

    void SendsMixerDisplay::visibilityChanged()
    {
        bool visible = isVisible();

        for( auto child : getChildren() )
            child->setVisible( visible );

        if( visible )
            updateData();
    }

    void SendsMixerDisplay::linkRotated( int ID, int value )
    {
        switch( ID )
        {
            case 0: sends1.knobRotated( value ); break;
            case 1: sends2.knobRotated( value ); break;
            case 2: sends3.knobRotated( value ); break;
            case 3: sends4.knobRotated( value ); break;
        }
    }

    juce::Rectangle<int> SendsMixerDisplay::getElementBounds( int state )
    {
        switch( state )
        {
            case 1: return sends1.getBounds(); break;
            case 2: return sends2.getBounds(); break;
            case 3: return sends3.getBounds(); break;
            case 4: return sends4.getBounds(); break;
            default: return sends1.getBounds(); break;
        }
    }
}
