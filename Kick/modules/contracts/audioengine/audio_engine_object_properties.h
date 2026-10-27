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

#pragma once

#include "juce_core/juce_core.h"
#include "audio_engine_obiect_property_ids.h"

namespace contracts
{
    class AudioEngineObjectProperties
    {
    public:
        AudioEngineObjectProperties()
            : properties( new juce::HashMap<int, juce::var> ())
        {

        }

        ~AudioEngineObjectProperties()
        {
            properties =  nullptr;
        }

        AudioEngineObjectProperties( const AudioEngineObjectProperties& props )
            : properties( new juce::HashMap<int, juce::var>() )
        {
            properties = props.properties;
        }

        AudioEngineObjectProperties& operator=( const AudioEngineObjectProperties& props )
        {
            properties = props.properties;
            return *this;
        }
    public:
        //copy propeties from props to this.
        void copy( const AudioEngineObjectProperties& props)
        {
            properties.reset( new juce::HashMap<int, juce::var>() );
            for( auto itr = props.properties->begin(); itr != props.properties->end(); itr.next() )
            {
                properties->set( itr.getKey(), itr.getValue() );
            }
        }

        const juce::var& getProperty( const contracts::AudioEngineObjectPropertyId id )
        {
            if( properties->contains( (int)id ) )
                return properties->getReference( (int)id );
            return juce::var::undefined();
        }

        void setProperty( const contracts::AudioEngineObjectPropertyId id, const juce::var& property )
        {
            properties->set( (int)id, property );
        }

        juce::var toJsonVar()
        {
            juce::DynamicObject* pObj = new juce::DynamicObject();

            for( auto itr = properties->begin(); itr != properties->end(); itr.next() )
            {
                pObj->setProperty( juce::String( itr.getKey() ), itr.getValue() );
            }
            return pObj;
        }

        void parseJsonVar( const juce::var& jsonVar )
        {
            auto* pObj  = jsonVar.getDynamicObject();
            auto  props = pObj->getProperties();
            for( auto& item : props )
            {
                properties->set( item.name.toString().getIntValue(), item.value );
            }
        }

        void iterate( std::function<void( const contracts::AudioEngineObjectPropertyId&, const juce::var& )> callback )
        {
            if( !callback ) return;

            for( auto itr = properties->begin(); itr != properties->end(); itr.next() )
            {
                callback( (contracts::AudioEngineObjectPropertyId)itr.getKey(), itr.getValue() );
            }
        }

    private:
        std::shared_ptr<juce::HashMap<int, juce::var>> properties;
    };
}
