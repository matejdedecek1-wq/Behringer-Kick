#pragma once

namespace mt::ac
{
    /////////////////////////////////////////
    //LocateDialog
    ////////////////////////////////////////

    class LocateDialog : public juce::Component
        , public juce::Button::Listener
    {
    public:
        std::function<void( int, int, int, int )> bbtCallBack;
        LocateDialog( int, int, int );
        ~LocateDialog();

    private:
        void resized() override;
        void paint( juce::Graphics& g ) override;
        void buttonClicked( juce::Button* ) override;
        void close();

        ui::KickSlider      start{ "SEQ LENGTH", juce::Slider::LinearBar };
        ui::KickSlider      bar{ "BAR", juce::Slider::LinearBar };
        ui::KickSlider      beat{ "BEAT", juce::Slider::LinearBar };
        ui::KickSlider      tick{ "TICKS", juce::Slider::LinearBar };
        ui::KickImageButton cancelBtn{ "Cancel", "Icon_Cancel" }, applyBtn{ "Apply", "Icon_Apply" };

        ui::KickLabel startLable{ "START" };
        ui::KickLabel barLable{ "BAR" };
        ui::KickLabel beatLable{ "BEAT" };
        ui::KickLabel tickLable{ "TICK" };

        int startInt = 1, barInt = 1, beatInt = 1, tickInt = 1;
    };

    LocateDialog::LocateDialog( int barValue, int beatValue, int tickvalue )
        : barInt( barValue ), beatInt( beatValue ), tickInt( tickvalue )
    {
        setSize( 650, 520 );
        start.setRange( 0, 100, 1 );
        bar.setRange( 0, 100, 1 );
        beat.setRange( 0, 100, 1 );
        tick.setRange( 0, 100, 1 );

        start.setValue( startInt );
        bar.setValue( barInt );
        beat.setValue( beatInt );
        tick.setValue( tickInt );

        start.setTextValueSuffix( " dB" );
        bar.setTextValueSuffix( " dB" );
        beat.setTextValueSuffix( " dB" );
        tick.setTextValueSuffix( " dB" );

        utils::addAndMakeVisible( this, start, bar, beat, tick, cancelBtn, applyBtn, startLable, barLable, beatLable, tickLable );
        start.showTitleAndValue( true, juce::Slider::TextBoxBelow, juce::Justification::centred );
        bar.showTitleAndValue( true, juce::Slider::TextBoxBelow, juce::Justification::centred );
        beat.showTitleAndValue( true, juce::Slider::TextBoxBelow, juce::Justification::centred );
        tick.showTitleAndValue( true, juce::Slider::TextBoxBelow, juce::Justification::centred );

        startLable.setValueJustification( juce::Justification::centred );
        startLable.setValueBackgroundColour( ui::Theme::backgroundColour( 9 ) );
        startLable.setValueFontHeight( 18.f );
        barLable.setValueJustification( juce::Justification::centred );
        barLable.setValueBackgroundColour( ui::Theme::backgroundColour( 9 ) );
        barLable.setValueFontHeight( 18.f );
        beatLable.setValueJustification( juce::Justification::centred );
        beatLable.setValueBackgroundColour( ui::Theme::backgroundColour( 9 ) );
        beatLable.setValueFontHeight( 18.f );
        tickLable.setValueJustification( juce::Justification::centred );
        tickLable.setValueBackgroundColour( ui::Theme::backgroundColour( 9 ) );
        tickLable.setValueFontHeight( 18.f );

        applyBtn.onClick = [&]() {
            startInt = start.getValue();
            barInt   = bar.getValue();
            beatInt  = beat.getValue();
            tickInt  = tick.getValue();
            if( bbtCallBack )
                bbtCallBack( startInt, barInt, beatInt, tickInt );
            close();
        };
        cancelBtn.onClick = [&]() {
            close();
        };

        ui::themeChanged.connect( [&]() {
            cancelBtn.setImages( "Icon_Cancel" );
            applyBtn.setImages( "Icon_Apply" );
        } );
    }

    LocateDialog::~LocateDialog() {}

