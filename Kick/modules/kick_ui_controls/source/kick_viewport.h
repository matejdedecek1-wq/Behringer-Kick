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
    class KickViewport : public juce::Viewport
    {
    public:
        std::function<void()> onVisibilityChanged;
        std::function<void()> onResized;

        KickViewport();

        class Listener
        {
        public:
            virtual ~Listener() = default;

            /** Called when a clicked on the this track. */
            virtual void updateOnViewPositionChange( KickViewport* pViewport ) = 0;
        };

        void addListener( Listener* listener );
        void removeListener( Listener* listener );

        void visibleAreaChanged( const juce::Rectangle<int>& newVisibleArea ) override;

        void knobRotated( int knob, double deltaValue );

        virtual void visibilityChanged()
        {
            if( onVisibilityChanged ) onVisibilityChanged();
        }
        virtual void resized()
        {
            juce::Viewport::resized();
            if( onResized ) onResized();
        }

    private:
        juce::ListenerList<Listener> listeners;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( KickViewport )
    };

}
