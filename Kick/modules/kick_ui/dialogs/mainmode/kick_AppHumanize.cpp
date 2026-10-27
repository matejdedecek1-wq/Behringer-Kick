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
AppHumanize::AppHumanize()
{
    addAndMakeVisible( timeLabel );
    timeLabel.setFont( juce::Font( font_15, juce::Font::plain ).withTypefaceStyle( "Bold" ) );
    timeLabel.setJustificationType( juce::Justification::centredLeft );
    timeLabel.setEditable( false, false, false );
    timeLabel.setColour( juce::Label::textColourId, juce::Colour( 0xffb2b2b2 ) );
    timeLabel.setColour( juce::TextEditor::textColourId, ui::Theme::backgroundColour( 5 ) );
    timeLabel.setColour( juce::TextEditor::backgroundColourId, juce::Colour( 0x00000000 ) );
    timeLabel.setText( "Time", dontSendNotification );
    timeLabel.setBounds( xStartHumTimeL, yStartHumTimeL, widthHumTimeL, heightHumTimeL );

    addAndMakeVisible( noteInputFilter );
    noteInputFilter.setFont( juce::Font( font_15, juce::Font::plain ).withTypefaceStyle( "Regular" ) );
    noteInputFilter.setJustificationType( juce::Justification::centredLeft );
    noteInputFilter.setEditable( false, false, false );
    noteInputFilter.setColour( juce::Label::textColourId, juce::Colour( 0xffb2b2b2 ) );
    noteInputFilter.setColour( juce::Label::outlineColourId, juce::Colour( 0xffb2b2b2 ) );
    noteInputFilter.setColour( juce::TextEditor::textColourId, ui::Theme::backgroundColour( 5 ) );
    noteInputFilter.setColour( juce::TextEditor::backgroundColourId, juce::Colour( 0x00000000 ) );
    noteInputFilter.setText( "", dontSendNotification );
    noteInputFilter.setBounds( xStartHumnoteFilter, yStartHumnoteFilter, widthHumnoteFilter, heightHumnoteFilter );

    addAndMakeVisible( lengthSelect );
    lengthSelect.setButtonText( TRANS( "Humanize Note Length" ) );
    lengthSelect.addListener( this );
    lengthSelect.setColour( juce::ToggleButton::textColourId, juce::Colour( 0xffff9300 ) );
    lengthSelect.setBounds( xStartHumLenSel, yStartHumLenSel, widthHumLenSel, heightHumLenSel );
    lengthSelect.setToggleState( true, sendNotification );

    addAndMakeVisible( humanizeApply );
    humanizeApply.setButtonText( TRANS( "Apply" ) );
    humanizeApply.addListener( this );
    humanizeApply.setColour( juce::TextButton::buttonColourId, ui::Theme::backgroundColour( 5 ) );
    humanizeApply.setBounds( xStartHumApply, yStartHumApply, widthHumApply, heightHumApply );

    addAndMakeVisible( humanizeCancel );
    humanizeCancel.setButtonText( TRANS( "Cancel" ) );
    humanizeCancel.addListener( this );
    humanizeCancel.setColour( juce::TextButton::buttonColourId, ui::Theme::backgroundColour( 5 ) );
    humanizeCancel.setBounds( xStartHumCancel, yStartHumCancel, widthHumCancel, heightHumCancel );

    addAndMakeVisible( humanizeTimeSelection );
    humanizeTimeSelection.setButtonText( TRANS( "Humanize Time" ) );
    humanizeTimeSelection.addListener( this );
    humanizeTimeSelection.setColour( juce::ToggleButton::textColourId, juce::Colour( 0xffff9300 ) );
    humanizeTimeSelection.setBounds( xStartHumTimSel, yStartHumTimSel, widthHumTimSel, heightHumTimSel );
    humanizeTimeSelection.setToggleState( true, sendNotification );

    addAndMakeVisible( intoLabel );
    intoLabel.setFont( juce::Font( font_11, juce::Font::plain ).withTypefaceStyle( "Regular" ) );
    intoLabel.setJustificationType( juce::Justification::centred );
    intoLabel.setEditable( false, false, false );
    intoLabel.setColour( juce::Label::textColourId, juce::Colour( 0xffff9300 ) );
    intoLabel.setColour( juce::TextEditor::textColourId, ui::Theme::backgroundColour( 5 ) );
    intoLabel.setColour( juce::TextEditor::backgroundColourId, juce::Colour( 0x00000000 ) );
    intoLabel.setText( "AMOUNT (PULSES)", dontSendNotification );
    intoLabel.setBounds( xStartHumInto, yStartHumInto, widthHumInto, heightHumInto );

    addAndMakeVisible( timeSliderNotify );
    timeSliderNotify.setFont( juce::Font( font_15, juce::Font::plain ).withTypefaceStyle( "Regular" ) );
    timeSliderNotify.setJustificationType( juce::Justification::centredLeft );
    timeSliderNotify.setEditable( false, false, false );
    timeSliderNotify.setColour( juce::Label::textColourId, juce::Colour( 0xffb2b2b2 ) );
    timeSliderNotify.setColour( juce::Label::outlineColourId, juce::Colour( 0xffb2b2b2 ) );
    timeSliderNotify.setColour( juce::TextEditor::textColourId, ui::Theme::backgroundColour( 5 ) );
    timeSliderNotify.setColour( juce::TextEditor::backgroundColourId, juce::Colour( 0x00000000 ) );
    timeSliderNotify.setText( "", dontSendNotification );
    timeSliderNotify.setBounds( xStartHumTimSliN, yStartHumTimSliN, widthHumTimSliN, heightHumTimSliN );

    //ui::KickHelpers::addSlider( this, timeSlider );
    timeSlider.makeVisibleAndAddListner( this );
    timeSlider.setRange( 1, 240, 1 );
    timeSlider.setBounds( xStartHumTimSlider, yStartHumTimSlider, widthHumTimSlider, heightHumTimSlider );

    addAndMakeVisible( timeAmountLabel );
    timeAmountLabel.setFont( juce::Font( font_11, juce::Font::plain ).withTypefaceStyle( "Regular" ) );
    timeAmountLabel.setJustificationType( juce::Justification::centred );
    timeAmountLabel.setEditable( false, false, false );
    timeAmountLabel.setColour( juce::Label::textColourId, juce::Colour( 0xffff9300 ) );
    timeAmountLabel.setColour( juce::TextEditor::textColourId, ui::Theme::backgroundColour( 5 ) );
    timeAmountLabel.setColour( juce::TextEditor::backgroundColourId, juce::Colour( 0x00000000 ) );
    timeAmountLabel.setText( "AMOUNT (PULSES)", dontSendNotification );
    timeAmountLabel.setBounds( xStartHumTimL, yStartHumTimL, widthHumTimL, heightHumTimL );

    addAndMakeVisible( eagernessNotify );
    eagernessNotify.setFont( juce::Font( font_15, juce::Font::plain ).withTypefaceStyle( "Regular" ) );
    eagernessNotify.setJustificationType( juce::Justification::centredLeft );
    eagernessNotify.setEditable( false, false, false );
    eagernessNotify.setColour( juce::Label::textColourId, juce::Colour( 0xffb2b2b2 ) );
    eagernessNotify.setColour( juce::Label::outlineColourId, juce::Colour( 0xffb2b2b2 ) );
    eagernessNotify.setColour( juce::TextEditor::textColourId, ui::Theme::backgroundColour( 5 ) );
    eagernessNotify.setColour( juce::TextEditor::backgroundColourId, juce::Colour( 0x00000000 ) );
    eagernessNotify.setText( "", dontSendNotification );
    eagernessNotify.setBounds( xStartHumEagN, yStartHumEagN, widthHumEagN, heightHumEagN );

    //ui::KickHelpers::addSlider( this, eagernessSlider );
    eagernessSlider.makeVisibleAndAddListner( this );
    eagernessSlider.setRange( -50, 50, 1 );
    eagernessSlider.setBounds( xStartHumEagSlider, yStartHumEagSlider, widthHumEagSlider, heightHumEagSlider );

    addAndMakeVisible( eagernessLabel );
    eagernessLabel.setFont( juce::Font( font_11, juce::Font::plain ).withTypefaceStyle( "Regular" ) );
    eagernessLabel.setJustificationType( juce::Justification::centred );
    eagernessLabel.setEditable( false, false, false );
    eagernessLabel.setColour( juce::Label::textColourId, juce::Colour( 0xffff9300 ) );
    eagernessLabel.setColour( juce::TextEditor::textColourId, ui::Theme::backgroundColour( 5 ) );
    eagernessLabel.setColour( juce::TextEditor::backgroundColourId, juce::Colour( 0x00000000 ) );
    eagernessLabel.setText( "EAGERNESS", dontSendNotification );
    eagernessLabel.setBounds( xStartHumEagL, yStartHumEagL, widthHumEagL, heightHumEagL );

    addAndMakeVisible( lengthLabel );
    lengthLabel.setFont( juce::Font( font_15, juce::Font::plain ).withTypefaceStyle( "Bold" ) );
    lengthLabel.setJustificationType( juce::Justification::centredLeft );
    lengthLabel.setEditable( false, false, false );
    lengthLabel.setColour( juce::Label::textColourId, juce::Colour( 0xffb2b2b2 ) );
    lengthLabel.setColour( juce::TextEditor::textColourId, ui::Theme::backgroundColour( 5 ) );
    lengthLabel.setColour( juce::TextEditor::backgroundColourId, juce::Colour( 0x00000000 ) );
    lengthLabel.setText( "Length", dontSendNotification );
    lengthLabel.setBounds( xStartHumLenL, yStartHumLenL, widthHumLenL, heightHumLenL );

    //ui::KickHelpers::addSlider( this, lengthSlider );
    lengthSlider.makeVisibleAndAddListner( this );
    lengthSlider.setRange( 2, 32, 1 );
    lengthSlider.setBounds( xStartHumLenSlider, yStartHumLenSlider, widthHumLenSlider, heightHumLenSlider );

    addAndMakeVisible( lengthValueLabel );
    lengthValueLabel.setFont( juce::Font( font_11, juce::Font::plain ).withTypefaceStyle( "Regular" ) );
    lengthValueLabel.setJustificationType( juce::Justification::centred );
    lengthValueLabel.setEditable( false, false, false );
    lengthValueLabel.setColour( juce::Label::textColourId, juce::Colour( 0xffff9300 ) );
    lengthValueLabel.setColour( juce::TextEditor::textColourId, ui::Theme::backgroundColour( 5 ) );
    lengthValueLabel.setColour( juce::TextEditor::backgroundColourId, juce::Colour( 0x00000000 ) );
    lengthValueLabel.setText( "LENGTH (%)", dontSendNotification );
    lengthValueLabel.setBounds( xStartHumLenValL, yStartHumLenValL, widthHumLenValL, heightHumLenValL );

    addAndMakeVisible( velocitySelect );
    velocitySelect.setButtonText( TRANS( "Humanize Velocity" ) );
    velocitySelect.addListener( this );
    velocitySelect.setColour( juce::ToggleButton::textColourId, juce::Colour( 0xffff9300 ) );
    velocitySelect.setBounds( xStartHumVelSel, yStartHumVelSel, widthHumVelSel, heightHumVelSel );
    velocitySelect.setToggleState( true, sendNotification );

    addAndMakeVisible( velocityLabel );
    velocityLabel.setFont( juce::Font( font_15, juce::Font::plain ).withTypefaceStyle( "Bold" ) );
    velocityLabel.setJustificationType( juce::Justification::centredLeft );
    velocityLabel.setEditable( false, false, false );
    velocityLabel.setColour( juce::Label::textColourId, juce::Colour( 0xffb2b2b2 ) );
    velocityLabel.setColour( juce::TextEditor::textColourId, ui::Theme::backgroundColour( 5 ) );
    velocityLabel.setColour( juce::TextEditor::backgroundColourId, juce::Colour( 0x00000000 ) );
    velocityLabel.setText( "Velocity", dontSendNotification );
    velocityLabel.setBounds( xStartHumVelL, yStartHumVelL, widthHumVelL, heightHumVelL );

    //ui::KickHelpers::addSlider( this, velocitySlider );
    velocitySlider.makeVisibleAndAddListner( this );
    velocitySlider.setRange( 1, 100, 1 );
    velocitySlider.setBounds( xStartHumVelSlider, yStartHumVelSlider, widthHumVelSlider, heightHumVelSlider );

    addAndMakeVisible( strengthLabel );
    strengthLabel.setFont( juce::Font( font_11, juce::Font::plain ).withTypefaceStyle( "Regular" ) );
    strengthLabel.setJustificationType( juce::Justification::centred );
    strengthLabel.setEditable( false, false, false );
    strengthLabel.setColour( juce::Label::textColourId, juce::Colour( 0xffff9300 ) );
    strengthLabel.setColour( juce::TextEditor::textColourId, ui::Theme::backgroundColour( 5 ) );
    strengthLabel.setColour( juce::TextEditor::backgroundColourId, juce::Colour( 0x00000000 ) );
    strengthLabel.setText( "STRENGTH (%)", dontSendNotification );
    strengthLabel.setBounds( xStartHumStrenL, yStartHumStrenL, widthHumStrenL, heightHumStrenL );

    addAndMakeVisible( onlySelectedCheck );
    onlySelectedCheck.setButtonText( TRANS( "Only apply to selected events" ) );
    onlySelectedCheck.addListener( this );
    onlySelectedCheck.setColour( juce::ToggleButton::textColourId, juce::Colour( 0xffff9300 ) );
    onlySelectedCheck.setBounds( xStartHumSelCheck, yStartHumSelCheck, widthHumSelCheck, heightHumSelCheck );

    addAndMakeVisible( inputFilterLabel );
    inputFilterLabel.setFont( juce::Font( font_15, juce::Font::plain ).withTypefaceStyle( "Bold" ) );
    inputFilterLabel.setJustificationType( juce::Justification::centredLeft );
    inputFilterLabel.setEditable( false, false, false );
    inputFilterLabel.setColour( juce::Label::textColourId, juce::Colour( 0xffb2b2b2 ) );
    inputFilterLabel.setColour( juce::TextEditor::textColourId, ui::Theme::backgroundColour( 5 ) );
    inputFilterLabel.setColour( juce::TextEditor::backgroundColourId, juce::Colour( 0x00000000 ) );
    inputFilterLabel.setText( "Input Filter", dontSendNotification );
    inputFilterLabel.setBounds( xStartHumFilter, yStartHumFilter, widthHumFilter, heightHumFilter );

    setSize( widthHumDia, heightHumDia );
}

