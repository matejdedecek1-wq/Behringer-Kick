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
        class SEScaleComponent : public juce::Component
        {
        public:
            SEScaleComponent( SampleEditorData& data );
            ~SEScaleComponent();
            void paint( juce::Graphics& g ) override;
            void resized() override;

        private:
            void drawMarkingText( juce::Graphics& g, juce::String& value, float xpos );
            void paintSamples( juce::Graphics& g );
            void paintBeats( juce::Graphics& g );
            void paintTime( juce::Graphics& g );
            void calculateBeatWidth();

        private:
            void getDisplayText( juce::String& markerText, int bar, int beat, int tick, int numDigits );

        private:
            SampleEditorData& seData;
            float             beatWidth       = 0;
            int               noSamplePerBeat = 0;
            float             defaultSize     = 0.0;
            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( SEScaleComponent )
        };
    }
}
