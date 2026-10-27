KickCopy::KickCopy()
{
    setName( "Copy Pad" );
    miniMap.setLayout( ui::KickPadMiniMap::Layout::Horizantal );
    miniMap.setColours( -1, ui::Theme::highlightColour( 3 ).withAlpha( 0.2f ), ui::Theme::highlightColour( 2 ), ui::Theme::outlineColour( 5 ) );

    sourcePad.onSelectionChanged = [&]( int Sel, const juce::String& ) {
    };

    miniMap.onClicked = [&]( juce::String Bank, int Pad ) {
        pads->setBank( Bank );
    };

    mt::utils::addAndMakeVisible( this, miniMap, sourcePad );
}

KickCopy::~KickCopy()
{
}

void KickCopy::paint( juce::Graphics& g )
{
    g.fillAll( ui::Theme::backgroundColour( 1 ) );
}

void KickCopy::resized()
{
    sourcePad.setBoundsRelative( 0.4f, 0.1f, 0.2f, 0.2f );
    miniMap.setBoundsRelative( .3f, .35f, .4f, .65f );
}
