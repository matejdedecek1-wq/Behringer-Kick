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
#include <kick_ui/modes/filebrowser/kick_fileUtils.h>

namespace kick_ui
{
    AssignSampleContent::AssignSampleContent()
        : ae( mt::getAudioInterface() ), pads( std::make_unique<ui::KickPadButtonGroupComponent>( ui::KickPadButton::PadButtonType::assignSample ) )
    {
        fileList.setFocus( true );
        setLookAndFeel( ui::KickLooknFeel::getLnf() );

        mt::utils::addChildComponents( this, pads.get(), fileList, sampleLabel );

        fileList.setBackgroundColour( ui::Theme::backgroundColour( 1 ) );
        fileList.setSpacing( 4, 4, 10 );
        files.insert( files.begin(), "Clear Assignment" );
        fileList.setContent( files, 0 );

        sampleLabel.setValueBackgroundColour( ui::Theme::backgroundColour( 9 ) );
        sampleLabel.setValueJustification( juce::Justification::centred );
        sampleLabel.setValueColour( ui::Theme::textColour( 1 ) );

        pads->setFocusOutline( false );
        pads->OnButtonClicked = [&]( int Index ) { //software padbutton pressed
            pads->setHighlight( Index, ui::Theme::padButtonColour( 3 ) );
            ui::fileUtils::setHighlightLED( Index - pads->getBankOffset() );
        };

        pads->OnDoubleClicked = [&]( int Index ) {
            DBG( "ON DOUBLE CLICKED" );
            auto index = fileList.getSelectedIndex();
            ui::fileUtils::setHighlightLED( Index - pads->getBankOffset() );
            assignSampleIntoPad( Index );
        };

        pads->onShiftPadPressed = [&]( int id ) { //from PadButtonGroup
            assignSampleIntoPad( id + pads->getBankOffset() );
            ui::fileUtils::setHighlightLED( id );
        };

        pads->onKnobClicked = [&]( int hwPadIndex ) {
            assignSampleIntoPad( hwPadIndex + pads->getBankOffset() );
            ui::fileUtils::setHighlightLED( hwPadIndex );
        };

        fileList.onSelectionChanged = [&]( int Index, juce::String Name ) {
            auto file = findFileInDirectory( getAppDirs( MTSamples ), Name );
            if( file.existsAsFile() )
                audition( file );
            else if( juce::File( ae.getProjectDir().getSiblingFile( Name ).getFullPathName() ).existsAsFile() )
                audition( juce::File( ae.getProjectDir().getSiblingFile( Name ).getFullPathName() ) );
            else
                ae.stopPreview();
        };
    }

    juce::File AssignSampleContent::findFileInDirectory( const juce::File& directory, const juce::String& fileName )
    {
        juce::DirectoryIterator iter( directory, true, fileName );

        while( iter.next() )
        {
            juce::File fileFound = iter.getFile();
            if( fileFound.existsAsFile() )
                return fileFound; // Return the file if found
        }

        return juce::File(); // Return an empty file if not found
    }