AppHumanize::~AppHumanize()
{}

//==============================================================================
void AppHumanize::paint( juce::Graphics& g )
{
    g.fillAll( juce::Colour( 0xff2d2d2d ) );

    {
        int x = xStartHumPaint, y = yStartHumPaint, width = widthHumPaint, height = heightHumPaint;
        juce::Colour fillColour = juce::Colour( 0xffb2b2b2 );
        g.setColour( fillColour );
        g.fillRect( x, y, width, height );
    }

    {
        int x = xStartHumPaint, y = yStartHumPaint2, width = widthHumPaint, height = heightHumPaint;
        juce::Colour fillColour = juce::Colour( 0xffb2b2b2 );
        g.setColour( fillColour );
        g.fillRect( x, y, width, height );
    }

    {
        int x = xStartHumPaint, y = yStartHumPaint3, width = widthHumPaint, height = heightHumPaint;
        juce::Colour fillColour = juce::Colour( 0xffb2b2b2 );
        g.setColour( fillColour );
        g.fillRect( x, y, width, height );
    }

    {
        int x = xStartHumPaint, y = yStartHumPaint4, width = widthHumPaint, height = heightHumPaint;
        juce::Colour fillColour = juce::Colour( 0xffb2b2b2 );
        g.setColour( fillColour );
        g.fillRect( x, y, width, height );
    }
}

