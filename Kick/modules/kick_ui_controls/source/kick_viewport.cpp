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
    //////////////////////
    /// KICK ViewPort
    //////////////////////

    KickViewport::KickViewport()
    {
        setScrollOnDragMode( ScrollOnDragMode::all );
        setScrollBarsShown( true, false );
    }

    void KickViewport::addListener( Listener* listener )
    {
        listeners.add( listener );
    }

    void KickViewport::removeListener( Listener* listener )
    {
        listeners.remove( listener );
    }
    void KickViewport::visibleAreaChanged( const juce::Rectangle<int>& /*newVisibleArea*/ )
    {
        Component::BailOutChecker checker( this );

        if( checker.shouldBailOut() )
            return;

        listeners.callChecked( checker, [this]( Listener& l ) { l.updateOnViewPositionChange( this ); } );
    }

    void KickViewport::knobRotated( int knob, double deltaValue )
    {
        switch( knob )
        {
            case 1:
            {
                auto&  vScrollBar = getVerticalScrollBar();
                double newValue   = vScrollBar.getCurrentRangeStart() + deltaValue;
                newValue          = std::clamp( newValue, 0.0, vScrollBar.getMaximumRangeLimit() - vScrollBar.getCurrentRangeSize() );
                vScrollBar.setCurrentRangeStart( newValue );
                break;
            }
            case 2:
            {
                auto&  hScrollBar = getHorizontalScrollBar();
                double newValue   = hScrollBar.getCurrentRangeStart() + deltaValue;
                newValue          = std::clamp( newValue, 0.0, hScrollBar.getMaximumRangeLimit() - hScrollBar.getCurrentRangeSize() );
                hScrollBar.setCurrentRangeStart( newValue );
                break;
            }
        }
    }

}
