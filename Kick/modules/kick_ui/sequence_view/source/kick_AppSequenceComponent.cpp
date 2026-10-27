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

AppSequenceDisplay::AppSequenceDisplay( int Index )
{
    mt::utils::addAndMakeVisible( this, name, bpm, bars, start, end, transpose, loop, seqGlobal );

    mt::utils::setTitleProportion( .35f, start, end, bars, bpm, seqGlobal );
    mt::utils::setTitleFontHeight( 16, start, end, bars, bpm, seqGlobal );
    mt::utils::setValueFontHeight( 16, transpose );
    mt::utils::setTitleJustification( juce::Justification::centredTop, bpm, seqGlobal, bars, start, end, transpose );
    mt::utils::setValueJustification( juce::Justification::centred, bpm, seqGlobal, bars, start, end, transpose );

    mt::utils::setShowTitle( true, bpm, bars, start, end );

    transpose.setCentered( true );
    transpose.setRange( TRANSPOSE::MIN, TRANSPOSE::MAX, TRANSPOSE::STEP );
    transpose.setNumDecimalPlacesToDisplay( 0 );

    bpm.setColours( ui::Theme::textColour( 2 ), ui::Theme::textColour( 1 ) );
    bars.setColours( ui::Theme::textColour( 2 ), ui::Theme::textColour( 1 ) );
    start.setColours( ui::Theme::textColour( 2 ), ui::Theme::textColour( 1 ) );
    end.setColours( ui::Theme::textColour( 2 ), ui::Theme::textColour( 1 ) );
    transpose.setColours( ui::Theme::textColour( 2 ), ui::Theme::textColour( 1 ) );

    name.setTitleJustification( juce::Justification::centredLeft );
    name.setValueJustification( juce::Justification::topLeft );
    name.setTitleFontHeight( 24 );
    name.setValueFontHeight( 18 );

    name.onTextChange   = [&]() { ae.renameSequence( seqNo, name.getText() ); };
    bars.onValueChanged = [&]( int Value ) { ae.setSequenceLength( seqNo, Value ); };
    bpm.onValueChanged  = [&]( int Value ) { ae.setSequenceBPM( seqNo, Value ); };

    start.onValueChanged     = [&]( int Value ) { ae.setSequenceLoopStart( seqNo, ( Value - 1 ) ); };
    end.onValueChanged       = [&]( int Value ) { ae.setSequenceLoopEnd( seqNo, Value ); };
    loop.onClick             = [&]() { if( pSeq ) pSeq->isLooping = loop.getToggleState(); };
    seqGlobal.onClick        = [&]() { if( pSeq ) pSeq->isMasterBPM = seqGlobal.getToggleState(); };
    transpose.onValueChanged = [&]( double Value ) { if( pSeq ) pSeq->transpose = Value; };

    ui::themeChanged.connect( [&]() {
        loop.setImages( "Loop_normal", "Loop_active" );
    } );

    setSequence( Index );
    setInterceptsMouseClicks( true, true );
}

void AppSequenceDisplay::setSequence( int Index )
{
    scope.reset();
    seqNo = Index;
    pSeq  = ae.getSequenceData( seqNo );

    if( pSeq )
    {
        scope = std::make_shared<mt::Kick::ScopeTracker>();
        pSeq->name.onValueChange.connect( [&]() { name.setText( pSeq->name.get(), juce::dontSendNotification ); }, scope );
        pSeq->length.onValueChange.connect( [&]() { bars.setValue( pSeq->length.get(), juce::dontSendNotification ); }, scope );
        pSeq->loopStart.onValueChange.connect( [&]() { start.setValue( ( pSeq->loopStart.get() + 1 ), juce::dontSendNotification ); }, scope );
        pSeq->loopEnd.onValueChange.connect( [&]() { end.setValue( pSeq->loopEnd.get(), juce::dontSendNotification ); }, scope );
        pSeq->transpose.onValueChange.connect( [&]() { transpose.setValue( pSeq->transpose.get(), juce::dontSendNotification ); }, scope );
        pSeq->isLooping.onValueChange.connect( [&]() { loop.setToggleState( pSeq->isLooping.get(), juce::dontSendNotification ); }, scope );
        pSeq->tempo.onValueChange.connect( [&]() { bpm.setValue( pSeq->tempo.get(), juce::dontSendNotification ); }, scope );
        pSeq->isMasterBPM.onValueChange.connect( [&]() { seqGlobal.setToggleState( pSeq->isMasterBPM.get(), juce::dontSendNotification ); }, scope );
        auto msg = "Tracks : " + juce::String( pSeq->numTracks );
        name.setLine2Text( msg );
    }

    if( onSeqChange )
        onSeqChange( Index );

    updateData();
}