    void AssignSampleContent::assignSampleIntoPad( int padIndex )
    {
        mt::MTProgramType programType = ae.getSelectedProgramData()->type;

        if( programType == mt::MTProgramType::Drum || programType == mt::MTProgramType::Clip || programType == mt::MTProgramType::Keygroup )
        {
            auto hwIndex = padIndex - pads->getBankOffset();
            if( juce::isPositiveAndBelow( hwIndex, _NUM_PADS_IN_COMPONENT ) )
            {
                auto sampleIndex = fileList.getSelectedIndex();
                if( sampleIndex == 0 )
                {
                    ae.removeSampleFromProgram( ae.getSelectedProgram(), padIndex );
                    contracts::DclSignals::sendCommandToDcl( contracts::DclContractsHelper::
                                                                 setPadDataMessage( hwIndex, contracts::DeviceControlContracts::PadButtonState::StateSampleNotAssigned,
                                                                                    contracts::DeviceControlContracts::padColourlightGreen1 ) );
                }
                else
                {
                    auto sample = mt::getAudioInterface().getAsset( mt::AssetTypes::Samples, sampleIndex - 1 );
                    ae.addSampleToProgram( ae.getSelectedProgram(), padIndex, sample, 0 );
                    if( ae.getSelectedProgramData()->type == mt::MTProgramType::Keygroup )
                    {
                        contracts::DclSignals::sendCommandToDcl( contracts::DclContractsHelper::
                                                                     setAllPadDataColourInfoMessage( contracts::DeviceControlContracts::PadButtonState::StateSampleAssigned,
                                                                                                     contracts::DeviceControlContracts::padColourDarkYellow ) );
                    }
                    contracts::DclSignals::sendCommandToDcl( contracts::DclContractsHelper::
                                                                 setPadDataMessage( hwIndex, contracts::DeviceControlContracts::PadButtonState::StateSampleAssigned,
                                                                                    contracts::DeviceControlContracts::padColourlightGreen1 ) );
                }
                juce::MessageManager::callAsync( [this, padIndex] {
                    refresh();
                    pads->setHighlight( padIndex, ui::Theme::padButtonColour( 3 ) );
                } );
            }
        }
    };

    void AssignSampleContent::paint( juce::Graphics& g )
    {
        g.fillAll( ui::Theme::backgroundColour( 5 ) );
        mt::utils::fillRoundedRect( g, { 10, 10, 342, 440 }, ui::Theme::backgroundColour( 1 ), 6.f );
        mt::utils::renderRoundedRect( g, { 10, 10, 342, 440 }, ui::Theme::backgroundColour( 0 ), 1, 6.f );
    }

    void AssignSampleContent::resized()
    {
        sampleLabel.setBounds( 10, 10, 342, 50 );
        fileList.setBounds( 10, 60, 342, 385 );
        pads->setBounds( 362, 10, 652, 440 );
    }

    void AssignSampleContent::visibilityChanged()
    {
        bool visible = isVisible();
        if( visible )
        {
            files.clear();
            files = mt::getAudioInterface().getAssetNames( mt::AssetTypes::Samples );
            files.insert( files.begin(), "Clear Assignment" );
            fileList.setContent( files, fileList.getSelectedIndex() );
            fileList.refreshViewPosition();
            pads->setBank( ae.getBank() );
            refresh();
            refreshLEDs();
            ui::fileUtils::lastPressedPad = -1;
        }
        else
            ae.stopPreview();

        for( auto& it : getChildren() )
            it->setVisible( visible );
    }

    void AssignSampleContent::refresh()
    {
        pads->clearAllPadData();
        pads->setActive( -1, true );
        pads->setBorderColor( ui::Theme::backgroundColour( 0 ), 1, juce::dontSendNotification );

        auto progIdx  = ae.getSelectedProgram();
        auto progType = ae.getProgramType( progIdx );

        samples.clear();
        ae.getProgramSamples( progIdx, samples );

        for( const auto& sample : samples )
        {
            if( sample )
            {
                auto text = juce::File( sample->layers[0]->source.get() ).getFileNameWithoutExtension();
                if( !text.isEmpty() )
                {
                    if( sample->minNote.get() != sample->maxNote.get() ) //For KeyGroup Program
                    {
                        for( auto i = 0; i <= sample->maxNote.get(); i++ )
                        {
                            pads->setRow1Text( i, text, juce::dontSendNotification );
                            pads->setBorderColor( i, ui::Theme::backgroundColour( 6 ), 1, juce::dontSendNotification );
                        }
                    }
                    else //For  Drum and Cip Programs
                    {
                        pads->setRow1Text( sample->keyNote.get(), text, juce::dontSendNotification );
                        pads->setBorderColor( sample->keyNote.get(), ui::Theme::backgroundColour( 6 ), 1, juce::dontSendNotification );
                    }
                }
            }
        }
    }

