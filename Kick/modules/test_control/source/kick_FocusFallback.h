/*
 * Copyright (c) 2024 MUSIC Tribe
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

namespace testcontrol
{
    /// Focus for scripted touches.
    ///
    /// A touched control receives the jog wheel, + / -, PUSH and the slider through JUCE keyboard
    /// focus (ui::getComponentInFocus, kick_ui_controls/source/kick_ControlBase.h), and JUCE
    /// grants keyboard focus only while the window is the key window. Scripted touches also run
    /// with the screen locked or the window in the background, so the test-control module records
    /// the component a touch (or setFocus) gave focus to, and the app uses it while JUCE has no
    /// focused component at all.
    ///
    /// Active only while the test-control module runs (KICK_TEST_CONTROL is set). Otherwise every
    /// function here does nothing and get() is always null, so focus is JUCE's alone, as on a unit.
    struct FocusFallback
    {
        /// Set by TestControl while it runs.
        static bool& enabled() noexcept
        {
            static bool on = false;
            return on;
        }

        static juce::Component::SafePointer<juce::Component>& target() noexcept
        {
            static juce::Component::SafePointer<juce::Component> t;
            return t;
        }

        /// The recorded component, while enabled, while JUCE has no focused component and while
        /// it is showing. Null otherwise.
        static juce::Component* get() noexcept
        {
            if( !enabled() || juce::Component::getCurrentlyFocusedComponent() != nullptr )
                return nullptr;
            auto* c = target().getComponent();
            return c != nullptr && c->isShowing() ? c : nullptr;
        }

        static bool is( const juce::Component* C ) noexcept { return C != nullptr && get() == C; }

        /// A control asked for focus (Focused true) or gave it away.
        static void note( juce::Component* C, bool Focused ) noexcept
        {
            if( !enabled() )
                return;
            if( Focused )
                target() = C;
            else if( target().getComponent() == C )
                target() = nullptr;
        }

        static void clear() noexcept { target() = nullptr; }
    };
} // namespace testcontrol
