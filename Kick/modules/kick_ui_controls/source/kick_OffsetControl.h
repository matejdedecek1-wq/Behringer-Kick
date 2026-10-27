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
    class KickOffSetControl
        : public KickControlBase<juce::Component>
        , public mt::MTProperties
    {
    public:
        explicit KickOffSetControl( int range, const juce::String& name = "" );
        ~KickOffSetControl();
        void paint( juce::Graphics& g ) override;
        void setRange( int r ) { range = r; }
        void mouseDrag( const juce::MouseEvent& event ) override;
        int  getValue() { return value; }
        void setDisabled( bool disable );

        void sliderMoved( float ) override{};
        void knobRotated( double ) override;
        void knobClicked() override;

    private:
        void setValue( int newValue );

    private:
        juce::Rectangle<float> getTitleRect();
        juce::Rectangle<float> getLabelRect();
        juce::Rectangle<float> getSliderRect();

    private:
        int          range;
        juce::String ctrlName;
        int          value    = 0;
        bool         disabled = false;
    };
}