    void AssignSampleContent::refreshLEDs()
    {
        std::vector<contracts::PadData> padDataList;
        auto                            bankOffset = pads->getBankOffset();

        ae.getProgramSamples( ae.getSelectedProgram(), samples );
        auto programData = ae.getSelectedProgramData();
        if( programData )
        {
            mt::MTProgramType programType = programData->type.get();
            if( programType == mt::MTProgramType::Keygroup )
            {
                if( samples.size() )
                {
                    for( auto& it : samples )
                    {
                        if( it->getSource().length() )
                        {
                            for( auto i = 0; i < 16; i++ )
                                padDataList.push_back( { i, contracts::DeviceControlContracts::PadButtonState::StateSampleAssigned, contracts::DeviceControlContracts::padColourDarkYellow } );
                        }
                        else //for Safety
                        {
                            for( auto i = 0; i < 16; i++ )
                                padDataList.push_back( { i, contracts::DeviceControlContracts::PadButtonState::StateSampleNotAssigned, contracts::DeviceControlContracts::PadColourTrans } );
                        }
                    }
                }
                else
                {
                    for( auto i = 0; i < 16; i++ )
                        padDataList.push_back( { i, contracts::DeviceControlContracts::PadButtonState::StateSampleNotAssigned, contracts::DeviceControlContracts::PadColourTrans } );
                }
            }

            else //For Drum and Clip Programs
            {
                for( auto colourIndex = bankOffset; colourIndex < bankOffset + NUM_PADS_IN_BANK; colourIndex++ )
                {
                    bool pushedToColourData = false;
                    for( auto& it : samples )
                    {
                        auto index = it->keyNote.get() - bankOffset;
                        if( juce::isPositiveAndBelow( index, 16 ) && it->getSource().length() && colourIndex == it->keyNote.get() )
                        {
                            pushedToColourData = true;
                            padDataList.push_back( { colourIndex - bankOffset, contracts::DeviceControlContracts::PadButtonState::StateSampleAssigned, contracts::DeviceControlContracts::padColourDarkYellow } );
                            break;
                        }
                    }
                    if( !pushedToColourData )
                        padDataList.push_back( { colourIndex - bankOffset, contracts::DeviceControlContracts::PadButtonState::StateSampleNotAssigned, contracts::DeviceControlContracts::PadColourTrans } );
                }
            }
        }
        contracts::DclSignals::sendCommandToDcl( contracts::DclContractsHelper::
                                                     setPadColourAndStateInfoMessage( padDataList ) );
        auto highlightedPad = pads->getHighlightedPad();
        if( highlightedPad != -1 ) //checking there is a highlight pad or not
        {
            auto padIndex = highlightedPad - pads->getBankOffset();
            if( juce::isPositiveAndBelow( padIndex, _NUM_PADS_IN_COMPONENT ) ) //checking highlighted pad is in current Bank
            {
                contracts::DclSignals::sendCommandToDcl( contracts::DclContractsHelper::
                                                             setPadDataMessage( padIndex, contracts::DeviceControlContracts::PadButtonState::StateOn, contracts::DeviceControlContracts::padColourlightGreen1 ) );
                ui::fileUtils::lastPressedPad = padIndex;
            }
        }
    }
    //Not used
    void AssignSampleContent::sendAssignedSamplesAtStartup( void )
    {
        refresh();
        auto bankOffset = pads->getBankOffset();
        for( auto& it : samples )
        {
            auto index = (int)it->keyNote.get() - bankOffset;
            if( juce::isPositiveAndBelow( index, 16 ) && it->getSource().length() )
            {
                DBG( "Assign Sample Start Up" );
                contracts::DclSignals::sendCommandToDcl( contracts::DclContractsHelper::
                                                             setPadDataMessage( index, contracts::DeviceControlContracts::PadButtonState::StateOn, contracts::DeviceControlContracts::PadColourTrans ) );
            }
        }
    }

    void AssignSampleContent::audition( juce::File f )
    {
        ae.stopPreview();
        if( f.existsAsFile() )
            ae.createPreview( f, mt::getSettings().preview.gain );
    }

