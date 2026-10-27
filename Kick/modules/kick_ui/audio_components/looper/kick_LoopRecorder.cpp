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
#define loopRecorderScaleHeight 20

    LoopRecorder::LoopRecorder( ui::KickIOComponent& Opts )
        : options( Opts )
    {
        progressBar.setTextToDisplay( "" );
        progressBar.setColour( juce::ProgressBar::backgroundColourId, ui::Theme::backgroundColour( 12 ) );
        progressBar.setColour( juce::ProgressBar::foregroundColourId, ui::Theme::foregroundColour( 2 ) );

        ae.addListener( this );

        mt::utils::addAndMakeVisible( this, bars, syncB, stopB, progressBar, wav );
        mt::utils::addAndMakeVisible( this, reverseB, replaceB, playB, recordB, ph, outGain, outPan );

        outGain.setRange( 0.0, 1.0, 0.001 );
        outPan.setRange( -1.0, 1.0, 0.01 );
        outPan.setCentered( true );

        mt::utils::setShowTitle( true, bars, syncB, stopB );
        mt::utils::setTitleColour( ui::Theme::textColour( 1 ), bars, syncB, stopB );
        mt::utils::setValueColour( ui::Theme::textColour( 1 ), bars, syncB, stopB );
        mt::utils::setTitleBackgroundColour( ui::Theme::backgroundColour( 9 ), bars, syncB, stopB );
        mt::utils::setValueBackgroundColour( ui::Theme::backgroundColour( 13 ), bars, syncB, stopB );
        mt::utils::setTitleFontHeight( 18, syncB, stopB );
        mt::utils::setValueFontHeight( 20, bars, syncB, stopB );
        mt::utils::setTitleProportion( .42f, syncB, stopB );

        ui::themeChanged.connect( [&]() {
            reverseB.setImages( "Looper_reverse", "Looper_reverse_active" );
            recordB.setImages( "Looper_rec", "Looper_rec_active" );
            replaceB.setImages( "Looper_repeat", "Looper_repeat_active" );
            playB.setImages( "Looper_play", "Looper_play_active" );
        } );

        setupCallbackHandlers();
    }

    LoopRecorder::~LoopRecorder()
    {
        stopTimer();
        ae.removeListener( this );
    }

    void LoopRecorder::onSequenceSelectionChange( const mt::MTSelected& Selected )
    {
        tempoMgr = ae.getTempoMgr( Selected.sequence );
    }

    void LoopRecorder::setupCallbackHandlers()
    {
        /// Sidebar, options panel.
        options.inputChanged.connect( [this]( int selectedIndex, const juce::String& ) { ae.setLooperRecordingMode( ( mt::Recorder::Mode )( selectedIndex - 1 ) ); } );
        options.gainChanged.connect( [this]( float dbVal ) { ae.setLooperThresholdDB( dbVal ); } );
        options.monitorStateChanged.connect( [this]( bool Value ) { ae.setLooperMonitorOn( Value ); } );
        options.monitorLevelChanged.connect( [&]( double Value ) { ae.setLooperMonitorGain( Value ); } );

        /// Controls...
        outGain.onValueChanged = [&]( double Gain ) { ae.setLooperGain( Gain ); };
        outPan.onValueChanged  = [&]( double Pan ) { ae.setLooperPan( Pan ); };

        // Setup length.
        bars.onValueChanged = [&]( int Value ) {
            stop();
            setRecordingLength( Value );
            setThumbnail();
        };
        // Direction
        reverseB.onClick = [&]() { ae.setLooperDirection( reverseB.getToggleState() ? mt::Recorder::REVERSE : mt::Recorder::FORWARD ); };

        // Record Mode
        recordB.onClick = [&]() {
            if( recordB.getToggleState() ) record( false );
            else ae.setLooperPlay();
            recordingStarted( !recordB.getToggleState() );
        };
        // Overdub Mode
        replaceB.onClick = [&]() {
            if( replaceB.getToggleState() ) record( true );
            else ae.setLooperPlay();
        };
        // Play Mode
        playB.onClick = [&]() { ae.setLooperPlay(); };

        // Stop
        stopB.onClick = [&]() {
            if( !stopB.getToggleState() ) stop();
            else ae.setLooperPlay();
            recordingStarted( !stopB.getToggleState() );
        };
    }

    void LoopRecorder::play( bool start )
    {
        stopB.setToggleState( true, juce::dontSendNotification );
        if( syncB.getToggleState() )
            ae.setSequencePlay( ae.getSelectedSequence(), start, true );

        ae.looperPlayback();
    }

    void LoopRecorder::stop()
    {
        recordB.setToggleState( false, juce::dontSendNotification );
        stopB.setToggleState( false, juce::dontSendNotification );
        if( syncB.getToggleState() )
            ae.setSequencePause();
        ae.looperStop();
    }

    void LoopRecorder::record( bool Overdub )
    {
        if( Overdub )
            ae.setLooperOverdub( true );
        else
            ae.setLooperRecord( true );

        stopB.setToggleState( true, juce::dontSendNotification );
        auto sync = syncB.getToggleState();
        if( sync )
            ae.setSequencePlay( ae.getSelectedSequence(), false, true );

        ae.setLooperDirection( mt::Recorder::FORWARD );
        ae.looperStart( false, sync );
    }

    void LoopRecorder::handleCommand( CPktMgr::HWMsg_t& hwMsg )
    {
        if( contracts::DclContractsHelper::isButtonMessage( hwMsg ) && ( contracts::DeviceControlContracts::BTN_PRESSED == contracts::DclContractsHelper::getButtonState( hwMsg ) ) )
        {
            auto buttonCode = contracts::DclContractsHelper::getButtonCode( hwMsg );
            switch( buttonCode )
            {
                case contracts::DeviceControlContracts::EN_STOP: stop(); break;
                case contracts::DeviceControlContracts::EN_PLAY: play( false ); break;
                case contracts::DeviceControlContracts::EN_PLAYSTART: play( true ); break;
                case contracts::DeviceControlContracts::EN_RECORD: record( false ); break;
                case contracts::DeviceControlContracts::EN_OVERDUB: record( true ); break;
            }
        }
        else if( contracts::DclContractsHelper::isKnobMessage( hwMsg ) )
        {
            auto knob  = contracts::DclContractsHelper::getKnobCode( hwMsg );
            auto value = contracts::DclContractsHelper::getKnobValue( hwMsg );
            if( knob != contracts::DeviceControlContracts::EN_KNOB_CENTER )
            {
                switch( knob )
                {
                    case contracts::DeviceControlContracts::EN_KNOB_3:
                        options.knobRotated( value );
                        break;
                }
                options.knobCommand( knob, value );
            }
        }
    }

    void LoopRecorder::setThumbnail()
    {
        auto& source = ae.getLooperSampleBuffer();
        wav.setSource( { const_cast<float**>( source.getArrayOfReadPointers() ), source.getNumChannels(), source.getNumSamples() }, ae.getLooperSampleRate() );
    }

    void LoopRecorder::clearRecording()
    {
        stop();
        wav.clearCache();
        ae.looperClear();
        ph.reDraw();
    }

    void LoopRecorder::exportRecording( juce::String& FilePath )
    {
        stop();
        ae.looperSave( FilePath );
    }

    double LoopRecorder::getRecordingLength( int Bars )
    {
        auto timeInSeconds = tempoMgr ? tempoMgr->getSecondsUpto( Bars ) : 1;
        return timeInSeconds;
    }

    void LoopRecorder::setRecordingLength( int Bars )
    {
        auto timeInSeconds = tempoMgr ? tempoMgr->getSecondsUpto( Bars ) : 1;
        ae.setLooperLength( timeInSeconds );
    }

    void LoopRecorder::getDisplayText( juce::String& markerText, int bar, int beat, int tick, int numDigits )
    {
        if( numDigits == 1 )
            markerText = markerText.formatted( "%d", bar );
        else if( numDigits == 2 )
            markerText = markerText.formatted( "%d.%d", bar, beat );
        else if( numDigits == 3 )
            markerText = markerText.formatted( "%d.%d.%d", bar, beat, tick );
    }

    void LoopRecorder::paint( juce::Graphics& g )
    {
        mt::utils::fillRoundedRect( g, wav.getBounds(), ui::Theme::backgroundColour( 11 ) );

        // draw scale
        g.setColour( ui::Theme::backgroundColour( 8 ) );
        juce::Font f( 9 );
        g.setFont( f );
        int   numBeats  = bars.getValue() * 4;
        float beatWidth = static_cast<float>( getWidth() / static_cast<float>( numBeats ) );
        paintBeats( g, numBeats, beatWidth, scaleArea );
    }

    void LoopRecorder::paintBeats( juce::Graphics& g, int numBeats, float beatWidth, const juce::Rectangle<int>& displayRect )
    {
        juce::String maxTick;
        int          totalBeats        = numBeats;
        int          maxTickDisplayLen = g.getCurrentFont().getStringWidth( juce::String().formatted( "%d.%d.%d", totalBeats, 4, 960 ) ) + seSamplesSpacing + seSpaceFromMarker;
        int          maxBeatDisplayLen = g.getCurrentFont().getStringWidth( juce::String().formatted( "%d.%d.%d", totalBeats, 4, 0 ) );

        int totalDigits      = 1;
        int noOfBeatMarkings = 0;

        // total beat markings
        noOfBeatMarkings = settings.projects.numerator().getIntValue();

        // total tick markings
        int noOfTickMarkings = 0;

        if( std::floor( ( beatWidth - maxBeatDisplayLen ) / maxTickDisplayLen ) > 0 )
            noOfTickMarkings = static_cast<int>( std::floor( beatWidth / maxTickDisplayLen ) - 1 );

        auto denominator = settings.projects.denominator().getIntValue();

        if( noOfBeatMarkings > 0 )
            totalDigits += 1;
        if( noOfTickMarkings > 0 )
            totalDigits += 1;

        // todo get ppq from settings
        int ppqResolution = 960;
        if( noOfTickMarkings > 0 )
            ppqResolution = 960 / ( denominator / 2 );

        float currentPosX = 0;
        float tickWidth   = beatWidth / ( noOfTickMarkings + 1 );

        int skipCount = 0;
        int skipIndex = 0;

        if( beatWidth <= ( maxBeatDisplayLen * 2 ) )
        {
            skipCount        = 2;
            int newBeatWidth = beatWidth * 2;
            if( newBeatWidth <= ( maxBeatDisplayLen * 2 ) )
                skipCount = 4;
        }
        else if( !noOfTickMarkings )
            skipCount = 1;

        int barInc = 4;
        if( noOfBeatMarkings <= 3 )
            barInc = noOfBeatMarkings;

        if( noOfBeatMarkings <= skipCount )
            totalDigits = 1;

        juce::String markingText;
        bool         tickDisplayed{ false };
        for( int i = 1; i <= totalBeats; i += barInc )
        {
            int bar;
            if( barInc == 1 )
                bar = static_cast<int>( std::ceil( i / barInc ) );
            else
                bar = static_cast<int>( std::ceil( i / barInc ) + 1 );

            juce::Colour scaleColur = ui::Theme::textColour( 1 );

            for( int j = 1; j <= noOfBeatMarkings + 1; j++ )
            {
                int beat = j;
                if( j != 1 )
                {
                    for( int k = 0; k < noOfTickMarkings; k++ )
                    {
                        markingText = juce::String().formatted( "%d.%d.%d", bar, beat - 1, ( k + 1 ) * ppqResolution );
                        drawMarkingText( g, markingText, currentPosX, displayRect );
                        currentPosX += tickWidth;
                        tickDisplayed = true;
                    }
                }
                if( j != noOfBeatMarkings + 1 )
                {
                    if( ( skipCount <= 1 ) || !( skipIndex++ % skipCount ) )
                    {
                        if( tickDisplayed || j == 1 || totalDigits == 1 )
                            scaleColur = ui::Theme::textColour( 1 );
                        else
                            scaleColur = ui::Theme::textColour( 3 );

                        getDisplayText( markingText, bar, beat, 0, totalDigits );
                        drawMarkingText( g, markingText, currentPosX, displayRect, scaleColur );

                        tickDisplayed = false;
                    }

                    if( noOfTickMarkings )
                        currentPosX += tickWidth;
                    else
                        currentPosX += beatWidth;
                }
            }
        }
    }

    void LoopRecorder::drawMarkingText( juce::Graphics& g, juce::String& markerText, float xpos, const juce::Rectangle<int>& displayRect, juce::Colour lineColour )
    {
        g.setColour( lineColour );
        g.drawLine( xpos + seSpaceFromMarker, displayRect.getTopLeft().getY() + 7.0f, xpos + seSpaceFromMarker, static_cast<float>( displayRect.getHeight() ) );
        g.drawText( markerText, static_cast<int>( xpos ), static_cast<int>( displayRect.getTopLeft().getY() + static_cast<float>( displayRect.getHeight() ) ), g.getCurrentFont().getStringWidth( markerText ), displayRect.getHeight() - 5,
                    juce::Justification::horizontallyCentred );
    }

    void LoopRecorder::timerCallback()
    {
        // Update play button state.
        reverseB.setEnabled( ae.isLooperPlay() );
        playB.setToggleState( ae.isLooperReverse(), juce::dontSendNotification );
        // Update toggle states...
        playB.setToggleState( ae.isLooperPlay(), juce::dontSendNotification );
        recordB.setToggleState( stopB.getToggleState() ? ae.isLooperRecording() : false, juce::dontSendNotification );
        replaceB.setToggleState( ae.isLooperOverdubbing(), juce::dontSendNotification );
        stopB.setEnabled( !ae.isLooperEmpty() );

        // Update level meters...
        {
            auto values = ae.getLooperInputMeter();
            if( 2 <= values.size() )
                options.signalStrengthChanged( { values[0].rms, values[1].rms, values[0].peak, values[1].peak } );
            else if( 1 <= values.size() )
                options.signalStrengthChanged( { values[0].rms, -100.f, values[1].rms, -100.f } );
            else
                options.signalStrengthChanged( {} );
        }

        // Update waveform.
        if( ae.isLooperRunning() )
        {
            setThumbnail();
            ph.reDraw();
        }
        return;
    }

    void LoopRecorder::resized()
    {
        bars.setBoundsRelative( .0306f, .8067f, .1073f, .1274f );

        reverseB.setBoundsRelative( .184f, .8466f, .095f, .0876f );

        replaceB.setBoundsRelative( .2852f, .8466f, .095f, .0876f );
        //playB.setBoundsRelative( .524f, .8466f, .095f, .0876f);
        recordB.setBoundsRelative( .3865f, .8466f, .095f, .0876f );

        outGain.setBoundsRelative( .4969f, .7986f, .1226f, .1394f );
        outPan.setBoundsRelative( .6042f, .7986f, .1226f, .1394f );

        syncB.setBoundsRelative( .7438f, .7928f, .0981f, .1394f );
        stopB.setBoundsRelative( .8481f, .7928f, .1226f, .1394f );

        auto b         = getLocalBounds();
        auto panelArea = b.withBottom( b.proportionOfHeight( 0.75f ) );

        progressBar.setBounds( panelArea.removeFromTop( 6 ) );
        ph.setBounds( panelArea );
        scaleArea = panelArea.removeFromTop( loopRecorderScaleHeight );
        wav.setBounds( panelArea );
    }

    void LoopRecorder::visibilityChanged()
    {
        auto visible = isVisible();
        if( visible )
        {
            options.setInputSelection( ae.getLooperRecordingMode(), juce::dontSendNotification );
            options.setGain( ae.getLooperThresholdDB() );
            options.setMonitor( ae.isLooperMonitorOn() );
            options.setMonitorLevelNormalized( ae.getLooperMonitorGain() );

            outGain.setValue( ae.getLooperGain(), juce::dontSendNotification );
            outPan.setValue( ae.getLooperPan(), juce::dontSendNotification );

            auto numBars = bars.getValue();
            if( !juce::approximatelyEqual<double>( getRecordingLength( numBars ), ae.getLooperLength() ) )
                setRecordingLength( numBars );

            startTimerHz( 30 );
        }
        else
        {
            stopTimer();
            stop();
        }
        // Enable or Disable looper backend.
        ae.setLooperEnable( visible );

        for( auto it : getChildren() )
            it->setVisible( isVisible() );
    }

    ///////////////////////////////////////////////////////////////////////
    // Looper Playhead
    ///////////////////////////////////////////////////////////////////////
    LooperPlayHead::LooperPlayHead()
    {
        setName( "LooperPlayHead-Canvas" );
        setInterceptsMouseClicks( false, false );
    }

    void LooperPlayHead::paint( juce::Graphics& g )
    {
        auto b = getLocalBounds().toFloat();
        if( b.isEmpty() )
            return;

        auto cX = normPos * b.getWidth();
        mt::utils::renderPlayhead( g, b, cX, ui::Theme::foregroundColour( 2 ) );
    }

#undef loopRecorderScaleHeight
}
