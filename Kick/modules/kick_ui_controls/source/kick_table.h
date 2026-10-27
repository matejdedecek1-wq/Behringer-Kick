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

#pragma once

namespace ui
{
    struct KickTableDataProviders
    {
        std::function<int()>                                 numRows; // This function will be called for obtaining number of rows in table.
        std::function<juce::String( int Row, int ColumnID )> cellContent; // This function will be called back to obtaining content.

        // This function will be called back to obtaining cell custom content.
        // Note:: Either use cellContent provider or cellComponet provider.
        //        Don't use both together.
        std::function<juce::Component*( int Row, int Column, bool IsRowSelected, juce::Component* existingToUpdate )> cellComponent;
    };

    class KickTable : public juce::Component
        , public juce::TableListBoxModel
    {
    public:
        struct TableColumn
        {
            juce::String Title{ "" };
            int          ColID;
            int          Width{ 40 };
            int          MinWidth{ 30 };
            int          MaxWidth{ -1 };
        };

        std::function<void( int Row )> onRowSelected;
        std::function<void( int Row )> onRowClicked;
        std::function<void( int Row )> onRowDoubleClicked;

        std::function<void( int Row, int Column )> onCellClicked;
        std::function<void( int Row, int Column )> onCellDoubleClicked;

        explicit KickTable( const juce::String& Name, KickTableDataProviders& Provider, const std::vector<TableColumn>& Columns = {} );
        ~KickTable() override;

        void makeVisible( Component* Parent );
        void refresh();
        int  getNumRows() override;

        void selectRow( int Row );
        int  getSelectedRow();

        void setHeight( const int HeaderHeight, const int RowHeight = 60 );
        void setColumnWidth( const int ColID, const int Width );
        void autoSizeColumn( const int ColID );
        void autoSizeAllColumns();

        void addColumn( const TableColumn& Column );
        void setPropertyFlag( juce::TableHeaderComponent::ColumnPropertyFlags = juce::TableHeaderComponent::notSortable );

        void setRowBackgroundColour( const juce::Colour& Col );

        Component* getCellComponent( int Row, int Column );

        void setHeader( std::unique_ptr<juce::TableHeaderComponent> newHeader );

        void setHeaderHeight( int newHeight );
        void setRowHeight( int newHeight );
        int  getHeaderHeight() const noexcept;
        int  getRowHeight() const noexcept;
        void setJustification( juce::Justification CellAlign );
        void setXY( int X, int Y );

    private:
        juce::TableListBox      table;
        KickTableDataProviders& provider;
        juce::Colour            backgroundColor, textColour{ ui::Theme::textColour( 1 ) }, rowTextColour{ ui::Theme::textColour( 1 ) }, column2TextColour{ ui::Theme::textColour( 2 ) };
        juce::Justification     cellAlign{ juce::Justification::centred };
        bool                    rowColourChange{ false };
        int                     row{ -1 };
        int                     x{ 0 }, y{ 0 };

        juce::TableHeaderComponent::ColumnPropertyFlags propertyFlag{ juce::TableHeaderComponent::notSortable };

        void       paint( juce::Graphics& g ) override;
        void       resized() override;
        void       paintRowBackground( juce::Graphics& g, int Row, int Width, int Height, bool IsRowSelected ) override;
        void       paintCell( juce::Graphics& g, int Row, int ColID, int Width, int Height, bool IsRowSelected ) override;
        Component* refreshComponentForCell( int Row, int columnId, bool IsRowSelected, juce::Component* existingComponentToUpdate ) override;
        void       selectedRowsChanged( int lastRowSelected ) override;
        void       cellClicked( int rowNumber, int columnId, const juce::MouseEvent& ) override;
        void       cellDoubleClicked( int rowNumber, int columnId, const juce::MouseEvent& ) override;

        class tableLnK : public juce::LookAndFeel_V4
        {
            void drawTableHeaderBackground( juce::Graphics&, juce::TableHeaderComponent& ) override;
            void drawTableHeaderColumn( juce::Graphics&, juce::TableHeaderComponent&, const juce::String& columnName, int columnId, int width, int height,
                                        bool isMouseOver, bool isMouseDown, int columnFlags ) override;
        };

        std::unique_ptr<tableLnK> lnk;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( KickTable )
    };

    struct KickTableViewProvider
    {
        // This function will be called for obtaining number of rows in table.
        std::function<int()> numRows;
        // This function will be called back for obtaining header component.
        std::function<juce::Component*( juce::Component* ExistingToUpdate )> hdrComponent;
        // This function will be called back to obtaining cell custom content.
        std::function<juce::Component*( int Row, juce::Component* ExistingToUpdate )> cellComponent;
        // This function will be called back on row selection.
        std::function<void( int Row )> onRowSelected;
    };

