namespace ui
{
    KickMultiSelectTabBarButton::KickMultiSelectTabBarButton( int numSel, juce::TabbedButtonBar& ownerBar, const juce::String& buttonName )
        : TabBarButton( buttonName, ownerBar )
        , noOfSelections( numSel )
        , mt::MTProperties( this )
    {
    }

    int KickMultiSelectTabBarButton::getNumOfSelection()
    {
        return noOfSelections;
    }
    int KickMultiSelectTabBarButton::getSelectionSpacing()
    {
        return 5;
    }
    int KickMultiSelectTabBarButton::getSelectionSize()
    {
        return 20;
    }
    int KickMultiSelectTabBarButton::getCurrentSelection()
    {
        return currentSelection;
    }
    void KickMultiSelectTabBarButton::setCurrentSelection( int selection )
    {
        currentSelection = selection;
    }
    void KickMultiSelectTabBarButton::setCurrentSelection( juce::String name )
    {
        selectedState = name;
    }
    void KickMultiSelectTabBarButton::incrementSelection()
    {
        currentSelection++;
        if( currentSelection >= noOfSelections )
            currentSelection = 0;
    }
    void KickMultiSelectTabBarButton::clearSelection()
    {
        currentSelection = -1;
    }
}
