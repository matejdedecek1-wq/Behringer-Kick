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
namespace mt
{
    namespace ac
    {
        class SeCuePositioner
        {
        public:
            SeCuePositioner( SampleEditorData& d );
            ~SeCuePositioner();
            void                   setViewArea( juce::Rectangle<int> area ) { totalArea = area; }
            void                   paint( juce::Graphics& g );
            juce::Rectangle<float> getCuePositioningRect();
            bool                   mouseMove( const juce::MouseEvent& event );
            void                   mouseDrag( const juce::MouseEvent& event );
            void                   mouseUp( const juce::MouseEvent& event );
            bool                   hasMouseFocus() { return mouseFocus; }
            void                   setHidden( bool newHideState ) { hide = newHideState; }

        private:
            SampleEditorData&    seData;
            Region               selectedRegion;
            juce::int64          cuePos     = 0;
            bool                 mouseFocus = false;
            juce::Rectangle<int> totalArea;
            bool                 hide           = false;
            bool                 mouseDragStart = true;
        };

        class SeLoopPositioner
        {
        public:
            SeLoopPositioner( SampleEditorData& d );
            ~SeLoopPositioner();
            void                   setViewArea( juce::Rectangle<int> area ) { totalArea = area; }
            void                   paint( juce::Graphics& g );
            juce::Rectangle<float> getCuePositioningRect();
            bool                   mouseMove( const juce::MouseEvent& event );
            void                   mouseDrag( const juce::MouseEvent& event );
            void                   mouseUp( const juce::MouseEvent& event );
            bool                   hasMouseFocus() { return mouseFocus; }
            void                   setSelectedRegion( const Region& region ) { selectedRegion = region; }
            void                   setHidden( bool newHideState ) { hide = newHideState; }

        private:
            SampleEditorData&    seData;
            Region               selectedRegion;
            juce::int64          loopPos;
            bool                 mouseFocus = false;
            juce::Rectangle<int> totalArea;
            bool                 hide = false;
        };
    }
}
