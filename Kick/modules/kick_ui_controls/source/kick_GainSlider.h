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

    class KickGainSlider
        : public KickControlBase<juce::Component>
        , public mt::MTProperties
    {
    public:
        KickGainSlider();

        void setRange( double start, double end );
        void setRangeAndDisplayValues( const juce::Point<float>& range, const std::vector<float>& displayValues );

        float getValue() { return currentDbValue; }
        void  setValue( float valueDb, juce::NotificationType Notify );
        void  setKnobImage( const juce::String& image );

        void  setNormalized( float Value );
        float getNormalized();

        SIGNALS : sigslot::signal<float> gainChanged;

    private:
        void  setSliderPos( float newPos, juce::NotificationType Notify );
        float getDbFromPixel( float pixel );
        float getPixelFromDb( float dbValue );

        void resized() override;
        void paint( juce::Graphics& g );

        void mouseDrag( const juce::MouseEvent& event ) override;
        void mouseUp( const juce::MouseEvent& event ) override;
        void mouseDoubleClick( const juce::MouseEvent& event ) override;

        void knobRotated( double ) override;
        void sliderMoved( float ) override;
        void knobClicked() override;

    private:
        bool                            isSliderDragged{ false };
        float                           min{ -100.f };
        float                           max{ 0.f };
        float                           currentDbValue{ -60.f };
        std::list<float>                markings;
        std::vector<float>              decibles = { 0, -3, -9, -20, -30, -100 };
        juce::NormalisableRange<double> normalized{ 0, 100 };

        juce::Rectangle<int>            bSliderTrack, bSliderKnob, bLabel, bMarkingLines, bMarkingText;
        std::unique_ptr<juce::Drawable> _knobImg;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( KickGainSlider )
    };
}
