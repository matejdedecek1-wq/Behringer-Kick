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
    KickGrid::KickGrid( unsigned int NumRows, unsigned int NumColumns, bool hasRowTitle )
        : mt::MTProperties( this )
        , isRowTitle( hasRowTitle )
    {
        setNumRows( NumRows );
        setNumColumns( NumColumns );
        setGap( 4, 4 );
        setBackgroundColour( ui::Theme::backgroundColour( 1 ) );
    }

    void KickGrid::setRowGap( int SpaceInPixel )
    {
        grid.rowGap = juce::Grid::Px( SpaceInPixel );
        resized();
    }

    void KickGrid::setColumnGap( int SpaceInPixel )
    {
        grid.columnGap = juce::Grid::Px( SpaceInPixel );
        resized();
    }

    void KickGrid::setGap( int RowSpaceInPixel, int ColSpaceInPixel )
    {
        setRowGap( RowSpaceInPixel );
        setColumnGap( ColSpaceInPixel );
    }

    void KickGrid::setNumRows( unsigned int NumRows )
    {
        nRows = NumRows;
        rowHeights.resize( nRows, -1 );
        rowTitles.resize( nRows, "" );
        updateRows();
    }

    void KickGrid::setNumColumns( unsigned int NumColumns )
    {
        nColumns = NumColumns;
        if( isRowTitle )
            ++nColumns;

        columnWidth.resize( nColumns, -1 );
        updateColumns();
    }

    void KickGrid::setColumnWidth( const std::vector<int>& clmWidth )
    {
        columnWidth = clmWidth;
        updateColumns();
    }

    void KickGrid::setRowHeights( const std::vector<int>& RowHeights )
    {
        rowHeights = RowHeights;
        updateRows();
    }

    void KickGrid::updateColumns()
    {
        auto cSize = columnWidth.size();

        using Fr = juce::Grid::Fr;
        using Px = juce::Grid::Px;

        grid.templateColumns.clear();
        for( auto i = 0U; i < nColumns; ++i )
        {
            if( ( i < cSize ) && ( 0 < columnWidth[i] ) )
                grid.templateColumns.add( Px( columnWidth[i] ) );
            else
                grid.templateColumns.add( Fr( 1 ) );
        }
        resized();
    }

    void KickGrid::updateRows()
    {
        auto rSize = rowHeights.size();

        using Fr = juce::Grid::Fr;
        using Px = juce::Grid::Px;

        grid.templateRows.clear();
        for( auto i = 0U; i < nRows; ++i )
        {
            if( ( i < rSize ) && ( 0 < rowHeights[i] ) )
                grid.templateRows.add( Px( rowHeights[i] ) );
            else
                grid.templateRows.add( Fr( 1 ) );
        }
        resized();
    }

    void KickGrid::setRowTitles( const std::vector<juce::String>& newTitles )
    {
        auto minSize = std::min( newTitles.size(), rowTitles.size() );

        std::fill( rowTitles.begin(), rowTitles.end(), "" );
        for( auto i = 0; i < minSize; i++ )
        {
            rowTitles[i] = newTitles[i];
        }
    }

    void KickGrid::setRowBackgroundColour( juce::Colour Colour )
    {
        setBackgroundColour( Colour );
    }

    void KickGrid::paint( juce::Graphics& g )
    {
        auto backgroundColour = getBackgroundColour();
        if( !isRowTitle && backgroundColour.isTransparent() )
            return;

        auto b       = getLocalBounds();
        auto rHeight = ( b.getHeight() - ( ( nRows - 1 ) * grid.rowGap.pixels ) ) / nRows;
        auto cWidth  = b.getWidth() / ( isRowTitle ? nColumns + 1 : nColumns );

        for( auto row = 0U; row < nRows; ++row )
        {
            auto bRow = b.removeFromTop( rHeight );
            b.removeFromTop( grid.rowGap.pixels );

            // Row background colour...
            if( !backgroundColour.isTransparent() )
                mt::utils::fillRoundedRect( g, bRow, backgroundColour );

            // Draw Title if required.
            if( isRowTitle )
            {
                auto bTitle = bRow.removeFromLeft( cWidth ).reduced( 20, 0 );
                mt::utils::renderText( g, rowTitles[row], bTitle, ui::Theme::textColour( 1 ), juce::Justification::centredLeft, 14.f );
            }
        }
    }

    void KickGrid::addComponent( juce::Component& Component, unsigned int RowStart, unsigned int ColumnStart, unsigned int HSpan, unsigned int VSpan, float TopMargin, float RightMargin, float BottomMargin, float LeftMargin )
    {
        auto rS = RowStart + 1;
        auto cS = ColumnStart + 1;
        if( isRowTitle )
            ++cS;

        // Create item span...
        auto gridItem = juce::GridItem( Component );
        gridItem.setArea( rS, cS, ( rS + VSpan ), ( cS + HSpan ) );
        gridItem.margin = juce::GridItem::Margin( TopMargin, RightMargin, BottomMargin, LeftMargin );
        grid.items.add( gridItem );
    }

    void KickGrid::removeComponent( juce::Component* component )
    {
        int i = 0;
        for( auto& it : grid.items )
        {
            if( it.associatedComponent == component )
                break;
            ++i;
        }
        if( i < grid.items.size() )
            grid.items.remove( i );
    }

    void KickGrid::resized()
    {
        auto bounds = getLocalBounds();
        if( bounds.isEmpty() )
            return;
        // Set components to show up...
        removeAllChildren();
        for( auto& it : grid.items )
            addAndMakeVisible( it.associatedComponent );
        // Magic...
        grid.performLayout( bounds );
    }

}
