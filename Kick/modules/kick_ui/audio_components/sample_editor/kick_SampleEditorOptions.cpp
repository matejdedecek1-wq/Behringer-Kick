#include "kick_SampleEditorOptions.h"
#include "kick_SampleEditorOptions.h"
namespace mt
{
#define SAMPLE_OPTIONS_RADIO_GROUP_ID ( 4563241 )
    namespace ac
    {
        // trim chop tab and options defines ..
#define topMarginMenuOptions     5
#define buttonHeightMenuOptions  34
#define buttonWidthMenuOptions   100
#define buttonSpacingMenuOptions 10

// trim and chop ..
#define topMarginSeOptions            10
#define leftMarginSeOptions           20
#define buttonWidthSeOptions          65
#define buttonHeightSeOptions         30
#define bottomMarginInput             5

#define numericWidthSeOptionsControl  210
#define numericHeightSeOptionsControl 51 // optionsHeight - controlsSpacing - (2 * topMarginSeOptions)
#define controlsSpacing               10
#define seOptionsControlButtonWidth   80
#define seOptionsControlButtonHeight  34
#define seOptionsImageButtonWidth     30
#define seOptionsImageButtonHeight    30
#define seOptionsImageButtonMargin    17
#define chopModeInputSpacing          20

//chop mode
#define chopModeButtonWidth   90
#define chopModeButtonHeight  34
#define chopModeButtonSpacing 10

#define trimChopActionSpacing 5

//scale
#define scaleHeight         15
#define scaleAndViewSpacing 10
#define optionsHeight       132

//actions
#define actionsViewHeight    54
#define actionControlsWidth  80 // orig 140
#define actionControlsHeight 34

//tool buttons
#define seToolButtonWidth           60
#define seToolButtonSpacing         5
#define seToolButtonTopBottomMargin 10
#define seToolButtonHeight          34

// Title height
#define titleHeight 16

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////sample editor options ///////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        SampleEditorOPtions::SampleEditorOPtions( SampleEditorData& data, ui::KickViewport& v, ui::KickViewport& s )
            : seData( data )
            , view( v )
            , scale( s )
        {}

        void SampleEditorOPtions::resized()
        {
            scale.setBoundsRelative( .0097f, .019f, .98f, .087f );
            view.setBoundsRelative( .0097f, .106f, .98f, .588f );
            scale.getViewedComponent()->setSize( scale.getWidth(), scale.getHeight() - 1 );
            view.getViewedComponent()->setSize( view.getWidth(), view.getHeight() - 5 );
        }

        juce::Rectangle<int> SampleEditorOPtions::getActionsRectangle()
        {
            int y = ui::_scaledY( getHeight() ) - ui::_scaledY( actionsViewHeight );
            return juce::Rectangle<int>( 0, y, ui::layout::getAppWidth(), actionsViewHeight );
        }

        juce::Rectangle<int> SampleEditorOPtions::getOptionsRectangle()
        {
            int startY = ui::_scaledY( getHeight() ) - ui::_scaledY( optionsHeight ) - ui::_scaledY( actionsViewHeight );
            return juce::Rectangle<int>( 0, startY, ui::layout::getAppWidth(), optionsHeight );
        }

