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
    ////////////////////////////////////////////////
    //   Class: KickTable
    ////////////////////////////////////////////////

    KickTable::KickTable( const juce::String& Name, KickTableDataProviders& Provider, const std::vector<TableColumn>& Columns )
        : provider( Provider ), backgroundColor( ui::Theme::backgroundColour( 1 ) )
    {
        setLookAndFeel( KickLooknFeel::getLnf() );

        addAndMakeVisible( table );
        table.setColour( juce::ListBox::backgroundColourId, ui::Theme::backgroundColour( 9 ) );
        table.setName( Name );
        table.setModel( this );
        table.setClickingTogglesRowSelection( false );
        table.setMultipleSelectionEnabled( false );

        table.getViewport()->setScrollOnDragMode( juce::Viewport::ScrollOnDragMode::all );

        setHeight( 30, 60 );

        auto& h = table.getHeader();
        lnk.reset( new tableLnK );
        h.setLookAndFeel( lnk.get() );

        int colId = 1;
        for( auto& i : Columns )
            h.addColumn( i.Title, colId++, i.Width, i.MinWidth, i.MaxWidth, juce::TableHeaderComponent::notSortable );
    }

    KickTable::~KickTable()
    {
        table.getHeader().setLookAndFeel( nullptr );
    }

    void KickTable::makeVisible( Component* Parent )
    {
        Parent->addAndMakeVisible( dynamic_cast<Component&>( *this ) );
    }

    void KickTable::refresh()
    {
        table.updateContent();
    }

    void KickTable::paint( juce::Graphics& /*g*/ ) {}

    void KickTable::resized()
    {
        table.setBoundsInset( juce::BorderSize<int>( 5 ) );
    }

    int KickTable::getNumRows()
    {
        return provider.numRows ? provider.numRows() : 0;
    }

    void KickTable::selectRow( int Row )
    {
        table.selectRow( Row );
    }

    int KickTable::getSelectedRow()
    {
        return table.getSelectedRow();
    }

    void KickTable::setHeight( const int HeaderHeight, const int RowHeight )
    {
        table.setHeaderHeight( HeaderHeight );
        table.setRowHeight( RowHeight );
    }

    void KickTable::setColumnWidth( const int ColID, const int Width )
    {
        table.getHeader().setColumnWidth( ColID, Width );
    }

    void KickTable::autoSizeColumn( const int ColID ) { table.autoSizeColumn( ColID ); }
    void KickTable::autoSizeAllColumns() { table.autoSizeAllColumns(); }

    void KickTable::setPropertyFlag( juce::TableHeaderComponent::ColumnPropertyFlags Flag )
    {
        propertyFlag = Flag;
    }

    void KickTable::addColumn( const TableColumn& Column )
    {
        table.getHeader().addColumn( Column.Title, Column.ColID, Column.Width, Column.MinWidth, Column.MaxWidth, propertyFlag );
    }

    void KickTable::setRowBackgroundColour( const juce::Colour& Col )
    {
        backgroundColor = Col;
    }

    juce::Component* KickTable::getCellComponent( int Row, int Column )
    {
        return table.getCellComponent( Column, Row );
    }

    void KickTable::paintRowBackground( juce::Graphics& g, int /*Row*/, int /*Width*/, int /*Height*/, bool IsRowSelected )
    {
        const auto c = IsRowSelected ? ui::Theme::backgroundColour( 6 ).withAlpha( .2f ) : backgroundColor;

        if( juce::Colours::transparentBlack != c )
        {
            auto       b = getLocalBounds().reduced( 4 );
            juce::Path p;
            p.addRoundedRectangle( b, 5.f, 5.f );
            g.setColour( c );
            g.fillPath( p );
        }
    }

    void KickTable::paintCell( juce::Graphics& g, int Row, int ColID, int Width, int Height, bool /*IsRowSelected*/ )
    {
        const auto cellText   = provider.cellContent ? provider.cellContent( Row, ColID ) : "";
        const auto cellColour = ColID == 1 ? ui::Theme::textColour( 2 ) : ui::Theme::textColour( 1 );

        g.setColour( cellColour );
        g.setFont( juce::Font( juce::jmin( 26.0f, ( 0.4f * Height ) ) ) );

        g.drawFittedText( cellText, x, y, Width, Height, cellAlign, 1, 0.9f );
    }

    juce::Component* KickTable::refreshComponentForCell( int rowNumber, int columnId, bool IsRowSelected, Component* existingComponentToUpdate )
    {
        if( provider.cellComponent )
            return provider.cellComponent( rowNumber, columnId, IsRowSelected, existingComponentToUpdate );

        return nullptr;
    }

    void KickTable::tableLnK::drawTableHeaderBackground( juce::Graphics& /*g*/, juce::TableHeaderComponent& /*hdr*/ )
    {
    }

    void KickTable::tableLnK::drawTableHeaderColumn( juce::Graphics&     g, juce::TableHeaderComponent& /*hdr*/,
                                                     const juce::String& columnName, int /*columnId*/, int width, int height,
                                                     bool /*isMouseOver*/, bool /*isMouseDown*/, int /*columnFlags*/ )
    {
        g.setColour( ui::Theme::textColour( 2 ) );
        g.setFont( juce::Font( juce::jmin( 26.0f, ( height * .308f ) ) ) );
        g.drawFittedText( columnName, 0, 0, width, height, juce::Justification::centred, 1, 0.9f );
    }

    void KickTable::selectedRowsChanged( int lastRowSelected )
    {
        if( onRowSelected )
            onRowSelected( lastRowSelected );
    }

    void KickTable::cellClicked( int rowNumber, int columnId, const juce::MouseEvent& )
    {
        if( onRowClicked )
            onRowClicked( rowNumber );
        if( onCellClicked )
            onCellClicked( rowNumber, columnId );
    }

    void KickTable::cellDoubleClicked( int rowNumber, int columnId, const juce::MouseEvent& )
    {
        if( onRowDoubleClicked )
            onRowDoubleClicked( rowNumber );
        if( onCellDoubleClicked )
            onCellDoubleClicked( rowNumber, columnId );
    }

    void KickTable::setHeader( std::unique_ptr<juce::TableHeaderComponent> newHeader )
    {
        table.setHeader( std::move( newHeader ) );
    }

    void KickTable::setHeaderHeight( int newHeight )
    {
        table.setHeaderHeight( newHeight );
    }

    void KickTable::setRowHeight( int newHeight )
    {
        table.setRowHeight( newHeight );
    }

    int KickTable::getHeaderHeight() const noexcept
    {
        return table.getHeaderHeight();
    }

    int KickTable::getRowHeight() const noexcept
    {
        return table.getRowHeight();
    }

    void KickTable::setJustification( juce::Justification CellAlign )
    {
        cellAlign = CellAlign;
    }

    void KickTable::setXY( int X, int Y )
    {
        x = X;
        y = Y;
    }

    //////////////////////
    /// KICK Table View
    //////////////////////
    KickTableView::KickTableView( const juce::String& Name, KickTableViewProvider& Provider, juce::Colour BackgroundColor )
        : mt::MTProperties( this )
        , provider( Provider )
    {
        setName( Name );
        viewPort.setViewedComponent( &canvas );
        addAndMakeVisible( viewPort );
        canvas.addMouseListener( this, true );
        canvas.backgroundColor = BackgroundColor;
        setHasFocusOutline( false );
    }

    void KickTableView::mouseDown( const juce::MouseEvent& event )
    {
        // The touch in the canvas's own coordinates (the canvas and its rows report it through a
        // mouse listener). Same as display pixels minus the canvas's display position on the
        // unit, and also right when the screen is drawn scaled or transformed.
        auto Y   = event.getEventRelativeTo( &canvas ).getPosition().getY();
        auto row = canvas.getRowFromY( Y );
        if( enableRowSelectionFlag )
            selectRow( row < getNumRows() ? row : getSelectedRow() );
    }

    std::vector<int> KickTableView::getRowNumbers( int rowCount, int state, int startX, int width, int height )
    {
        int              rowHeight      = canvas.rowHeight;
        int              rowWidth       = canvas.rowWidth;
        // The Q-Link row maths below works in 1024x600 screen pixels. On the unit the screen is the
        // whole display, so this is the canvas's display position; measure it from the top-level
        // component (or a transformed ancestor that draws the screen scaled) so it stays right
        // when the window is elsewhere on a larger display.
        juce::Component* root = &canvas;
        while( root->getParentComponent() != nullptr && !root->isTransformed() )
            root = root->getParentComponent();
        auto             viewPos        = root->getLocalPoint( &canvas, juce::Point<int>() );
        auto             startingHeight = height;
        auto             startingWidth  = width;
        std::vector<int> rows;

        for( auto i = 1; i <= rowCount; i++ )
        {
            auto RowY     = startingHeight + ( rowHeight * i ) - viewPos.getY();
            auto RowX     = startX;
            int  trackNum = canvas.getRowFromY( RowY );
            if( i == 1 )
                canvas.num = trackNum;
            if( state == i && contracts::DclContractsHelper::isShiftPressed() )
            {
                showColumnBorder( false, RowX, startingWidth );
                canvas.rowX = RowX;
            }
            else if( state == i )
            {
                showKnobBorder( true, RowY );
                canvas.trackNum = trackNum;
            }

            DBG( "Track number for i ---> " << i << " track  " << trackNum << " ROW Y : " << RowY << "ViewPos : " << viewPos.getY() );
            rows.push_back( trackNum );
        }

        return rows;
    }

    void KickTableView::mouseDoubleClick( const juce::MouseEvent& event )
    {
        auto Y   = event.getEventRelativeTo( &canvas ).getPosition().getY(); // as in mouseDown
        auto row = canvas.getRowFromY( Y );
        if( rowComponents.size() > 0 )
            if( onRowDoubleClicked && juce::isPositiveAndBelow( row, rowComponents.size() ) )
                onRowDoubleClicked( row );
    }

    void KickTableView::knobRotated( double value )
    {
        if( enableRowSelectionFlag )
        {
            selectRow( getSelectedRow() + value );
            setViewPosition();
        }
    };

    void KickTableView::knobClicked()
    {
        if( onTableKnobClicked )
            onTableKnobClicked( getSelectedRow() );
    };

    void KickTableView::sliderMoved( float value )
    {
        int  totalHeight = canvas.getHeight();
        auto yPos        = totalHeight * value;
        viewPort.setViewPosition( viewPort.getViewPositionX(), yPos );
    }

    void KickTableView::setViewPosition()
    {
        auto row = getSelectedRow();
        if( row == -1 )
            return;
        if( ( rowComponents.size() > 0 ) )
        {
            const auto rb = rowComponents[getSelectedRow()]->getBounds();
            const auto va = viewPort.getViewArea();
            if( !va.contains( rb ) )
            {
                auto yPos = va.getY() > rb.getY() ? rb.getY() : rb.getY() + rb.getHeight() - viewPort.getHeight();

                viewPort.setViewPosition( va.getX(), yPos );
            }
        }
    }

    void KickTableView::refresh()
    {
        auto bounds = getLocalBounds();
        if( bounds.isEmpty() )
            return;

        hdrComponents.ensureStorageAllocated( 1 );
        if( bShowHeader )
        {
            if( provider.hdrComponent )
            {
                hdrComponents.set( 0, provider.hdrComponent( hdrComponents[0] ) );
                if( hdrComponents[0] )
                {
                    hdrComponents[0]->setInterceptsMouseClicks( false, false );
                }
            }
        }

        // Resize to hold components...
        auto num = getNumRows();
        rowComponents.ensureStorageAllocated( num );

        // Get the components...
        for( auto i = 0; i < num; i++ )
            if( provider.cellComponent )
                rowComponents.set( i, provider.cellComponent( i, rowComponents[i] ) );

        // Check if selection is out of bounds.
        if( canvas.selectedRow.size() && ( 1 < num ) )
        {
            for( auto& i : canvas.selectedRow )
                i = juce::jlimit( 0, num - 1, i );
        }

        // Place components...
        canvas.removeAllChildren();
        if( bShowHeader && hdrComponents[0] )
        {
            addAndMakeVisible( hdrComponents[0] );
            hdrComponents[0]->setBounds( hdrBounds );
        }

        for( auto i = 0; i < num; i++ )
        {
            if( auto comp = rowComponents[i] )
            {
                canvas.addAndMakeVisible( comp );
                comp->setBounds( canvas.getRowBounds( i ) );
            }
        }
        if( bEndLabelEnabled )
        {
            canvas.addAndMakeVisible( endLabelInfo );
            endLabelInfo.setBounds( canvas.getRowBounds( num ) );
            num += 1;
        }
        endLabelInfo.setVisible( bEndLabelEnabled );
        canvas.setSize( canvas.rowWidth, canvas.getRowY( num ) );
    }

    void KickTableView::visibilityChanged()
    {
        auto visible = isVisible();

        for( auto& it : getChildren() )
            it->setVisible( visible );

        for( auto& it : canvas.getChildren() )
            it->setVisible( visible );

        if( visible )
            refresh();
    }

    int KickTableView::getNumRows()
    {
        canvas.numRows = provider.numRows ? provider.numRows() : 0;
        return canvas.numRows;
    }

    void KickTableView::clearSelection()
    {
        canvas.selectedRow.clear();
    }

    void KickTableView::selectRow( int Row, juce::NotificationType notify )
    {
        if( ( rowComponents.size() > 0 ) )
        {
            auto prevSel     = canvas.selectedRow.getLast();
            auto selectedRow = juce::jlimit( 0, ( rowComponents.size() - 1 ), Row );
            if( rowComponents[selectedRow]->isEnabled() )
            {
                if( !bEnableSelectMode )
                {
                    canvas.selectedRow.clear();
                    canvas.selectedRow.addIfNotAlreadyThere( selectedRow );
                }
                else
                {
                    if( canvas.selectedRow.contains( Row ) )
                        canvas.selectedRow.removeAllInstancesOf( Row );
                    else
                        canvas.selectedRow.addIfNotAlreadyThere( selectedRow );
                }

                if( isAutoScrollToSelection() )
                    setViewPosition();
                if( notify == juce::sendNotification )
                {
                    if( provider.onRowSelected && juce::isPositiveAndBelow( selectedRow, rowComponents.size() ) )
                        provider.onRowSelected( selectedRow );
                }

                repaint();
            }

            if( prevSel != selectedRow && enableFocus )
                setFocus();
        }
    }

    void KickTableView::forEachRowComponent( std::function<void( juce::Component* )> Func )
    {
        auto num = getNumRows();
        if( Func )
        {
            for( int i = 0; i < num; ++i )
            {
                Func( rowComponents[i] );
            }
        }
    }

    void KickTableView::setRowHeight( const int RowHeight )
    {
        canvas.rowHeight = RowHeight;
        resized();
    }

    void KickTableView::setRowSpacing( const int RowSpacing )
    {
        canvas.rowSpacing = RowSpacing;
        resized();
    }

    void KickTableView::setMargin( const juce::Rectangle<int> Margin ) noexcept
    {
        canvas.margin = Margin;
        resized();
    }

    void KickTableView::setMargin( const int left, const int right, const int top, const int bottom ) noexcept
    {
        canvas.margin.setX( left );
        canvas.margin.setY( top );
        canvas.margin.setWidth( right );
        canvas.margin.setHeight( bottom );
        resized();
    }

    void KickTableView::setHeaderHeight( const int HeaderHeight )
    {
        hdrHeight = HeaderHeight;
        resized();
    }

    void KickTableView::showHeader( bool Show ) noexcept
    {
        bShowHeader = Show;
        resized();
    };

    void KickTableView::setBackgroundColour( const juce::Colour& Col )
    {
        canvas.backgroundColor = Col;
        repaint();
    }

    void KickTableView::setRowBackgroundColour( const juce::Colour& Col )
    {
        canvas.rowBGColor = Col;
        repaint();
    }

    void KickTableView::setSelectedRowBackgroundColour( const juce::Colour& Col )
    {
        canvas.selRowBGColor = Col;
        repaint();
    }

    void KickTableView::setRowOutlineColour( const juce::Colour& Col )
    {
        canvas.outlineColor = Col;
        repaint();
    }

    void KickTableView::setSelectedRowOutlineColour( const juce::Colour& Col )
    {
        canvas.selOutlineColor = Col;
        repaint();
    }

    void KickTableView::showKnobBorder( bool highlight, int pos )
    {
        canvas.knobHighlight = highlight;
        canvas.pos           = pos;
        repaint();
    }
    void KickTableView::showColumnBorder( bool highlight, int pos, int width )
    {
        canvas.knobHighlight = highlight;
        canvas.pos           = pos;
        canvas.width         = width;
        repaint();
    }

    void KickTableView::enableScrollBar( bool enable )
    {
        viewPort.setScrollBarsShown( enable, false );
    }

    void KickTableView::enableSelectMode( bool enable )
    {
        bEnableSelectMode = enable;
    }

    void KickTableView::setTableViewPosition( int yPosition )
    {
        auto viewPosition = viewPort.getViewPosition();
        viewPort.setViewPosition( viewPosition.getX(), yPosition );
    }

    juce::Viewport& KickTableView::getTableViewPort()
    {
        return viewPort;
    }

    juce::Rectangle<int> KickTableView::getSelectedRowBounds()
    {
        return canvas.getRowBounds( getSelectedRow() );
    }

    juce::Rectangle<int> KickTableView::getRowBounds( int Row )
    {
        return canvas.getRowBounds( Row );
    }

    bool KickTableView::getSelectMode()
    {
        return bEnableSelectMode;
    }

    int KickTableView::getScrollBarThickness() const
    {
        return viewPort.getScrollBarThickness();
    }

    void KickTableView::setScrollBarThickness( const int thickness )
    {
        viewPort.setScrollBarThickness( thickness );
    }

    void KickTableView::resized()
    {
        auto b = getLocalBounds();

        if( b.isEmpty() )
            return;

        b.removeFromLeft( canvas.margin.getX() );
        b.removeFromTop( canvas.margin.getY() );
        b.removeFromRight( canvas.margin.getWidth() );
        b.removeFromBottom( canvas.margin.getHeight() );

        canvas.rowWidth = b.getWidth() - viewPort.getScrollBarThickness() - 2;

        if( bShowHeader )
            hdrBounds = b.removeFromTop( hdrHeight );

        viewPort.setBounds( b );
    }

    void KickTableView::paint( juce::Graphics& g )
    {
        g.fillAll( canvas.backgroundColor );
    }

    void KickTableView::KickComponent::paint( juce::Graphics& g )
    {
        juce::Rectangle<int> rowBounds;
        juce::Rectangle<int> knobBounds;
        for( auto i = 0; i < numRows; ++i )
        {
            rowBounds = getRowBounds( i ).reduced( 1 );

            auto isSelected = selectedRow.contains( i );
            mt::utils::fillRoundedRect( g, rowBounds, ( isSelected ? selRowBGColor : rowBGColor ) );
            if( isSelected )
                rowBounds = rowBounds.expanded( 1 );
            mt::utils::renderRoundedRect( g, rowBounds, ( isSelected ? selOutlineColor : outlineColor ), 2 );
        }
        knobBounds = getRowBounds( trackNum ).reduced( 1 );
        if( knobHighlight )
            mt::utils::renderRoundedRect( g, knobBounds, ui::Theme::backgroundColour( 6 ), 2 );
        else
        {
            juce::Rectangle<int> bounds{ rowX, getRowBounds( num ).getY(), width, 200 };
            mt::utils::renderRoundedRect( g, bounds, ui::Theme::backgroundColour( 6 ), 2 );
        }
    }

    int KickTableView::KickComponent::getRowX( const int RowNo )
    {
        return margin.getX();
    }

    int KickTableView::KickComponent::getRowY( const int RowNo )
    {
        return margin.getY() + ( RowNo * ( rowHeight + rowSpacing ) );
    }

    int KickTableView::KickComponent::getRowFromY( const int Y )
    {
        auto row = juce::jmax( 0, ( Y - margin.getY() ) / ( rowHeight + rowSpacing ) );
        return row;
    }

    juce::Rectangle<int> KickTableView::KickComponent::getRowBounds( int Row )
    {
        return { getRowX( Row ), getRowY( Row ), rowWidth, rowHeight };
    }

    void KickTableView::setTableEndInfo( const juce::String& Info, juce::Colour ValueColour, float Font, bool bEnable )
    {
        bEndLabelEnabled = bEnable;
        if( bEndLabelEnabled )
        {
            endLabelInfo.setText( Info, juce::dontSendNotification );
            endLabelInfo.setValueJustification( juce::Justification::centred );
            endLabelInfo.setValueColour( ValueColour );
            endLabelInfo.setValueFontHeight( Font );
        }
    }
}