void AppSequenceDisplay::updateData()
{
    if( pSeq )
    {
        pSeq->name.onValueChange();
        pSeq->length.onValueChange();
        pSeq->loopStart.onValueChange();
        pSeq->loopEnd.onValueChange();
        pSeq->transpose.onValueChange();
        pSeq->isLooping.onValueChange();
        pSeq->tempo.onValueChange();
        pSeq->isMasterBPM.onValueChange();
    }
}

void AppSequenceDisplay::mouseDown( const juce::MouseEvent& e )
{
    ui::clearFocusComponent();
}

void AppSequenceDisplay::resized()
{
    name.setBoundsRelative( .065f, .3035f, .16f, .3928f );
    bpm.setBoundsRelative( .238f, .0892f, .1f, .6071f );
    seqGlobal.setBoundsRelative( .351f, .3035f, .08f, .3928f );
    bars.setBoundsRelative( .444f, .0892f, .1f, .6071f );
    start.setBoundsRelative( .557f, .0892f, .1f, .6071f );
    end.setBoundsRelative( .670f, .0892f, .1f, .6071f );
    loop.setBoundsRelative( .783f, .3035f, .08f, .3928f );
    transpose.setBoundsRelative( .874f, .0892f, .062f, .8214f );
}

void AppSequenceDisplay::linkRotated( int ID, int Value )
{
    DBG( "AppSequenceDisplay linkRotated with ID ---> " << ID << "Value -->  " << Value );
    switch( ID )
    {
        case 0: bpm.knobRotated( Value ); break;
        case 1: bars.knobRotated( Value ); break;
        case 2: start.knobRotated( Value ); break;
        case 3: end.knobRotated( Value ); break;
    }
}

void AppSequenceDisplay::visibilityChanged()
{
    setSequence( getSeqNo() );
}
//==============================================================================
//  AppSequenceComponent
//==============================================================================
AppSequenceComponent::AppSequenceComponent( int Index, juce::String Name )
{
    setName( Name );
    addMouseListener( this, true );
    setComponentID( "AppSequenceComponent" );

    mt::utils::addChildComponents( this, srNo, seq, options );

    options.setBackgroundColour( ui::Theme::backgroundColour( 1 ) );

    srNo.setInterceptsMouseClicks( false, false );
    srNo.setColours( ui::Theme::textColour( 0 ), ui::Theme::backgroundColour( 6 ), ui::Theme::backgroundColour( 12 ) );
    addMouseListener( this, true );

    options.onClick = [&]() {
        mt::getAudioInterface().setSelectedSequence( seq.getSeqNo() );
        onSeqEdit();
    };

    seq.onSeqChange = [&]( int SeqNo ) { srNo.setText( juce::String( SeqNo + 1 ) ); };

    ui::themeChanged.connect( [&]() {
        srNo.setImage( "ID_sequence" );
        options.setImages( "editOptions" );
    } );
}

AppSequenceComponent::~AppSequenceComponent()
{
    removeMouseListener( this );
}

void AppSequenceComponent::setSequence( int SequenceNo )
{
    auto& ae = mt::getAudioInterface();

    if( 0 <= SequenceNo && SequenceNo < ae.getNumSequences() )
    {
        seq.setSequence( SequenceNo );
        mt::utils::makeVisible( srNo, seq, options );
    }
    else
    {
        mt::utils::makeHidden( srNo, seq, options );
    }
}

void AppSequenceComponent::resized()
{
    srNo.setBoundsRelative( 0.001f, .01f, .044f, .97f );
    seq.setBoundsRelative( 0.0f, 0.f, 1.f, 1.f );
    options.setBoundsRelative( .957f, .0136f, .04f, .965f );
}

void AppSequenceComponent::visibilityChanged()
{
    if( isVisible() )
        updateData();
}

void AppSequenceComponent::mouseUp( const juce::MouseEvent& e )
{
    if( e.mouseWasClicked() )
        mt::getAudioInterface().setSelectedSequence( seq.getSeqNo() );
}
