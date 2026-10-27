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
    class KickImage
        : public juce::Component
        , public mt::MTProperties
    {
    public:
        explicit KickImage( const juce::String& Name = "", const juce::String& Image = "", bool ShowTitle = false, bool ShowBorder = false );

        void setImage( const juce::String& Image );
        void setImage( juce::Image Image );

        void setColours( const juce::Colour& Text, const juce::Colour& Backgroud, const juce::Colour& Outline = juce::Colours::transparentBlack );

    private:
        std::unique_ptr<juce::Drawable> d_;

        void paint( juce::Graphics& g ) override;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( KickImage )
    };

}
