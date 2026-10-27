
namespace ui
{
    KickTimerControl::KickTimerControl()
    {
        mt::utils::addChildComponents( this, mm, ss, ms, mSep, sSep );
        mSep.setValueJustification( juce::Justification::centred );
        sSep.setValueJustification( juce::Justification::centred );
        mSep.setTextColour( ui::Theme::textColour( 2 ) );
        sSep.setTextColour( ui::Theme::textColour( 2 ) );
        mm.setColours( ui::Theme::textColour( 2 ), ui::Theme::textColour( 2 ) );
        ss.setColours( ui::Theme::textColour( 2 ), ui::Theme::textColour( 2 ) );
        ms.setColours( ui::Theme::textColour( 2 ), ui::Theme::textColour( 2 ) );
        mm.setTitleColour( ui::Theme::textColour( 2 ) );
        ss.setTitleColour( ui::Theme::textColour( 2 ) );
        ms.setTitleColour( ui::Theme::textColour( 2 ) );

        mm.setValueBackgroundColour( juce::Colours::transparentBlack );
        ss.setValueBackgroundColour( juce::Colours::transparentBlack );
        ms.setValueBackgroundColour( juce::Colours::transparentBlack );
        mm.setTitleProportion( 0.25f );
        ss.setTitleProportion( 0.25f );
        ms.setTitleProportion( 0.25f );
        mm.setPadding( "0", 2 );
        ss.setPadding( "0", 2 );
        ms.setPadding( "0", 2 );

        mm.onValueChanged = [&]( int ) { recalculateTime(); };
        ss.onValueChanged = [&]( int ) { recalculateTime(); };
        ms.onValueChanged = [&]( int ) { recalculateTime(); };

        mSep.setValueBackgroundColour( ui::Theme::backgroundColour( 9 ) );
        sSep.setValueBackgroundColour( ui::Theme::backgroundColour( 9 ) );
    }

    void KickTimerControl::setNumberDisplay( ViewType type )
    {
        mm.setDisplayFormat( type );
        ss.setDisplayFormat( type );
        ms.setDisplayFormat( type );
    }

    void KickTimerControl::setValueColour( juce::Colour colour )
    {
        mm.setColours( colour, colour );
        ss.setColours( colour, colour );
        ms.setColours( colour, colour );
        mSep.setTextColour( colour );
    }

    void KickTimerControl::resized()
    {
        auto b     = getLocalBounds().reduced( 2 );
        int  parts = ( displayFormat == DisplayFormat::displayFormatMS ) ? 3 : 2;
        auto w     = ( ( b.getWidth() - ( ( parts - 1 ) * 18 ) ) / parts );
        auto fH    = 0.8 * b.getHeight();

        mSep.setFontHeight( fH );
        sSep.setFontHeight( fH );
        mm.setValueFontHeight( fH );
        ss.setValueFontHeight( fH );
        ms.setValueFontHeight( fH );

        mm.setTitleFontHeight( 20 );
        ss.setTitleFontHeight( 20 );
        ms.setTitleFontHeight( 20 );

        b.removeFromLeft( .2 * w );
        mm.setBounds( b.removeFromLeft( w * .8 ) );
        mSep.setBounds( b.removeFromLeft( 18 ).removeFromTop( ( mm.getHeight() / 3 ) * 2 ) );
        ss.setBounds( b.removeFromLeft( w * .8 ) );

        mm.setVisible( true );
        ss.setVisible( true );
        mSep.setVisible( true );

        if( displayFormat == DisplayFormat::displayFormatMS )
        {
            sSep.setBounds( b.removeFromLeft( 18 ).removeFromTop( ( ss.getHeight() / 3 ) * 2 ) );
            ms.setBounds( b.removeFromLeft( w * .8 ) );
            sSep.setVisible( true );
            ms.setVisible( true );
        }

        if( displayFormat == DisplayFormat::displayFormatSS )
        {
            mm.setValueFontHeight( 30.0f );
            ss.setValueFontHeight( 30.0f );
            mSep.setFontHeight( 30.0f );
            mSep.setValueJustification( juce::Justification::centred );
        }
    }

    void KickTimerControl::setEditable( bool Editable )
    {
        editable = Editable;
        mm.setEnabled( editable );
        ss.setEnabled( editable );
        ms.setEnabled( editable );
        if( !Editable )
        {
            mm.setFocus( false );
            ss.setFocus( false );
            ms.setFocus( false );
        }
    }

    void KickTimerControl::setTime( double newTime )
    {
        if( newTime > timeMaxValue )
            timeCounter = timeMaxValue;
        else
            timeCounter = newTime;

        double timeCounterTmp = CommonUtils::convertToPrecision( ( timeCounter ), 2 );
        double totalSeconds;
        double mSecs                = std::modf( timeCounterTmp, &totalSeconds );
        int    milliSeconds         = static_cast<int>( mSecs * 100 );
        int    totalSecondsIntegral = (int)totalSeconds;
        int    minutes              = totalSecondsIntegral / 60;
        int    seconds              = totalSecondsIntegral % 60;
        mm.setValue( minutes, juce::dontSendNotification );
        ss.setValue( seconds, juce::dontSendNotification );
        ms.setValue( milliSeconds, juce::dontSendNotification );

        timeChanged( timeCounter );
    }

    void KickTimerControl::recalculateTime()
    {
        double temp = 60.0 * mm.getValue();
        temp += ss.getValue();
        temp += ( ms.getValue() * 0.01 );
        if( !juce::approximatelyEqual( timeCounter, temp ) )
        {
            timeCounter = temp;
            timeChanged( timeCounter );
        }
    }

    double KickTimerControl::getTime()
    {
        recalculateTime();
        return timeCounter;
    }

    void KickTimerControl::knobRotated( double deltaValue, bool Minutes )
    {
        double currentTime = getTime();
        double newTime;
        if( Minutes )
        {
            int currentMinutes = static_cast<int>( currentTime ) / 60;
            int currentSeconds = static_cast<int>( currentTime ) % 60;
            currentMinutes += static_cast<int>( deltaValue );
            newTime = currentMinutes * 60 + currentSeconds;
            setTime( newTime );
        }
        else
        {
            newTime = currentTime + deltaValue;
            setTime( newTime );
        }
    }
}
