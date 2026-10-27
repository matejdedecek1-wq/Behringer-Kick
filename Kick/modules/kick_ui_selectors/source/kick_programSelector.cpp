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

    void setProgramIcon( ui::KickImageButton& Prog, mt::MTProgramType Type )
    {
        Prog.setImages( getDisplayProgramIcon( Type ) );
    }

    juce::String getDisplayProgramIcon( mt::MTProgramType Type )
    {
        switch( Type )
        {
            case mt::MTProgramType::Drum: return "PgmSel_Drum"; break;
            case mt::MTProgramType::Keygroup: return "PgmSel_Key"; break;
            case mt::MTProgramType::Midi: return "PgmSel_MIDI"; break;
            case mt::MTProgramType::Audio: return "PgmSel_Audio"; break;
            case mt::MTProgramType::Clip: return "PgmSel_Clip"; break;
            case mt::MTProgramType::Plugin: return "PgmSel_PLUGIN"; break;
            case mt::MTProgramType::Cv: return "PgmSel_CV"; break;
            default:
                return "PgmSel_Default";
                break;
        }
    }

    juce::String getProgramIcon( mt::MTProgramType Type )
    {
        switch( Type )
        {
            case mt::MTProgramType::Drum: return "PgmIcon_Drum"; break;
            case mt::MTProgramType::Keygroup: return "PgmIcon_Key"; break;
            case mt::MTProgramType::Midi: return "PgmIcon_MIDI"; break;
            case mt::MTProgramType::Audio: return "PgmIcon_Audio"; break;
            case mt::MTProgramType::Clip: return "PgmIcon_Clip"; break;
            case mt::MTProgramType::Plugin: return "PgmIcon_PLUGIN"; break;
            case mt::MTProgramType::Cv: return "PgmIcon_CV"; break;
            default:
                return "PgmSel_Default";
                break;
        }
    }

    juce::String getEditProgramIcon( mt::MTProgramType Type )
    {
        switch( Type )
        {
            case mt::MTProgramType::Drum: return "ID_Drum"; break;
            case mt::MTProgramType::Keygroup: return "ID_KEY"; break;
            case mt::MTProgramType::Midi: return "ID_MIDI"; break;
            case mt::MTProgramType::Audio: return "ID_AUdio"; break;
            case mt::MTProgramType::Clip: return "ID_CLIP"; break;
            case mt::MTProgramType::Plugin: return "ID_PLUGIN"; break;
            case mt::MTProgramType::Cv: return "ID_CV"; break;
            default: return "ID_Program"; break;
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////    Header Bar - Program Selector
    //////////////////////////////////////////////////////////////////////////////////////////
    KickProgramSelector::KickProgramSelector( const juce::String& Title )
    {
        setTitle( Title );
        setName( Title );

        table.reset( new ui::KickTableView( "List", provider ) );
        addAndMakeVisible( *table );
        table->setRowHeight( 44 );
        table->setRowBackgroundColour( ui::Theme::backgroundColour( 9 ) );
        table->setSelectedRowBackgroundColour( ui::Theme::backgroundColour( 0 ) );
        table->setSelectedRowOutlineColour( ui::Theme::outlineColour( 0 ) );
        table->setRowSpacing( 20 );
        table->enableScrollBar( false );
        table->setBackgroundColour( ui::Theme::backgroundColour( 1 ) );

        mt::utils::addAndMakeVisible( this, progList );
        progList.setColumns( 3 );
        progList.onSelectionChanged = [&]( int Index, const juce::String& Option ) {
            if( Option.isNotEmpty() )
                selectedProgram = Option;
            selectedType = tempType;
        };
        provider.numRows = [&]() {
            return 7;
        };
        provider.cellComponent = [&]( int Row, Component* Existing ) -> juce::Component* {
            if( juce::isPositiveAndBelow( Row, 7 ) )
            {
                auto comp = dynamic_cast<ProgramSelectorDisplay*>( Existing );
                if( comp == nullptr )
                {
                    comp = new ProgramSelectorDisplay( Row );
                }
                auto type = getprogramType( Row );
                comp->setProperties( getProgramIcon( type ), mt::toString( type ) );

                if( programs[type].size() )
                    comp->setDisableComp( false );
                else
                {
                    comp->setDisableComp( true );
                }

                return comp;
            }
            return nullptr;
        };

        provider.onRowSelected = [&]( int Row ) {
            selectedType = getprogramType( Row );
            refreshContent();
            setupListContent( programs[selectedType] );
        };
        progList.setBackgroundColour( ui::Theme::backgroundColour( 1 ) );
        progList.drawCurvesAndOutline( true, true );
        progList.setButtonHeight( 44 );
    }

    KickProgramSelector::~KickProgramSelector()
    {
        DBG( "::~KickProgramSelector()" );
    }

    const juce::String& KickProgramSelector::getSelectedProgram()
    {
        return selectedProgram;
    }

    const mt::MTProgramType KickProgramSelector::getSelectedProgramType()
    {
        return selectedType;
    }

    mt::MTProgramType KickProgramSelector::getprogramType( int Index )
    {
        switch( Index )
        {
            default:
                return mt::MTProgramType::Unknown;
                break;
            case 0:
                return mt::MTProgramType::Drum;
                break;
            case 1:
                return mt::MTProgramType::Keygroup;
                break;
            case 2:
                return mt::MTProgramType::Clip;
                break;
            case 3:
                return mt::MTProgramType::Audio;
                break;
            case 4:
                return mt::MTProgramType::Plugin;
                break;
            case 5:
                return mt::MTProgramType::Midi;
                break;
            case 6:
                return mt::MTProgramType::Cv;
                break;
        }
    }
    void KickProgramSelector::setValues( const juce::String& Program, const mt::MTProgramType Type )
    {
        selectedProgram = Program;
        selectedType    = Type;
        refreshContent();
        setupListContent( programs[selectedType] );
        progList.setSelected( Program, juce::dontSendNotification );
    }
    void KickProgramSelector::paint( juce::Graphics& g )
    {
        mt::utils::fillRoundedRect( g, tableBounds, ui::Theme::backgroundColour( 1 ), 6.0f );
        mt::utils::renderRoundedRect( g, tableBounds, ui::Theme::outlineColour( 3 ), 1, 6.0f );
    }

    void KickProgramSelector::refreshContent()
    {
        std::vector<mt::MTIProgramData*> programsList;
        ae.getProgramList( programsList );

        programs.clear();
        for( auto it : programsList )
        {
            auto type = it->type.get();
            programs[type].push_back( it->name.get() );
        }
    }

    void KickProgramSelector::setupListContent( std::vector<juce::String>& Options )
    {
        int                 rowNum = -1;
        int                 count  = -1;
        mt::MTIProgramData* progData;

        //If it is triggering from AppMainMode
        if( isBasedOnTrackProgram )
            progData = ae.getProgramDataForTrack( ae.getSelectedSequence(), ae.getSelectedTrack() );
        else //Triggered not from AppMainMode
            progData = ae.getSelectedProgramData();
        if( progData )
        {
            for( auto options : Options )
            {
                count++;

                if( options == progData->name )
                {
                    rowNum = count;
                    break;
                }
            }
        }
        progList.setContent( Options, rowNum, juce::sendNotificationAsync );
    }

    void KickProgramSelector::resized()
    {
        auto b  = getLocalBounds();
        auto b1 = getLocalBounds();
        table->setBounds( b.removeFromLeft( 350 ).withTrimmedLeft( 20 ).withTrimmedTop( 15 ).reduced( 2 ) );
        progList.setBounds( b1.withTrimmedLeft( 360 ).withTrimmedTop( 8 ).withTrimmedRight( 6 ) );
    }

    void KickProgramSelector::visibilityChanged()
    {
    }
    void KickProgramSelector::updateRowSelection( bool BasedOnTrackProgram )
    {
        isBasedOnTrackProgram = BasedOnTrackProgram;
        table->clearSelection();

        auto progType = mt::getAudioInterface().getProgramType( getSelectedProgram() );
        if( progType == mt::MTProgramType::Unknown )
        {
            for( auto Row = 0; Row < NUM_OF_PROGRAMS; Row++ )
            {
                auto type = getprogramType( Row );
                if( programs[type].size() )
                {
                    table->selectRow( Row );
                    break;
                }
            }
        }
        else
        {
            for( auto Row = 0; Row < NUM_OF_PROGRAMS; Row++ )
            {
                auto type = getprogramType( Row );
                if( type == progType )
                {
                    table->selectRow( Row );
                    break;
                }
            }
        }
    }
    KickProgramSelectorWithDisplay::KickProgramSelectorWithDisplay( bool ShowName )
        : selector( std::make_unique<KickProgramSelector>( "Select Program" ) )
        , bShowName( ShowName )
    {
        progIcon.setBackgroundColour( ui::Theme::backgroundColour( 9 ) );
        progLabel.setEditable( true, false );
        progLabel.showDropdown( true );

        auto onClicked = [&]() {
            selector->refreshContent();
            mt::utils::showOptions(
                selector->getName(), selector.get(), [&]( int r ) {
                    if( r == UI::APPLY )
                    {
                        refresh();
                        if( onSelectionChanged )
                            onSelectionChanged( selector->getSelectedProgram(), selector->getSelectedProgramType() );
                    }
                },
                ui::layout::screen, ui::Theme::backgroundColour( 5 ), ui::Theme::titleColour( 1 ), ui::Theme::backgroundColour( 2 ), ui::Theme::backgroundColour( 5 ) );
            selector->table->refresh();
            selector->updateRowSelection();
        };

        progLabel.onEditorShow = onClicked;
        progIcon.onClick       = onClicked;

        refresh();
    }

    void KickProgramSelectorWithDisplay::showName()
    {
        bShowName = true;
        resized();
    };
    void KickProgramSelectorWithDisplay::showIcon()
    {
        bShowName = false;
        resized();
    };

    void KickProgramSelectorWithDisplay::setValues( const juce::String& Program, const mt::MTProgramType Type, juce::NotificationType Notification )
    {
        selector->setValues( Program, Type );
        refresh();
        if( onSelectionChanged && Notification != juce::dontSendNotification )
            onSelectionChanged( selector->getSelectedProgram(), selector->getSelectedProgramType() );
    }

    void KickProgramSelectorWithDisplay::refresh()
    {
        auto& ae      = mt::getAudioInterface();
        auto  program = selector->getSelectedProgram();
        progLabel.setText( program, juce::dontSendNotification );
        setProgramIcon( progIcon, ae.getProgramType( program ) );
        repaint();
    }

    void KickProgramSelectorWithDisplay::resized()
    {
        removeAllChildren();
        if( bShowName )
            addAndMakeVisible( progLabel );

        else
            addAndMakeVisible( progIcon );

        auto b = getLocalBounds();
        progLabel.setBounds( b );
        progIcon.setBounds( b );
    }

    void KickProgramSelectorWithDisplay::visibilityChanged()
    {
    }
    ProgramSelectorDisplay::ProgramSelectorDisplay( int rowId )
    {
        mt::utils::addAndMakeVisible( this, imagebutton, label );
        label.setValueBackgroundColour( ui::Theme::backgroundColour( 9 ) );
        label.setFontHeight( 14 );
        label.setValueColour( ui::Theme::textColour( 1 ) );
    }
    void ProgramSelectorDisplay::resized()
    {
        auto b = getLocalBounds();
        imagebutton.setBounds( b.removeFromLeft( 72 ).withTrimmedLeft( 38 ).withTrimmedTop( 0 ).withTrimmedBottom( 0 ) );
        label.setBoundsRelative( .3687f, .1333f, .8085f, .7333f );
    }
    void ProgramSelectorDisplay::setProperties( const juce::String& image, const juce::String& labeltext )
    {
        imagebutton.setImages( image );
        label.setText( labeltext, juce::dontSendNotification );
    }

    void ProgramSelectorDisplay::setDisableComp( bool disable )
    {
        setEnabled( !disable );
        if( disable )
            label.setValueColour( ui::Theme::textColour( 1 ) );
        else
            label.setValueColour( ui::Theme::textColour( 2 ) );
    }

} // namespace