        void SampleEditorOPtions::paint( juce::Graphics& g )
        {
            g.setColour( ui::Theme::backgroundColour( 1 ) );
            g.fillRoundedRectangle( { 10, 10, 1002, 396 }, 6.0f );
            g.setColour( ui::Theme::backgroundColour( 11 ) );
            g.fillRoundedRectangle( { 10, 10, 1002, 310 }, 6.0f );
            g.setColour( ui::Theme::outlineColour( 4 ) );
            g.drawRoundedRectangle( { 10.f, 10.f, 1002.f, 310.f }, 1.f, 1.f );
        }

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////sample editor chop ///////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        SampleEditorChopOptions::SampleEditorChopOptions( SampleEditorData& data, ui::KickViewport& v, ui::KickViewport& s )
            : SampleEditorOPtions( data, s, v )
        {
            utils::addAndMakeVisible( this, startNumber, endNumber, cueNumber, sliceListBox, linkButton, scButton, manualButton, thresholdButton, regionsButton, bpmButton, thresholdNumber, minTimeNumber, regionsNumber, barsNumber, beatsNumber, timeDivListBox );
            utils::setRadioGroupID( (int)kick_ui::RadioGroupId::SampleEditorOptions, manualButton, thresholdButton, regionsButton, bpmButton );
            startNumber.setColours( ui::Theme::textColour( 2 ), ui::Theme::textColour( 1 ) );
            endNumber.setColours( ui::Theme::textColour( 2 ), ui::Theme::textColour( 1 ) );
            cueNumber.setColours( ui::Theme::textColour( 2 ), ui::Theme::textColour( 1 ) );
            sliceListBox.setColours( ui::Theme::textColour( 2 ), ui::Theme::textColour( 1 ) );
            thresholdNumber.setColours( ui::Theme::textColour( 2 ), ui::Theme::textColour( 1 ) );
            minTimeNumber.setColours( ui::Theme::textColour( 2 ), ui::Theme::textColour( 1 ) );
            regionsNumber.setColours( ui::Theme::textColour( 2 ), ui::Theme::textColour( 1 ) );
            barsNumber.setColours( ui::Theme::textColour( 2 ), ui::Theme::textColour( 1 ) );
            beatsNumber.setColours( ui::Theme::textColour( 2 ), ui::Theme::textColour( 1 ) );
            timeDivListBox.setColours( ui::Theme::textColour( 2 ), ui::Theme::textColour( 1 ) );

            sliceListBox.setJustificationType( juce::Justification::centred );
            timeDivListBox.setJustificationType( juce::Justification::centred );

            startNumber.setValueBackgroundColour( ui::Theme::backgroundColour( 13 ) );
            endNumber.setValueBackgroundColour( ui::Theme::backgroundColour( 13 ) );
            cueNumber.setValueBackgroundColour( ui::Theme::backgroundColour( 13 ) );
            thresholdNumber.setValueBackgroundColour( ui::Theme::backgroundColour( 13 ) );
            regionsNumber.setValueBackgroundColour( ui::Theme::backgroundColour( 13 ) );
            minTimeNumber.setValueBackgroundColour( ui::Theme::backgroundColour( 13 ) );
            barsNumber.setValueBackgroundColour( ui::Theme::backgroundColour( 13 ) );
            beatsNumber.setValueBackgroundColour( ui::Theme::backgroundColour( 13 ) );
            sliceListBox.setBackGroundColours( ui::Theme::backgroundColour( 13 ), ui::Theme::backgroundColour( 1 ) );
            timeDivListBox.setBackGroundColours( ui::Theme::backgroundColour( 13 ), ui::Theme::backgroundColour( 1 ) );
            manualButton.setValueColour( ui::Theme::textColour( 1 ) );
            manualButton.setValueFontHeight( 18 );
            thresholdButton.setValueColour( ui::Theme::textColour( 1 ) );
            thresholdButton.setValueFontHeight( 18 );
            regionsButton.setValueColour( ui::Theme::textColour( 1 ) );
            regionsButton.setValueFontHeight( 18 );
            bpmButton.setValueColour( ui::Theme::textColour( 1 ) );
            bpmButton.setValueFontHeight( 18 );
            scButton.setValueColour( ui::Theme::textColour( 1 ) );
            scButton.setValueFontHeight( 18 );
            linkButton.setValueColour( ui::Theme::textColour( 1 ) );
            linkButton.setValueFontHeight( 18 );

            startNumber.setTitleFontHeight( titleHeight );
            endNumber.setTitleFontHeight( titleHeight );
            cueNumber.setTitleFontHeight( titleHeight );
            thresholdNumber.setTitleFontHeight( titleHeight );
            minTimeNumber.setTitleFontHeight( titleHeight );
            regionsNumber.setTitleFontHeight( titleHeight );
            barsNumber.setTitleFontHeight( titleHeight );
            beatsNumber.setTitleFontHeight( titleHeight );
            sliceListBox.setTitleFontHeight( titleHeight );
            timeDivListBox.setTitleFontHeight( titleHeight );

            auto notes = CommonUtils::getTimeDivAsString();
            timeDivListBox.setListOptions( notes );
            timeDivListBox.setSelected( 0 );

            utils::makeHidden( thresholdNumber, minTimeNumber, regionsNumber, barsNumber, beatsNumber, timeDivListBox );

            sliceListBox.onSelectionChanged = [&]( int index, const juce::String& ) {
                seData.selectRegion( index - 1 );
            };

            regionsNumber.onValueChanged = [this]( int newValue ) {
                setSlices( newValue );
            };

            barsNumber.onValueChanged = [this]( int /*newValue*/ ) {
                setBpmSlices();
            };

            beatsNumber.onValueChanged = [this]( int /*newValue*/ ) {
                setBpmSlices();
            };

            timeDivListBox.onSelectionChanged = [&]( int index, const juce::String& value ) {
                if( index < 0 )
                {
                    timeDivListBox.setSelected( 0 );
                    return;
                }
                setBpmSlices();
            };

            startNumber.onValueChanged = [&]( juce::int64 value ) {
                if( value < endNumber.getValue() )
                {
                    seData.moveTime( seData.getSelectedRegion(), value, false );
                    startNumber.setValue( value, juce::dontSendNotification );
                }
                else
                    startNumber.setValue( endNumber.getValue() - 1, juce::dontSendNotification );
            };

            endNumber.onValueChanged = [&]( juce::int64 value ) {
                if( value > startNumber.getValue() )
                    seData.moveTime( seData.getSelectedRegion(), value, true );
                else
                    endNumber.setValue( startNumber.getValue() + 1, juce::dontSendNotification );
            };

            seData.splitModeChanged.connect( [this]( SampleEditorData::SplitMode splitMode ) {
                ignoreModeChanges = true;

                if( splitMode == SampleEditorData::SplitMode::splitModeBpm )
                    bpmButton.setToggleState( true, juce::dontSendNotification );
                else if( splitMode == SampleEditorData::SplitMode::splitModeRegions )
                    regionsButton.setToggleState( true, juce::dontSendNotification );
                else if( splitMode == SampleEditorData::SplitMode::splitModeThreshold )
                    thresholdButton.setToggleState( true, juce::dontSendNotification );
                else if( splitMode == SampleEditorData::SplitMode::splitModeManual )
                    manualButton.setToggleState( true, juce::dontSendNotification );

                regionsNumber.setVisible( regionsButton.getToggleState() );

                barsNumber.setVisible( bpmButton.getToggleState() );
                beatsNumber.setVisible( bpmButton.getToggleState() );
                timeDivListBox.setVisible( bpmButton.getToggleState() );

                thresholdNumber.setVisible( thresholdButton.getToggleState() );
                minTimeNumber.setVisible( thresholdButton.getToggleState() );
                if( bpmButton.getToggleState() )
                    timeDivListBox.setSelected( 0 );
                ignoreModeChanges = false;
            } );

            seData.cuePositionChanged.connect( [this]( juce::int64 pos ) {
                cueNumber.setValue( pos );
            } );

            cueNumber.onValueChanged = [this]( juce::int64 newValue ) {
                seData.setCuePosition( newValue );
            };

            seData.sampleChanged.connect( [this]() {
                juce::int64 sampleLength = seData.getSampleLengthInSamples();
                if( 0 < sampleLength )
                {
                    for( auto& comp : getChildren() )
                        comp->setEnabled( true );
                    reset( sampleLength );
                }
                else
                {
                    for( auto& comp : getChildren() )
                        comp->setEnabled( false );
                }
            } );

            seData.regionSelected.connect( [&]( const Region& regionValue ) {
                startNumber.setValueBounds( 0, regionValue.getEnd() - 1, regionValue.getStart() - 1 );
                endNumber.setValue( regionValue.getEnd(), juce::dontSendNotification );
                sliceListBox.setSelected( regionValue.getIndex() == -1 ? 0 : regionValue.getIndex(), juce::dontSendNotification );
                seData.selectRegion( regionValue );
                selectedRegion = sliceListBox.getSelectedIndex();
            } );

            manualButton.onClick = [&]() {
                seData.setSplitMode( SampleEditorData::splitModeManual );
            };

            thresholdButton.onClick = [&]() {
                seData.setSplitMode( SampleEditorData::splitModeThreshold );
            };

            regionsButton.onClick = [&]() {
                seData.setSplitMode( SampleEditorData::splitModeRegions );
                setSlices( regionsNumber.getValue() );
            };

            bpmButton.onClick = [&]() {
                seData.setSplitMode( SampleEditorData::splitModeBpm );
            };

            linkButton.onClick = [&]() {
                seData.setLinkSlices( linkButton.getToggleState() );
                seData.selectRegion( selectedRegion - 1 );
            };

            seData.linkSlicesChanged.connect( [&]( bool state ) {
                linkButton.setToggleState( state, juce::dontSendNotification );
            } );

            scButton.onClick = [&]() {
                auto p            = new SliceAndCombine();
                p->onSliceClicked = [p, this]() {
                    std::vector<juce::var> params;
                    params.push_back( thresholdNumber.getValue() );
                    params.push_back( minTimeNumber.getValue() );
                    seData.splitByThresh( params );
                    utils::exitDialog( p->getCurrentlyFocusedComponent(), UI::CANCEL );
                };

                p->onCombineClicked = [p, this]() {
                    seData.joinSelectedAndPrev();
                    utils::exitDialog( p->getCurrentlyFocusedComponent(), UI::CANCEL );
                };

                utils::showDialog( "SLICE / COMBINE", p, [p, this]( int R ) {
                    scButton.setToggleState( false, juce::dontSendNotification );
                    if( p ) delete p;
                },
                                   { 0, 0, 400, 300 } );
            };

            manualButton.setToggleState( true, juce::sendNotification );

            ui::themeChanged.connect( [&]() {
                manualButton.setToggleButtonText( "Manual", "Manual" );
                thresholdButton.setToggleButtonText( "Threshold", "Threshold" );
                regionsButton.setToggleButtonText( "Regions", "Regions" );
                bpmButton.setToggleButtonText( "Bpm", "Bpm" );
                linkButton.setToggleButtonText( "Link", "Link" );
                scButton.setToggleButtonText( "S/C", "S/C" );
            } );
        }