void AppHumanize::resized()
{}

void AppHumanize::updateTrackInfo( AppTrackComponent::trackData_t* data )
{
    trackDataInfoPtr.reset( data );
}

void AppHumanize::buttonClicked( juce::Button* buttonThatWasClicked )
{
    if( buttonThatWasClicked == &lengthSelect )
    {
        humanizeData.isNoteLengthEnabled = lengthSelect.getToggleState();
        if( lengthSelect.getToggleState() == true )
        {
            lengthSlider.setEnabled( true );
        }
        else
        {
            lengthSlider.setEnabled( false );
        }
    }
    else if( buttonThatWasClicked == &humanizeApply )
    {
        // Add logic for sending command to AE.
        if( juce::DialogWindow* dw = findParentComponentOfClass<juce::DialogWindow>() )
            dw->exitModalState( 1 );
    }
    else if( buttonThatWasClicked == &humanizeCancel )
    {
        if( juce::DialogWindow* dw = findParentComponentOfClass<juce::DialogWindow>() )
            dw->exitModalState( 987 );
    }
    else if( buttonThatWasClicked == &humanizeTimeSelection )
    {
        humanizeData.isTimeEnabled = humanizeTimeSelection.getToggleState();
        if( humanizeTimeSelection.getToggleState() == true )
        {
            timeSlider.setEnabled( true );
            eagernessSlider.setEnabled( true );
        }
        else
        {
            timeSlider.setEnabled( false );
            eagernessSlider.setEnabled( false );
        }
    }
    else if( buttonThatWasClicked == &velocitySelect )
    {
        humanizeData.isVelocityEnabled = velocitySelect.getToggleState();
        if( velocitySelect.getToggleState() == true )
        {
            velocitySlider.setEnabled( true );
        }
        else
        {
            velocitySlider.setEnabled( false );
        }
    }
    else if( buttonThatWasClicked == &onlySelectedCheck )
    {
        humanizeData.onlyForSelectedNotes = onlySelectedCheck.getToggleState();
    }
}

