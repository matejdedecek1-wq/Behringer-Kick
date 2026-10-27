
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
    enum class EnvelopDecayMode
    {
        decayModeEnd,
        decayModeStart
    };

    class KickEnvelopViewBase : public juce::Component
    {
    public:
        enum class MouseDragPoint
        {
            dragPointNone,
            dragPointA,
            dragPointH,
            dragPointD,
            dragPointS,
        };

        KickEnvelopViewBase( const std::vector<int>& values, int noPoints );
        virtual ~KickEnvelopViewBase();
        virtual std::vector<int> getAHDSValues() = 0;
        void                     paint( juce::Graphics& g );
        void                     updateControls();
        void                     resized() override;

    private:
        void                 drawScale( juce::Graphics& g, float xVal );
        juce::Rectangle<int> getControlsRect();

    protected:
        juce::Path   getLogPath( float x1, float x2, float y1, float y2, bool reverse );
        int          evalueatePointValue( float p1, float p2, int totalPoints, bool yAxis = false );
        float        convertValueToPoint( int value, int totalPoints, bool yAxis = false );
        virtual void updateValues() = 0;

    protected:
        float                                      xStart = 0.0f;
        float                                      yStart = 0.0f;
        float                                      xEnd   = 0.0f;
        float                                      yEnd   = 0.0f;
        std::vector<int>                           values;
        int                                        points = 0;
        std::list<std::unique_ptr<ui::KickNumber>> controls;
    };

    class KickAHDSView : public KickEnvelopViewBase
    {
    public:
        KickAHDSView( const std::vector<int>& initValues );
        void             paint( juce::Graphics& g ) override;
        void             resized() override;
        void             mouseDrag( const juce::MouseEvent& event ) override;
        void             mouseUp( const juce::MouseEvent& event ) override;
        juce::Path       getAPath();
        juce::Path       getHPath();
        std::vector<int> getAHDSValues() override;

    private:
        void setPointToMaxIfExceeds( float& mouseX, float& mouseY );
        void updateValues() override;
        bool checkDragCondition( MouseDragPoint dragPoint, float mouseX, float mouseY );

    private:
        juce::Rectangle<float> getARect();
        juce::Rectangle<float> getHRect();
        juce::Rectangle<float> getDRect();
        juce::Rectangle<float> getSRect();

    private:
        MouseDragPoint mouseDragpoint = MouseDragPoint::dragPointNone;
        float          attack         = 0.0f;
        float          hold           = 0.0f;
        float          decay          = 0.0f;
        float          sustain        = 0.0f;

        float controlPointDistance = 0.0f;
    };

    class KickADView : public KickEnvelopViewBase
    {
    public:
        KickADView( const std::vector<int>& initValues );
        ~KickADView();
        void             setDecayMode( EnvelopDecayMode decayMode ) { envDecaypMode = decayMode; }
        void             resized() override;
        void             paint( juce::Graphics& g ) override;
        void             mouseDrag( const juce::MouseEvent& event ) override;
        void             mouseUp( const juce::MouseEvent& event ) override;
        std::vector<int> getAHDSValues() override;
        void             setPointToMaxIfExceeds( float& mouseX );

    private:
        void                   updateValues() override;
        bool                   checkDragCondition( MouseDragPoint dragPoint, float mouseX );
        juce::Rectangle<float> getARect();
        juce::Rectangle<float> getDRect();
        juce::Path             getAPath();
        juce::Path             getDPath();

    private:
        EnvelopDecayMode envDecaypMode  = EnvelopDecayMode::decayModeEnd;
        MouseDragPoint   mouseDragpoint = MouseDragPoint::dragPointNone;
        float            attack         = 0.0f;
        float            decay          = 0.0f;
    };
}
