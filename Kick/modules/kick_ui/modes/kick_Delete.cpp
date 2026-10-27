KickDelete::KickDelete()
{
    setName( "Copy Pad" );
    miniMap.setLayout( ui::KickPadMiniMap::Layout::Horizantal );
    miniMap.setColours( -1, ui::Theme::highlightColour( 3 ).withAlpha( 0.2f ), ui::Theme::highlightColour( 2 ), ui::Theme::outlineColour( 5 ) );

    miniMap.onClicked = [&]( juce::String Bank, int Pad ) {
        pads->setBank( Bank );
    };

    mt::utils::addAndMakeVisible( this, miniMap );
}

KickDelete::~KickDelete()
{
}

void KickDelete::paint( juce::Graphics& g )
{
    g.fillAll( ui::Theme::backgroundColour( 1 ) );
}

void KickDelete::resized()
{
    miniMap.setBoundsRelative( .3f, .35f, .4f, .65f );
}