    void AssignSampleContent::handleCommand( CPktMgr::HWMsg_t& f_HwMsg )
    {
        auto padIndex = pads->getHighlightedPad();
        auto ledIndex = padIndex - pads->getBankOffset();
        if( contracts::DclContractsHelper::isButtonMessage( f_HwMsg ) )
        {
            auto buttonCode  = contracts::DclContractsHelper::getButtonCode( f_HwMsg );
            auto buttonState = contracts::DclContractsHelper::getButtonState( f_HwMsg );
            if( buttonCode == contracts::DeviceControlContracts::EN_ERASE )
            {
                if( buttonState == contracts::DeviceControlContracts::BTN_PRESSED )
                {
                    mt::getAudioInterface().removeSampleFromProgram( mt::getAudioInterface().getSelectedProgram(), padIndex );

                    contracts::DclSignals::sendCommandToDcl( contracts::DclContractsHelper::
                                                                 setPadDataMessage( ledIndex,
                                                                                    contracts::DeviceControlContracts::PadButtonState::StateSampleNotAssigned,
                                                                                    contracts::DeviceControlContracts::padColourlightGreen1 ) );

                    contracts::DclSignals::sendCommandToDcl( contracts::DclContractsHelper::setLEDONMessage( contracts::DeviceControlContracts::EN_ERASE ) );

                    refresh();
                    pads->setHighlight( padIndex, ui::Theme::padButtonColour( 3 ) ); //highlight UI pad
                }
                else if( buttonState == contracts::DeviceControlContracts::BTN_RELEASED )
                {
                    contracts::DclSignals::sendCommandToDcl( contracts::DclContractsHelper::setLEDOFFMessage( contracts::DeviceControlContracts::EN_ERASE ) );
                }
            }
            else if( buttonCode == contracts::DeviceControlContracts::EN_KNOB_ENTER && buttonState == contracts::DeviceControlContracts::BTN_PRESSED )
            {
                assignSampleIntoPad( padIndex );
                ui::fileUtils::setHighlightLED( padIndex );
            }
            else
            {
                auto& aeSel = ae.getSelected();
                switch( buttonCode )
                {
                    case contracts::DeviceControlContracts::EN_PLAY: ae.setSequencePlay( aeSel.sequence, false, true ); break;
                    case contracts::DeviceControlContracts::EN_PLAYSTART: ae.setSequencePlay( aeSel.sequence, true, true ); break;
                    case contracts::DeviceControlContracts::EN_STOP: ae.setSequencePause(); break;
                }
            }
        }
        else if( contracts::DclContractsHelper::isKnobMessage( f_HwMsg ) )
        {
            if( contracts::DclContractsHelper::getKnobCode( f_HwMsg ) == contracts::DeviceControlContracts::EN_KNOB_CENTER )
            {
                fileList.knobRotated( contracts::DclContractsHelper::getKnobValue( f_HwMsg ) );
                auto selectedIndex = juce::jlimit( 0, fileList.getSize() - 1, fileList.getSelectedIndex() + contracts::DclContractsHelper::getKnobValue( f_HwMsg ) );
                fileList.setSelected( selectedIndex, juce::sendNotification );
            }
        }
        else if( contracts::DclContractsHelper::isPadButtonMessage( f_HwMsg ) )
        {
            if( auto prog = ae.getProgram( ae.getSelected().program ) )
            {
                auto bankOffset = ae.getBankOffset();
                auto padNo      = bankOffset + contracts::DclContractsHelper::getPadNumber( f_HwMsg );
                auto padState   = contracts::DclContractsHelper::getPadState( f_HwMsg );
                auto velocity   = contracts::DclContractsHelper::getPadVelocity( f_HwMsg );

                if( padState == contracts::DeviceControlContracts::BTN_PRESSED )
                    prog->noteOn( padNo, velocity );
                else if( padState == contracts::DeviceControlContracts::BTN_RELEASED )
                {
                    switch( prog->getType() )
                    {
                        default: prog->noteOff( padNo ); break;
                        case mt::MTProgramType::Clip: break;
                    }
                }
            }
        }
        else // Need to check when it's should call
            ui::getComponentInFocus()->handleCommand( f_HwMsg );
    }
#if NOT_USED
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DrumAssignSampleContent::DrumAssignSampleContent()
        : ae( mt::getAudioInterface() )
        , pads( std::make_unique<ui::KickPadButtonGroupComponent>( ui::KickPadButton::PadButtonType::assignSample ) )
    {
        setLookAndFeel( ui::KickLooknFeel::getLnf() );

        mt::utils::addChildComponents( this, pads.get(), pads->getBankComponent() );

        fileList.setBackgroundColour( ui::Theme::backgroundColour( 4 ) );

        pads->OnButtonClicked = [this]( int Id ) { //software pad button clicked
            auto bankOffset = pads->getBankOffset();
            fileList.setContent( ( files ), 0 );
            fileList.show( "Pad - " + juce::String( Id ), [this, Id]( int R ) {
                if( ( R == UI::APPLY ) || ( 0 <= R ) )
                {
                    auto index = fileList.getSelectedIndex();
                    if( 0 == index )
                    {
                        ae.removeSampleFromProgram( ae.getSelectedProgram(), Id );
                        contracts::DclSignals::sendCommandToDcl( contracts::DclContractsHelper::
                                                                     setPadDataMessage( index, contracts::DeviceControlContracts::PadButtonState::StateOff ) );
                    }
                    else
                    {
                        auto sample = mt::getAudioInterface().getAsset( mt::AssetTypes::Samples, index - 1 );
                        ae.addSampleToProgram( ae.getSelectedProgram(), Id, sample, 0 );
                    }
                    refresh();
                }
            } );
        };

        pads->OnBankChanged = [this]( juce::String Id ) {
            refreshLEDs();
        };
    }

