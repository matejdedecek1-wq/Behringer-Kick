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

enum MidiControllerType
{
    MODULATION_WHEEL             = 1,
    BREATH_CONTROLLER            = 2,
    UNDEFINED_3                  = 3,
    FOOT_PEDAL                   = 4,
    PORTAMENTO_TIME              = 5,
    DATA_ENTRY_MSB               = 6,
    MAIN_VOLUME                  = 7,
    BALANCE                      = 8,
    UNDEFINED_9                  = 9,
    PAN_VAL                      = 10,
    EXPRESSION_CONTROLLER        = 11,
    EFFECT_CONTROL1              = 12,
    EFFECT_CONTROL2              = 13,
    UNDEFINED_14                 = 14,
    UNDEFINED_15                 = 15,
    GENERAL_PURPOSE_CONTROLLER1  = 16,
    GENERAL_PURPOSE_CONTROLLER2  = 17,
    GENERAL_PURPOSE_CONTROLLER3  = 18,
    GENERAL_PURPOSE_CONTROLLER4  = 19,
    UNDEFINED_20                 = 20,
    UNDEFINED_21                 = 21,
    UNDEFINED_22                 = 22,
    UNDEFINED_23                 = 23,
    UNDEFINED_24                 = 24,
    UNDEFINED_25                 = 25,
    UNDEFINED_26                 = 26,
    UNDEFINED_27                 = 27,
    UNDEFINED_28                 = 28,
    UNDEFINED_29                 = 29,
    UNDEFINED_30                 = 30,
    UNDEFINED_31                 = 31,
    HOLD_PEDAL                   = 64,
    PORTAMENTO_ON_OFF            = 65,
    SUSTENUTO_PEDAL              = 66,
    SOFT_PEDAL                   = 67,
    LEGATO_FOOTSWITCH            = 68,
    HOLD2_PEDAL                  = 69,
    SOUND_CONTROLLER1            = 70,
    SOUND_CONTROLLER2            = 71,
    SOUND_CONTROLLER3            = 72,
    SOUND_CONTROLLER4            = 73,
    SOUND_CONTROLLER5            = 74,
    SOUND_CONTROLLER6            = 75,
    SOUND_CONTROLLER7            = 76,
    SOUND_CONTROLLER8            = 77,
    SOUND_CONTROLLER9            = 78,
    SOUND_CONTROLLER10           = 79,
    GENERAL_PURPOSE_CONTROLLER5  = 80,
    GENERAL_PURPOSE_CONTROLLER6  = 81,
    GENERAL_PURPOSE_CONTROLLER7  = 82,
    GENERAL_PURPOSE_CONTROLLER8  = 83,
    PORTAMENTO_CONTROL           = 84,
    UNDEFINED_85                 = 85,
    UNDEFINED_86                 = 86,
    UNDEFINED_87                 = 87,
    UNDEFINED_88                 = 88,
    UNDEFINED_89                 = 89,
    UNDEFINED_90                 = 90,
    UNDEFINED_91                 = 91,
    EFFECTS_LEVEL                = 91,
    TREMOLO_LEVEL                = 92,
    CHORUS_LEVEL                 = 93,
    CELESTE_LEVEL                = 94,
    PHASER_LEVEL                 = 95,
    DATA_BUTTON_INCREMENT        = 96,
    DATA_BUTTON_DECREMENT        = 97,
    NON_REGISTERED_PARAMETER_MSB = 98,
    NON_REGISTERED_PARAMETER_LSB = 99,
    REGISTERED_PARAMETER_MSB     = 100,
    REGISTERED_PARAMETER_LSB     = 101,
    ALL_SOUND_OFF                = 120,
    RESET_ALL_CONTROLLERS        = 121,
    LOCAL_CONTROL                = 122,
    ALL_NOTES_OFF                = 123,
    OMNI_MODE_OFF                = 124,
    OMNI_MODE_ON                 = 125,
    MONO_MODE_ON                 = 126,
    POLY_MODE_ON                 = 127
};
