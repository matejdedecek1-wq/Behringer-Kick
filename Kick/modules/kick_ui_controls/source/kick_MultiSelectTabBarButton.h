#pragma once
namespace ui
{
    class KickMultiSelectTabBarButton : public juce::TabBarButton
        , public mt::MTProperties
    {
    public:
        explicit KickMultiSelectTabBarButton( int numSel, juce::TabbedButtonBar& ownerBar, const juce::String& buttonName = "" );
        int  getNumOfSelection();
        int  getSelectionSpacing();
        int  getSelectionSize();
        int  getCurrentSelection();
        void setCurrentSelection( int selection );
        void setCurrentSelection( juce::String name );

        void incrementSelection();
        void clearSelection();

    private:
        int          noOfSelections   = 0;
        int          currentSelection = -1;
        juce::String selectedState;
    };
}
