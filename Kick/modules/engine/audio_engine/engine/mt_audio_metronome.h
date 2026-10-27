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
    class MTMetronome
    {
    public:
        explicit MTMetronome( AudIO::DawEngine& engine, const juce::ValueTree& VT )
            : _engine( engine ), _vt( VT )
        {
            if( auto pMetronome = std::make_unique<Metronome>( engine, _vt ) )
            {
                _metronome = pMetronome.get();
                _engine.createObject( std::move( pMetronome ) );
                _metronome->initAudioRouting();
            }
            assert( _metronome != nullptr );

            /// Setup callbacks...
            signals::onBar.connect( [&]( int ) { triggerBar(); } );
            signals::onBeat.connect( [&]( int ) { if( MTMetronomeRate::MET_SEQ == _met.rate() ) triggerBeat(); } );

            signals::on1_2.connect( [&]() { if( MTMetronomeRate::MET_1_2 == _met.rate() ) triggerBeat(); } );
            signals::on1_4.connect( [&]() { if( MTMetronomeRate::MET_1_4 == _met.rate() ) triggerBeat(); } );
            signals::on1_8.connect( [&]() { if( MTMetronomeRate::MET_1_8 == _met.rate() ) triggerBeat(); } );
            signals::on1_16.connect( [&]() { if( MTMetronomeRate::MET_1_16 == _met.rate() ) triggerBeat(); } );

            _met.source.onValueChanged = [&]() {
                auto src = _met.source();
                if( _metronome )
                    _metronome->setSource( src );
            };

            _met.enabled.onValueChanged = [&]() {
                if( _metronome )
                    _metronome->setBypass( !_met.enabled() );
            };

            // Force value updates...
            _met.source.onValueChanged();
            _met.enabled.onValueChanged();
        }

        ~MTMetronome()
        {
            _metronome->noteOffAll();
            _engine.removeObject( _metronome->getId() );
            _metronome = nullptr;
        }

        void triggerBar()
        {
            //DBG( "Bar: " << juce::String( juce::Time::getMillisecondCounterHiRes() ) );
            if( _metronome && _met.enabled() )
                _metronome->noteOn( 0, 127 );
        }

        void triggerBeat()
        {
            //DBG( "Beat: " << juce::String( juce::Time::getMillisecondCounterHiRes() ) );
            if( _metronome && _met.enabled() )
                _metronome->noteOn( 1, 64 );
        }

    private:
    private:
        juce::ValueTree   _vt{ IDs::METRONOME };
        AudIO::DawEngine& _engine;
        KickMetronome&    _met{ getSettings().metronome };

        Metronome* _metronome{ nullptr };
    };

} // namespace MT
