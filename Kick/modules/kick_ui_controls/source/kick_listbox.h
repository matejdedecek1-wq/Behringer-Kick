#pragma once
/*
 * Copyright (c) 2021 MUSIC Tribe
 *
 * This file is part of Kick.
 *
 * Kick is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Affero General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * Kick is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with Kick. If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

namespace ui
{
    class KickList
        : public KickControlBase<KickViewport>
        , public mt::MTProperties
    {
    public:
        std::function<void( int Index, const juce::String& Option )> onSelectionChanged = []( int, const juce::String& ) {};

        explicit KickList( const juce::String& Title, const std::vector<juce::String>& Options = {}, int Selected = 0, int NumColumns = 1 );
        ~KickList();

        void       setContent( const std::vector<juce::String>& Data, int Selected = 0, juce::NotificationType Notification = juce::dontSendNotification );
        inline int getSize() const { return (int)options.size(); }

        int getColumns() const { return noOfColumns; }
        int getSelectedIndex() const { return selection; }
        int getNumOptions() { return numOptions; }

        const juce::String&       getSelected();
        std::vector<juce::String> getOptions();

        void setColumns( int Cols );
        void setSelected( int O, juce::NotificationType notification );
        void setSelected( const juce::String& O, juce::NotificationType notification );
        void setHighlight( int O, bool Highlight );
        void setBackgroundColour( juce::Colour color )
        {
            backgroundColour = color;
            repaint();
        };

        void sliderMoved( float ) override;
        void knobRotated( double ) override;
        void knobClicked() override;

        int   getIndex( const juce::String& O );
        void  setNormalized( float Value );
        float getNormalized();
        void  setSpacing( int H, int W, int S );
        void  setButtonHeight( int H );
        void  makeVisible( Component* Parent );
        void  show( const juce::String& Title = "", std::function<void( int )> CallbackOnExit = nullptr, juce::Rectangle<int> Bounds = layout::dialog );
        void  drawCurvesAndOutline( const bool curves, const bool outline );
        void  refreshViewPosition();

        void setOptionEnable( int Index, bool State );

    private:
        void paint( juce::Graphics& g ) override;
        void resized() override;
        void mouseDrag( const juce::MouseEvent& ) override { wasMouseDragging = true; }
        void mouseUp( const juce::MouseEvent& ) override { wasMouseDragging = false; }

        void addOption( const juce::String& Opt, int Index );

    private:
        std::vector<std::unique_ptr<KickToggleButton>> options;

        int  numOptions{ 0 };
        int  noOfColumns{ 1 };
        int  selection{ -1 };
        int  highlight{ -1 };
        int  rowHeight{ 60 };
        int  spaceW{ 15 };
        int  spaceH{ 10 };
        int  spaceSurround{ 20 };
        bool wasMouseDragging{ false };
        bool bDrawOutline{ false };
        bool bCurves{ false };

        std::atomic<bool> isDialogOpen{ false };
        juce::Component   listComp;
        juce::Colour      backgroundColour{ ui::Theme::backgroundColour() };

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( KickList )
    };

    class KickListWithImage
        : public KickControlBase<KickViewport>
        , public mt::MTProperties
    {
    public:
        std::function<void( int Index, const juce::String& Option )> onSelectionChanged;

        explicit KickListWithImage( const juce::String& Title, const std::vector<juce::String>& defaultOptions = {}, const std::vector<juce::String>& activeOptions = {}, int Selected = 0, int NumColumns = 1 );
        ~KickListWithImage();
        void setContent( const std::vector<juce::String>& Data1, const std::vector<juce::String>& Data2, int Selected = 0, juce::NotificationType Notification = juce::dontSendNotification );
        void addOption( const juce::String& Opt1, const juce::String& Opt2, int Index );
        void paint( juce::Graphics& g ) override;
        void resized() override;

        void sliderMoved( float ) override {}
        void knobRotated( double ) override {}
        void knobClicked() override {}
        int  getSelectedIndex() const { return selection; }
        void setSelected( int O, juce::NotificationType notification );

        inline void  setNormalized( float Value ) { setSelected( (int)( Value * numOptions ), juce::sendNotification ); }
        inline float getNormalized() { return ( (float)selection / (float)numOptions ); }

    private:
        std::vector<std::unique_ptr<KickImageButton>> options;

        juce::Component listComp;
        int             selection{ -1 };
        int             numOptions{ 0 };
        int             noOfColumns{ 1 };
        int             rowHeight{ 34 };
        juce::Colour    backgroundColour{ ui::Theme::backgroundColour( 1 ) };
    };

    class KickListBox
        : public KickControlBase<juce::Component>
        , public mt::MTProperties
    {
    public:
        enum BoxType
        {
            withoutTitleBar,
            withTitleBar,
            withoutTitleBarWithNavigationButtons,
            withTitleBarWithNavigationButtons,
        };

        std::function<void( const juce::String& )>      onValueChanged;
        std::function<void( int, const juce::String& )> onSelectionChanged;

        explicit KickListBox( const juce::String& Title = "", int NumColumns = 1 );
        explicit KickListBox( const juce::String& Title, const std::vector<juce::String>& Options, int Selected = 0, int NumColumns = 1 );
        explicit KickListBox( BoxType Type, const juce::String& Title, const std::vector<juce::String>& Options, int Selected = 0, int NumColumns = 1 );

        void setTitle( const juce::String& Title );
        void setNumColumns( int NumColumns );

        void setTitleColour( juce::Colour TitleColour ) { title.setTextColour( TitleColour ); }
        void setColours( juce::Colour ValueColour, juce::Colour TitleColour )
        {
            value.setTextColour( ValueColour );
            title.setTextColour( TitleColour );
        }
        void setBackGroundColours( juce::Colour Value, juce::Colour Title )
        {
            title.setValueBackgroundColour( Title );
            value.setValueBackgroundColour( Value );
        }
        void setListOptions( const std::vector<juce::String>& Options, int SelectedIndex = 0 );

        void setDisplayFormat( BoxType Format );
        void setJustificationType( juce::Justification newJustification );
        void setJustificationType( juce::Justification TitleJustification, juce::Justification ValueJustification );
        void setTitleFontHeight( float font ) { title.setFontHeight( font ); }
        void setValueFontHeight( float font ) { value.setFontHeight( font ); }

        juce::String        getSelectedValue( void );
        const juce::String& getSelected();
        int                 getSelectedIndex();
        int                 getNumOptions() { return list.getNumOptions(); }
        int                 getIndex( const juce::String& Option ) { return list.getIndex( Option ); }

        void setSelected( const juce::String& O, juce::NotificationType notification = juce::NotificationType::sendNotification );
        void setSelected( int O, juce::NotificationType notification = juce::NotificationType::sendNotification );

        inline void  setNormalized( float Value ) { list.setNormalized( Value ); }
        inline float getNormalized() { return list.getNormalized(); }

        std::vector<juce::String> getListOptions();
        inline int                getSize() { return list.getSize(); }

        void         makeVisible( juce::Component* Parent );
        void         knobRotated( double deltaValue ) override;
        void         knobClicked() override;
        virtual void sliderMoved( float );

        void setSpacing( int H, int W, int S ) { list.setSpacing( H, W, S ); }

        void setOptionEnable( int Index, bool State ) { list.setOptionEnable( Index, State ); }

        // Utility function to generate options with "Off" and the specified range
        static std::vector<juce::String> generateOptions( int start, int end, bool off )
        {
            std::vector<juce::String> options;
            if( off )
            {
                options.push_back( "Off" ); // Add "Off" option
            }
            for( int i = start; i <= end; ++i )
            {
                options.push_back( juce::String( i ) );
            }

            return options;
        }

    private:
        BoxType    type;
        KickList   list;
        KickLabel  title{ "" }, value{ "" };
        KickButton plus{ "+" }, minus{ "-" };

        juce::Time lastClickTime{ 0 };
        float      doubleClickTimeoutMs{ 400 };

        void mouseDoubleClick( const juce::MouseEvent& e );
        void mouseDown( const juce::MouseEvent& e );
        void resized() override;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( KickListBox )
    };
}
