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

namespace mt
{

    static void insertsSerialize( juce::ValueTree& vt, AudIO::DawInserts* synth )
    {
        juce::UndoManager* um{ nullptr };
        if( synth && vt.hasType( IDs::INSERTS ) )
        {
            const auto& insNames = synth->getEffectsNames();
            auto        numIns   = insNames.size();

            vt.setProperty( IDs::bypass, synth->isAllEffectsBypassed(), um );
            std::vector<float> vals;

            for( auto i = 0; i < numIns; ++i )
            {
                synth->serializeEffect( i, vals );
                auto state = VectorToJuceVar<float>( vals );

                auto childVT = vt.getChild( i );
                if( childVT.isValid() )
                {
                    childVT.setProperty( IDs::name, juce::String( insNames[i] ), um );
                    childVT.setProperty( IDs::state, state, um );
                }
                else
                {
                    auto insVT = createVTree( IDs::Insert,
                                              IDs::name, juce::String( insNames[i] ),
                                              IDs::state, state );
                    vt.addChild( insVT, i, um );
                }
            }
        }
    }

    static void insertsDeserialize( const juce::ValueTree& vt, AudIO::DawInserts* synth )
    {
        if( synth && vt.hasType( IDs::INSERTS ) )
        {
            auto numIns = vt.getNumChildren();

            synth->setBypassAllEffects( vt.getProperty( IDs::bypass, false ) );

            for( auto i = 0; i < numIns; ++i )
            {
                auto insVT = vt.getChild( i );
                if( insVT.isValid() )
                {
                    auto name  = insVT.getProperty( IDs::name, "" ).toString();
                    auto state = insVT.getProperty( IDs::state );
                    auto vals  = JuceVarToVector<float>( state );

                    synth->addEffect( name.toStdString(), i );
                    synth->deserializeEffect( i, vals );
                }
            }
        }
    }

    // Ensure all parameters are correct before calling this API, this API will not check for correctness.
    static void insertsSyncInfo( unsigned int Position, AudIO::DawInserts* synth, MTInsertsInfo& Info )
    {
        Info.name   = synth->getEffectName( Position );
        Info.bypass = synth->isEffectBypassed( Position );

        Info.params.clear();
        const auto& insParams = synth->getEffectParameters( Position );
        for( const auto& it : insParams )
        {
            Info.params.emplace_back( MTInsertsParamInfo{ juce::String( it.name ), juce::String( it.unit ), juce::String( it.displayValue ), it.value } );
        }
    }

    ///
    /// Inserts interface API's
    ///

    struct MTIInsertsImpl
    {
        AudIO::DawInserts*         synth{ nullptr };
        std::vector<MTInsertsInfo> inserts;
    };

    MTIInsertsInterface::MTIInsertsInterface()
        : state( juce::ValueTree{ IDs::INSERTS } )
    {
        pImpl = std::make_unique<MTIInsertsImpl>();
        setup( state, nullptr, nullptr );
    }

    MTIInsertsInterface::~MTIInsertsInterface()
    {
        pImpl.reset();
    }

    void MTIInsertsInterface::setup( juce::ValueTree& VT, juce::UndoManager* UM, AudIO::DawInserts* Impl )
    {
        jassert( VT.hasType( IDs::INSERTS ) );

        state        = VT;
        um           = UM;
        pImpl->synth = Impl;

        if( pImpl->synth )
        {
            const auto numEffects = pImpl->synth->getNumEffects();
            pImpl->inserts.reserve( numEffects );

            pImpl->synth->removeAllEffects();
        }

        insertsDeserialize( state, pImpl->synth );
    }

    void MTIInsertsInterface::getInserts( std::vector<juce::String>& InsertNames )
    {
        InsertNames.clear();
        if( pImpl->synth )
        {
            const auto& effects = pImpl->synth->getEffectsNames();
            for( auto& it : effects )
                InsertNames.emplace_back( juce::String( it ) );
        }
    }

    std::vector<MTInsertsInfo>* MTIInsertsInterface::getInserts()
    {
        if( pImpl->synth )
        {
            const auto numEffects = pImpl->synth->getNumEffects();
            pImpl->inserts.clear();
            for( auto pos = 0; pos < numEffects; ++pos )
                pImpl->inserts.emplace_back( getInsertsInfo( pos ) );
        }
        return &pImpl->inserts;
    }

    MTInsertsInfo MTIInsertsInterface::getInsertsInfo( unsigned int Position )
    {
        MTInsertsInfo info;
        if( pImpl->synth && ( (int)Position < pImpl->synth->getNumEffects() ) )
            insertsSyncInfo( Position, pImpl->synth, info );
        return info;
    }

