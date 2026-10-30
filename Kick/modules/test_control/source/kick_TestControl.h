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
    /// An opt-in test channel for the app's screen, for scripted checks.
    ///
    /// Created only when KICK_TEST_CONTROL=<file> is set (createFromEnvironment). Without it
    /// nothing is created, polled or logged. The app then polls <file> (relative to the working
    /// directory) for lines appended after start-up and runs each one on the message thread:
    ///
    ///     <seq> <command> [args]
    ///
    /// and logs "TestControl: ok <seq>" when it is done, or "TestControl: error <seq> <reason>".
    /// Arguments with spaces can be quoted ("Firmware Update"). Commands:
    ///
    ///     tap X Y             touch pixel (X, Y) of the screen (the 1024x600 main window)
    ///     touch X Y           the same, as a real touch through the window (see below)
    ///     click NAME          touch the centre of the showing component titled (or named) NAME
    ///     clickat NAME FX FY  touch NAME at fraction (FX, FY) of its size
    ///     list [FILTER]       log the showing, titled components: "TestControl: item <type>
    ///                         <name> <x y w h on the screen> screen|popup"
    ///     shot OUT.png        render the screen, popups and bubbles included, to OUT.png at its
    ///                         native size; any other showing desktop window to OUT-<n>.png
    ///     windows             log the main window, the overlays on it (modal popups, bubbles)
    ///                         and any other desktop window, with their bounds on the screen and
    ///                         on the display
    ///     status              log "TestControl: status <text>" from statusProvider (the app
    ///                         fills it in: transport, BAR:BEAT:TICK, tempo, ...)
    ///     echo TEXT           log "TestControl: echo TEXT" (a marker for scripts)
    ///
    /// Touches are delivered to the touched component directly (mouseDown, then mouseUp 80 ms
    /// later; buttons get triggerClick), never through the window's ComponentPeer, so they work
    /// while the computer's screen is locked or the window is covered. Like a finger, a touch
    /// goes to whatever is on top at that point. A touch also gives keyboard focus to the touched
    /// control and records it in FocusFallback, which is active only while this module runs.
    /// Mouse listeners of parent components (a list's canvas, KickTableView) do not hear these.
    ///
    /// "touch X Y" is the exception: it hands a button-down and, 80 ms later, a button-up at that
    /// point to the window's ComponentPeer (as the main mouse source), so JUCE dispatches it as it
    /// dispatches a click or a finger: to the component under the point, its mouse listeners and
    /// its parents' listeners, with JUCE's own focus handling. JUCE asks the operating system
    /// whether the window is the topmost one at that point. If it is not (window covered), the
    /// window floats above the others for the touch (always-on-top, without activating the app)
    /// and is put back afterwards. If it still is not (screen locked), the command answers with
    /// an error instead of touching. A real mouse moved over the window during those 80 ms
    /// shares the source and can move the button-up.
    ///
    /// Only the screen is driven here. Keys, pads, encoders and the slider are the control
    /// surface's job and arrive over the serial port.
    class TestControl : private juce::Timer
    {
    public:
        /// Text for the "status" command. Set by the app, which knows the engine.
        static inline std::function<juce::String()> statusProvider;

        /// A running TestControl if KICK_TEST_CONTROL is set, otherwise null (and nothing else
        /// happens). Screen: the app's main window, whose coordinates are the screen's.
        static std::unique_ptr<TestControl> createFromEnvironment( juce::Component& Screen );

        TestControl( juce::Component& Screen, const juce::File& File );
        ~TestControl() override;

    private:
        void timerCallback() override;
        void run( const juce::String& Line );

        juce::Component* findComponent( const juce::String& Name );
        juce::String     touch( juce::Component& Target, juce::Point<float> PosInTarget, std::function<void()> Done );
        juce::String     peerTouch( juce::Point<float> PosOnScreen, std::function<void()> Done, std::function<void( const juce::String& )> Fail );
        juce::String     snapshot( const juce::File& Out );
        juce::Array<juce::Component*> desktopWindows();

        juce::Component::SafePointer<juce::Component> screen;
        juce::File                                    file;
        juce::int64                                   readPos{ 0 };
        juce::String                                  partial;
        juce::StringArray                             queue;
        bool                                          busy{ false };
        std::shared_ptr<bool>                         alive{ std::make_shared<bool>( true ) }; // for delayed callbacks

        JUCE_DECLARE_NON_COPYABLE( TestControl )
    };
} // namespace testcontrol
