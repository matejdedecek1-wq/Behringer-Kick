
namespace ui
{
    KickAlertWindow::KickAlertWindow( KickAlertWindowType t, const std::vector<juce::String>& b, const juce::String& m )
        : type( t )
        , buttons( b )
        , message( m )
    {
        int i = 0;
        for( const juce::String& button : buttons )
        {
            std::unique_ptr<KickButton>& alertButton = alertButtons.emplace_back( std::unique_ptr<KickButton>( new KickButton( juce::String( i ), button ) ) );

            addAndMakeVisible( *alertButton );
            mt::utils::addListener( this, *alertButton );
            if( alertButton->getButtonText() == "Shutdown" )
                alertButton->onClick = [&]() { mt::utils::exitDialog( this, UI::SHUTDOWN ); };
            if( alertButton->getButtonText() == "Restart" )
                alertButton->onClick = [&]() { mt::utils::exitDialog( this, UI::RESTART ); };
            if( alertButton->getButtonText() == "Cancel" )
                alertButton->onClick = [&]() { mt::utils::exitDialog( this, UI::CANCEL ); };
            i++;
        }
    }

    void KickAlertWindow::paint( juce::Graphics& g )
    {
        juce::Rectangle<int> iconRect( 55, 40, 60, 60 );
        if( type == KickAlertWindowType::Info || type == KickAlertWindowType::Warning )
            iconRect = juce::Rectangle<int>( 40, 20, 90, 80 );

        juce::Rectangle<int> textRect( 135, 5, getWidth() - 140, 141 - 5 );
        juce::Image          msgTypeImg;
        if( type == KickAlertWindowType::Info )
            msgTypeImg = ui::Theme::getImage( "Alert Icon - Warning" );
        else if( type == KickAlertWindowType::Error )
            msgTypeImg = ui::Theme::getImage( "Alert Icon - Error" );
        else if( type == KickAlertWindowType::Question )
            msgTypeImg = ui::Theme::getImage( "Alert Icon - Question" );
        else
            msgTypeImg = ui::Theme::getImage( "Alert Icon - Warning" );

        g.drawImage( msgTypeImg, iconRect.toFloat() );
        g.setColour( ui::Theme::textColour( 2 ) );
        g.drawText( message, textRect, juce::Justification::centred );
    }

    KickAlertWindow::~KickAlertWindow()
    {
    }

    void KickAlertWindow::resized()
    {
        int btnWidth = 100;
        int startX   = ( getWidth() / 2 ) - ( ( ( (int)buttons.size() * btnWidth ) + ( ( (int)buttons.size() - 1 ) * 10 ) ) / 2 );
        for( const std::unique_ptr<KickButton>& button : alertButtons )
        {
            button->setBounds( startX, 170, btnWidth, 34 );
            startX += btnWidth + 10;
        }
    }

    void KickAlertWindow::buttonClicked( juce::Button* btn )
    {
        int                 status = btn->getName().getIntValue();
        juce::DialogWindow* dw     = findParentComponentOfClass<juce::DialogWindow>();
        if( dw == nullptr )
            return;

        dw->exitModalState( status );
    }

    void KickAlertWindow::showYesNoCancelAlert( KickAlertWindowType type, const juce::String& message, const juce::String& title, std::function<void( int )> callback )
    {
        showAlert( type, { "Yes", "No", "Cancel" }, message, title, callback );
    }

    void KickAlertWindow::showShutdownRestartAlert( KickAlertWindowType type, const juce::String& message, const juce::String& title, std::function<void( int )> callback )
    {
        showAlert( type, { "Shutdown", "Restart", "Cancel" }, message, title, callback );
    }

    void KickAlertWindow::showYesNoAlert( KickAlertWindowType type, const juce::String& message, const juce::String& title, std::function<void( int )> callback )
    {
        showAlert( type, { "Yes", "No" }, message, title, callback );
    }

    void KickAlertWindow::showCancelAlert( KickAlertWindowType type, const juce::String& message, const juce::String& title, std::function<void( int )> callback )
    {
        showAlert( type, { "Cancel" }, message, title, callback );
    }

    void KickAlertWindow::showAlert( KickAlertWindowType type, const std::vector<juce::String>& buttons, const juce::String& message, const juce::String& title, std::function<void( int )> callback )
    {
        KickAlertWindow* alert = new KickAlertWindow( type, buttons, message );
        mt::utils::showDialog( title, alert, [callback, alert]( int r ) {
            delete alert;
            if( callback )
                callback( r );
        },
                               { 0, 0, 400, 300 } );
    };
}
