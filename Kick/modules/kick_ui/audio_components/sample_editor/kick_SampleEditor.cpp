#include "kick_SampleEditor.h"
namespace kick_ui
{
#define menuHeight       70
#define menuItemsSpacing 20
#define screenExludingHaS \
    {                     \
        10, 86, 1004, 396 \
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////SampleEditor///////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SampleEditor::SampleEditor()
        : kick_ui::IScreen( kick_ui::Screen::MenuSampleEdit )
    {
        setLookAndFeel( ui::KickLooknFeel::getLnf() );
        mt::utils::addAndMakeVisible( this, tuneCompButton, chopCompButton, assignButton, processButton, header, sampleListBox, plusButton, minusButton, zoomLabel, chopComp, tuneComp );
        mt::utils::setRadioGroupID( (int)kick_ui::RadioGroupId::SampleEditor, tuneCompButton, chopCompButton );
        scaleViewport.setViewedComponent( &scale, false );
        seViewport.setViewedComponent( &editor, false );

        tuneComp.addViewAndScale();

        seViewport.addListener( this );

        scaleViewport.setScrollBarsShown( false, false );
        seViewport.setScrollBarThickness( 4 );

        editor.sizeChanged.connect( [this]() {
            resized();
        } );

        seData.setView( &editor );
        sampleListBox.setJustificationType( juce::Justification::centredLeft );
        zoomLabel.setValueJustification( juce::Justification::centred );
        zoomLabel.setColours( ui::Theme::textColour( 1 ), ui::Theme::backgroundColour( 3 ) );
        zoomLabel.setValueFontHeight( 18 );
        zoomLabel.toFront( true );
        plusButton.toFront( true );
        minusButton.toFront( true );
        tuneCompButton.toFront( true );
        chopCompButton.toFront( true );
        assignButton.toFront( true );
        processButton.toFront( true );
        header.showOptions( true, false );
        header.showSettingsButton( true );
        tuneComp.setVisible( true );
        sampleListBox.setBackGroundColours( ui::Theme::backgroundColour( 13 ), ui::Theme::backgroundColour( 2 ) );

        mt::utils::setValueColour( ui::Theme::textColour( 1 ), chopCompButton, tuneCompButton, assignButton, processButton );
        mt::utils::setValueFontHeight( 18, chopCompButton, tuneCompButton, assignButton, processButton );

        header.onBackButtonPressed = [this]( bool ) { closeScreen(); };
        header.onSettingsClick     = [&]() {
            auto pS = new mt::ac::SampleEditorSettings( seData );
            mt::utils::showOptions( "SETTINGS", pS, [pS, this]( int R ) {
                // After Setting Changes Apply The Changes Here
                if( R == UI::APPLY )
                {
                    if( pS->hasCuePreviewChanged() )
                        seData.setCuePreview( pS->getCuePreview() );
                    if( pS->hasSlicePreciewChanged() )
                        seData.setSlicePreview( pS->getSlicePreview() );
                    if( pS->hasAutoScrollChanged() )
                        seData.setAutoScroll( pS->getAutoscroll() );
                    if( pS->hasTimeLineUnitsChanged() )
                        seData.setTimeLineUnits( pS->getTimeLineUnits() );
                }
                delete pS;
            },
                                        { 0, 0, 600, 500 } );
        };

        sigSampleEditLoadFileForEditing.connect( [&]( const juce::String& File, SampleEditorMode Mode ) {
            loadSample( File );
            setMode( Mode );
        } );

        sigSampleEditAddRegionAt.connect( [&]( double time ) {
            addRegionAt( time );
        } );
        sampleListBox.onSelectionChanged = [&]( int id, const juce::String& sampleName ) {
            if( id < 0 )
            {
                sampleListBox.setSelected( 0, juce::dontSendNotification );
                return;
            }
            auto file = mt::getAudioInterface().getAsset( mt::AssetTypes::Samples, id );
            seData.initialize( file );
            plusButton.setEnabled( sampleListBox.getSelected() != "None" );
            minusButton.setEnabled( sampleListBox.getSelected() != "None" );
            assignButton.setEnabled( sampleListBox.getSelected() != "None" );
            processButton.setEnabled( sampleListBox.getSelected() != "None" );
        };

        tuneCompButton.onClick = [this]() {
            tuneComp.removeViewAndScale();
            seData.setMode( mt::ac::SampleEditorData::Mode::modeTrim );
            tuneComp.addViewAndScale();
            mt::utils::makeVisible( tuneComp, assignButton, processButton );
            mt::utils::makeHidden( chopComp );
            zoomLabel.setBoundsRelative( 0.252f, 0.92f, .495f, 0.0636f );
            minusButton.setBoundsRelative( 0.252f, 0.92f, 0.0332, 0.0636f );
            plusButton.setBoundsRelative( 0.714f, 0.92f, 0.0332, 0.0636f );
        };

        chopCompButton.onClick = [this]() {
            chopComp.removeViewAndScale();
            seData.setMode( mt::ac::SampleEditorData::Mode::modeChop );
            chopComp.addViewAndScale();
            mt::utils::makeHidden( tuneComp, assignButton, processButton );
            mt::utils::makeVisible( chopComp );
            zoomLabel.setBoundsRelative( 0.252f, 0.92f, 0.131f, 0.0636f );
            minusButton.setBoundsRelative( 0.252f, 0.92f, 0.0332, 0.0636f );
            plusButton.setBoundsRelative( 0.35f, 0.92f, 0.0332, 0.0636f );
        };

        sampleProcessAction.connect( [this]( const int Index, std::list<juce::var>& Params ) {
            if( sampleListBox.getSelected() != "None" )
                seData.performAction( Index, Params );
        } );

        processButton.onClick = [&]() {
            closeScreen();
            kick_ui::signals::sigScreenOpen( kick_ui::Screen::ProcessEdit );
        };

        plusButton.onClick = [&]() {
            editor.setViewArea( 0.24f );
        };

        minusButton.onClick = [&]() {
            editor.setViewArea( -0.24f );
        };

        assignButton.onClick = [&]() {
            auto p = new AssignSampleToPad();

            mt::utils::showOptions( "ASSIGN SAMPLE", p, [p, this]( int R ) {
                auto index = p->getSelectedProgram();
                if( index >= 0 )
                {
                    if( R == UI::APPLY )
                    {
                        if( ae.getProgram( index )->getType() == mt::MTProgramType::Drum )
                        {
                            //Based on the slice type and assign type need to assign the sample
                            auto file = ae.getAsset( mt::AssetTypes::Samples, sampleListBox.getSelectedIndex() );
                            ae.addSampleToProgram( p->getSelectedProgram(), p->getSelectedPadId(), file );
                        }
                    }
                }
                delete p;
            } );
        };

        ui::themeChanged.connect( [&]() {
            plusButton.setImages( "Plus", "Plus", "Plus_Active" );
            minusButton.setImages( "Minus", "Minus", "Minus_Active" );
            header.setIcon( "sEdit_Icon" );
        } );

        tuneCompButton.triggerClick();
    }

    void SampleEditor::handleCommand( CPktMgr::HWMsg_t& f_HwMsg )
    {
        if( contracts::DclContractsHelper::isButtonMessage( f_HwMsg ) && ( contracts::DclContractsHelper::getButtonState( f_HwMsg ) == contracts::DeviceControlContracts::BTN_PRESSED ) )
        {
            auto buttonCode = contracts::DclContractsHelper::getButtonCode( f_HwMsg );
            switch( buttonCode )
            {
                case contracts::DeviceControlContracts::EN_PLAY: ae.setSequencePlay( aeSel.sequence, false, true ); return;
                case contracts::DeviceControlContracts::EN_PLAYSTART: ae.setSequencePlay( aeSel.sequence, true, true ); return;
                case contracts::DeviceControlContracts::EN_STOP: ae.setSequencePause(); return;
            }
        }
        else if( seData.isChopMode() )
            chopComp.handleCommand( f_HwMsg );
        else
            tuneComp.handleCommand( f_HwMsg );

        if( contracts::DclContractsHelper::isKnobMessage( f_HwMsg ) )
        {
            auto knob  = contracts::DclContractsHelper::getKnobCode( f_HwMsg );
            auto value = contracts::DclContractsHelper::getKnobValue( f_HwMsg );
            switch( knob )
            {
                case contracts::DeviceControlContracts::KnobSequence::EN_KNOB_9:
                    editor.setViewArea( value );
                    break;
                case contracts::DeviceControlContracts::KnobSequence::EN_KNOB_10:
                    editor.setViewArea( -value );
                    break;
            }
        }

        if( seData.isTrimMode() && contracts::DclContractsHelper::isPadButtonMessage( f_HwMsg ) )
        {
            int padNum   = contracts::DclContractsHelper::getPadNumber( f_HwMsg );
            int padState = contracts::DclContractsHelper::getPadState( f_HwMsg );
            //int           padVelocity = contracts::DclContractsHelper::getPadVelocity( f_HwMsg );
            const mt::ac::Region& sel = seData.getSelectedRegion();

            switch( padState )
            {
                case contracts::DeviceControlContracts::BTN_PRESSED:
                    switch( padNum )
                    {
                        case contracts::DeviceControlContracts::PadSequence::EN_PAD1:
                        case contracts::DeviceControlContracts::PadSequence::EN_PAD5:
                            seData.play( sel, mt::MTSamplePlayMode::samplePlayFromStart );
                            break;
                        case contracts::DeviceControlContracts::PadSequence::EN_PAD2:
                        case contracts::DeviceControlContracts::PadSequence::EN_PAD6:
                            seData.play( sel, mt::MTSamplePlayMode::samplePlayToStart );
                            break;
                        case contracts::DeviceControlContracts::PadSequence::EN_PAD3:
                        case contracts::DeviceControlContracts::PadSequence::EN_PAD7:
                            seData.play( sel, mt::MTSamplePlayMode::samplePlayToEnd );
                            break;
                        case contracts::DeviceControlContracts::PadSequence::EN_PAD4:
                        case contracts::DeviceControlContracts::PadSequence::EN_PAD8:
                            seData.play( sel, mt::MTSamplePlayMode::samplePlayFromEnd );
                            break;
                    }
                    break;

                case contracts::DeviceControlContracts::BTN_RELEASED:
                    switch( padNum )
                    {
                        case contracts::DeviceControlContracts::PadSequence::EN_PAD5:
                        case contracts::DeviceControlContracts::PadSequence::EN_PAD6:
                        case contracts::DeviceControlContracts::PadSequence::EN_PAD7:
                        case contracts::DeviceControlContracts::PadSequence::EN_PAD8:
                            seData.stop();
                            break;
                    }
                    break;
            }
        }
    }

    void SampleEditor::updateSampleList()
    {
        samples = ae.getAssetNames( mt::AssetTypes::Samples );
        if( samples.size() == 0 )
            samples.push_back( "None" );
        sampleListBox.setListOptions( samples, -1 );
        auto name = juce::File( seData.getSamplePath() ).getFileName();
        if( name.isNotEmpty() && samples.size() > 1 )
            sampleListBox.setSelected( name, juce::sendNotification );
        else
            sampleListBox.setSelected( samples.size() > 1 ? 1 : 0, juce::sendNotification );
    }

    void SampleEditor::updateOnViewPositionChange( ui::KickViewport* pViewport )
    {
        scaleViewport.setViewPosition( pViewport->getViewPositionX(), pViewport->getViewPositionY() );
    }

    void SampleEditor::loadSample( const juce::String& samplePath )
    {
        return seData.initialize( samplePath );
    }

    void SampleEditor::addRegionAt( double time )
    {
        seData.splitAtTime( seData.convertTimeToSamples( time ) );
    }

    void SampleEditor::setMode( SampleEditorMode /*mode*/ )
    {}

    void SampleEditor::showEditor()
    {}

    void SampleEditor::resized()
    {
        auto b = getLocalBounds();
        header.setBounds( b.removeFromTop( ui::layout::header.getHeight() ) );

        chopComp.setBoundsRelative( 0.f, .142f, 1.f, .86f );
        tuneComp.setBoundsRelative( 0.f, .142f, 1.f, .86f );

        tuneCompButton.setBoundsRelative( .0097f, .92f, .117f, .0636f );
        chopCompButton.setBoundsRelative( .131f, .92f, .117f, .0636f );
        assignButton.setBoundsRelative( .751f, .92f, .117f, .0636f );
        processButton.setBoundsRelative( .872f, .92f, .117f, .0636f );

        sampleListBox.setBoundsRelative( 0.225f, 0.029f, 0.1957f, .082f );

        scale.setSize( editor.getWidth(), scale.getHeight() );
    }

    void SampleEditor::visibilityChanged()
    {
        if( isVisible() )
        {
            sampleListBox.setFocus();
            vTree = ae.getAssetVT();
            updateSampleList();
        }
        else
            seData.stop();
    }

    SampleProcess::SampleProcess()
        : kick_ui::IScreen( kick_ui::Screen::ProcessEdit )
    {
        mt::utils::addAndMakeVisible( this, header, discard, deletebtn, silence, extract, normalize, reverse, fadeIn, fadeOut, pitchshift, timeStretch, gainChange, copy, bitReduce, stereoMono, sample, slice, cancel );
        header.onBackButtonPressed = [this]( bool ) {
            closeScreen();
        };
        header.showOptions( false, false );
        slice.setValueBackgroundColour( ui::Theme::backgroundColour( 9 ) );
        sample.setValueBackgroundColour( ui::Theme::backgroundColour( 9 ) );
        slice.setValueFontHeight( 17 );
        sample.setValueFontHeight( 17 );
        cancel.setValueColour( ui::Theme::textColour( 1 ) );
        cancel.setValueFontHeight( 18 );

        cancel.onClick = [&]() {
            closeScreen();
        };
        discard.onClick     = [&]() { openProcessPopup( 0 ); };
        deletebtn.onClick   = [&]() { openProcessPopup( 1 ); };
        silence.onClick     = [&]() { openProcessPopup( 2 ); };
        extract.onClick     = [&]() { openProcessPopup( 3 ); };
        normalize.onClick   = [&]() { openProcessPopup( 4 ); };
        reverse.onClick     = [&]() { openProcessPopup( 5 ); };
        fadeIn.onClick      = [&]() { openProcessPopup( 6 ); };
        fadeOut.onClick     = [&]() { openProcessPopup( 7 ); };
        pitchshift.onClick  = [&]() { openProcessPopup( 8 ); };
        timeStretch.onClick = [&]() { openProcessPopup( 9 ); };
        gainChange.onClick  = [&]() { openProcessPopup( 10 ); };
        copy.onClick        = [&]() { openProcessPopup( 11 ); };
        bitReduce.onClick   = [&]() { openProcessPopup( 12 ); };
        stereoMono.onClick  = [&]() { openProcessPopup( 13 ); };
    }

    SampleProcess::~SampleProcess()
    {
        setVisible( false );
    }

    void SampleProcess::paint( juce::Graphics& g )
    {
        mt::utils::fillRoundedRect( g, { 10, 86, 1004, 460 }, ui::Theme::backgroundColour( 1 ) );
    }

    void SampleProcess::resized()
    {
        auto b = getLocalBounds();
        header.setBounds( b.removeFromTop( ui::layout::header.getHeight() ) );

        discard.setBoundsRelative( .0733f, .2257f, .1956f, .1337f );
        deletebtn.setBoundsRelative( .2925f, .2257f, .1956f, .1337f );
        silence.setBoundsRelative( .5117f, .2257f, .1956f, .1337f );
        extract.setBoundsRelative( .7309f, .2257f, .1956f, .1337f );
        normalize.setBoundsRelative( .0733f, .3762f, .1956f, .1337f );
        reverse.setBoundsRelative( .2925f, .3762f, .1956f, .1337f );
        fadeIn.setBoundsRelative( .5117f, .3762f, .1956f, .1337f );
        fadeOut.setBoundsRelative( .7309f, .3762f, .1956f, .1337f );
        pitchshift.setBoundsRelative( .0733f, .5267f, .1956f, .1337f );
        timeStretch.setBoundsRelative( .2925f, .5267f, .1956f, .1337f );
        gainChange.setBoundsRelative( .5117f, .5267f, .1956f, .1337f );
        copy.setBoundsRelative( .7309f, .5267f, .1956f, .1337f );
        bitReduce.setBoundsRelative( .0733f, .7424f, .1956f, .1337f );
        stereoMono.setBoundsRelative( .2925f, .7424f, .1956f, .1337f );
        sample.setBoundsRelative( .0733f, .6939f, .1956f, .0484f );
        slice.setBoundsRelative( .0733f, .1772f, .1956f, .0484f );
        cancel.setBoundsRelative( .8747f, .9297f, .1174f, .0568f );
    }

    void SampleProcess::openProcessPopup( int index )
    {
        ProcessSlice* p = new ProcessSlice( index );
        mt::utils::showOptions(
            "PROCESS SLICE", p, [p, this]( int R ) {
                if( R == UI::APPLY )
                {
                    auto  index  = p->getSelectedProcess();
                    auto& params = p->getParams();
                    sampleProcessAction( index, params );
                    cancel.triggerClick();
                }
                delete p;
            },
            ui::layout::dialog, ui::Theme::backgroundColour( 1 ), ui::Theme::titleColour( 1 ), ui::Theme::backgroundColour( 2 ) );
    }

    ProcessSlice::ProcessSlice( int index )
    {
        mt::utils::addAndMakeVisible( this, process, image, rotary, warningIcon, rotary, linear, beat, ratio, originalTempo, newTempo, pointing, text, data, sampleName, left );
        process.setEnabled( false );
        process.setJustificationType( juce::Justification::centred );

        text.setValueBackgroundColour( ui::Theme::backgroundColour( 9 ) );
        text.setTitleJustification( juce::Justification::left );
        text.setTitleFontHeight( 18 );
        text.setValueFontHeight( 18 );
        sampleName.setEditable( false, true );
        sampleName.setValueFontHeight( 18 );
        sampleName.setValueJustification( juce::Justification::centred );

        sampleName.onTextChange = [&]() {
            params.clear();
            if( selectedID == mt::MTSampleEditorAction::seActionExtract || selectedID == mt::MTSampleEditorAction::seActionCopy )
                params.push_back( sampleName.getText() );
            else if( selectedID == mt::MTSampleEditorAction::seActionFadeIn || selectedID == mt::MTSampleEditorAction::seActionFadeOut )
                params.push_back( linear.getSelected() );
            else if( selectedID == mt::MTSampleEditorAction::seActionStereoToMono )
            {
                params.push_back( left.getSelected() );
                params.push_back( sampleName.getText() );
            }
        };

        auto onSelection = [this]( const int index, const juce::String& value ) {
            params.clear();
            params.push_back( index );
            params.push_back( sampleName.getText() );
        };
        left.onSelectionChanged   = onSelection;
        linear.onSelectionChanged = onSelection;

        rotary.onValueChanged = [&]( double Value ) {
            params.clear();
            params.push_back( Value );
        };

        process.onSelectionChanged = [&]( int Index, const juce::String& value ) {
            params.clear();
            if( Index == -1 && process.getSize() > 0 )
            {
                process.setSelected( 0, juce::dontSendNotification );
                return;
            }
            selectedID = Index;
            switch( Index )
            {
                default:
                    break;
                case 0:
                    mt::utils::makeHidden( rotary, linear, beat, ratio, originalTempo, newTempo, pointing, sampleName, left );
                    mt::utils::makeVisible( image, warningIcon );
                    image.setImages( "sEdit_Gdiscard" );
                    text.setText( "Pressing Apply will discard the section", juce::dontSendNotification );
                    text.setLine2Text( "beyond the START and End points" );
                    break;
                case 1:
                    mt::utils::makeHidden( rotary, linear, beat, ratio, originalTempo, newTempo, pointing, sampleName, left );
                    mt::utils::makeVisible( image, warningIcon, text );
                    text.setText( "Pressing Apply will delete the section", juce::dontSendNotification );
                    text.setLine2Text( "between the START/END points" );
                    image.setImages( "sEdit_Gdelete" );
                    break;
                case 2:
                    mt::utils::makeHidden( rotary, linear, beat, ratio, originalTempo, newTempo, pointing, sampleName, left );
                    mt::utils::makeVisible( image, warningIcon, text );
                    image.setImages( "sEdit_Gsilence" );
                    text.setText( "Pressing Apply will silence the section", juce::dontSendNotification );
                    text.setLine2Text( "between the START/END points" );
                    break;
                case 3:
                    mt::utils::makeHidden( rotary, linear, beat, ratio, originalTempo, newTempo, pointing, text, warningIcon, left );
                    mt::utils::makeVisible( image, sampleName );
                    image.setImages( "sEdit_Gextract" );
                    break;
                case 4:
                    mt::utils::makeHidden( rotary, warningIcon, linear, beat, ratio, originalTempo, newTempo, pointing, text, sampleName, left );
                    mt::utils::makeVisible( image );
                    image.setImages( "sEdit_Gnormalize" );
                    break;
                case 5:
                    mt::utils::makeHidden( rotary, warningIcon, linear, beat, ratio, originalTempo, newTempo, pointing, text, sampleName, left );
                    mt::utils::makeVisible( image );
                    image.setImages( "sEdit_Greverse" );
                    break;
                case 6:
                    mt::utils::makeHidden( rotary, beat, ratio, originalTempo, newTempo, pointing, sampleName, left );
                    mt::utils::makeVisible( image, warningIcon, linear, text );
                    image.setImages( "sEdit_Gfadein" );
                    text.setText( "Pressing Apply will put a fade in", juce::dontSendNotification );
                    text.setLine2Text( "beyond the START and End points" );
                    params.push_back( linear.getSelected() );
                    break;
                case 7:
                    mt::utils::makeHidden( rotary, beat, ratio, originalTempo, newTempo, pointing, sampleName, left );
                    mt::utils::makeVisible( image, warningIcon, linear, text );
                    image.setImages( "sEdit_Gfadeout" );
                    text.setText( "Pressing Apply will put a fade out", juce::dontSendNotification );
                    text.setLine2Text( "beyond the START and End points" );
                    params.push_back( linear.getSelected() );
                    break;
                case 8:
                    mt::utils::makeHidden( image, warningIcon, linear, beat, ratio, originalTempo, newTempo, pointing, text, sampleName, left );
                    mt::utils::makeVisible( rotary );
                    rotary.setRange( -12, 12, 1 );
                    params.push_back( rotary.getValue() );
                    break;
                case 9:
                    mt::utils::makeHidden( rotary, image, linear, sampleName, left );
                    mt::utils::makeVisible( warningIcon, beat, ratio, originalTempo, newTempo, pointing, text );
                    text.setText( "Pressing Apply will change the ", juce::dontSendNotification );
                    text.setLine2Text( "length of the sample" );
                    break;
                case 10:
                    mt::utils::makeHidden( image, warningIcon, linear, beat, ratio, originalTempo, newTempo, pointing, text, sampleName, left );
                    mt::utils::makeVisible( rotary );
                    rotary.setRange( -18, 18, 1 );
                    params.push_back( rotary.getValue() );
                    break;
                case 11:
                    mt::utils::makeHidden( warningIcon, linear, beat, ratio, originalTempo, newTempo, pointing, text, rotary, left );
                    mt::utils::makeVisible( image, sampleName );
                    image.setImages( "sEdit_Gcopy" );
                    break;
                case 12:
                    mt::utils::makeHidden( image, warningIcon, linear, beat, ratio, originalTempo, newTempo, pointing, text, left, sampleName );
                    mt::utils::makeVisible( rotary );
                    rotary.setRange( 1, 16, 1 );
                    params.push_back( rotary.getValue() );
                    break;
                case 13:
                    mt::utils::makeHidden( warningIcon, linear, beat, ratio, originalTempo, newTempo, pointing, text, rotary, image );
                    mt::utils::makeVisible( sampleName, left );
                    params.push_back( left.getSelected() );
                    break;
            }
        };
        process.setSelected( index, juce::sendNotification );
    }

    void ProcessSlice::resized()
    {
        image.setBoundsRelative( .1926f, .3312f, .6146f, .2868f );
        process.setBoundsRelative( .2907f, .0737f, .4261f, .1670f );
        warningIcon.setBoundsRelative( .2230f, .7960f, .1046f, .1474f );
        beat.setBoundsRelative( .2784f, .28f, .1846f, .1523f );
        ratio.setBoundsRelative( .5276f, .28f, .1846f, .1523f );
        originalTempo.setBoundsRelative( .2784f, .5061f, .1846f, .1523f );
        newTempo.setBoundsRelative( .5276f, .5061f, .1846f, .1523f );
        rotary.setBoundsRelative( .4415f, .4078f, .123f, .1965f );
        pointing.setBoundsRelative( .4938f, .4570f, .0184f, .0343f );
        linear.setBoundsRelative( .2261f, .5995f, .5538f, .1081f );
        left.setBoundsRelative( .2261f, .4692f, .5538f, .1081f );
        text.setBoundsRelative( .3584f, .8157f, .4584f, .1081f );
        sampleName.setBoundsRelative( .2907f, .8353f, .4261f, .1081f );
    }

    void ProcessSlice::close( int Return )
    {
        if( auto dw = getCurrentlyModalComponent() )
            dw->exitModalState( Return );
    }

    AssignSampleToPad::AssignSampleToPad()
    {
        mt::utils::addAndMakeVisible( this, assignTo, pad, sliceType, cropSample, program, assignToList, padsList, sliceTypeList, cropSampleBtn, programList );
        int letterAsciiCode = 65;

        std::vector<juce::String> padData;
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
        cropSample.setVisible( false );
        cropSampleBtn.setVisible( false );

        padsList.setListOptions( padData );

        assignToList.setListOptions( { "Assign Slice To Pad", "Make New Sample" } );
        sliceTypeList.setListOptions( { "Non-Destructive Slice", "Pad Parameters" } );

        mt::utils::setValueColour( ui::Theme::textColour( 1 ), assignTo, sliceType, pad, cropSample, program );
        mt::utils::setValueFontHeight( 16, assignTo, sliceType, pad, cropSample, program );
        mt::utils::setValueBackgroundColour( ui::Theme::backgroundColour( 9 ), assignTo, sliceType, pad, cropSample, program );

        assignToList.onSelectionChanged = [&]( int index, const juce::String& value ) {
            cropSample.setVisible( index == 1 );
            cropSampleBtn.setVisible( index == 1 );
            sliceType.setVisible( index == 0 );
            sliceTypeList.setVisible( index == 0 );
        };
    }

    void AssignSampleToPad::resized()
    {
        assignTo.setBoundsRelative( .1923f, .0737f, .1846f, .1081f );
        pad.setBoundsRelative( .1923f, .2334f, .1846f, .1081f );
        sliceType.setBoundsRelative( .1923f, .3931f, .1846f, .1081f );
        cropSample.setBoundsRelative( .1923f, .3931f, .1846f, .1081f );
        program.setBoundsRelative( .1923f, .5282f, .1846f, .1081f );

        assignToList.setBoundsRelative( .3892f, .0737f, .4230f, .1081f );
        padsList.setBoundsRelative( .3892f, .2334f, .4230f, .1081f );
        sliceTypeList.setBoundsRelative( .3892f, .3931f, .4230f, .1081f );
        cropSampleBtn.setBoundsRelative( .6276f, .3931f, .1846f, .1081f );
        programList.setBoundsRelative( .3892f, .5282f, .4230f, .1081f );
    }

    void AssignSampleToPad::visibilityChanged()
    {
        if( isVisible() )
        {
            auto& ae       = mt::getAudioInterface();
            auto  programs = ae.getProgramList();

            std::vector<juce::String> list;
            for( auto it : programs )
            {
                if( ae.getProgramType( it ) == mt::MTProgramType::Drum )
                    list.push_back( it );
            }
            if( list.size() == 0 )
                list.push_back( "No Programs" );
            programList.setListOptions( list );
        }
    }
}