        SampleEditorChopOptions::~SampleEditorChopOptions() {}

        void SampleEditorChopOptions::paint( juce::Graphics& g )
        {
            SampleEditorOPtions::paint( g );
        }

        void SampleEditorChopOptions::reset( const juce::int64 samples )
        {
            startNumber.setValueBounds( 0, samples - 1, 0 );
            endNumber.setValueBounds( 0, samples, seData.getSelectedRegion().getEnd() );
            cueNumber.setValueBounds( 1, samples, 1 );
            linkButton.setToggleState( true, juce::sendNotification );
            sliceListBox.setListOptions( { "All" }, 0 );
            thresholdNumber.setValue( 0, juce::dontSendNotification );
            minTimeNumber.setValue( 0, juce::dontSendNotification );
            regionsNumber.setValue( 1, juce::sendNotification );
            barsNumber.setValue( 1, juce::dontSendNotification );
            beatsNumber.setValue( 1, juce::dontSendNotification );
            timeDivListBox.setSelected( 0, juce::dontSendNotification );
            manualButton.getToggleState() ? manualButton.triggerClick() : thresholdButton.getToggleState() ? thresholdButton.triggerClick() :
                                                                      regionsButton.getToggleState()       ? regionsButton.triggerClick() :
                                                                      bpmButton.getToggleState()           ? bpmButton.triggerClick() :
                                                                                                             manualButton.triggerClick();
        }

        void SampleEditorChopOptions::resized()
        {
            SampleEditorOPtions::resized();

            startNumber.setBoundsRelative( 0.0577f, 0.4242f, 0.1195f, 0.1565f );
            endNumber.setBoundsRelative( 0.243f, 0.4242f, 0.1195f, 0.1565f );
            cueNumber.setBoundsRelative( 0.4382f, 0.4242f, 0.1195f, 0.1565f );
            sliceListBox.setBoundsRelative( 0.6334f, 0.4242f, 0.1195f, 0.1565f );
            linkButton.setBoundsRelative( 0.8286f, 0.45f, 0.1195f, 0.1565f );

            thresholdNumber.setBoundsRelative( 0.4382f, 0.6313f, 0.1195f, 0.1565f );
            minTimeNumber.setBoundsRelative( 0.6334f, 0.6313f, 0.1195f, 0.1565f );

            regionsNumber.setBoundsRelative( 0.6334f, 0.6313f, 0.1195f, 0.1565f );

            barsNumber.setBoundsRelative( 0.243f, 0.6313f, 0.1195f, 0.1565f );
            beatsNumber.setBoundsRelative( 0.4382f, 0.6313f, 0.1195f, 0.1565f );
            timeDivListBox.setBoundsRelative( 0.6334f, 0.6313f, 0.1195f, 0.1565f );

            scButton.setBoundsRelative( 0.8286f, 0.6767f, 0.1195f, 0.1111f );

            manualButton.setBoundsRelative( 0.0577f, 0.8383f, 0.1195f, 0.1111f );
            thresholdButton.setBoundsRelative( 0.243f, 0.8383f, 0.1195f, 0.1111f );
            regionsButton.setBoundsRelative( 0.4382f, 0.8383f, 0.1195f, 0.1111f );
            bpmButton.setBoundsRelative( 0.6334f, 0.8383f, 0.1195f, 0.1111f );
        }

        std::vector<juce::var> SampleEditorChopOptions::getSplitModeBpmOptions()
        {
            std::vector<juce::var> params;
            params.push_back( barsNumber.getValue() );
            params.push_back( beatsNumber.getValue() );
            params.push_back( timeDivListBox.getSelectedValue() );
            return params;
        }

        void SampleEditorChopOptions::setSlices( int newValue )
        {
            seData.setNoSlices( (unsigned int)newValue );
            std::vector<juce::String> options;
            auto                      totalRegions = seData.getTotalRegions();
            options.resize( totalRegions + 1 );
            options[0] = "All";
            for( unsigned int i = 1; i <= totalRegions; i++ )
                options[i] = juce::String( i );
            sliceListBox.setListOptions( options, 1 );
        }

