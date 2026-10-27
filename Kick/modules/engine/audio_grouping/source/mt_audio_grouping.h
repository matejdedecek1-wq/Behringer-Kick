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

namespace mt{
    constexpr int MinGroupIDs{ 1 };
    constexpr int MaxGroupIDs{ 16 };
#define returnIfNotValidId( Id ) if( MaxGroupIDs < Id || MinGroupIDs > Id ) return -1;

    class MTGrouping{
    public:
        //typedef std::function<void( MTGroup&, bool )> onGroupChanged;

        explicit MTGrouping( juce::ValueTree vt );
        ~MTGrouping();

        void setVT( juce::ValueTree vt );

        void renameGrouping( const juce::String& Name );
        const juce::String& getName(){ return name; }
        const juce::Array<MTGroup> getGroups();

        int renameGroup( int Id, const juce::String& Name );
        int addToGroup( int Id, int Member );
        int removeFromGroup( int Id, int Member );
        int removeFromAllGroups( int Member );

        bool isPartOfGroup( int Member );
        void doForGroupMembers( int Member, std::function<void( int Member )> callThis );

        juce::Array<int> getGroupedMembers( int Member );

        MTGroup convert( juce::ValueTree& VT );
        juce::ValueTree convert( MTGroup& Group );

    private:
        juce::ValueTree vTree{ IDs::GROUPING };
        juce::UndoManager* um{ nullptr };
        juce::String name;

        juce::ValueTree getGroup( int Id );
        juce::ValueTree getGroupMember( int Id, int Member );
        juce::ValueTree getGroupForMember( int Member );
        void doForGroups( std::function<void( juce::ValueTree VT )> callThis );
    };
} // namespace mt
