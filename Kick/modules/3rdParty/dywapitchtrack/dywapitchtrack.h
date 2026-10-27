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

/*
 BEGIN_JUCE_MODULE_DECLARATION
    ID:               dywapitchtrack
    name:             dywapitchtrack
    description:      Dynamic Wavelet Algorithm Pitch Tracking library
    website:          https://github.com/antoineschmitt/dywapitchtrack
    license:          MIT

    dependencies:     juce_audio_basics
END_JUCE_MODULE_DECLARATION
*/

#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

namespace PitchTracking
{
    double detect( const float* Samples, int NumSamples, float SampleRate );
    double detect( const juce::AudioSampleBuffer& Buffer, int StartSample, int NumSamples, double SampleRate );

} // namespace hise