    void DrumAssignSampleContent::refresh()
    {
        pads->clearAllPadData();
        pads->setActive( -1, true );
        pads->setBorderColor( ui::Theme::selectionColour( 1 ), 2.5f, juce::dontSendNotification );

        auto progIdx  = ae.getSelectedProgram();
        auto progType = ae.getProgramType( progIdx );

        switch( progType )
        {
            case mt::MTProgramType::Drum:
                pads->setPrefix( "DR" );
                break;
            case mt::MTProgramType::Keygroup:
                pads->setPrefix( "KG" );
                break;
            case mt::MTProgramType::Audio:
                pads->setPrefix( "AU" );
                break;
            case mt::MTProgramType::Clip:
                pads->setPrefix( "CL" );
                break;
            default:
            case mt::MTProgramType::Unknown:
            case mt::MTProgramType::Plugin:
            case mt::MTProgramType::Cv:
            case mt::MTProgramType::Midi:
                break;
        }

        samples.clear();
        ae.getProgramSamples( progIdx, samples );
        for( const auto& sample : samples )
        {
            auto text = juce::File( sample.sources[0] ).getFileNameWithoutExtension();
            pads->setRow1Text( sample.keyNote, text, juce::dontSendNotification );
        }

        refreshLEDs();
    }

    void DrumAssignSampleContent::refreshLEDs()
    {
        auto bankOffset = pads->getBankOffset();
        //DBG( "Get BankOFfset -> " << bankOffset );
        // Clear all pad LED's.
        contracts::DclSignals::sendCommandToDcl( contracts::DclContractsHelper::clearAllPadsMessage() );
        //Controls::InputEvents::clearAllPads();
        // Set required pad LED's
        for( auto& it : samples )
        {
            auto index = it.keyNote - bankOffset;
            if( juce::isPositiveAndBelow( index, 16 ) && it.sources[0].length() )
            {
                contracts::DclSignals::sendCommandToDcl( contracts::DclContractsHelper::
                                                             setPadDataMessage( index, contracts::DeviceControlContracts::PadButtonState::StateOn, contracts::DeviceControlContracts::PadColourOrange ) );
            }
        }
    }

    void DrumAssignSampleContent::sendAssignedSamplesAtStartup( void )
    {
        refresh();
        auto bankOffset = pads->getBankOffset();
        for( auto& it : samples )
        {
            auto index = it.keyNote - bankOffset;
            if( juce::isPositiveAndBelow( index, 16 ) && it.sources[0].length() )
            {
                DBG( "Assign Sample Start Up" );
                contracts::DclSignals::sendCommandToDcl( contracts::DclContractsHelper::
                                                             setPadDataMessage( index, contracts::DeviceControlContracts::PadButtonState::StateOn, contracts::DeviceControlContracts::PadColourTrans ) );
            }
        }
    }

