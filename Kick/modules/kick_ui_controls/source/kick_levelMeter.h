/*
 * Copyright (c) 2023 MUSIC Tribe
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
    class KickLevelMeter : public juce::Component
    {
    public:
        enum class Style
        {
            Vertical,
            Horizantal,
            VerticalWithoutMarks,
            HorizantalWithMarks,
            VerticalWithMarks
        };

        explicit KickLevelMeter( Style Style = Style::Vertical );

        void showTitle( const bool showTitle );
        void setStyle( Style Style );
        void setRange( double start, double end );
        void setSignalStrength( const std::vector<float>& sigStrength );
        void setSignalStrength( float lRms, float rRms, float lPeak = -100.f, float rPeak = -100.f );

    private:
        float getPixelFromDb( float dbValue, bool vertical );

        void paint( juce::Graphics& g ) override;
        void resized() override;

    private:
        inline static constexpr int levelMeterMargin = 5;

        juce::Rectangle<int>            bLeft;
        juce::Rectangle<int>            bRight;
        juce::Rectangle<int>            bLabel;
        juce::Rectangle<int>            Marking_left;
        juce::Rectangle<int>            Marking_right;
        juce::ColourGradient            leftGradient;
        juce::ColourGradient            rightGradient;
        std::list<float>                markings;
        juce::NormalisableRange<double> norRange{ 0, 100 };
        Style                           style{ Style::Vertical };
        double                          startValue  = 0;
        double                          endValue    = 0;
        float                           leftMax     = 0.0f;
        float                           rightMax    = 0.0f;
        float                           meterHeight = 0.0f;
        std::vector<float>              decibles    = { +20, +10, +5, 0, -10, -20, -30, -40, -50 };
        float                           min{ -100.f };
        float                           max{ 0.f };
        float                           lrms{ -100.f };
        float                           rrms{ -100.f };
        float                           lpeak{ -100.f };
        float                           rpeak{ -100.f };
        bool                            isShowTitle{ false };

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( KickLevelMeter )
    };

    struct KickMeterHorizantal : public KickLevelMeter
    {
        KickMeterHorizantal()
            : KickLevelMeter( KickLevelMeter::Style::Horizantal ){};
    };

    struct KickMeterHorizantalWithMarks : public KickLevelMeter
    {
        KickMeterHorizantalWithMarks()
            : KickLevelMeter( KickLevelMeter::Style::HorizantalWithMarks ){};
    };

    struct KickMeterVerticalWithMarks : public KickLevelMeter
    {
        KickMeterVerticalWithMarks()
            : KickLevelMeter( KickLevelMeter::Style::VerticalWithMarks ){};
    };

}