        void SampleEditorChopOptions::setBpmSlices()
        {
            std::vector<juce::var> params = getSplitModeBpmOptions();
            seData.splitByBpm( params );
            int timeSig             = CommonUtils::timeDivStrToQl( params[2] );
            int barDivisionFraction = timeSig / 4;
            int totalDivisions      = (int)params[0] * ( (int)params[1] * barDivisionFraction );
            setSlices( totalDivisions );
        }

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////sample editor trim ///////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        SampleEditorTrimOptions::SampleEditorTrimOptions( SampleEditorData& data, ui::KickViewport& v, ui::KickViewport& s )
            : SampleEditorOPtions( data, v, s )
        {
            utils::addAndMakeVisible( this, timingNpitchLabel, startNumber, endNumber, semiNumber, fineNumber, xFadeNumber, tuneNumber, bpmNumber, rootNoteListBox, cancel1, cancel2, repeat, loopLock, linkSlice );
            utils::setRadioGroupID( (int)kick_ui::RadioGroupId::SampleEditorXFade, cancel1, cancel2 );
            cancel1.setToggleState( true, juce::dontSendNotification );

            timingNpitchLabel.setPadding( 10 );
            timingNpitchLabel.setFontHeight( 17 );
            timingNpitchLabel.setValueBackgroundColour( ui::Theme::backgroundColour( 1 ) );

            startNumber.setColours( ui::Theme::textColour( 2 ), ui::Theme::textColour( 1 ) );
            endNumber.setColours( ui::Theme::textColour( 2 ), ui::Theme::textColour( 1 ) );
            semiNumber.setColours( ui::Theme::textColour( 2 ), ui::Theme::textColour( 1 ) );
            fineNumber.setColours( ui::Theme::textColour( 2 ), ui::Theme::textColour( 1 ) );

            xFadeNumber.setColours( ui::Theme::textColour( 2 ), ui::Theme::textColour( 1 ) );
            tuneNumber.setColours( ui::Theme::textColour( 2 ), ui::Theme::textColour( 1 ) );
            bpmNumber.setColours( ui::Theme::textColour( 2 ), ui::Theme::textColour( 1 ) );
            rootNoteListBox.setColours( ui::Theme::textColour( 2 ), ui::Theme::textColour( 1 ) );
            rootNoteListBox.setBackGroundColours( ui::Theme::backgroundColour( 13 ), ui::Theme::backgroundColour( 1 ) );
            rootNoteListBox.setJustificationType( juce::Justification::centred );
            repeat.setValueColour( ui::Theme::textColour( 1 ) );
            repeat.setValueFontHeight( 18 );
            loopLock.setValueColour( ui::Theme::textColour( 1 ) );
            loopLock.setValueFontHeight( 18 );
            linkSlice.setValueColour( ui::Theme::textColour( 1 ) );
            linkSlice.setValueFontHeight( 18 );

            startNumber.setTitleFontHeight( titleHeight );
            endNumber.setTitleFontHeight( titleHeight );
            semiNumber.setTitleFontHeight( titleHeight );
            fineNumber.setTitleFontHeight( titleHeight );
            xFadeNumber.setTitleFontHeight( titleHeight );
            tuneNumber.setTitleFontHeight( titleHeight );
            bpmNumber.setTitleFontHeight( titleHeight );
            rootNoteListBox.setTitleFontHeight( titleHeight );

            seData.sampleChanged.connect( [this]() {
                juce::int64 sampleLength = seData.getSampleLengthInSamples();
                if( 0 < sampleLength )
                {
                    for( auto& comp : getChildren() )
                        comp->setEnabled( true );
                    reset( sampleLength );
                }
                else
                {
                    for( auto& comp : getChildren() )
                        comp->setEnabled( false );
                }
            } );

            seData.regionChanged.connect( [&]( const Region& regionValue ) {
                startNumber.setValueBounds( 0, regionValue.getEnd() - 1, regionValue.getStart() - 1 );
                endNumber.setValueBounds( startNumber.getMin() + 1, seData.getSampleLengthInSamples(), regionValue.getEnd() );
            } );

            seData.loopPositionChanged.connect( [&]( juce::int64 value ) {
                xFadeNumber.setEnabled( !( value < 1 ) && repeat.getToggleState() );
                cancel1.setEnabled( !( value < 1 ) && repeat.getToggleState() );
                cancel2.setEnabled( !( value < 1 ) && repeat.getToggleState() );
                if( value > 1 && repeat.getToggleState() )
                    xFadeNumber.setValueBounds( 0, value - 1, 0 );
            } );

            ui::themeChanged.connect( [&]() {
                repeat.setToggleButtonText( "Repeat", "Repeat" );
                loopLock.setToggleButtonText( "Loop Lock", "Loop Lock" );
                linkSlice.setToggleButtonText( "Link Slice", "Link Slice" );
                cancel1.setImages( "SE_Xfade_Sharp", "SE_Xfade_Sharp_Active", "SE_Xfade_Sharp_Active" );
                cancel2.setImages( "SE_Xfade_Smooth", "SE_Xfade_Smooth_Active", "SE_Xfade_Smooth_Active" );
            } );

            startNumber.onValueChanged = [&]( juce::int64 value ) {
                if( value < endNumber.getValue() )
                {
                    seData.moveTime( seData.getSelectedRegion(), value, false );
                    startNumber.setValue( value, juce::dontSendNotification );
                }
                else
                    startNumber.setValue( endNumber.getValue() - 1, juce::dontSendNotification );
                xFadeNumber.setEnabled( !( value < 1 ) && repeat.getToggleState() );
                cancel1.setEnabled( !( value < 1 ) && repeat.getToggleState() );
                cancel2.setEnabled( !( value < 1 ) && repeat.getToggleState() );
                if( loopLock.getToggleState() && value >= 1 )
                    xFadeNumber.setValueBounds( 0, startNumber.getValue(), 0 );
            };

            endNumber.onValueChanged = [&]( juce::int64 value ) {
                if( value > startNumber.getValue() )
                    seData.moveTime( seData.getSelectedRegion(), value, true );
                else
                    endNumber.setValue( startNumber.getValue() + 1, juce::dontSendNotification );
            };

            seData.loopLockChanged.connect( [&]( bool state ) {
                loopLock.setToggleState( state, juce::dontSendNotification );
            } );

            loopLock.onClick = [&]() {
                seData.setLoopMode( LoopMode::loopModeForward );
                seData.setLoopLock( loopLock.getToggleState() );
                seData.setLoopPosition( startNumber.getValue() );
            };

            repeat.onClick = [&]() {
                // Based on the Loop Direction Set the LoopMode
                seData.setLoopMode( LoopMode::loopModeForward ); ///Default
                setEnableXfade();
            };

            xFadeNumber.onValueChanged = [&]( juce::int64 value ) {
                if( value >= startNumber.getValue() && loopLock.getToggleState() )
                    xFadeNumber.setValue( startNumber.getValue() - 1, juce::dontSendNotification );
            };

            seData.linkSlicesChanged.connect( [&]( bool state ) {
                linkSlice.setToggleState( state, juce::dontSendNotification );
            } );

            linkSlice.onClick = [&]() {
                seData.setLinkSlices( linkSlice.getToggleState() );
            };
        }

        SampleEditorTrimOptions::~SampleEditorTrimOptions() {}

        void SampleEditorTrimOptions::paint( juce::Graphics& g )
        {
            SampleEditorOPtions::paint( g );
        }

        void SampleEditorTrimOptions::setEnableXfade()
        {
            xFadeNumber.setEnabled( repeat.getToggleState() && ( seData.getLoopPosition() >= 1 && !loopLock.getToggleState() || startNumber.getValue() >= 1 ) );
            cancel1.setEnabled( repeat.getToggleState() && ( seData.getLoopPosition() >= 1 && !loopLock.getToggleState() || startNumber.getValue() >= 1 ) );
            cancel2.setEnabled( repeat.getToggleState() && ( seData.getLoopPosition() >= 1 && !loopLock.getToggleState() || startNumber.getValue() >= 1 ) );
        }

        void SampleEditorTrimOptions::reset( const juce::int64 samples )
        {
            setEnableXfade();
            startNumber.setValueBounds( 0, samples - 1, 0 );
            endNumber.setValueBounds( 1, samples, seData.getSelectedRegion().getEnd() );
            xFadeNumber.setValueBounds( 0, 1, 0 );
            semiNumber.setValue( 0, juce::sendNotification );
            fineNumber.setValue( 0, juce::sendNotification );
            tuneNumber.setValue( 0, juce::sendNotification );
            bpmNumber.setValue( seData.detectBpm(), juce::sendNotification );
            rootNoteListBox.setSelected( 0, juce::sendNotification );
            loopLock.setToggleState( true, juce::sendNotification );
            linkSlice.setToggleState( true, juce::sendNotification );
            repeat.setToggleState( false, juce::sendNotification );
        }