    class KickTableView
        : public KickControlBase<juce::Component>
        , public mt::MTProperties
    {
    public:
        std::function<void( int Row )> onRowClicked;
        std::function<void( int Row )> onRowDoubleClicked;
        std::function<void( int )>     onTableKnobClicked;

        explicit KickTableView( const juce::String& Name, KickTableViewProvider& Provider, juce::Colour BackgroundColor = ui::Theme::backgroundColour( 9 ) );

        void             clearRowComponents() { rowComponents.clear(); }
        void             refresh();
        int              getNumRows();
        std::vector<int> getRowNumbers( int, int, int, int, int );
        void             selectRow( int Row, juce::NotificationType notify = juce::NotificationType::sendNotification );
        void             clearSelection();

        inline bool isAutoScrollToSelection() const { return bAutoScrollEnabed; }
        void        setAutoScrollToSelection( bool Enable ) { bAutoScrollEnabed = Enable; }

        inline int getSelectedRow() const noexcept
        {
            return canvas.selectedRow.getLast();
        };

        void       setRowHeight( const int RowHeight );
        inline int getRowHeight() const noexcept { return canvas.rowHeight; };

        void       setRowSpacing( const int RowSpacing );
        inline int getRowSpacing() const noexcept { return canvas.rowSpacing; };

        void setMargin( const juce::Rectangle<int> Margin ) noexcept;
        void setMargin( const int left, const int right, const int top, const int bottom ) noexcept;

        void       setHeaderHeight( const int HeaderHeight );
        inline int getHeaderHeight() const noexcept { return hdrHeight; };
        void       showHeader( bool Show ) noexcept;

        int  getScrollBarThickness() const;
        void setScrollBarThickness( const int thickness );

        void setBackgroundColour( const juce::Colour& Col );
        void setRowBackgroundColour( const juce::Colour& Col );
        void setSelectedRowBackgroundColour( const juce::Colour& Col );
        void setRowOutlineColour( const juce::Colour& Col );
        void setSelectedRowOutlineColour( const juce::Colour& Col );
        void showKnobBorder( bool highlight, int pos );
        void showColumnBorder( bool highlight, int pos, int width );

        void forEachRowComponent( std::function<void( juce::Component* )> Func );

        inline const juce::Colour&                getBackgroundColour() const noexcept { return canvas.backgroundColor; }
        inline const juce::Colour&                getRowBackgroundColour() const noexcept { return canvas.rowBGColor; }
        inline const juce::Colour&                getSelectedRowBackgroundColour() const noexcept { return canvas.selRowBGColor; }
        inline const juce::Colour&                getRowOutlineColour() const noexcept { return canvas.outlineColor; }
        inline const juce::Colour&                getSelectedRowOutlineColour() const noexcept { return canvas.selOutlineColor; }
        inline juce::OwnedArray<juce::Component>& getRowComponents() noexcept { return rowComponents; }
        void                                      enableScrollBar( bool );
        void                                      enableSelectMode( bool );
        bool                                      getSelectMode();

        void                 setTableViewPosition( int yPosition );
        juce::Viewport&      getTableViewPort();
        juce::Rectangle<int> getSelectedRowBounds();
        juce::Rectangle<int> getRowBounds( int Row );
        // Virtual function...
        virtual void visibilityChanged();

        //Controbase overrided functions...
        void knobRotated( double ) override;

        void knobClicked() override;
        void sliderMoved( float ) override;
        void enableSetFocus( bool State ) { enableFocus = State; }
        void enableRowSelection( bool State ) { enableRowSelectionFlag = State; }
        void setTableEndInfo( const juce::String&, juce::Colour, float, bool bEnable = false );

    private:
        void resized() override;
        void paint( juce::Graphics& g ) override;
        void mouseDown( const juce::MouseEvent& event ) override;
        void mouseDoubleClick( const juce::MouseEvent& event ) override;

        void setViewPosition();

        KickLabel endLabelInfo{ "END" };

        bool bEndLabelEnabled{ false };
        bool bAutoScrollEnabed{ false };
        bool bEnableSelectMode{ false };
        bool bShowHeader{ false };
        int  hdrHeight{ 60 };
        bool knobHighlight{ false };
        bool enableFocus{ true };
        bool enableRowSelectionFlag{ true };

        juce::Rectangle<int> hdrBounds;

    private:
        struct KickComponent : public juce::Component
        {
            juce::Colour         outlineColor{ ui::Theme::outlineColour( 0 ) };
            juce::Colour         selOutlineColor{ ui::Theme::outlineColour( 1 ) };
            juce::Colour         backgroundColor{ ui::Theme::backgroundColour( 1 ) };
            juce::Colour         rowBGColor{ ui::Theme::backgroundColour( 10 ) };
            juce::Colour         selRowBGColor{ ui::Theme::backgroundColour( 6 ) };
            juce::Rectangle<int> margin{ 0, 0, 0, 0 };
            juce::Array<int>     selectedRow{ 0 };
            bool                 knobHighlight{ false };
            int                  pos{ -1 };
            int                  trackNum{ -1 };
            int                  rowX{ -1 };
            int                  num{ -1 };
            int                  width{ -1 };

            int rowHeight{ 60 };
            int rowWidth{ 0 };
            int rowSpacing{ 4 };
            int numRows{ 0 };

            int                  getRowX( const int Row );
            int                  getRowY( const int Row );
            int                  getRowFromY( const int Row );
            juce::Rectangle<int> getRowBounds( int Row );
            void                 paint( juce::Graphics& g ) override;
        };

        KickTableViewProvider& provider;
        KickViewport           viewPort;
        KickComponent          canvas;

        juce::OwnedArray<juce::Component> hdrComponents;
        juce::OwnedArray<juce::Component> rowComponents;
    };
}
