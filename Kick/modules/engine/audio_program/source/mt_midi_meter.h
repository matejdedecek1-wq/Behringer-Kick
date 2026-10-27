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
    static juce::NormalisableRange<float> meterNorm{ -100.f, 20.f };

    class MTMidiMeter
    {
    public:
        virtual ~MTMidiMeter() = default;

        explicit MTMidiMeter( int Steps )
        {
            meterNorm.setSkewForCentre( 0.8f );
            meterData.resize( 2 );
            setSteps( Steps );
        };

        void enable( bool Enable ) noexcept { enabled.store( Enable ); }
        bool isEnabled() const noexcept { return enabled.load(); }

        void setSteps( int Steps ) noexcept { meter.reset( Steps ); }

        void process() noexcept
        {
            auto rms = meter.getNextValue();
            for( auto& it : meterData )
                it.rms = rms;
        }

        void reload( float normLevel ) noexcept
        {
            if( !isEnabled() )
                return;

            auto newLevel = meterNorm.convertFrom0to1( normLevel );
            auto curLevel = meter.getCurrentValue();
            if( newLevel > curLevel )
            {
                meter.setCurrentAndTargetValue( newLevel );
                meter.setTargetValue( DB::MIN );
            }
        }

        const MTMeterData& getMeter() const { return meterData; }

    private:
        MTMeterData                meterData;
        juce::SmoothedValue<float> meter{ DB::MIN };

        std::atomic<bool> enabled{ true };

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( MTMidiMeter )
    };

} // namespace MT
