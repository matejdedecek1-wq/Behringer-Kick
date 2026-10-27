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
        class RegionSelector
        {
        public:
            RegionSelector() = delete;
            RegionSelector( const Region& p, SampleEditorData& seData, bool end = false );
            void                  paint( juce::Graphics& g );
            bool                  mouseDrag( const juce::MouseEvent& event );
            bool                  mouseMove( const juce::MouseEvent& event );
            void                  mouseUp( const juce::MouseEvent& event );
            void                  setArea( juce::Rectangle<int> area ) { totalArea = area; }
            void                  setEnabled( bool enable ) { enabled = enable; }
            bool                  hasMouseFocus() { return mouseFocus; }
            const Region&         getRegion() { return pos; }
            SampleEditorData&     getdata() { return seData; }
            juce::Rectangle<int>& getTotalArea() { return totalArea; }

        private:
            juce::Rectangle<float> getResizeRect();

        private:
            const Region&        pos;
            SampleEditorData&    seData;
            juce::Rectangle<int> totalArea;
            bool                 enabled        = true;
            bool                 end            = false;
            bool                 mouseFocus     = false;
            bool                 mouseDragStart = true;
        };

        class RegionComponent
        {
        public:
            RegionComponent() = delete;
            ~RegionComponent();

            static RegionComponent* createRegion( const Region& region, SampleEditorData& data, juce::Rectangle<int> area );

            const Region& getRegion() { return start->getRegion(); }
            void          update();
            void          updateArea( juce::Rectangle<int>& area );
            bool          hasRegion( const Region& region );
            bool          mouseDrag( const juce::MouseEvent& event );
            bool          mouseMove( const juce::MouseEvent& event );
            void          mouseUp( const juce::MouseEvent& event );
            bool          select( juce::int64 time );
            void          paint( juce::Graphics& g, bool selection = false );

        private:
            RegionComponent( RegionSelector* start, RegionSelector* end );
            RegionSelector* start = nullptr;
            RegionSelector* end   = nullptr;
        };

        class SEViewComponent : public juce::Component
            , public juce::Timer
        {
        public:
            SEViewComponent( SampleEditorData& seData, ui::KickViewport* parent );
            ~SEViewComponent();
            void                        paint( juce::Graphics& g ) override;
            void                        mouseMove( const juce::MouseEvent& event ) override;
            void                        mouseDrag( const juce::MouseEvent& event ) override;
            void                        mouseUp( const juce::MouseEvent& event ) override;
            void                        mouseDown( const juce::MouseEvent& event ) override;
            void                        resized() override;
            void                        timerCallback() override;
            void                        mouseWheelMove( const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel );
            SIGNALS : sigslot::signal<> sizeChanged;
            void                        setViewArea( float viewPoint );
            void                        resetZoomFactor();

        private:
            bool isSplitManual( juce::Point<int> point );
            void update();
            void updateArea();
            bool setNextScrollPosition( float posinPix );

        private:
            SampleEditorData&           seData;
            std::list<RegionComponent*> regions;
            RegionComponent*            resizeRegion   = nullptr;
            RegionComponent*            selectedRegion = nullptr;
            SeCuePositioner             cue{ seData };
            SeLoopPositioner            loopPositioner{ seData };
            float                       zoomAt1     = 0.0;
            float                       zoomLevel   = 1;
            juce::int64                 playHeadPos = 0;
            bool                        isReset{ false };

            ui::KickViewport* parent = nullptr;
            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( SEViewComponent )
        };
    }
}
