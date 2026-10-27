namespace ui
{
    //////////////////////////////////////////////////////////////////////
    // Grid Settings
    /////////////////////////////////////////////////////////////////////

    GridSettings::GridSettings()
    {
        mt::utils::addAndMakeVisible( this, snapMode, hittingPadsMode, autoScroll, autoAdvance );
        mt::utils::setValueColour( ui::Theme::textColour( 2 ), snapMode, hittingPadsMode, autoScroll, autoAdvance );
        mt::utils::setValueFontHeight( 20, snapMode, hittingPadsMode, autoScroll, autoAdvance );
        mt::utils::setTitleFontHeight( 16, snapMode, hittingPadsMode, autoScroll, autoAdvance );
        mt::utils::setTitleProportion( .4f, snapMode, hittingPadsMode, autoScroll, autoAdvance );
        mt::utils::makeHidden( autoAdvance );
    }

    bool GridSettings::getHittingPadMode()
    {
        return hittingPadsMode.getSelected() == 1;
    }

    bool GridSettings::getAutoAdvance()
    {
        return autoAdvance.getSelected() == 1;
    }

    int GridSettings::getSnapMode()
    {
        return snapMode.getSelected();
    }

    AutoScrollMode GridSettings::getAutoScrollType()
    {
        auto selected = autoScroll.getSelected();
        if( selected == 0 )
            return AutoScrollMode::Off;
        else if( selected == 1 )
            return AutoScrollMode::Follow;
        else if( selected == 2 )
            return AutoScrollMode::Page;
        return AutoScrollMode::Off;
    }

    void GridSettings::setScreen( ScreenType Screen )
    {
        screenType = Screen;
        resized();
    }

    void GridSettings::resized()
    {
        mt::utils::makeHidden( snapMode, hittingPadsMode, autoAdvance, autoScroll );
        if( screenType == GridMidi )
        {
            mt::utils::makeVisible( snapMode, hittingPadsMode, autoScroll );
            snapMode.setBoundsRelative( .1076f, .1458f, .784f, .2083f );
            hittingPadsMode.setBoundsRelative( .1076f, .396f, .784f, .2083f );
        }
        else if( screenType == ListEdit )
        {
            mt::utils::makeVisible( hittingPadsMode, autoAdvance, autoScroll );
            hittingPadsMode.setBoundsRelative( .1076f, .1458f, .784f, .2083f );
            autoAdvance.setBoundsRelative( .1076f, .396f, .784f, .2083f );
        }
        else if( screenType == GridAudio )
        {
            mt::utils::makeVisible( snapMode, autoScroll );
            snapMode.setBoundsRelative( .1076f, .1458f, .784f, .2083f );
        }
        autoScroll.setBoundsRelative( .1076f, .645f, .784f, .2083f );
    }
}