void AppHumanize::sliderValueChanged( juce::Slider* sliderThatWasMoved )
{
    double timeSliderValue = 0.0f;
    if( sliderThatWasMoved == &splitSlider.getSlider() )
    {
    }
    else if( sliderThatWasMoved == &timeSlider.getSlider() )
    {
        timeSliderValue = timeSlider.getValue();
        humanizeData.time = (uint8_t)timeSliderValue;

        if( timeSliderValue < 12 )
        {
            timeSliderNotify.setText( "A very small amount (practically mechanical)", dontSendNotification );
        }
        else if( timeSliderValue < 30 )
        {
            timeSliderNotify.setText( "A small amount (very human)", dontSendNotification );
        }
        else if( timeSliderValue < 52 )
        {
            timeSliderNotify.setText( "Less than a 64th note", dontSendNotification );
        }
        else if( timeSliderValue < 60 )
        {
            timeSliderNotify.setText( "A bit Less than a 64th note", dontSendNotification );
        }
        else if( timeSliderValue == 60 )
        {
            timeSliderNotify.setText( "Exactly a 64th note", dontSendNotification );
        }
        else if( timeSliderValue < 69 )
        {
            timeSliderNotify.setText( "A bit more than a 64th note", dontSendNotification );
        }
        else if( timeSliderValue < 90 )
        {
            timeSliderNotify.setText( "More than a 64th note", dontSendNotification );
        }
        else if( timeSliderValue < 110 )
        {
            timeSliderNotify.setText( "Less than a 32nd note", dontSendNotification );
        }
        else if( timeSliderValue < 120 )
        {
            timeSliderNotify.setText( "A bit less than a 32nd note", dontSendNotification );
        }
        else if( timeSliderValue == 120 )
        {
            timeSliderNotify.setText( "Exactly a 32nd note", dontSendNotification );
        }
        else if( timeSliderValue < 131 )
        {
            timeSliderNotify.setText( "A bit more than a 32nd note", dontSendNotification );
        }
        else if( timeSliderValue < 180 )
        {
            timeSliderNotify.setText( "More than a 32nd note", dontSendNotification );
        }
        else if( timeSliderValue < 240 )
        {
            timeSliderNotify.setText( "A bit Less than a 16th note", dontSendNotification );
        }
        else if( timeSliderValue == 240 )
        {
            timeSliderNotify.setText( "Exactly a 16th note", dontSendNotification );
        }
    }
    else if( sliderThatWasMoved == &eagernessSlider.getSlider() )
    {
        humanizeData.eagerness = (int8_t)eagernessSlider.getValue();
    }
    else if( sliderThatWasMoved == &lengthSlider.getSlider() )
    {
        humanizeData.noteLength = (uint8_t)lengthSlider.getValue();
    }
    else if( sliderThatWasMoved == &velocitySlider.getSlider() )
    {
        humanizeData.velocity = (uint8_t)velocitySlider.getValue();
    }
}