    int               MTIInsertsInterface::getNumInserts() { return pImpl->synth ? pImpl->synth->getNumEffects() : 0; }
    int               MTIInsertsInterface::getNumParameters( unsigned int Position ) { return pImpl->synth ? pImpl->synth->getNumEffectParameters( Position ) : 0; }
    std::vector<bool> MTIInsertsInterface::getInsertBypassState() { return pImpl->synth ? pImpl->synth->getEffectsBypass() : std::vector<bool>{}; }
    bool              MTIInsertsInterface::isAllInsertsBypassed() { return pImpl->synth ? pImpl->synth->isAllEffectsBypassed() : true; }
    bool              MTIInsertsInterface::isInsertBypassed( unsigned int Position ) { return pImpl->synth ? pImpl->synth->isEffectBypassed( Position ) : true; }

    void MTIInsertsInterface::setAllInsertsBypass( bool Bypass )
    {
        if( pImpl->synth )
        {
            state.setProperty( IDs::bypass, Bypass, um );
            pImpl->synth->setBypassAllEffects( Bypass );
        }
    }

    int MTIInsertsInterface::setInsertBypass( unsigned int Position, bool Bypass )
    {
        int retVal = -1;
        if( pImpl->synth )
        {
            pImpl->synth->setEffectBypass( Position, Bypass );
            retVal = 0;
            insertsSerialize( state, pImpl->synth ); // Also, save effects latest state to value tree.
            insertsSyncInfo( Position, pImpl->synth, pImpl->inserts[Position] ); // Synchronize as well.
        }
        return retVal;
    }

    int MTIInsertsInterface::addInsert( const juce::String& Effect, unsigned int Position )
    {
        int retVal = -1;
        if( pImpl->synth )
        {
            if( 0 == pImpl->synth->addEffect( Effect.toStdString(), Position ) )
            {
                std::vector<float> vals;
                pImpl->synth->serializeEffect( Position, vals );
                auto valState = VectorToJuceVar<float>( vals );

                auto insVT = createVTree( IDs::Insert,
                                          IDs::name, Effect,
                                          IDs::state, valState );
                state.addChild( insVT, Position, um );
                retVal = 0;
            }
            insertsSerialize( state, pImpl->synth ); // Also, save effects latest state to value tree.
            getInserts();
        }
        return retVal;
    }

    int MTIInsertsInterface::removeInsert( const juce::String& Effect )
    {
        int retVal = -1;
        if( pImpl->synth )
        {
            retVal = pImpl->synth->removeEffect( Effect.toStdString() );
            insertsSerialize( state, pImpl->synth ); // Also, save effects latest state to value tree.
            getInserts();
        }
        return retVal;
    }

    int MTIInsertsInterface::removeInsert( unsigned int Position )
    {
        int retVal = -1;
        if( pImpl->synth )
        {
            retVal = pImpl->synth->removeEffect( Position );
            insertsSerialize( state, pImpl->synth ); // Also, save effects latest state to value tree.
            getInserts();
        }
        return retVal;
    }

    void MTIInsertsInterface::removeAllInserts()
    {
        if( pImpl->synth )
        {
            pImpl->synth->removeAllEffects();
            insertsSerialize( state, pImpl->synth ); // Also, save effects latest state to value tree.
            getInserts();
        }
    }

    juce::String MTIInsertsInterface::getInsertName( unsigned int Position )
    {
        return pImpl->synth ? pImpl->synth->getEffectName( Position ) : "";
    }

    juce::String MTIInsertsInterface::getParameterName( unsigned int Position, int Parameter )
    {
        if( pImpl->synth )
        {
            const auto& params = pImpl->synth->getEffectParameters( Position );
            if( Parameter < (int)params.size() )
                return params[Parameter].name;
        }
        return "";
    }

    juce::String MTIInsertsInterface::getParameterValueString( unsigned int Position, int Parameter )
    {
        if( pImpl->synth )
        {
            const auto& params = pImpl->synth->getEffectParameters( Position );
            if( Parameter < (int)params.size() )
                return params[Parameter].displayValue;
        }
        return "";
    }

    float MTIInsertsInterface::getParameterValue( unsigned int Position, int Parameter )
    {
        if( pImpl->synth )
        {
            const auto& params = pImpl->synth->getEffectParameters( Position );
            if( Parameter < (int)params.size() )
                return params[Parameter].value;
        }
        return 0.f;
    }

    void MTIInsertsInterface::setParameterValue( unsigned int Position, int Parameter, float Value )
    {
        if( pImpl->synth )
        {
            pImpl->synth->setEffectParameter( Position, Parameter, Value );
            insertsSerialize( state, pImpl->synth ); // Also, save effects latest state to value tree.
            getInserts();
        }
    }
}