    void DrumAssignSampleContent::visibilityChanged()
    {
        bool visible = isVisible();
        if( visible )
        {
            files = mt::getAudioInterface().getAssetNames( mt::AssetTypes::Samples );
            files.insert( files.begin(), "Clear Assignment" );

            refresh();
        }

        for( auto& it : getChildren() )
            it->setVisible( visible );
    }

    void DrumAssignSampleContent::resized()
    {
        pads->setBoundsRelative( .02f, .02f, 0.96f, 0.90f );
        pads->getBankComponent().setBoundsRelative( .60f, .93f, 0.35f, 0.06f );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    KeysAssignSampleContent::KeysAssignSampleContent()
        : ae( mt::getAudioInterface() )
    {
        setLookAndFeel( ui::KickLooknFeel::getLnf() );
        mt::utils::addChildComponents( this, left, right, bottom );

        left.provider.numRows = [&]() {
            return (int)ae.getAssets().size();
        };
        right.provider.numRows = [&]() {
            return numSamples;
        };

        left.provider.cellComponent = [&]( int Row, int Col, int IsSelected, Component* Existing ) {
            auto comp = static_cast<ui::KickLabel*>( Existing );
            if( juce::isPositiveAndBelow( Row, ae.getAssets().size() ) )
            {
                if( comp == nullptr )
                {
                    comp = new ui::KickLabel( "" );
                    comp->setInterceptsMouseClicks( false, false );
                }
                if( 1 == Col ) comp->setText( juce::String( Row + 1 ), juce::dontSendNotification );
                if( 2 == Col ) comp->setText( juce::File( ae.getAssets()[(size_t)Row] ).getFileName(), juce::dontSendNotification );
                comp->setColours( IsSelected ? ui::Theme::textColour( 2 ) : ui::Theme::textColour( 1 ), ui::Theme::backgroundColour( 9 ) );
            }
            return comp;
        };

        right.provider.cellComponent = [&]( int Row, int Col, int IsSelected, Component* Existing ) {
            auto comp = static_cast<ui::KickLabel*>( Existing );
            if( juce::isPositiveAndBelow( Row, numSamples ) )
            {
                if( comp == nullptr )
                {
                    comp = new ui::KickLabel( "" );
                    comp->setInterceptsMouseClicks( false, false );
                }

                if( 1 == Col ) comp->setText( juce::String( Row + 1 ), juce::dontSendNotification );
                if( 2 == Col ) comp->setText( juce::File( samples[Row].sources[0] ).getFileName(), juce::dontSendNotification );

                comp->setColours( IsSelected ? ui::Theme::textColour( 2 ) : ui::Theme::textColour( 1 ), ui::Theme::backgroundColour( 9 ) );
            }
            return comp;
        };

        left.table->onRowSelected = [&]( int Row ) {
            selectedAsset = ae.getAssets()[(size_t)Row];
        };
        right.table->onRowSelected = [&]( int Row ) {
            if( juce::isPositiveAndBelow( Row, samples.size() ) ) selectedRow = Row;
        };

        bottom.clear.onClick = [&]() {
            if( juce::isPositiveAndBelow( selectedRow, samples.size() ) )
            {
                ae.removeSampleFromProgram( ae.getSelectedProgram(), samples[selectedRow].keyNote, samples[selectedRow].sources[0] );
                refreshSampleList();
                right.table->refresh();
            }
        };

        bottom.load.onClick = [&]() {
            auto progNum = ae.getSelectedProgram();
            auto idx     = ae.getNumProgramSamples( progNum );
            ae.addSampleToProgram( progNum, idx, selectedAsset, 0 );
            refreshSampleList();
            right.table->refresh();
        };

        bottom.audition.onClick = [&]() {
            ae.createPreview( { selectedAsset }, 0.0f );
        };
    }

    void KeysAssignSampleContent::paint( juce::Graphics& g )
    {
        g.fillAll( ui::Theme::backgroundColour( 5 ) );
    }

    void KeysAssignSampleContent::refreshSampleList()
    {
        ae.getProgramSamples( ae.getSelectedProgram(), samples );
        numSamples = ae.getNumProgramSamples( ae.getSelectedProgram() );
    }

    void KeysAssignSampleContent::visibilityChanged()
    {
        bool visible = isVisible();
        if( visible )
        {
            refreshSampleList();
            left.table->refresh();
            right.table->refresh();
        }
        else
        {
            samples.clear();
            numSamples = 0;
            ae.stopPreview();
        }

        for( auto it : getChildren() )
            it->setVisible( visible );
    }

    void KeysAssignSampleContent::resized()
    {
        left.setBoundsRelative( .0f, .0f, 0.35f, .9f );
        right.setBoundsRelative( .35f, .0f, 0.65f, .9f );
        bottom.setBoundsRelative( .0f, .9f, 1.0f, .090f );
    }

    /// ///
    SamplesList::SamplesList()
    {
        setLookAndFeel( ui::KickLooknFeel::getLnf() );
        table.reset( new ui::KickTable( "List", provider ) );

        addAndMakeVisible( *table );
        table->addColumn( { "#", 1, 30 } );
        table->addColumn( { "Name", 2, 280 } );
        table->setHeight( 30, 30 );
        table->setRowBackgroundColour( juce::Colours::transparentBlack );
    }

    void SamplesList::paint( juce::Graphics& g )
    {
        juce::Path p;
        auto       b = getLocalBounds().withTrimmedLeft( 10 );
        p.addRoundedRectangle( (float)b.getX(), (float)b.getY(), (float)b.getWidth(), (float)b.getHeight(), 5.0f, 5.0f, true, false, true, false );
        g.setColour( ui::Theme::backgroundColour( 2 ) );
        g.fillPath( p );
    }

    void SamplesList::resized()
    {
        table->setBoundsRelative( .02f, .02f, .96f, .96f );
    }

    KeysPanel::KeysPanel()
    {
        setLookAndFeel( ui::KickLooknFeel::getLnf() );
        table.reset( new ui::KickTable( "List", provider ) );

        addAndMakeVisible( *table );
        table->addColumn( { "#", 1, 30 } );
        table->addColumn( { "Name", 2, 400 } );
        table->addColumn( { "Min", 3, 60 } );
        table->addColumn( { "Root", 4, 60 } );
        table->addColumn( { "Max", 5, 60 } );
        table->setHeight( 30, 40 );
        table->setRowBackgroundColour( juce::Colours::transparentBlack );
    }

    void KeysPanel::paint( juce::Graphics& g )
    {
        juce::Path p;
        auto       b = getLocalBounds().withTrimmedRight( 10 );
        p.addRoundedRectangle( (float)b.getX(), (float)b.getY(), (float)b.getWidth(), (float)b.getHeight(), 5.0f, 5.0f, false, true, false, true );
        g.setColour( ui::Theme::backgroundColour( 4 ) );
        g.fillPath( p );
    }

    void KeysPanel::resized()
    {
        table->setBoundsRelative( .02f, .02f, .96f, .96f );
    }

    KeysBottomControls::KeysBottomControls()
    {
        mt::utils::addAndMakeVisible( this, audition, clear, load );
    }

    void KeysBottomControls::paint( juce::Graphics& g )
    {
        g.setColour( ui::Theme::backgroundColour( 3 ) );
    }
    void KeysBottomControls::resized()
    {
        auto y          = (float)( 10 ) / (float)getLocalBounds().getHeight();
        auto bottomBtnH = 1.0f - ( 2 * y );
        audition.setBoundsRelative( .61f, y, .11f, bottomBtnH );
        clear.setBoundsRelative( .74f, y, .11f, bottomBtnH );
        load.setBoundsRelative( .87f, y, .11f, bottomBtnH );
    }

    void KeysBottomControls::visibilityChanged() {}
    ///
#endif
    /// @brief
    /// Class AppAssignSample...
    ///
    AppAssignSample::AppAssignSample()
        : kick_ui::IScreen( kick_ui::Screen::MenuAssignSample )
    {
        setLookAndFeel( ui::KickLooknFeel::getLnf() );

        mt::utils::addChildComponents( this, header, drum, bankLabel, leftNav, rightNav, fileBrowser );
        mt::utils::makeHidden( drum );

        header.onBackButtonPressed = [this]( bool ) { closeScreen(); };

        header.showOptions( false, false );

        bankLabel.setText( drum.pads->getBank(), juce::dontSendNotification );
        bankLabel.setTextColour( ui::Theme::textColour( 1 ) );
        bankLabel.setValueFontHeight( 24 );
        bankLabel.setValueBackgroundColour( ui::Theme::backgroundColour( 3 ) );
        bankLabel.setValueJustification( juce::Justification::centred );

        leftNav.onClick = [&]() {
            auto index = drum.pads->getBankIndex();
            if( index == 0 )
                drum.pads->setBank( 0 );
            else
                drum.pads->setBank( index - 1 );
            bankLabel.setText( drum.pads->getBank(), juce::dontSendNotification );
            mt::getAudioInterface().setBank( drum.pads->getBank() );
        };

        rightNav.onClick = [&] {
            auto index = drum.pads->getBankIndex();
            if( index == 7 )
                drum.pads->setBank( 7 );
            else
                drum.pads->setBank( index + 1 );
            bankLabel.setText( drum.pads->getBank(), juce::dontSendNotification );
            mt::getAudioInterface().setBank( drum.pads->getBank() );
        };

        fileBrowser.onClick = [&]() {
            kick_ui::signals::sigScreenOpen( kick_ui::Screen::MenuFileBrowser );
        };

        ui::themeChanged.connect( [&]() {
            leftNav.setImages( "Icon_minus" );
            rightNav.setImages( "Icon_plus" );
            header.setIcon( "sEdit_Icon" );
        } );

        mt::signals::bankChanged.connect( [&]( int Index, juce::String Bank ) {
            setBank( Index, Bank );
        } );
    }

    void AppAssignSample::resized()
    {
        auto b = getLocalBounds();
        header.setBounds( b.removeFromTop( ui::layout::header.getHeight() ) );

        // Childrens...
        drum.setBounds( b );
        fileBrowser.setBoundsRelative( .6892f, .04f, .0429f, .08f );

        bankLabel.setBoundsRelative( .74f, .038f, .19f, .085f );
        leftNav.setBoundsRelative( .74f, .034f, .043f, .09f );
        rightNav.setBoundsRelative( .9f, .034f, .043f, .09f );
    }
    void AppAssignSample::visibilityChanged()
    {
        auto& ae = mt::getAudioInterface();
        for( auto it : getChildren() )
            it->setVisible( isVisible() );
        if( isVisible() )
        {
            contracts::DclSignals::sendCommandToDcl( contracts::DclContractsHelper::
                                                         setPadColorModeMessage( mt::PadColorMode::DISABLE_PAD_COLOUR_MODE ) );
            bankLabel.setText( ae.getBank(), juce::dontSendNotification );
        }
        else
        {
            auto MTIProg = ae.getProgramData( ae.getSelectedProgram() );
            contracts::DclSignals::sendCommandToDcl( contracts::DclContractsHelper::
                                                         setPadColorModeMessage( MTIProg ? MTIProg->colourMode.get() : mt::OFF ) );

            contracts::DclSignals::refreshPadLeds();
        }
    }

    void AppAssignSample::handleCommand( CPktMgr::HWMsg_t& f_HwMsg )
    {
        drum.handleCommand( f_HwMsg );
    }

    void AppAssignSample::setBank( int Index, juce::String Bank )
    {
        if( kick_ui ::KickHome::Instance().getCurrentScreen() == kick_ui::Screen::MenuAssignSample )
        {
            juce::MessageManager::callAsync( [this, Index, Bank] {
                bankLabel.setText( Bank, juce::dontSendNotification );
                ui::fileUtils::lastPressedPad = -1;
                drum.pads->setBank( Index );
                drum.refreshLEDs();
            } );
        }
    }
}
