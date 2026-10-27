namespace ui
{
    KickAnimationText::KickAnimationText( const juce::String& text )
        : mt::MTProperties( this )
    {
        setText( text, juce::dontSendNotification );
    }

    void KickAnimationText::paint( juce::Graphics& g )
    {
        const auto labelTextArea    = getBorderSize().subtractedFrom( getLocalBounds() );
        const auto optimumTextWidth = getFont().getStringWidth( getText() );

        juce::Rectangle<int> textBounds = labelTextArea;
        if( optimumTextWidth > labelTextArea.getWidth() && getScrollProgression() > 0.f )
        {
            const auto xTranslation = optimumTextWidth - labelTextArea.getWidth();
            textBounds              = juce::Rectangle<int>{
                labelTextArea.getX() - (int)( xTranslation * getScrollProgression() ),
                labelTextArea.getY(),
                optimumTextWidth,
                labelTextArea.getHeight()
            };
        }

        g.setColour( findColour( juce::Label::backgroundColourId ) );
        g.fillRoundedRectangle( getLocalBounds().toFloat(), 4.f );

        g.setColour( findColour( juce::Label::textColourId ) );
        g.setFont( getFont() );
        g.drawFittedText( getText(), textBounds, getJustificationType(), 1, 1.f );
    }

    void KickAnimationText::timerCallback()
    {
        if( getScrollProgression() >= 1.f )
        {
            setScrollProgression( 0.f );
            stopTimer();
            return;
        }
        setScrollProgression( getScrollProgression() + 0.05f );
        repaint();
    }

    void KickAnimationText::mouseEnter( const juce::MouseEvent& e )
    {
        if( getFont().getStringWidth( getText() ) > getWidth() )
            startTimer( getTimerSpeed() );
    }

    void KickAnimationText::mouseExit( const juce::MouseEvent& e )
    {
        setScrollProgression( 0.f );
        stopTimer();
        repaint();
    }
}
