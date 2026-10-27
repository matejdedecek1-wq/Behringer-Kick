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

#include "juce_core/juce_core.h"

#define NUM_OF_BANKS 8

namespace contracts
{
    class DeviceControlContracts
    {
    public:
        enum PadColourIds
        {
            padColourDarkRed,
            padColourVividPurple,
            padColourDarkYellow,
            padColourlightGreen1,
            padColourlightGreen3,
            padColourlightGreen4,
            PadColourLightSkyBlue,
            PadColourAqua,
            PadColourBlue1,
            PadColourPink,
            PadColourPink1,
            PadColourPink2,
            PadColourRed,
            PadColourOrange1,
            PadColourOrange,
            PadColourTrans,
            PadColourNeutralWhite,
            PadColourNeutralGrey,
        };

        enum PadButtonState
        {
            StateOff = 0, // pads are off cannot be higlighted or clicked
            StateOn, // pads are on but cannot be highlegthed when notes are played
            StateSampleAssigned,
            StateSampleNotAssigned,
            StateOnBlink,
            StateOffBlink,
        };

        enum PadAction
        {
            SetPadColour, //params pad, bank, colour id
            SetPadState, //params startpad, endpad, state
            HighlightPad //params todo
        };

        enum SysCmd
        {
            EN_SYS_RESET_GD = 0,
            EN_SYS_GET_SW_VER,
            EN_SYS_COMM_INIT,
            EN_SYS_SW_UPDATE,
            EN_SYS_CHECK_COMM_STATUS,
            EN_SYS_BPM,
            EN_SYS_RECONNECT,
            EN_SYS_SHUTDOWN
        };

        enum SysCommCmd
        {
            EN_SYS_COMM_NOT_CONNECTED = 0,
            EN_SYS_COMM_CONNECTED
        };

        enum PadSequence
        {
            EN_PAD1 = 0,
            EN_PAD2,
            EN_PAD3,
            EN_PAD4,
            EN_PAD5,
            EN_PAD6,
            EN_PAD7,
            EN_PAD8,
            EN_PAD9,
            EN_PAD10,
            EN_PAD11,
            EN_PAD12,
            EN_PAD13,
            EN_PAD14,
            EN_PAD15,
            EN_PAD16,

            EN_ALL_PADS
        };

        enum KnobSequence
        {
            EN_KNOB_CENTER = 0,
            EN_KNOB_1,
            EN_KNOB_2,
            EN_KNOB_3,
            EN_KNOB_4,
            EN_KNOB_5,
            EN_KNOB_6,
            EN_KNOB_7,
            EN_KNOB_8,
            EN_KNOB_9,
            EN_KNOB_10,
            EN_KNOB_11,
            EN_KNOB_12,
            EN_KNOB_13,
            EN_KNOB_14,
            EN_KNOB_15,
            EN_KNOB_16,
        };

        enum KnobCmd
        {
            EN_KNOB_ROTARY = 0,
            EN_KNOB_QLINK
        };

        enum KnobDirection
        {
            EN_KNOB_ANTICLOCKWISE = 0,
            EN_KNOB_CLOCKWISE,
        };

        enum BtnLedSequence
        {
            EN_INVALID = -1,
            EN_BANK_A  = 0,
            EN_BANK_B  = 1,
            EN_BANK_C  = 2,
            EN_BANK_D  = 3,
            EN_BANK_E  = 4,
            EN_BANK_F  = 5,
            EN_BANK_G  = 6,
            EN_BANK_H  = 7,

            EN_MAINMODE     = 8,
            EN_GRID         = 9,
            EN_BROWSE       = 10,
            EN_SAVE         = 11,
            EN_TRACK_MIX    = 12,
            EN_PAD_MIX      = 13,
            EN_TRACK_MUTE   = 14,
            EN_PAD_MUTE     = 15,
            EN_NEXT_SEQ     = 16,
            EN_FX           = 17,
            EN_TAP_TEMPO    = 18,
            EN_MASTER       = 19,
            EN_COPY         = 20,
            EN_PLUS         = 21,
            EN_MENU         = 22,
            EN_PREF         = 23,
            EN_NOTE_REPEAT  = 24,
            EN_LATCH        = 25,
            EN_KNOB_ENTER   = 26,
            EN_MINUS        = 27,
            EN_KNOB_CCW     = 28,
            EN_BACK         = 29,
            EN_KNOB_CW      = 30,
            EN_SHIFT        = 31,
            EN_STEP_SEQ     = 32,
            EN_AUTOMATION   = 33,
            EN_TC           = 34,
            EN_ON_OFF       = 35,
            EN_SAMPLER      = 36,
            EN_LOOPER       = 37,
            EN_SAMPLE_EDIT  = 38,
            EN_LINK_EDIT    = 39,
            EN_PROG_EDIT    = 40,
            EN_MIDI_CONTROL = 41,
            EN_RECORD       = 42,
            EN_OVERDUB      = 43,
            EN_STOP         = 44,
            EN_PLAY         = 45,
            EN_PLAYSTART    = 46,
            EN_LINK         = 47,
            EN_FULL_LEVEL   = 48,
            EN_HALF_LEVEL   = 49,
            EN_16LEVELS     = 50,
            EN_NOTES        = 51,
            EN_ERASE        = 52,

            SIZE = 53,

            EN_COLOR_EDIT = 54,

            EN_INVALID_ID = -1
        };

        enum BtnCmd
        {
            EN_BTN_STATUS = 0
        };

        enum BtnState
        {
            BTN_RELEASED = 0,
            BTN_PRESSED,
            BTN_LONGPRESSED,
            BTN_DOUBLECLICK,
        };

        enum LedCmd
        {
            EN_LED_SET_SINGLE_LED = 0,
            EN_LED_BLINK_SINGLE,
            EN_LED_SET_ALL,
        };

        enum PadCmd
        {
            EN_PAD_STATE = 0,
            EN_PAD_SET_SINGLE_PAD,
            EN_PAD_SET_ALL_SINGLE_COLOUR,
            EN_PAD_SET_ALL_MULTI_COLOUR,
            EN_PAD_SETTINGS,
            EN_PAD_COLOR_MODE,
            EN_PAD_SET_COLOUR_STATE,
            EN_PAD_SET_AFTER_TOUCH,
            EN_PAD_SET_BLINKING,
            EN_PAD_SET_STEP_SINGLE_PAD, //Only for STEP SEQUENCER
            EN_PAD_SET_STEP_ALL_PAD //Only for STEP SEQUENCER
        };

        enum SliderCmd
        {
            EN_SLIDER_TOUCH_INPUT = 0,
            EN_SLIDER_BAR_LED,
            EN_SLIDER_LEVEL_LED,
            EN_SLIDER_INDIVIDUAL_LED
        };
    };

} // namespace
