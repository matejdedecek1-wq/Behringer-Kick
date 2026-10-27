
namespace ui
{
    KickToggleOptions::KickToggleOptions( const juce::String& Title, const std::vector<juce::String>& Options )
        : mt::MTProperties( this )
    {
        setName( Title );
        setTitle( Title );
        setOptions( Options );

        setLookAndFeel( KickLooknFeel::getLnf() );

        setTitleBackgroundColour( ui::Theme::backgroundColour( 9 ) );
        setValueBackgroundColour( ui::Theme::backgroundColour( 3 ) );

        setTitleColour( ui::Theme::textColour( 1 ) );
        setValueColour( ui::Theme::textColour( 1 ) ); // 3
    }

    void KickToggleOptions::setOptions( const std::vector<juce::String>& newOptions )
    {
        options = newOptions;
        if( options.size() )
            norm = juce::NormalisableRange<int>( 0, options.size() );
        repaint();
    }

    void KickToggleOptions::setSelected( const int Selection, juce::NotificationType notification )
    {
        auto S = juce::jlimit( 0, ( (int)options.size() - 1 ), Selection );
        if( S != selected )
        {
            selected = S;
            if( onSelectionChanged && notification != juce::NotificationType::dontSendNotification )
                onSelectionChanged( selected, options[selected] );

            Component::BailOutChecker checker( this );
            listeners.callChecked( checker, [&]( KickToggleOptions::Listener& l ) { l.toggleOptionsChanged( this ); } );
        }
        repaint();
    }

    void KickToggleOptions::knobRotated( double deltaValue )
    {
        setSelected( selected + (int)( deltaValue ), juce::sendNotificationSync );
    }

    void KickToggleOptions::sliderMoved( float percentValue )
    {
        int tempValue = (int)( norm.convertFrom0to1( ( percentValue ) ) );
        setSelected( tempValue, juce::sendNotificationAsync );
    }

    void KickToggleOptions::sendPercentValueToHardware()
    {
        auto valueInPercent = (int)( 100.0 * norm.convertTo0to1( selected ) );
        contracts::DclSignals::sendCommandToDcl( contracts::DclContractsHelper::sendSliderValueInPercentMessage( valueInPercent ) );
    }

    void KickToggleOptions::knobClicked() {}

    void KickToggleOptions::paint( juce::Graphics& g )
    {
        auto bounds = getLocalBounds().toFloat();

        // Render Title...
        if( !getTitle().isEmpty() )
        {
            auto titleBounds = bounds.removeFromTop( getTitleProportion() * bounds.getHeight() ).toNearestInt();
            // Title background colour
            g.setColour( getTitleBackgroundColour() );
            g.fillRoundedRectangle( bounds, 5.f );
            // Title text
            mt::utils::renderText( g, getTitle().toUpperCase(), titleBounds, getTitleColour(), getTitleJustification(), getTitleFontHeight() );
        }

        // Render value background
        g.setColour( getValueBackgroundColour() );
        g.fillRoundedRectangle( bounds, 5.f );

        // Render options.
        if( !options.empty() )
        {
            auto width          = getWidth();
            auto optionWidth    = width / options.size();
            auto optionBounds   = bounds;
            auto selectedBounds = bounds.removeFromLeft( optionWidth * selected );
            selectedBounds      = bounds.removeFromLeft( optionWidth ).reduced( 2 );

            // Render background for selected option.
            juce::Colour selectedBackgroundColour( 0xFF422f05 );
            g.setColour( selectedBackgroundColour );
            g.fillRoundedRectangle( selectedBounds, 5.f );

            // Render outline for selected option.
            juce::Colour selectedOutlineColour( 0xFFffb608 );
            g.setColour( selectedOutlineColour );
            g.drawRoundedRectangle( selectedBounds, 5.f, 0.8f );

            // Render all option texts.
            juce::Colour optionTextColour = getValueColour();
            for( const auto& option : options )
            {
                auto optionTextBounds = optionBounds.removeFromLeft( optionWidth ).toNearestInt();
                mt::utils::renderText( g, option, optionTextBounds, optionTextColour, getValueJustification(), getValueFontHeight() );
            }
        }
    }

    void KickToggleOptions::mouseDown( const juce::MouseEvent& event )
    {
        if( options.size() )
        {
            auto optionWidth = getWidth() / (int)options.size();
            int  sel         = event.getMouseDownX() / optionWidth;
            setSelected( sel, juce::sendNotificationAsync );
        }
    }

}