    void LocateDialog::paint( juce::Graphics& g )
    {
        g.setColour( ui::Theme::textColour( 5 ) );
        g.drawLine( { 20, 116, 580, 116 } );
        g.drawLine( { 20, 232, 580, 232 } );
        g.drawLine( { 20, 342, 580, 342 } );
    }

    void LocateDialog::resized()
    {
        start.setBoundsRelative( .1738f, .0576f, .6538f, .1461f );
        bar.setBoundsRelative( .1738f, .2615f, .6538f, .1461f );
        beat.setBoundsRelative( .1738f, .4846f, .6538f, .1461f );
        tick.setBoundsRelative( .1738f, .7076f, .6538f, .1461f );
        cancelBtn.setBoundsRelative( .0307f, .8980f, .2692f, .0634f );
        applyBtn.setBoundsRelative( .7f, .8980f, .2692f, .0634f );
        /* startLable.setBoundsRelative( 0.031f, 0.084f, 0.078f, 0.036f );
        barLable.setBoundsRelative( 0.031f, 0.307f, 0.051f, 0.036f );
        beatLable.setBoundsRelative( 0.031f, 0.53f, 0.063f, 0.036f );
        tickLable.setBoundsRelative( 0.031f, 0.74f, 0.056f, 0.036f );*/
    }

    void LocateDialog::close()
    {
        if( auto dw = getCurrentlyModalComponent() )
            dw->exitModalState( 0 );
    }

    void LocateDialog::buttonClicked( juce::Button* ) {}

    /////////////////////////////////////////
    //Locate
    ////////////////////////////////////////

    Locate::Locate()
    {
        setInterceptsMouseClicks( false, false );

        barBeatTickLabel.setValueJustification( juce::Justification::centred );
        barBeatTickLabel.setTitleJustification( juce::Justification::centredTop );
        barBeatTickLabel.showTitle( true );
        barBeatTickLabel.setTitleColour( ui::Theme::textColour( 1 ) );
        barBeatTickLabel.setValueFontHeight( 18.f );
        barBeatTickLabel.setTitleFontHeight( 16.f );
        barBeatTickLabel.setTitleProportion( .4f );
        barBeatTickLabel.setValueBackgroundColour( ui::Theme::backgroundColour( 13 ) );

        utils::addAndMakeVisible( this, barBeatTickLabel );
        barBeatTickLabel.setText( juce::String( 0 ).paddedLeft( '0', 3 ), juce::dontSendNotification );
    }

    Locate::~Locate()
    {
    }

    void Locate::resized()
    {
        barBeatTickLabel.setBoundsRelative( .4878f, .0f, .4459f, 1.0f );
    }

    void Locate::displayPopup()
    {
        LocateDialog* dlg = new LocateDialog( playHeadBnB.bar, playHeadBnB.beat, playHeadBnB.tick );
        utils::showDialog(
            "BBT SETTINGS", dlg, [this, dlg]( int ) {
                if( dlg ) delete dlg;
            },
            dlg->getBounds(), ui::Theme::backgroundColour( 1 ), ui::Theme::titleColour( 1 ), ui::Theme::backgroundColour( 2 ) );

        dlg->bbtCallBack = [&, this]( int startValue, int barValue, int beatValue, int tickValue ) {
            playHeadBnB.bar  = barValue;
            playHeadBnB.beat = beatValue;
            playHeadBnB.tick = tickValue;
            barBeatTickLabel.setText( getBarBeatTickInfo( barValue, beatValue, tickValue ).paddedLeft( '0', 3 ), juce::dontSendNotification );
        };
    }

    void Locate::updatePlayHead()
    {
        barBeatTickLabel.setText( getBarBeatTickInfo( playHeadBnB.bar + 1, playHeadBnB.beat + 1, playHeadBnB.tick ).paddedLeft( '0', 3 ), juce::dontSendNotification );
    }

    void Locate::refresh()
    {
        updatePlayHead();
        repaint();
    }

