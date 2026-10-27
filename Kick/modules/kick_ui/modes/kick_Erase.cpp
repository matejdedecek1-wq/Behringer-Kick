KickErase::KickErase()
{
    setName( "Erase Notes" );
    auto& ae  = mt::getAudioInterface();
    auto& sel = ae.getSelected();

    mt::utils::addAndMakeVisible( this, sequences, tracks, startBar, endBar, startBeat, endBeat );

    tracks.setListOptions( ae.getTrackList( sel.sequence ), sel.track );
    sequences.setListOptions( ae.getSequenceList(), sel.sequence );

    //seqList.setSelected(ae.getSelectedSequence());
    //trackList.setSelected( ae.getSelectedTrack() );

    mt::MTBarNBeats BnB;
    BnB = ae.getMasterLength();

    int startBarValue, endBarValue, startBeatValue, endBeatValue = 0;

    endBarValue = BnB.bar;
    if( endBarValue > 0 )
        endBeatValue = 4;
    else
        endBeatValue = 0;

    startBar.setMax( endBarValue );
    endBar.setMax( endBarValue );
    startBeat.setMax( endBeatValue );
    endBeat.setMax( endBeatValue );

    startBar.showTitle( true );
    endBar.showTitle( true );
    startBeat.showTitle( true );
    endBeat.showTitle( true );

    startBar.onValueChanged = [this]( int bars ) {
        endBar.setMin( bars );
        startBnB.bar = bars;
    };

    startBeat.onValueChanged = [this]( int beats ) {
        endBeat.setMin( beats );
        startBnB.beat = beats;
    };
    endBar.onValueChanged = [this]( int bars ) {
        endBar.setMin( bars );
        endBnB.bar = bars;
    };

    endBeat.onValueChanged = [this]( int beats ) {
        endBeat.setMin( beats );
        endBnB.beat = beats;
    };

    sequences.onSelectionChanged = [this]( int index, const juce::String& Value ) {
        selectedSequenceIndex = index;
        eraseSelect.sequence  = selectedSequenceIndex;
    };

    tracks.onSelectionChanged = [this]( int index, const juce::String& Value ) {
        selectedTrackIndex = index;
        eraseSelect.track  = selectedTrackIndex;
    };
}

KickErase::~KickErase()
{
}

void KickErase::paint( juce::Graphics& g )
{
    g.fillAll( ui::Theme::backgroundColour( 1 ) );
}

void KickErase::resized()
{
    sequences.setBoundsRelative( .20f, .20f, .20f, .20f );
    tracks.setBoundsRelative( .60f, .20f, .20f, .20f );

    startBar.setBoundsRelative( .20f, .60f, .10f, .20f );
    startBeat.setBoundsRelative( .35f, .60f, .10f, .20f );
    endBar.setBoundsRelative( .60f, .60f, .10f, .20f );
    endBeat.setBoundsRelative( .75f, .60f, .10f, .20f );
}
