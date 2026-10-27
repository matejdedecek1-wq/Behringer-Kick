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

namespace kick_ui
{
    class GridSelectTool : public IGridTool
    {
    public:
        GridSelectTool( GridComponent& gridComponent, GridData& gData, juce::Viewport& ViewPort );

        void paint( juce::Graphics& g ) override;

        void mouseUp( const juce::MouseEvent& ) override;
        void mouseDrag( const juce::MouseEvent& ) override;
        void mouseMove( const juce::MouseEvent& event ) override;

        bool keyPressed( const juce::KeyPress& key, juce::Component* /*originatingComponent*/ ) override;

    private:
        void drawInfoBar( int noteNum, int startTime, juce::MouseEvent event );

        float startX, endX, startY, endY = 0;

        void clearSelectRegion();
        bool isSelectRegionValid();
    };
}
