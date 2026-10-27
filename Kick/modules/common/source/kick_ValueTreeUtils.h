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

class ValueTreeAllEventListener : public juce::ValueTree::Listener
{
public:
    virtual void valueTreeChanged() = 0;

private:
    //==============================================================================
    void valueTreePropertyChanged( juce::ValueTree&, const juce::Identifier& ) override { valueTreeChanged(); }
    void valueTreeChildAdded( juce::ValueTree&, juce::ValueTree& ) override { valueTreeChanged(); }
    void valueTreeChildRemoved( juce::ValueTree&, juce::ValueTree&, int ) override { valueTreeChanged(); }
    void valueTreeChildOrderChanged( juce::ValueTree&, int, int ) override { valueTreeChanged(); }
    void valueTreeParentChanged( juce::ValueTree& ) override { valueTreeChanged(); }
    void valueTreeRedirected( juce::ValueTree& ) override { valueTreeChanged(); }
};

template<typename... Properties>
void addVTreeProperties( juce::ValueTree& v, const juce::Identifier& name, const juce::var& value, Properties&&... properties )
{
    static_assert( ( sizeof...( properties ) & 1 ) == 0, "The property list must be a sequence of name, value pairs" );

    v.setProperty( name, value, nullptr );

    if constexpr( sizeof...( properties ) != 0 )
        addVTreeProperties( v, std::forward<Properties>( properties )... );
}

template<typename... Properties>
juce::ValueTree createVTree( const juce::Identifier& name, Properties&&... properties )
{
    static_assert( ( sizeof...( properties ) & 1 ) == 0, "The property list must be a sequence of name, value pairs" );

    juce::ValueTree v( name );

    if constexpr( sizeof...( properties ) != 0 )
        addVTreeProperties( v, std::forward<Properties>( properties )... );
    return v;
}

template<typename Function>
static void forEachChildOfTypeInVTree( juce::ValueTree& tree, const juce::Identifier& Type, const Function function )
{
    for( auto child : tree )
    {
        if( Type == child.getType() )
            function( child );
    }
}

template<typename Function>
static void forEachChildInVTree( juce::ValueTree& tree, const Function function )
{
    for( auto child : tree )
        function( child );
}

template<typename Function>
static void forEachItemInVTree( juce::ValueTree& tree, const Function function )
{
    function( tree );
#if defined( __APPLE_CPP__ ) || defined( __APPLE_CC__ ) || defined( JUCE_MAC )
    for( auto child : tree )
#else
    for( auto& child : tree )
#endif
        forEachItemInVTree( child, function );
}

template<typename T>
static juce::var VectorToJuceVar( const std::vector<T>& vec )
{
    juce::MemoryBlock mb( vec.data(), vec.size() * sizeof( T ) );
    return juce::var( mb );
}

template<typename T>
static std::vector<T> JuceVarToVector( const juce::var& var )
{
    if( var.isBinaryData() )
    {
        auto           mb = var.getBinaryData();
        juce::Array<T> arr( static_cast<T*>( mb->getData() ),
                            static_cast<int>( mb->getSize() ) / sizeof( T ) );

        std::vector<T> vec;
        vec.resize( arr.size() );

        for( size_t i = 0; i < vec.size(); ++i )
        {
            vec[i] = arr[static_cast<int>( i )];
        }

        return vec;
    }

    return {};
}
