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
    class XYFXGrid : public juce::Component
        , public juce::Timer
    {
    public:
        std::function<void( const float, const float )> onValueChanged;
        XYFXGrid();
        ~XYFXGrid() = default;
        void  setXYCoordinates( std::vector<juce::String> );
        float getXCoordinate() { return xCoordinate; }
        float getYCoordinate() { return yCoordinate; }
        void  setLatch( bool State )
        {
            isLatch = State;
            if( prevSel >= 0 )
                selectedCells[prevSel] = isLatch ? 255 : 0;
            repaint();
        }

    private:
        void                   updateSelection( const juce::MouseEvent& event );
        void                   paint( juce::Graphics& g ) override;
        void                   resized() override;
        juce::Rectangle<float> getCellRect( int row, int column );
        void                   mouseDown( const juce::MouseEvent& event ) override;
        void                   mouseUp( const juce::MouseEvent& event ) override;
        void                   mouseDrag( const juce::MouseEvent& event ) override;
        void                   timerCallback() override;
        void                   setXYCoordinateValue( float XCoordinate, float YCoordinate );
        void                   paintCells( juce::Graphics& g );

    private:
        KickLabel        xLabel;
        KickLabel        yLabel;
        float            cellWidth{ .0f };
        float            cellHeight{ .0f };
        std::vector<int> selectedCells;
        int              currentIndex = -1, prevSel{ -1 };
        bool             isLatch{ false };
        float            xCoordinate{ 0 }, yCoordinate{ 0 }, xCoord{ 0 }, yCoord{ 0 };
        juce::String     xLeft{ "" }, xRight{ "" }, yTop{ "" }, yBottom{ "" };
    };
}
