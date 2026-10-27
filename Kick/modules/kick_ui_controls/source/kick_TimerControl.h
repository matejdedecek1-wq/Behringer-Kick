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
    class KickTimerControl : public juce::Component
    {
    public:
        enum class DisplayFormat
        {
            displayFormatMS,
            displayFormatSS
        };

        KickTimerControl();

        void setDisplayFormat( DisplayFormat format ) { displayFormat = format; }
        void setEditable( bool Editable );

        void   setTime( double newTime );
        double getTime();

        void setMaxTime( double maxValue ) { timeMaxValue = maxValue; }
        void setValueColour( juce::Colour colour );
        void setNumberDisplay( ViewType type );

        void knobRotated( double deltaValue, bool Minutes );

        SIGNALS : sigslot::signal<double> timeChanged;

    private:
        void recalculateTime();
        void resized() override;

    private:
        DisplayFormat displayFormat         = DisplayFormat::displayFormatMS;
        double        timeCounter           = 0.0;
        double        timeMaxValue          = 1000.0;
        int           currentTimeEditFormat = 0;
        bool          editable{ false };

        KickNumber mm{ "mm", 0, 20, 0, 1, true };
        KickNumber ss{ "ss", 0, 59, 20, 1, true };
        KickNumber ms{ "ms", 0, 99, 0, 1, true };
        KickLabel  mSep{ ":" };
        KickLabel  sSep{ ":" };
    };
}