    juce::String Locate::getBarBeatTickInfo( int barValue, int beatValue, int tickValue )
    {
        juce::String beats;
        juce::String ticks;
        juce::String barInfo         = juce::String( ( barValue == -1 ? 0 : barValue ) ) + " : ";
        juce::String beatInfo        = ( ( beatValue >= 10 ) ? ( beats = juce::String( beatValue ) ) : juce::String( beats = "0" + juce::String( beatValue ) ) ) + " : ";
        juce::String tickInfo        = ( ( tickValue >= 100 ) ? ( ticks = juce::String( tickValue ) ) : ( ( tickValue >= 10 ) ? ( juce::String( ( ticks = "0" + juce::String( tickValue ) ) ) ) : juce::String( ticks = "00" + juce::String( tickValue ) ) ) );
        juce::String barBeatTickInfo = juce::String( barInfo + beatInfo + tickInfo );

        return barBeatTickInfo;
    }

    /////////////////////////////////////////
    //LocateSTPDialog
    ////////////////////////////////////////
    class LocateSTPDialog : public juce::Component
        , public juce::Button::Listener
    {
    public:
        std::function<void( juce::String, juce::String, juce::String )> stpCallBack;

        LocateSTPDialog();
        ~LocateSTPDialog();

    private:
        int               stpEnumNum = -1;
        void              resized() override;
        void              buttonClicked( juce::Button* ) override;
        void              close();
        void              updateList();
        MTAudioInterface& ae{ getAudioInterface() };

        std::vector<juce::String> seqList{};
        std::vector<juce::String> trackList{};
        std::vector<juce::String> progList{};
        ui::KickListBox           seq{ "SEQUENCE", {} }, track{ "TRACK", { "Track-1" } }, prog{ "PROGRAM", { "Program-1" } };
        ui::KickImageButton       cancelBtn{ "Cancel", "Icon_Cancel" }, applyBtn{ "Apply", "Icon_Apply" };
        juce::String              seqString, trackString, progString;
        int                       seqRow{ 0 };
    };

    LocateSTPDialog::LocateSTPDialog()
    {
        seqRow = ae.getSelectedSequence();

        setSize( 604, 560 );

        utils::addAndMakeVisible( this, cancelBtn, applyBtn, seq, track, prog );
        seq.setJustificationType( juce::Justification::centred );
        track.setJustificationType( juce::Justification::centred );
        prog.setJustificationType( juce::Justification::centred );

        seq.setTitleColour( ui::Theme::textColour( 1 ) );
        track.setTitleColour( ui::Theme::textColour( 1 ) );
        prog.setTitleColour( ui::Theme::textColour( 1 ) );

        applyBtn.onClick = [&]() {
            seqString   = seq.getSelectedValue();
            trackString = track.getSelectedValue();
            progString  = prog.getSelectedValue();
            if( stpCallBack )
                stpCallBack( seqString, trackString, progString );
            ae.setSelectedSequence( seq.getSelectedIndex() );
            ae.setSelectedTrack( track.getSelectedIndex() );
            ae.setSelectedProgram( prog.getSelectedIndex() );
            close();
        };
        cancelBtn.onClick = [&]() {
            ae.setSelectedSequence( seqRow );
            updateList();
            close();
        };

        seq.onSelectionChanged = [&]( int row, const juce::String& ) {
            ae.setSelectedSequence( row );
        };
        updateList();

        ui::themeChanged.connect( [&]() {
            applyBtn.setImages( "Icon_Apply" );
            cancelBtn.setImages( "Icon_Cancel" );
        } );
    }

    LocateSTPDialog::~LocateSTPDialog() {}

    void LocateSTPDialog::resized()
    {
        seq.setBoundsRelative( .2698f, .1423f, .4586f, .1307f );
        track.setBoundsRelative( .2698f, .3386f, .4586f, .1307f );
        prog.setBoundsRelative( .2698f, .5538f, .4586f, .1307f );
        cancelBtn.setBoundsRelative( .0298f, .9f, .4586f, .0596f );
        applyBtn.setBoundsRelative( .5066f, .9f, .4586f, .0596f );
    }

    void LocateSTPDialog::close()
    {
        if( auto dw = getCurrentlyModalComponent() )
            dw->exitModalState( 0 );
    }

    void LocateSTPDialog::updateList()
    {
        seqList.clear();
        trackList.clear();
        progList.clear();
        seqList   = ae.getSequenceList();
        trackList = ae.getTrackList( ae.getSelectedSequence() );
        progList  = ae.getProgramList();

        seq.setNumColumns( ae.getNumSequences() );
        seq.setListOptions( seqList, ae.getSelectedSequence() );

        track.setNumColumns( ae.getNumSequences() );
        track.setListOptions( trackList, ae.getSelectedTrack() );

        prog.setNumColumns( ae.getNumSequences() );
        prog.setListOptions( progList, ae.getSelectedProgram() );
    }

