namespace ui
{
    FilePopup::FilePopup( juce::String Info, bool isYes, bool isNo, int x, int y, int width )
        : info( Info ), startX( x ), startY( y ), width( width )
    {
        mt::utils::addChildComponents( this, yes, no, info );
        yes.setVisible( isYes );
        no.setVisible( isNo );
        info.setVisible( true );
        yes.onClick = [&]() {
            close( UI::APPLY );
        };
        no.onClick = [&]() {
            close( UI::CANCEL );
        };
        info.setFontHeight( 18.0f );
        info.setTextColour( ui::Theme::textColour( 2 ) );
        info.setValueBackgroundColour( ui::Theme::backgroundColour( 9 ) );
        info.setValueJustification( juce::Justification::centred );

        ui::themeChanged.connect( [&]() {
            no.setImages( "No" );
            yes.setImages( "Yes" );
        } );
    }

    void FilePopup::close( int State )
    {
        if( auto dw = getCurrentlyModalComponent() )
            dw->exitModalState( State );
    }

    void FilePopup::resized()
    {
        info.setBounds( startX, startY, width, textHeight );
        no.setBoundsRelative( 0.05f, 0.86f, 0.4f, 0.106f );
        yes.setBoundsRelative( 0.55f, 0.86f, 0.4f, 0.106f );
    }
}
