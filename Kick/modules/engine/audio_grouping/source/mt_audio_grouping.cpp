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

#include "mt_audio_grouping.h"

namespace mt{
    MTGrouping::MTGrouping( juce::ValueTree vt ) : vTree( vt ){}

    MTGrouping::~MTGrouping(){}

    void MTGrouping::setVT( juce::ValueTree vt ){
        vTree = vt;
    }

    MTGroup MTGrouping::convert( juce::ValueTree& V ){
        MTGroup group{};
        if( V.hasType( IDs::GROUP ) ){
            group.id = V[IDs::id];
            group.name = V[IDs::name];
            for( auto it : V ){
                if( it.hasType( IDs::member ) )
                    group.members.addIfNotAlreadyThere( it[IDs::id] );
            }
        }
        return group;
    }

    juce::ValueTree MTGrouping::convert( MTGroup& Group ){
        juce::ValueTree vt( IDs::GROUP, { {IDs::id, Group.id},{IDs::name, Group.name} } );
        for( auto it : Group.members ){
            juce::ValueTree m( IDs::member, { {IDs::id, it} } );
            vt.addChild( m, -1, nullptr );
        }
        return vt;
    }

    const juce::Array<MTGroup> MTGrouping::getGroups(){
        juce::Array<MTGroup> info;
        doForGroups( [&]( juce::ValueTree V )                 {
            info.add( convert( V ) );
            } );
        return info;
    }

    juce::ValueTree MTGrouping::getGroup( int Id ){
        for( auto v : vTree )    {
            if( v.hasType( IDs::GROUP ) )
                if( (int)v[IDs::id] == Id )
                    return v;
        }
        // Create node, if it is not present....
        juce::ValueTree vt( IDs::GROUP, { {IDs::id, Id},{IDs::name, ""} } );
        vTree.addChild( vt, -1, um );
        return vt;
    }

    //juce::ValueTree MTGrouping::getGroupMember( int , int  )
    juce::ValueTree MTGrouping::getGroupMember( int Id, int Member ){
        auto g = getGroup( Id );
        for( auto v : g ){
            if( v.hasType( IDs::member ) )
                if( (int)v[IDs::id] == Member )
                    return v;
        }
        return {};
    }

    juce::ValueTree MTGrouping::getGroupForMember( int Member ){
        for( auto v : vTree )    {
            if( v.hasType( IDs::GROUP ) )        {
                auto m = v.getChildWithProperty( IDs::id, Member );
                if( m.hasType( IDs::member ) )
                    return v;
            }
        }
        return {};
    }

    void MTGrouping::renameGrouping( const juce::String& Name ){
        name = Name;
    }

    int MTGrouping::renameGroup( int Id, const juce::String& Name ){
        returnIfNotValidId( Id );

        auto vt = getGroup( Id );
        vt.setProperty( IDs::name, Name, um );

        return 0;
    }

    int MTGrouping::addToGroup( int Id, int Member ){
        returnIfNotValidId( Id );

        auto vt = getGroupMember( Id, Member );
        if( !vt.hasType( IDs::member ) ){
            juce::ValueTree m( IDs::member, { {IDs::id, Member} } );
            getGroup( Id ).addChild( m, -1, um );
        }

        return 0;
    }

    int MTGrouping::removeFromGroup( int Id, int Member ){
        returnIfNotValidId( Id );

        auto vt = getGroupMember( Id, Member );
        if( vt.hasType( IDs::member ) ){
            getGroup( Id ).removeChild( vt, um );
        }

        return 0;
    }

    int MTGrouping::removeFromAllGroups( int Member ){
        doForGroups( [&]( juce::ValueTree V )                 {
            removeFromGroup( V[IDs::id], Member );
            } );
        return 0;
    }

    bool MTGrouping::isPartOfGroup( int Member ){
        auto v = getGroupForMember( Member );
        if( v.hasType( IDs::GROUP ) )
            return true;
        return false;
    }

    void MTGrouping::doForGroups( std::function<void( juce::ValueTree VT )> callThis ){
        for( auto v : vTree ){
            if( v.hasType( IDs::GROUP ) ){
                if( callThis ) callThis( v );
            }
        }
    }

    void MTGrouping::doForGroupMembers( int Member, std::function<void( int Member )> callThis ){
        auto g = getGroupForMember( Member );
        if( callThis && g.hasType( IDs::GROUP ) ){
            for( auto it : g ){
                if( it.hasType( IDs::member ) )
                    callThis( it[IDs::id] );
            }
        }
    }

    juce::Array<int> MTGrouping::getGroupedMembers( int Member ){
        juce::Array<int> m;
        auto g = getGroupForMember( Member );
        if( g.hasType( IDs::GROUP ) ){
            for( auto it : g )
                m.addIfNotAlreadyThere( it[IDs::id] );
        }
        return m;
    }
}
