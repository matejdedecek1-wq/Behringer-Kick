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
    class KeyboardComponent : public juce::Component
        , public juce::ChangeBroadcaster
    {
    public:
        enum Orientation
        {
            horizontalKeyboard,
            verticalKeyboardFacingLeft,
            verticalKeyboardFacingRight
        };

        //==============================================================================
        /** A set of colour IDs to use to change the colour of various aspects of the keyboard.

        These constants can be used either via the Component::setColour(), or LookAndFeel::setColour()
        methods.

        @see Component::setColour, Component::findColour, LookAndFeel::setColour, LookAndFeel::findColour
    */
        enum ColourIds
        {
            whiteNoteColourId              = 0x1005000,
            blackNoteColourId              = 0x1005001,
            keySeparatorLineColourId       = 0x1005002,
            mouseOverKeyOverlayColourId    = 0x1005003, /**< This colour will be overlaid on the normal note colour. */
            keyDownOverlayColourId         = 0x1005004, /**< This colour will be overlaid on the normal note colour. */
            textLabelColourId              = 0x1005005,
            upDownButtonBackgroundColourId = 0x1005006,
            upDownButtonArrowColourId      = 0x1005007,
            shadowColourId                 = 0x1005001
        };

        //==============================================================================
        KeyboardComponent( juce::MidiKeyboardState& s, Orientation o );
        ~KeyboardComponent() override;
        //==============================================================================
        float                  getKeyWidthModifier( int midiNoteNumber ) const noexcept;
        void                   setBlackNoteWidthProportion( float ratio ) noexcept;
        float                  getBlackNoteLength() const noexcept;
        juce::Range<float>     getKeyPos( int midiNoteNumber ) const;
        juce::Rectangle<float> getRectangleForKey( int note ) const;
        juce::Range<float>     getKeyPosition( int midiNoteNumber, float targetKeyWidth ) const;

        /** Returns the keyboard's current direction. */
        Orientation getOrientation() const noexcept { return orientation; }

        /** Returns the number of the first key shown in the component.
    @see setLowestVisibleKey
*/
        int getLowestVisibleKey() const noexcept { return (int)firstKey; }

        //==============================================================================
        /** Draws a white note in the given rectangle.

        isOver indicates whether the mouse is over the key, isDown indicates whether the key is
        currently pressed down.

        When doing this, be sure to note the keyboard's orientation.
    */
        void drawWhiteNote( int             midiNoteNumber,
                            juce::Graphics& g, juce::Rectangle<float> area,
                            bool isDown, bool isOver,
                            juce::Colour lineColour, juce::Colour textColour );

        /** If the keyboard extends beyond the size of the component, this will scroll
        it to show the given key at the start.

        Whenever the keyboard's position is changed, this will use the ChangeBroadcaster
        base class to send a callback to any ChangeListeners that have been registered.
    */
        void setLowestVisibleKey( int noteNumber );
        /** Draws a black note in the given rectangle.

        isOver indicates whether the mouse is over the key, isDown indicates whether the key is
        currently pressed down.

        When doing this, be sure to note the keyboard's orientation.
    */
        void drawBlackNote( int             midiNoteNumber,
                            juce::Graphics& g, juce::Rectangle<float> area,
                            bool isDown, bool isOver,
                            juce::Colour noteFillColour );
        /** Allows text to be drawn on the white notes.
            By default this is used to label the C in each octave, but could be used for other things.
            @see setOctaveForMiddleC
        */
        juce::String getWhiteNoteText( int midiNoteNumber );
        /** This sets the octave number which is shown as the octave number for middle C.

        This affects only the default implementation of getWhiteNoteText(), which
        passes this octave number to MidiMessage::getMidiNoteName() in order to
        get the note text. See MidiMessage::getMidiNoteName() for more info about
        the parameter.

        By default this value is set to 3.

        @see getOctaveForMiddleC
    */

        /** Draws the up and down buttons that scroll the keyboard up/down in octaves. */
        virtual void drawUpDownButton( juce::Graphics& g, int w, int h,
                                       bool isMouseOver,
                                       bool isButtonPressed,
                                       bool movesOctavesUp );
        /** If set to true, then scroll buttons will appear at either end of the keyboard
        if there are too many notes to fit them all in the component at once.
    */
        void setScrollButtonsVisible( bool canScroll );

        /** @internal */
        void handleNoteOn( juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity );
        /** @internal */
        void handleNoteOff( juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity );

        void setOctaveForMiddleC( int octaveNumForMiddleC );
        void paint( juce::Graphics& g );
        void setKeyPressForNote( const juce::KeyPress& key, int midiNoteOffsetFromC );
        void removeKeyPressForNote( int midiNoteOffsetFromC );
        /** @internal */
        void                       colourChanged() override;
        void                       resized() override;
        float                      getKeyStartPosition( int midiNoteNumber ) const;
        float                      getTotalKeyboardWidth() const noexcept;
        int                        getNoteAtPosition( juce::Point<float> p );
        int                        xyToNote( juce::Point<float> pos, float& mousePositionVelocity );
        int                        remappedXYToNote( juce::Point<float> pos, float& mousePositionVelocity ) const;
        void                       setLowestVisibleKeyFloat( float noteNumber );
        void                       mouseDown( const juce::MouseEvent& e ) override;
        void                       setKeyWidth( float width ) { keyWidth = width; }
        float                      firstKey = 12 * 4.0f;
        float                      keyWidth = 16.0f;
        std::function<void( int )> noteSelectionChanged;

    private:
        //==============================================================================
        // Your private member variables go here...
        struct UpDownButton;

        float blackNoteLengthRatio = 0.7f;
        float blackNoteWidthRatio  = 0.7f;

        float xOffset    = 0;
        int   rangeStart = 0, rangeEnd = 127;
        int   scrollButtonWidth = 12;

        int   midiChannel = 1, midiInChannelMask = 0xffff;
        float velocity = 1.0f;

        juce::Array<int> mouseOverNotes, mouseDownNotes;
        juce::BigInteger keysPressed, keysCurrentlyDrawnDown;
        bool             shouldCheckState = false;

        std::unique_ptr<juce::Button> scrollDown, scrollUp;
        bool                          canScroll = true, useMousePositionForVelocity = true;

        juce::Array<juce::KeyPress> keyPresses;
        juce::Array<int>            keyPressNotes;
        int                         keyMappingOctave = 6, octaveNumForMiddleC = 3;

        juce::MidiKeyboardState&                 state;
        std::unique_ptr<juce::MidiKeyboardState> keyboardState;
        std::unique_ptr<KeyboardComponent>       keyboardComponent;
        Orientation                              orientation;

        bool mouseDownOnKey( int, const juce::MouseEvent& );
        bool mouseDraggedToKey( int, const juce::MouseEvent& );
        void mouseUpOnKey( int, const juce::MouseEvent& );
        void mouseUp( const juce::MouseEvent& e );
        void mouseEnter( const juce::MouseEvent& e );
        void mouseExit( const juce::MouseEvent& e );

        void updateNoteUnderMouse( juce::Point<float>, bool isDown, int fingerNum );
        void updateNoteUnderMouse( const juce::MouseEvent&, bool isDown );
        void repaintNote( int midiNoteNumber );
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( KeyboardComponent )
    };
}