    void LocateSTPDialog::buttonClicked( juce::Button* ) {}

    /////////////////////////////////////////////////////////
    /////////LocateSTP
    /////////////////////////////////////////////////////////

    LocateSTP::LocateSTP()
    {
        setInterceptsMouseClicks( false, false );
        sequenceLable.setValueJustification( juce::Justification::centred );
        sequenceLable.setTitleJustification( juce::Justification::centredTop );
        sequenceLable.showTitle( true );
        sequenceLable.setTitleColour( ui::Theme::textColour( 1 ) );
        sequenceLable.setValueFontHeight( 18.f );
        sequenceLable.setTitleFontHeight( 16.f );
        sequenceLable.setTitleProportion( .4f );
        sequenceLable.setValueBackgroundColour( ui::Theme::backgroundColour( 13 ) );

        trackLable.setValueJustification( juce::Justification::centred );
        trackLable.setTitleJustification( juce::Justification::centredTop );
        trackLable.showTitle( true );
        trackLable.setTitleColour( ui::Theme::textColour( 1 ) );
        trackLable.setValueFontHeight( 18.f );
        trackLable.setTitleFontHeight( 16.f );
        trackLable.setTitleProportion( .4f );
        trackLable.setValueBackgroundColour( ui::Theme::backgroundColour( 13 ) );

        programLable.setValueJustification( juce::Justification::centred );
        programLable.setTitleJustification( juce::Justification::centredTop );
        programLable.showTitle( true );
        programLable.setTitleColour( ui::Theme::textColour( 1 ) );
        programLable.setValueFontHeight( 18.f );
        programLable.setTitleFontHeight( 16.f );
        programLable.setTitleProportion( .4f );
        programLable.setValueBackgroundColour( ui::Theme::backgroundColour( 13 ) );

        utils::addAndMakeVisible( this, sequenceLable, trackLable, programLable );

        signals::onSequenceSelectionChange.connect( [this]( const MTSelected& Selected ) {
            juce::MessageManager::callAsync( [this]() { updateSelection(); } );
        } );
    }

    LocateSTP::~LocateSTP()
    {
    }

    void LocateSTP::resized()
    {
        sequenceLable.setBoundsRelative( .0238f, .0f, .2840f, 1.0f );
        trackLable.setBoundsRelative( .3341f, .0f, .2840f, 1.0f );
        programLable.setBoundsRelative( .6873f, .0f, .2840f, 1.0f );
    }

    void LocateSTP::displaySTPPopup()
    {
        LocateSTPDialog* dlg = new LocateSTPDialog();
        utils::showDialog(
            "PLAYHEAD CONTROL MENU", dlg, [this, dlg]( int ) {
                if( dlg ) delete dlg;
            },
            dlg->getBounds(), ui::Theme::backgroundColour( 1 ), ui::Theme::titleColour( 1 ), ui::Theme::backgroundColour( 2 ) );

        dlg->stpCallBack = [&, this]( juce::String seqString, juce::String trackString, juce::String progString ) {
            sequenceLable.setText( seqString, juce::dontSendNotification );
            trackLable.setText( trackString, juce::dontSendNotification );
            programLable.setText( progString, juce::dontSendNotification );
        };
    }

    void LocateSTP::updateSelection()
    {
        auto pSeq      = ae.getSequenceData( ae.getSelectedSequence() );
        auto pTrack    = ae.getTrackData( ae.getSelectedSequence(), ae.getSelectedTrack() );
        auto pTrackPgm = ae.getProgramDataForTrack( ae.getSelectedSequence(), ae.getSelectedTrack() );
        sequenceLable.setText( pSeq ? pSeq->name.get() : "", juce::dontSendNotification );
        trackLable.setText( pTrack ? pTrack->name.get() : "", juce::dontSendNotification );
        programLable.setText( pTrackPgm ? pTrackPgm->name.get() : "", juce::dontSendNotification );
    }

}
