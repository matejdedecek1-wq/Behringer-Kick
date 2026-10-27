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
    class APPTrackOptionEraseEvents : public juce::Component
    {
    public:
        APPTrackOptionEraseEvents()
        {
            setName( "Erase Events" );
            mt::utils::addAndMakeVisible( this, startBar, endBar, startNote, endNote );
            startBar.showTitle( true, juce::Justification::centred );
            endBar.showTitle( true, juce::Justification::centred );
            startBar.setRange( 1, ae.getSequenceLength( ae.getSelectedSequence() ), 1 );
            endBar.setRange( 1, ae.getSequenceLength( ae.getSelectedSequence() ), 1 );
            startBar.setValue( 0, juce::dontSendNotification );
            endBar.setValue( ae.getSequenceLength( ae.getSelectedSequence() ), juce::dontSendNotification );

            int letterAsciiCode = 65;

            padData.clear();
            for( int i = 0; i <= kick_Constants::totalPianoKeys; i += 16 )
            {
                for( int j = 0; j < 16; j++ )
                {
                    juce::String noteLabel;
                    noteLabel = noteLabel.formatted( "%c%02d", letterAsciiCode, ( j + 1 ) );
                    padData.push_back( noteLabel );
                }
                letterAsciiCode++;
            }
            startNote.setListOptions( padData );
            endNote.setListOptions( padData );
            startNote.setSelected( 0, juce::dontSendNotification );
            endNote.setSelected( padData.size() - 1, juce::dontSendNotification );
        }
        ~APPTrackOptionEraseEvents() {}

    private:
        ui::KickSlider            startBar{ "Start Bar", juce::Slider::LinearBar, juce::Slider::TextBoxBelow };
        ui::KickSlider            endBar{ "End Bar", juce::Slider::LinearBar, juce::Slider::TextBoxBelow };
        ui::KickListBox           startNote{ "Start Note", { "" }, 0, 3 };
        ui::KickListBox           endNote{ "End Note", { "" }, 0, 3 };
        std::vector<juce::String> padData;
        mt::MTAudioInterface&     ae{ mt::getAudioInterface() };
        void                      resized() override
        {
            startBar.setBoundsRelative( .1f, .2f, .35f, .3f );
            endBar.setBoundsRelative( .55f, .2f, .35f, .3f );
            startNote.setBoundsRelative( .1f, .6f, .35f, .2f );
            endNote.setBoundsRelative( .55f, .6f, .35f, .2f );
        }

    public:
        int getNoteStart() { return startNote.getSelectedIndex(); }
        int getNoteEnd() { return endNote.getSelectedIndex(); }
        int getBarStart() { return startBar.getValue(); }
        int getBarEnd() { return endBar.getValue(); }
    };
    class APPTrackOptionCopy : public juce::Component
    {
    public:
        APPTrackOptionCopy()
        {
            setName( "COPY TRACK" );
            mt::utils::addAndMakeVisible( this, msg1, msg2, from, to );
            mt::utils::setTitleFontHeight( 16, msg1, msg2 );
            mt::utils::setValueFontHeight( 18, msg2, msg1 );
            msg1.setJustificationType( juce::Justification::centred );
            msg2.setJustificationType( juce::Justification::centred );
            msg1.setValueBackgroundColour( ui::Theme::backgroundColour( 9 ) );
            msg2.setValueBackgroundColour( ui::Theme::backgroundColour( 9 ) );

            msg1.setColour( juce::Label::textColourId, ui::Theme::textColour( 3 ) );
            msg2.setColour( juce::Label::textColourId, ui::Theme::textColour( 3 ) );

            from.onSelectionChanged = [&]( int, const juce::String& ) { info.from = from.getSelectedValue(); };
            to.onSelectionChanged   = [&]( int, const juce::String& ) { info.to = to.getSelectedValue(); };
        };

        const mt::track::copyInfo& getData() { return info; };
        void                       setTrackInfo( juce::String& From, const std::vector<juce::String>& TO )
        {
            fromTrackInfo.clear();
            fromTrackInfo.push_back( From );
            toTrackInfo = TO;
            from.setListOptions( toTrackInfo, 0 );
            to.setListOptions( toTrackInfo, 0 );
        }

    private:
        ui::KickLabel             msg1{ "Copy contents of" }, msg2{ "Over contents of" };
        std::vector<juce::String> fromTrackInfo, toTrackInfo;
        ui::KickListBox           from{ ui::KickListBox::BoxType::withTitleBar, "SOURCE TRACK", fromTrackInfo };
        ui::KickListBox           to{ ui::KickListBox::BoxType::withTitleBar, "DESTINATION TRACK", toTrackInfo };
        mt::track::copyInfo       info;

        void paint( juce::Graphics& g ) override
        {
            g.fillAll( ui::Theme::backgroundColour( 1 ) );
        };
        void resized() override
        {
            msg1.setBoundsRelative( .42f, .1f, 1.0f, .2f );
            from.setBoundsRelative( .1f, .25f, .80f, 0.2f );
            msg2.setBoundsRelative( .42f, .45f, .80f, 0.25f );
            to.setBoundsRelative( .1f, .63f, .8f, 0.2f );
        };
    };

    AppTrackOptionsMenu::AppTrackOptionsMenu()
        : KickMenuComponent( "TrackOptions", 4 )
    {
        addOption( { "HalfSpeed", "Half Speed" }, "" );
        addOption( { "DoubleSpeed", "Double Speed" }, "" );
        addOption( { "Explode", "Explode" }, "" );
        //addOption( { "Bounce", "Bounce_Track" }, "" );
        addOption( { "Quantize", "Quantize_Track" }, "" );

        addOption( { "Humanize", "Humanize" }, "" );
        addOption( { "Split", "Split_Events" }, "" );
        addOption( { "Copy", "CopyTrack" }, "" );
        addOption( { "Random", "Generate Random_Events" }, "" );

        addOption( { "Erase", "Erase_Event" }, "" );

        setVisible( true );
        setSelectionEnabled( false );
    }

    AppTrackOptionsMenu::~AppTrackOptionsMenu()
    {
        setVisible( false );
    }

    //==============================================================================

    AppTrackOptions::AppTrackOptions( void )

    {
        setComponentID( "TrackOptions" );
        setName( "Track Options" );
        setVisible( false );
        mt::utils::addAndMakeVisible( this, srNo, track, opts, close );

        srNo.setInterceptsMouseClicks( false, false );

        close.onClick = [&]() {
            if( onClose )
                onClose();
        };

        opts.onMenuOptionClicked = [&]( const juce::String& /*option*/, int Index, const juce::String& /*group*/ ) {
            auto& ae  = mt::getAudioInterface();
            auto& sel = ae.getSelected();
            switch( Index )
            {
                break;
                case 0: // Half Speed Events.
                    ae.halfSpeedNotes( sel );
                    close.triggerClick();
                    break;
                case 1: // Double Speed Events.
                    ae.doubleSpeedNotes( sel );
                    close.triggerClick();
                    break;
                case 2: // Explode.
                {
                    ae.explode( sel );
                    close.triggerClick();
                    break;
                }
                //case 3: // Bounce to Sample
                //    break;
                case 3: // Pitch Quantize
                {
                    auto p = new APPTrackOptionPitchQuantize();
                    mt::utils::showOptions( p->getName(), p, [&, p]( int R ) {
                        if( R == UI::APPLY ) ae.pitchQuantizeNotes( sel, p->getData() );
                        if( p ) delete p;
                    } );
                }
                break;
                case 4: // Humanize
                {
                    auto p = new APPTrackOptionHumanize();
                    mt::utils::showOptions( p->getName(), p, [&, p]( int R ) {
                        if( R == UI::APPLY ) ae.humanizeNotes( sel, p->getData() );
                        if( p ) delete p;
                    } );
                }
                break;
                case 5: // Split Events
                {
                    auto p = new APPTrackOptionSplitEvents();
                    mt::utils::showOptions(
                        p->getName(), p, [&, p]( int R ) {
                            if( R == UI::APPLY )
                            {
                                auto& Info = p->getData();
                                if( Info.onlySelected )
                                {
                                    auto selNotes = ae.getSelectedNoteList();
                                    if( !selNotes.isEmpty() )
                                        ae.splitSelectedNotes( sel, Info );
                                    else
                                        openWarningPopUp( "No Selected Events Are Present To Split" );
                                }
                                else
                                {
                                    if( ae.getNumNotes( ae.getSelected() ) > 0 )
                                        ae.splitNotes( sel, Info );
                                    else
                                        openWarningPopUp( "No Events Are Present To Split" );
                                }

                                if( p ) delete p;
                            }
                        },
                        ui::layout::warningPopup );
                }
                break;
                case 6: // Copy Track
                {
                    auto p         = new APPTrackOptionCopy();
                    auto fromT     = ae.getTrackData( sel.sequence, sel.track );
                    auto trackName = fromT ? fromT->name.get() : "";
                    p->setTrackInfo( trackName, ae.getTrackList( sel.sequence ) );
                    mt::utils::showOptions( p->getName(), p, [&, p]( int R ) {
                        if( R == UI::APPLY ) ae.copyTrack( sel.sequence, p->getData().from, p->getData().to );
                        if( p ) delete p;
                    } );
                    close.triggerClick();
                }
                break;
                case 7: // Random Events
                {
                    auto p = new APPTrackOptionRandomEvents();
                    mt::utils::showOptions( p->getName(), p, [&, p]( int R ) {
                        if( R == UI::APPLY ) ae.randomNotes( sel, p->getData() );
                        if( p ) delete p;
                    } );
                }
                break;
                case 8: // Erase
                {
                    auto p = new APPTrackOptionEraseEvents();
                    mt::utils::showOptions( p->getName(), p, [&, p]( int R ) {
                        if( R == UI::APPLY )
                        {
                            mt::NotesInRange notesInRange;
                            GridData&        gridData{ GridData::Instance() };
                            notesInRange.noteStart = p->getNoteStart();
                            notesInRange.noteEnd   = p->getNoteEnd();
                            notesInRange.startTime = gridData.getTicksUptoBar( p->getBarStart() - 1 );
                            notesInRange.endTime   = gridData.getTicksUptoBar( p->getBarEnd() );
                            if( notesInRange.noteStart == MIDI::MIN && notesInRange.noteEnd == MIDI::MAX )
                            {
                                auto p1 = new ui::FilePopup( "Do You Really Want To Delete all the notes", false, false );
                                mt::utils::showOptions(
                                    "WARNING!", p1, [&, notesInRange, p1]( int R ) {
                                        if( R == UI::APPLY )
                                        {
                                            ae.deleteNotesInRange( ae.getSelected(), notesInRange );
                                        }
                                        if( p1 ) delete p1;
                                    },
                                    { ui::layout::warningPopup } );
                            }
                            else
                                ae.deleteNotesInRange( ae.getSelected(), notesInRange );
                        };
                        if( p ) delete p;
                    } );
                }
                break;
            }
        };

        ui::themeChanged.connect( [&]() {
            srNo.setImage( "ID_Program" );
            close.setImages( "Edit_Close" );
        } );
        srNo.toFront( true );
    }

    void AppTrackOptions::openWarningPopUp( const juce::String& Text )
    {
        auto wP = new ui::FilePopup( Text, false, false );
        mt::utils::showDialog( "WARNING!", wP, [wP, this]( int R ) { delete wP; }, { 0, 0, 600, 360 } );
    }

    void AppTrackOptions::paint( juce::Graphics& g )
    {
        g.fillAll( ui::Theme::backgroundColour( 5 ) );
        mt::utils::fillRoundedRect( g, { 0, 0, 1000, 386 }, ui::Theme::backgroundColour( 1 ) );
        mt::utils::fillRoundedRect( g, { 956, 0, 43, 386 }, ui::Theme::backgroundColour( 12 ) );
    }

    void AppTrackOptions::resized()
    {
        srNo.setBoundsRelative( .0f, .0f, .0429f, .9698f );
        track.setBoundsRelative( .0f, .0f, .9765f, .2814f );
        opts.setBoundsRelative( .0585f, .2814f, .8544f, .6884f );
        close.setBoundsRelative( .9531f, .0234f, 0.023f, 0.05633f );
    }

    void AppTrackOptions::visibilityChanged()
    {
        if( isVisible() )
        {
            auto& s     = mt::getAudioInterface().getSelected();
            auto  ports = ae.getMidiDeviceNames( true );
            track.setTrack( s.sequence, s.track );
            track.setMidiPorts( &ports );
            srNo.setText( juce::String( track.getTrackNo() + 1 ) );
            auto pProg = ae.getProgramDataForTrack( ae.getSelectedSequence(), ae.getSelectedTrack() );
            srNo.setImage( getEditProgramIcon( pProg ? pProg->type.get() : mt::MTProgramType::Unknown ) );
        }
        opts.visibilityChanged();
    }

    void AppTrackOptions::handleCommand( CPktMgr::HWMsg_t& HwMsg )
    {
        auto fc = ui::getComponentInFocus();
        if( nullptr == fc || &opts == fc )
            opts.handleCommand( HwMsg );
    }
    //==============================================================================
}