//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="AppHumanize" componentName=""
                 parentClasses="public juce::Component" constructorParams="" variableInitialisers=""
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="440" initialHeight="760">
  <BACKGROUND backgroundColour="ff2d2d2d">
    <RECT pos="10 35 430 1" fill="solid: ffb2b2b2" hasStroke="0"/>
    <RECT pos="12 340 430 1" fill="solid: ffb2b2b2" hasStroke="0"/>
    <RECT pos="12 484 430 1" fill="solid: ffb2b2b2" hasStroke="0"/>
    <RECT pos="12 640 430 1" fill="solid: ffb2b2b2" hasStroke="0"/>
  </BACKGROUND>
  <LABEL name="" id="2c25f3ebfa81205e" memberName="timeLabel" virtualName=""
         explicitFocusOrder="0" pos="10 10 150 25" textCol="ffb2b2b2"
         edTextCol="ff000000" edBkgCol="0" labelText="Time" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="1" italic="0" justification="33"
         typefaceStyle="Bold"/>
  <LABEL name="" id="f1cba97b911f75d3" memberName="noteInputFilter" virtualName=""
         explicitFocusOrder="0" pos="10 152 422 25" textCol="ffb2b2b2"
         outlineCol="ffb2b2b2" edTextCol="ff000000" edBkgCol="0" labelText="Note Input Filter"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default font" fontsize="15.0" kerning="0.0" bold="0"
         italic="0" justification="33"/>
  <TOGGLEBUTTON name="" id="769847cf14ab6ca1" memberName="lengthSelect" virtualName=""
                explicitFocusOrder="0" pos="10 350 214 25" txtcol="ffff9300"
                buttonText="Humanize Note Length" connectedEdges="0" needsCallback="1"
                radioGroupId="0" state="0"/>
  <TEXTBUTTON name="" id="de3d92ad3a06bfc2" memberName="humanizeApply" virtualName=""
              explicitFocusOrder="0" pos="120 712 80 24" bgColOff="ff000000"
              buttonText="Apply" connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="d42ca367e764f9a7" memberName="humanizeCancel" virtualName=""
              explicitFocusOrder="0" pos="240 712 80 24" bgColOff="ff000000"
              buttonText="Cancel" connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TOGGLEBUTTON name="" id="b4bc9a461207e14a" memberName="humanizeTimeSelection"
                virtualName="" explicitFocusOrder="0" pos="10 48 214 25" txtcol="ffff9300"
                buttonText="Humanize Time" connectedEdges="0" needsCallback="1"
                radioGroupId="0" state="0"/>
  <SLIDER name="" id="fe80f0f3821417c3" memberName="splitSlider" virtualName=""
          explicitFocusOrder="0" pos="10 105 425 25" bkgcol="ff000000"
          trackcol="ffff9300" rotarysliderfill="ffff9300" rotaryslideroutline="ffff9300"
          textboxbkgd="ff9300" min="2.0" max="32.0" int="1.0" style="LinearBar"
          textBoxPos="TextBoxBelow" textBoxEditable="1" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <LABEL name="" id="96dc95b6c1418128" memberName="intoLabel" virtualName=""
         explicitFocusOrder="0" pos="176 80 96 18" textCol="ffff9300"
         edTextCol="ff000000" edBkgCol="0" labelText="AMOUNT (PULSES)"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default font" fontsize="11.0" kerning="0.0" bold="0"
         italic="0" justification="36"/>
  <LABEL name="" id="fe556ad5e4b52d2c" memberName="timeSliderNotify" virtualName=""
         explicitFocusOrder="0" pos="10 152 422 25" textCol="ffb2b2b2"
         outlineCol="ffb2b2b2" edTextCol="ff000000" edBkgCol="0" labelText=""
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default font" fontsize="15.0" kerning="0.0" bold="0"
         italic="0" justification="33"/>
  <SLIDER name="" id="51c238305bf11bf7" memberName="timeSlider" virtualName=""
          explicitFocusOrder="0" pos="10 105 425 25" bkgcol="ff000000"
          trackcol="ffff9300" rotarysliderfill="ffff9300" rotaryslideroutline="ffff9300"
          textboxbkgd="ff9300" min="1.0" max="240.0" int="1.0" style="LinearBar"
          textBoxPos="TextBoxBelow" textBoxEditable="1" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <LABEL name="" id="25ddee1c13f7ad80" memberName="timeAmountLabel" virtualName=""
         explicitFocusOrder="0" pos="176 80 96 18" textCol="ffff9300"
         edTextCol="ff000000" edBkgCol="0" labelText="AMOUNT (PULSES)"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default font" fontsize="11.0" kerning="0.0" bold="0"
         italic="0" justification="36"/>
  <LABEL name="" id="1597778091940c50" memberName="eagernessNotify" virtualName=""
         explicitFocusOrder="0" pos="10 267 422 25" textCol="ffb2b2b2"
         outlineCol="ffb2b2b2" edTextCol="ff000000" edBkgCol="0" labelText="Note Input Filter"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default font" fontsize="15.0" kerning="0.0" bold="0"
         italic="0" justification="33"/>
  <SLIDER name="" id="438bbad3411a1df3" memberName="eagernessSlider" virtualName=""
          explicitFocusOrder="0" pos="10 220 425 25" bkgcol="ff000000"
          trackcol="ffff9300" rotarysliderfill="ffff9300" rotaryslideroutline="ffff9300"
          textboxbkgd="ff9300" min="-50.0" max="50.0" int="1.0" style="LinearBar"
          textBoxPos="TextBoxBelow" textBoxEditable="1" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <LABEL name="" id="75546745113e6d5a" memberName="eagernessLabel" virtualName=""
         explicitFocusOrder="0" pos="176 195 96 18" textCol="ffff9300"
         edTextCol="ff000000" edBkgCol="0" labelText="EAGERNESS" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="11.0" kerning="0.0" bold="0" italic="0" justification="36"/>
  <LABEL name="" id="4d2fa5d47fb563ba" memberName="lengthLabel" virtualName=""
         explicitFocusOrder="0" pos="10 312 150 25" textCol="ffb2b2b2"
         edTextCol="ff000000" edBkgCol="0" labelText="Length" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="1" italic="0" justification="33"
         typefaceStyle="Bold"/>
  <SLIDER name="" id="6bc1f8cf38572369" memberName="lengthSlider" virtualName=""
          explicitFocusOrder="0" pos="10 412 425 25" bkgcol="ff000000"
          trackcol="ffff9300" rotarysliderfill="ffff9300" rotaryslideroutline="ffff9300"
          textboxbkgd="ff9300" min="2.0" max="32.0" int="1.0" style="LinearBar"
          textBoxPos="TextBoxBelow" textBoxEditable="1" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <LABEL name="" id="ef351e286c703038" memberName="lengthValueLabel" virtualName=""
         explicitFocusOrder="0" pos="176 387 96 18" textCol="ffff9300"
         edTextCol="ff000000" edBkgCol="0" labelText="LENGTH (%)" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="11.0" kerning="0.0" bold="0" italic="0" justification="36"/>
  <TOGGLEBUTTON name="" id="ae3368e350542961" memberName="velocitySelect" virtualName=""
                explicitFocusOrder="0" pos="10 497 214 25" txtcol="ffff9300"
                buttonText="Humanize Velocity" connectedEdges="0" needsCallback="1"
                radioGroupId="0" state="0"/>
  <LABEL name="" id="1a4103e8cb9badd8" memberName="velocityLabel" virtualName=""
         explicitFocusOrder="0" pos="10 459 150 25" textCol="ffb2b2b2"
         edTextCol="ff000000" edBkgCol="0" labelText="Velocity" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="1" italic="0" justification="33"
         typefaceStyle="Bold"/>
  <SLIDER name="" id="f11a7cd0fdccdf91" memberName="velocitySlider" virtualName=""
          explicitFocusOrder="0" pos="10 559 425 25" bkgcol="ff000000"
          trackcol="ffff9300" rotarysliderfill="ffff9300" rotaryslideroutline="ffff9300"
          textboxbkgd="ff9300" min="1.0" max="100.0" int="1.0" style="LinearBar"
          textBoxPos="TextBoxBelow" textBoxEditable="1" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <LABEL name="" id="951da89dcdcd3613" memberName="strengthLabel" virtualName=""
         explicitFocusOrder="0" pos="176 534 96 18" textCol="ffff9300"
         edTextCol="ff000000" edBkgCol="0" labelText="STRENGTH (%)" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="11.0" kerning="0.0" bold="0" italic="0" justification="36"/>
  <TOGGLEBUTTON name="" id="631eb5ea8d13d335" memberName="onlySelectedCheck"
                virtualName="" explicitFocusOrder="0" pos="10 652 238 25" txtcol="ffff9300"
                buttonText="Only apply to selected events" connectedEdges="0"
                needsCallback="1" radioGroupId="0" state="0"/>
  <LABEL name="" id="7ac30661219463a3" memberName="inputFilterLabel" virtualName=""
         explicitFocusOrder="0" pos="10 614 150 25" textCol="ffb2b2b2"
         edTextCol="ff000000" edBkgCol="0" labelText="Input Filter" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="1" italic="0" justification="33"
         typefaceStyle="Bold"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

//[EndFile] You can add extra defines here...
//[/EndFile]
