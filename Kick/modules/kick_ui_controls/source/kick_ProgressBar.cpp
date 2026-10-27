namespace ui
{
    KickProgressBar::KickProgressBar()
    {
    }

    KickProgressBar::~KickProgressBar()
    {
    }

    void KickProgressBar::updateProgress( int newProgreess )
    {
        progress = newProgreess;
        repaint();
    }

    void KickProgressBar::close()
    {
        juce::DialogWindow* dw = findParentComponentOfClass<juce::DialogWindow>();
        if( dw == nullptr )
            return;

        dw->exitModalState( UI::BUTTONS::APPLY );
    }

    void KickProgressBar::paint( juce::Graphics& g )
    {
        int                  progressHeight = 40;
        int                  progressWidth  = getWidth() - 40;
        juce::Rectangle<int> progressRect( 20, ( getHeight() / 2 ) - ( progressHeight / 2 ), progressWidth, progressHeight );
        float                cornerRadius = 6.0f;
        g.setColour( ui::Theme::backgroundColour() );
        g.fillRoundedRectangle( progressRect.toFloat(), cornerRadius );

        int progressInPx = (int)std::round( progressWidth * progress / 100 );
        progressRect.setWidth( progressInPx );
        g.setColour( ui::Theme::foregroundColour() );
        g.fillRoundedRectangle( progressRect.toFloat(), cornerRadius );
    }

    void KickProgressBar::paintOverChildren( juce::Graphics& g )
    {
        int                  progressHeight = 40;
        int                  progressWidth  = getWidth() - 40;
        juce::Rectangle<int> progressRect( 20, ( getHeight() / 2 ) - ( progressHeight / 2 ), progressWidth, progressHeight );
        float                cornerRadius = 6.0f;

        // Background fill
        g.setColour( ui::Theme::backgroundColour( 5 ) );
        g.fillRoundedRectangle( progressRect.toFloat(), cornerRadius );

        int progressInPx = (int)std::round( progressWidth * progress / 100 );
        if( progressInPx < 0 )
            progressInPx = 0;
        progressRect.setWidth( progressInPx );

        // Foreground fill
        juce::Rectangle<float> fillRect   = progressRect.toFloat();
        juce::ColourGradient   fillColour = juce::ColourGradient::horizontal( juce::Colour( 0xff, 0xb6, 0x08 ), juce::Colour( 0xff, 0x5c, 0x00 ), progressRect );

        // Recalculate fill area..
        fillRect = fillRect.removeFromLeft( progressInPx );
        fillColour.addColour( 0.5f, juce::Colour( 0xff, 0xb6, 0x08 ) );

        g.setGradientFill( fillColour );
        g.fillRoundedRectangle( fillRect, cornerRadius );
    }
}
