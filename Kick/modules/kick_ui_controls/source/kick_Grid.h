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

    /***
    `KickGrid` is a layout component which organizes the controls inside it as rows and columns.
    Each row can have additional column on the left to display "Title" for that particular Row's; this needs to be enabled by hasRowTitle flag.
    */
    class KickGrid
        : public juce::Component
        , public mt::MTProperties
    {
    public:
        explicit KickGrid( unsigned int NumRows, unsigned int NumColumns, bool hasRowTitle = true );

        void setRowGap( int SpaceInPixel );
        void setColumnGap( int SpaceInPixel );
        void setGap( int RowSpaceInPixel, int ColSpaceInPixel );
        void setNumRows( unsigned int NumRows );
        void setNumColumns( unsigned int NumColumns );
        // While setting width for columns, use -1 to divide columns equally; and other positive value to set width (in pixels)
        void setColumnWidth( const std::vector<int>& clmWidth );
        // While setting height for rows, use -1 to divide rows equally; and other positive value to set height (in pixels)
        void setRowHeights( const std::vector<int>& RowHeights );
        void setRowTitles( const std::vector<juce::String>& newTitles );
        void setRowBackgroundColour( juce::Colour Colour );

        // Row and Column numbers start from 0.
        void addComponent( juce::Component& Component, unsigned int RowStart, unsigned int ColumnStart, unsigned int HSpan = 1, unsigned int VSpan = 1, float TopMargin = 5.f, float RightMargin = 5.f, float BottomMargin = 5.f, float LeftMargin = 5.f ); // (H/V) Span : 1, 2, 3, 4 etc...
        void removeComponent( juce::Component* component );

        void resized() override;

    private:
        void paint( juce::Graphics& g ) override;
        void updateColumns();
        void updateRows();

    private:
        juce::Grid grid;

        bool         isRowTitle{ true };
        unsigned int nRows{ 0 };
        unsigned int nColumns{ 0 };

        std::vector<int>          columnWidth;
        std::vector<int>          rowHeights;
        std::vector<juce::String> rowTitles;
    };

}