        void SampleEditorTrimOptions::resized()
        {
            SampleEditorOPtions::resized();

            timingNpitchLabel.setBoundsRelative( 0.0199f, 0.4697f, 0.1593f, 0.1111f );

            startNumber.setBoundsRelative( 0.243f, 0.4242f, 0.1195f, 0.1565f );
            endNumber.setBoundsRelative( 0.4382f, 0.4242f, 0.1195f, 0.1565f );
            semiNumber.setBoundsRelative( 0.6334f, 0.4242f, 0.1195f, 0.1565f );
            fineNumber.setBoundsRelative( 0.8286f, 0.4242f, 0.1195f, 0.1565f );

            xFadeNumber.setBoundsRelative( 0.243f, 0.6313f, 0.1195f, 0.1565f );
            tuneNumber.setBoundsRelative( 0.4382f, 0.6313f, 0.1195f, 0.1565f );
            rootNoteListBox.setBoundsRelative( 0.6334f, 0.6313f, 0.1195f, 0.1565f );
            bpmNumber.setBoundsRelative( 0.8286f, 0.6313f, 0.1195f, 0.1565f );

            cancel1.setBoundsRelative( 0.243f, 0.8383f, 0.0577f, 0.1111f );
            cancel2.setBoundsRelative( 0.3047f, 0.8383f, 0.0577f, 0.1111f );
            repeat.setBoundsRelative( 0.4382f, 0.8383f, 0.1195f, 0.1111f );
            loopLock.setBoundsRelative( 0.6334f, 0.8383f, 0.1195f, 0.1111f );
            linkSlice.setBoundsRelative( 0.8286f, 0.8383f, 0.1195f, 0.1111f );
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////sample editor program ///////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        SampleEditorProgramOptions::SampleEditorProgramOptions( SampleEditorData& d, ui::KickViewport& vw, ui::KickViewport& sc )
            : SampleEditorOPtions( d, vw, sc )
        {
            utils::addAndMakeVisible( this, timingNpitchLabel, startNumber, endNumber, semiNumber, fineNumber, tuneNumber, rootNoteListBox, repeat, loopLock, linkSlice, fromBpm );

            timingNpitchLabel.setPadding( 10 );
            timingNpitchLabel.setFontHeight( 17 );
            timingNpitchLabel.setValueBackgroundColour( ui::Theme::backgroundColour( 1 ) );

            startNumber.setColours( ui::Theme::textColour( 2 ), ui::Theme::textColour( 1 ) );
            endNumber.setColours( ui::Theme::textColour( 2 ), ui::Theme::textColour( 1 ) );
            semiNumber.setColours( ui::Theme::textColour( 2 ), ui::Theme::textColour( 1 ) );
            fineNumber.setColours( ui::Theme::textColour( 2 ), ui::Theme::textColour( 1 ) );

            tuneNumber.setColours( ui::Theme::textColour( 2 ), ui::Theme::textColour( 1 ) );
            rootNoteListBox.setColours( ui::Theme::textColour( 2 ), ui::Theme::textColour( 1 ) );
            rootNoteListBox.setBackGroundColours( ui::Theme::backgroundColour( 13 ), ui::Theme::backgroundColour( 1 ) );
            rootNoteListBox.setJustificationType( juce::Justification::centred );
            repeat.setValueColour( ui::Theme::textColour( 1 ) );
            repeat.setValueFontHeight( 18 );
            loopLock.setValueColour( ui::Theme::textColour( 1 ) );
            loopLock.setValueFontHeight( 18 );
            linkSlice.setValueColour( ui::Theme::textColour( 1 ) );
            linkSlice.setValueFontHeight( 18 );
            fromBpm.setValueColour( ui::Theme::textColour( 1 ) );
            fromBpm.setValueFontHeight( 18 );

            startNumber.setTitleFontHeight( titleHeight );
            endNumber.setTitleFontHeight( titleHeight );
            semiNumber.setTitleFontHeight( titleHeight );
            fineNumber.setTitleFontHeight( titleHeight );
            tuneNumber.setTitleFontHeight( titleHeight );
            rootNoteListBox.setTitleFontHeight( titleHeight );

            startNumber.onValueChanged = [&]( juce::int64 value ) {
                if( value < endNumber.getValue() )
                {
                    startNumber.setValue( value, juce::dontSendNotification );
                    seData.moveTime( seData.getSelectedRegion(), value, false );
                }
                else
                    startNumber.setValue( endNumber.getValue() - 1, juce::dontSendNotification );
            };

            endNumber.onValueChanged = [&]( juce::int64 value ) {
                if( value > startNumber.getValue() )
                    seData.moveTime( seData.getSelectedRegion(), value, true );
                else
                    endNumber.setValue( startNumber.getValue() + 1, juce::dontSendNotification );
            };

            seData.sampleChanged.connect( [this]() {
                juce::int64 sampleLength = seData.getSampleLengthInSamples();
                if( 0 < sampleLength )
                {
                    for( auto& comp : getChildren() )
                        comp->setEnabled( true );
                    reset( sampleLength );
                }
                else
                {
                    for( auto& comp : getChildren() )
                        comp->setEnabled( false );
                }
            } );

            seData.regionChanged.connect( [&]( const Region& regionValue ) {
                startNumber.setValueBounds( 0, regionValue.getEnd() - 1, regionValue.getStart() - 1 );
                endNumber.setValue( regionValue.getEnd(), juce::dontSendNotification );
            } );

            seData.loopLockChanged.connect( [&]( bool state ) {
                loopLock.setToggleState( state, juce::dontSendNotification );
            } );

            loopLock.onClick = [&]() {
                seData.setLoopMode( LoopMode::loopModeForward );
                seData.setLoopLock( loopLock.getToggleState() );
            };

            repeat.onClick = [&]() {
                // Based on the Loop Direction Set the LoopMode
                seData.setLoopMode( LoopMode::loopModeForward ); ///Default
                if( !repeat.getToggleState() )
                    seData.setLoopPosition( seData.getLoopPosition() );
            };

            fromBpm.onClick = [&]() {
                FromBPMMatch* p = new FromBPMMatch( juce::File( seData.getSamplePath() ).getFileName(), tuneNumber.getValue() );
                utils::showDialog( "EDIT TUNING", p, [p, this]( int R ) {
                    tuneNumber.setValue( p->getTuneValue() );
                    if( p ) delete p;
                },
                                   { 0, 0, 400, 200 } );
            };

            seData.linkSlicesChanged.connect( [&]( bool state ) {
                linkSlice.setToggleState( state, juce::dontSendNotification );
            } );

            linkSlice.onClick = [&]() {
                seData.setLinkSlices( linkSlice.getToggleState() );
            };

            ui::themeChanged.connect( [&]() {
                repeat.setToggleButtonText( "Repeat", "Repeat" );
                loopLock.setToggleButtonText( "Loop Lock", "Loop Lock" );
                linkSlice.setToggleButtonText( "Link Slice", "Link Slice" );
            } );
        }

        SampleEditorProgramOptions::~SampleEditorProgramOptions() {}

        void SampleEditorProgramOptions::paint( juce::Graphics& g )
        {
            SampleEditorOPtions::paint( g );
        }

        void SampleEditorProgramOptions::reset( const juce::int64 samples )
        {
            startNumber.setValueBounds( 0, samples - 1, 0 );
            endNumber.setValueBounds( 1, samples, seData.getSelectedRegion().getEnd() );
            semiNumber.setValue( 0, juce::sendNotification );
            fineNumber.setValue( 0, juce::sendNotification );
            tuneNumber.setValue( 0, juce::sendNotification );
            rootNoteListBox.setSelected( 0, juce::sendNotification );
            loopLock.setToggleState( true, juce::sendNotification );
            linkSlice.setToggleState( true, juce::sendNotification );
            repeat.setToggleState( false, juce::sendNotification );
        }

        void SampleEditorProgramOptions::resized()
        {
            SampleEditorOPtions::resized();

            timingNpitchLabel.setBoundsRelative( 0.0199f, 0.4697f, 0.1593f, 0.1111f );

            startNumber.setBoundsRelative( 0.243f, 0.4242f, 0.1195f, 0.1565f );
            endNumber.setBoundsRelative( 0.4382f, 0.4242f, 0.1195f, 0.1565f );
            semiNumber.setBoundsRelative( 0.6334f, 0.4242f, 0.1195f, 0.1565f );
            fineNumber.setBoundsRelative( 0.8286f, 0.4242f, 0.1195f, 0.1565f );

            rootNoteListBox.setBoundsRelative( 0.243f, 0.6313f, 0.1195f, 0.1565f );
            tuneNumber.setBoundsRelative( 0.4382f, 0.6313f, 0.1195f, 0.1565f );
            fromBpm.setBoundsRelative( 0.6334f, 0.6767f, 0.1195f, 0.1111f );
            repeat.setBoundsRelative( 0.8286f, 0.6767f, 0.1195f, 0.1111f );

            loopLock.setBoundsRelative( 0.6334f, 0.8383f, 0.1195f, 0.1111f );
            linkSlice.setBoundsRelative( 0.8286f, 0.8383f, 0.1195f, 0.1111f );
        }

        FromBPMMatch::FromBPMMatch( const juce::String text, int tuneValue )
        {
            utils::addAndMakeVisible( this, sample, beats, tune, bpm, toSeq, match, close );
            bpm.setEnabled( false );

            sample.setText( text, juce::dontSendNotification );
            sample.setValueFontHeight( 20 );
            sample.setValueBackgroundColour( ui::Theme::backgroundColour( 9 ) );
            sample.setValueJustification( juce::Justification::centred );
            sample.setValueColour( ui::Theme::textColour( 1 ) );
            toSeq.setValueColour( ui::Theme::textColour( 1 ) );
            match.setValueColour( ui::Theme::textColour( 1 ) );
            close.setValueColour( ui::Theme::textColour( 1 ) );

            beats.setTitleColour( ui::Theme::textColour( 1 ) );
            tune.setTitleColour( ui::Theme::textColour( 1 ) );
            bpm.setTitleColour( ui::Theme::textColour( 1 ) );

            tune.setValue( tuneValue, juce::dontSendNotification );

            close.onClick = [&] {
                utils::exitDialog( this, UI::CANCEL );
            };
        }

        void FromBPMMatch::resized()
        {
            sample.setBoundsRelative( .025f, .062f, .95f, .187f );

            beats.setBoundsRelative( .025f, .3125f, .25f, .375f );
            tune.setBoundsRelative( .3f, .3125f, .25f, .375f );
            bpm.setBoundsRelative( .725f, .3125f, .25f, .375f );

            toSeq.setBoundsRelative( .025f, .75f, .3f, .1875f );
            match.setBoundsRelative( .35f, .75f, .3f, .1875f );
            close.setBoundsRelative( .675f, .75f, .3f, .1875f );
        }

        SliceAndCombine::SliceAndCombine()
        {
            utils::addAndMakeVisible( this, slice, combine );

            slice.onClick = [&]() {
                if( onSliceClicked )
                    onSliceClicked();
            };

            combine.onClick = [&]() {
                if( onCombineClicked )
                    onCombineClicked();
            };
        }

        void SliceAndCombine::resized()
        {
            slice.setBoundsRelative( .125f, .423f, .325f, .153f );
            combine.setBoundsRelative( .55f, .423f, .325f, .153f );
        }

        SampleEditorChopOPtions::SampleEditorChopOPtions( SampleEditorData& Data, ui::KickViewport& view, ui::KickViewport& scale )
            : SampleEditorOPtions( Data, view, scale )
        {
            utils::addAndMakeVisible( this, modeType, slicePlus, split, combine, legato, playCue, repeat, startNumber, endNumber, cueNumber, xFadeNumber, thresholdNumber, minSamplesNumber, regionsNumber, barsNumber, timeDivList, sliceList, clrAll );
            utils::makeHidden( thresholdNumber, minSamplesNumber, regionsNumber, barsNumber, timeDivList );

            modeType.setListOptions( { "Manual", "Threshold", "Regions", "Quantize" } );

            modeType.setJustificationType( juce::Justification::centredTop, juce::Justification::centred );
            timeDivList.setJustificationType( juce::Justification::centredTop, juce::Justification::centred );
            sliceList.setJustificationType( juce::Justification::centredTop, juce::Justification::centred );

            utils::setValueFontHeight( titleHeight, slicePlus, repeat, split, combine, legato, playCue, clrAll );
            utils::setValueColour( ui::Theme::textColour( 1 ), slicePlus, repeat, split, combine, legato, playCue, clrAll );
            utils::setTitleFontHeight( titleHeight, startNumber, endNumber, cueNumber, xFadeNumber, thresholdNumber, minSamplesNumber, regionsNumber, barsNumber, modeType, sliceList, timeDivList );
            utils::setTitleProportion( .36f, startNumber, endNumber, cueNumber, xFadeNumber, thresholdNumber, minSamplesNumber, regionsNumber, barsNumber );
            utils::setValueFontHeight( 24, modeType, sliceList, timeDivList );

            auto notes = CommonUtils::getTimeDivAsString();
            timeDivList.setListOptions( notes );
            timeDivList.setSelected( 0 );

            seData.sampleChanged.connect( [&]() {
                juce::int64 sampleLength = seData.getSampleLengthInSamples();
                if( 0 < sampleLength )
                {
                    for( auto& comp : getChildren() )
                        comp->setEnabled( true );
                    startNumber.setValueBounds( 0, sampleLength - 1, 0 );
                    endNumber.setValueBounds( 1, sampleLength, sampleLength );
                    cueNumber.setValueBounds( 0, sampleLength, 0 );
                    seData.setCuePosition( 0 );
                }
                else
                {
                    for( auto& comp : getChildren() )
                        comp->setEnabled( false );
                }
            } );

            seData.splitModeChanged.connect( [this]( SampleEditorData::SplitMode splitMode ) {
                slicePlus.setVisible( splitMode == SampleEditorData::SplitMode::splitModeManual );
                clrAll.setVisible( splitMode == SampleEditorData::SplitMode::splitModeManual );
                thresholdNumber.setVisible( splitMode == SampleEditorData::SplitMode::splitModeThreshold );
                minSamplesNumber.setVisible( splitMode == SampleEditorData::SplitMode::splitModeThreshold );
                regionsNumber.setVisible( splitMode == SampleEditorData::SplitMode::splitModeRegions );
                barsNumber.setVisible( splitMode == SampleEditorData::SplitMode::splitModeBpm );
                timeDivList.setVisible( splitMode == SampleEditorData::SplitMode::splitModeBpm );
                if( splitMode == SampleEditorData::SplitMode::splitModeThreshold )
                    setThreshSlices();
                else if( splitMode == SampleEditorData::SplitMode::splitModeBpm )
                    setBpmSlices();
                else if( splitMode == SampleEditorData::SplitMode::splitModeRegions )
                    setSlices( regionsNumber.getValue() );
            } );

            modeType.onSelectionChanged = [&]( int Index, const juce::String& Option ) {
                if( Index == 0 )
                    seData.setSplitMode( SampleEditorData::SplitMode::splitModeManual );
                else if( Index == 1 )
                    seData.setSplitMode( SampleEditorData::SplitMode::splitModeThreshold );
                else if( Index == 2 )
                    seData.setSplitMode( SampleEditorData::SplitMode::splitModeRegions );
                else if( Index == 3 )
                    seData.setSplitMode( SampleEditorData::SplitMode::splitModeBpm );
            };

            seData.regionChanged.connect( [&]( const Region& regionValue ) {
                startNumber.setValueBounds( 0, regionValue.getEnd() - 1, regionValue.getStart() );
                endNumber.setValue( regionValue.getEnd(), juce::dontSendNotification );
                sliceList.setSelected( regionValue.getIndex(), juce::dontSendNotification );
            } );

            seData.playbackStopped.connect( [&]() {
                playCue.setToggleState( false, juce::dontSendNotification );
            } );

            timeDivList.onSelectionChanged = [&]( int Index, const juce::String& Option ) {
                setBpmSlices();
            };

            sliceList.onSelectionChanged = [&]( int Index, const juce::String& Option ) {
                seData.selectRegion( Index - 1 );
            };

            startNumber.onValueChanged = [&]( juce::int64 Value ) {
                if( Value < endNumber.getValue() )
                {
                    startNumber.setValue( Value, juce::dontSendNotification );
                    seData.moveTime( seData.getSelectedRegion(), Value, false );
                }
                else
                    startNumber.setValue( endNumber.getValue() - 1, juce::dontSendNotification );
                modeType.setSelected( 0 );
            };

            endNumber.onValueChanged = [&]( juce::int64 Value ) {
                if( Value > startNumber.getValue() )
                    seData.moveTime( seData.getSelectedRegion(), Value, true );
                else
                    endNumber.setValue( startNumber.getValue() + 1, juce::dontSendNotification );
                modeType.setSelected( 0 );
            };

            seData.regionMoved.connect( [&]( const Region& regionValue ) {
                modeType.setSelected( 0 );
            } );

            seData.cuePositionChanged.connect( [this]( juce::int64 pos ) {
                cueNumber.setValue( pos );
            } );

            cueNumber.onValueChanged = [this]( juce::int64 newValue ) {
                seData.setCuePosition( newValue );
            };

            xFadeNumber.onValueChanged = [&]( juce::int64 Value ) {
            };

            regionsNumber.onValueChanged = [&]( juce::int64 Value ) {
                setSlices( Value );
            };

            barsNumber.onValueChanged = [&]( juce::int64 Value ) {
                setBpmSlices();
            };

            thresholdNumber.onValueChanged = [&]( juce::int64 Value ) {
                setThreshSlices();
            };

            minSamplesNumber.onValueChanged = [&]( juce::int64 Value ) {
                setThreshSlices();
            };

            legato.onClick = [&]() {
                seData.setLinkSlices( legato.getToggleState() );
            };

            playCue.onClick = [&]() {
                if( playCue.getToggleState() )
                    seData.playCue();
                else
                    seData.stop();
            };

            combine.onClick = [&]() {
                seData.joinSelectedAndPrev();
                updateSliceList( seData.getSelectedRegion().getIndex() );
            };

            split.onClick = [&]() {
                seData.splitSelectedRegion();
                updateSliceList( seData.getSelectedRegion().getIndex() );
            };

            slicePlus.onClick = [&]() {
                setSlices( sliceList.getNumOptions() );
            };

            clrAll.onClick = [&]() {
                seData.clearAllRegions();
                sliceList.setListOptions( { "All" } );
            };

            legato.triggerClick();
        }

        void SampleEditorChopOPtions::setThreshSlices()
        {
            std::vector<juce::var> params;
            params.push_back( thresholdNumber.getValue() );
            params.push_back( minSamplesNumber.getValue() );
            seData.splitByThresh( params );
            updateSliceList();
        }

        void SampleEditorChopOPtions::setBpmSlices()
        {
            std::vector<juce::var> params;
            params.push_back( barsNumber.getValue() );
            params.push_back( 4 );
            params.push_back( timeDivList.getSelectedValue() );
            seData.splitByBpm( params );
            int timeSig             = CommonUtils::timeDivStrToQl( params[2] );
            int barDivisionFraction = timeSig / 4;
            int totalDivisions      = (int)params[0] * ( (int)params[1] * barDivisionFraction );
            setSlices( totalDivisions );
        }

        void SampleEditorChopOPtions::setSlices( int Slices )
        {
            seData.setNoSlices( (unsigned int)Slices );
            updateSliceList();
        }

        void SampleEditorChopOPtions::updateSliceList( int SelectedRegion )
        {
            std::vector<juce::String> options;
            auto                      totalRegions = seData.getTotalRegions();
            options.resize( totalRegions + 1 );
            options[0] = "All";
            for( unsigned int i = 1; i <= totalRegions; i++ )
                options[i] = juce::String( i );
            sliceList.setListOptions( options, SelectedRegion );
            if( seData.getSpliMode() == SampleEditorData::SplitMode::splitModeRegions )
                regionsNumber.setValue( seData.getTotalRegions(), juce::dontSendNotification );
        }

        void SampleEditorChopOPtions::paint( juce::Graphics& g )
        {
            SampleEditorOPtions::paint( g );
        }

        void SampleEditorChopOPtions::knobCommand( int knob, int value )
        {
            auto Value = pow( 10, knob % 4 == 0 ? 3 : ( knob % 4 ) - 1 );

            if( knob >= contracts::DeviceControlContracts::KnobSequence::EN_KNOB_1 && knob <= contracts::DeviceControlContracts::KnobSequence::EN_KNOB_4 )
            {
                if( startNumber.getValue() < endNumber.getValue() && startNumber.getValue() >= startNumber.getMin() )
                {
                    startNumber.knobRotated( Value * value );
                    seData.moveTime( seData.getSelectedRegion(), startNumber.getValue(), false );
                }
            }
            else if( knob >= contracts::DeviceControlContracts::KnobSequence::EN_KNOB_5 && knob <= contracts::DeviceControlContracts::KnobSequence::EN_KNOB_8 )
            {
                if( endNumber.getValue() > startNumber.getValue() && endNumber.getValue() <= endNumber.getMax() )
                {
                    endNumber.knobRotated( Value * value );
                    seData.moveTime( seData.getSelectedRegion(), endNumber.getValue(), true );
                }
            }
        }

        void SampleEditorChopOPtions::handleCommand( CPktMgr::HWMsg_t& f_HwMsg )
        {
            if( contracts::DclContractsHelper::isPadButtonMessage( f_HwMsg ) )
            {
                int padNum = contracts::DclContractsHelper::getPadNumber( f_HwMsg );

                if( contracts::DclContractsHelper::getPadState( f_HwMsg ) == contracts::DeviceControlContracts::BTN_PRESSED )
                {
                    bool isFirstSlice = false;
                    if( (int)seData.getTotalRegions() == 0 )
                    {
                        setSlices( 1 );
                        isFirstSlice = true;
                    }
                    padNum += getAudioInterface().getBankOffset();
                    if( padNum < (int)seData.getTotalRegions() )
                    {
                        seData.selectRegion( padNum );
                        seData.playSelectedRegion( MTSamplePlayMode::samplePlayFromStart );
                    }
                    else
                    {
                        if( seData.getTotalRegions() == 1 && isFirstSlice )
                        {
                            seData.selectRegion( 0 );
                            seData.playSelectedRegion( MTSamplePlayMode::samplePlayFromStart );
                            return;
                        }
                    }

                    if( padNum >= seData.getTotalRegions() && seData.isRunning() )
                    {
                        seData.splitAtCuePos();
                        auto numRegions = seData.getTotalRegions();
                        updateSliceList( numRegions );
                        seData.playSelectedRegion( MTSamplePlayMode::samplePlayFromStart );
                    }
                    else if( seData.getSpliMode() == SampleEditorData::splitModeThreshold || seData.getSpliMode() == SampleEditorData::splitModeRegions || seData.getSpliMode() == SampleEditorData::splitModeBpm )
                    {
                        seData.setSplitMode( SampleEditorData::splitModeManual );
                    }
                }
                else if( contracts::DclContractsHelper::getPadState( f_HwMsg ) == contracts::DeviceControlContracts::BTN_RELEASED )
                {
                    if( !seData.isRunning() )
                        seData.playSelectedRegion( MTSamplePlayMode::samplePlayFromStart );
                }
            }
            else if( contracts::DclContractsHelper::isKnobMessage( f_HwMsg ) )
            {
                auto knob  = contracts::DclContractsHelper::getKnobCode( f_HwMsg );
                auto value = contracts::DclContractsHelper::getKnobValue( f_HwMsg );
                if( knob != contracts::DeviceControlContracts::EN_KNOB_CENTER )
                    knobCommand( knob, value );
            }
        }

        void SampleEditorChopOPtions::resized()
        {
            SampleEditorOPtions::resized();

            //Default Options
            repeat.setBoundsRelative( .387f, .904f, .117f, .074f );
            split.setBoundsRelative( .508f, .904f, .117f, .074f );
            combine.setBoundsRelative( .63f, .904f, .117f, .074f );
            legato.setBoundsRelative( .751f, .904f, .117f, .074f );
            playCue.setBoundsRelative( .872f, .904f, .117f, .074f );

            modeType.setBoundsRelative( .02f, .72f, .141f, .139f );
            sliceList.setBoundsRelative( .165f, .72f, .103f, .139f );

            startNumber.setBoundsRelative( .554f, .72f, .103f, .139f );
            endNumber.setBoundsRelative( .662f, .72f, .103f, .139f );
            xFadeNumber.setBoundsRelative( .77f, .72f, .103f, .139f );
            cueNumber.setBoundsRelative( .877f, .72f, .103f, .139f );

            //Manual
            slicePlus.setBoundsRelative( .272f, .768f, .103f, .09f );
            clrAll.setBoundsRelative( .378f, .768f, .103f, .09f );

            //Threshold
            thresholdNumber.setBoundsRelative( .272f, .72f, .103f, .139f );
            minSamplesNumber.setBoundsRelative( .378f, .72f, .103f, .139f );

            //Regions
            regionsNumber.setBoundsRelative( .272f, .72f, .103f, .139f );

            //Quantize
            barsNumber.setBoundsRelative( .272f, .72f, .103f, .139f );
            timeDivList.setBoundsRelative( .378f, .72f, .103f, .139f );
        }

        SampleEditorTuneOPtions::SampleEditorTuneOPtions( SampleEditorData& Data, ui::KickViewport& view, ui::KickViewport& scale )
            : SampleEditorOPtions( Data, view, scale )
        {
            utils::addAndMakeVisible( this, detectBpm, toSeq, matchBpm, sampleBPM, /*bpm,*/ semi, fine, sliceList );

            sampleBPM.setEnabled( false );
            sampleBPM.setRange( 0, 1 );
            //bpm.setRange( 0, 1 );
            semi.setRange( SEMI::MIN, SEMI::MAX, 1 );
            fine.setRange( FINE::MIN, FINE::MAX, 1 );

            sliceList.setJustificationType( juce::Justification::centredTop, juce::Justification::centred );

            utils::setValueColour( ui::Theme::textColour( 1 ), detectBpm, toSeq, matchBpm );
            utils::setValueFontHeight( titleHeight, detectBpm, toSeq, matchBpm );
            utils::setTitleFontHeight( titleHeight, sampleBPM, /*bpm,*/ semi, fine, sliceList );
            utils::setTitleProportion( .36f, sampleBPM, /*bpm,*/ semi, fine, sliceList );
            utils::setValueFontHeight( 24, sliceList );

            /*bpm.onValueChanged = [&]( juce::int64 Value ) {
            };*/

            seData.sampleChanged.connect( [&]() {
                juce::int64 sampleLength = seData.getSampleLengthInSamples();
                if( 0 < sampleLength )
                {
                    for( auto& comp : getChildren() )
                        comp->setEnabled( true );
                    sampleBPM.setEnabled( false );
                    sampleBPM.setValueBounds( 0, seData.detectBpm(), seData.detectBpm() );
                }
                else
                {
                    for( auto& comp : getChildren() )
                        comp->setEnabled( false );
                }
            } );

            sliceList.onSelectionChanged = [&]( int Index, const juce::String& ) {
                if( !seData.isChopMode() )
                    seData.selectRegion( Index - 1 );
            };

            seData.regionAdded.connect( [&]( const Region& regionValue ) {
                updateSliceList();
            } );

            seData.regionRemoved.connect( [&]( const Region& regionValue ) {
                updateSliceList();
            } );

            seData.regionSelected.connect( [&]( const Region& regionValue ) {
                sliceList.setSelected( regionValue.getIndex(), juce::dontSendNotification );
            } );
        }

        void SampleEditorTuneOPtions::updateSliceList()
        {
            std::vector<juce::String> list;
            list.push_back( "All" );
            for( auto id = 1; id <= seData.getTotalRegions(); ++id )
                list.push_back( juce::String( id ) );
            sliceList.setListOptions( list, seData.getSelectedRegion().getIndex() );
        }

        void SampleEditorTuneOPtions::paint( juce::Graphics& g )
        {
            SampleEditorOPtions::paint( g );
        }

        void SampleEditorTuneOPtions::resized()
        {
            SampleEditorOPtions::resized();

            sliceList.setBoundsRelative( .02f, .72f, .103f, .139f );
            detectBpm.setBoundsRelative( .127f, .768f, .103f, .09f );
            sampleBPM.setBoundsRelative( .235f, .72f, .103f, .139f );
            toSeq.setBoundsRelative( .342f, .768f, .103f, .09f );

            //bpm.setBoundsRelative( .554f, .72f, .103f, .139f );
            semi.setBoundsRelative( .662f, .72f, .103f, .139f );
            fine.setBoundsRelative( .77f, .72f, .103f, .139f );
            matchBpm.setBoundsRelative( .877f, .768f, .103f, .09f );
        }

        void SampleEditorTuneOPtions::handleCommand( CPktMgr::HWMsg_t& f_HwMsg )
        {
        }
    }
}
