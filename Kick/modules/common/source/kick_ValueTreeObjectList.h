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

#define ENABLE_ASSERTS ( 1 )

///
/// ValueTree Object List
///

//==============================================================================
template<typename ObjectType, typename CriticalSectionType = juce::DummyCriticalSection>
class ValueTreeObjectList : public juce::ValueTree::Listener
{
public:
    ValueTreeObjectList( const juce::ValueTree& parentTree )
        : parent( parentTree )
    {
        parent.addListener( this );
    }

    virtual ~ValueTreeObjectList()
    {
        jassert( objects.size() == 0 ); // must call freeObjects() in the subclass destructor!
    }

    void setValueTree( const juce::ValueTree& parentTree )
    {
        deleteAllObjects();
        parent = parentTree;
        rebuildObjects();
    }

    // call in the sub-class when being created
    void rebuildObjects()
    {
#if defined( ENABLE_ASSERTS ) && ( 0 < ENABLE_ASSERTS )
        jassert( objects.size() == 0 ); // must only call this method once at construction
#endif

        for( const auto& v : parent )
            if( isSuitableType( v ) )
                if( ObjectType* newObject = createNewObject( v ) )
                    objects.add( newObject );
    }

    // call in the sub-class when being destroyed
    void freeObjects()
    {
        parent.removeListener( this );
        deleteAllObjects();
    }

    ///
    /// ==============================================================================
    /// Pure virtual functions.
    ///
    virtual bool        isSuitableType( const juce::ValueTree& ) const = 0;
    virtual ObjectType* createNewObject( const juce::ValueTree& )      = 0;
    virtual void        deleteObject( ObjectType* )                    = 0;

    virtual void newObjectAdded( ObjectType* ){};
    virtual void objectRemoved( ObjectType* ){};
    virtual void objectOrderChanged(){};

    ///
    /// ==============================================================================
    /// Implementation.
    ///
    inline int         size() const { return objects.size(); }
    inline bool        isEmpty() const noexcept { return size() == 0; }
    ObjectType*        operator[]( int idx ) const { return objects[idx]; }
    ObjectType*        at( int idx ) { return objects[idx]; }
    ObjectType**       begin() { return objects.begin(); }
    ObjectType* const* begin() const { return objects.begin(); }
    ObjectType**       end() { return objects.end(); }
    ObjectType* const* end() const { return objects.end(); }

    ///
    /// ==============================================================================
    /// Implementation.
    ///
    void valueTreeChildAdded( juce::ValueTree&, juce::ValueTree& tree ) override
    {
        if( isChildTree( tree ) )
        {
            const int index = parent.indexOf( tree );
            (void)index;

#if defined( ENABLE_ASSERTS ) && ( 0 < ENABLE_ASSERTS )
            jassert( index >= 0 );
#endif

            if( ObjectType* newObject = createNewObject( tree ) )
            {
                {
                    const ScopedLockType sl( arrayLock );

                    if( index == parent.getNumChildren() - 1 )
                        objects.add( newObject );
                    else
                        objects.addSorted( *this, newObject );
                }

                newObjectAdded( newObject );
            }
#if defined( ENABLE_ASSERTS ) && ( 0 < ENABLE_ASSERTS )
            else
                jassertfalse;
#endif
        }
    }

    void valueTreeChildRemoved( juce::ValueTree& exParent, juce::ValueTree& tree, int ) override
    {
        if( parent == exParent && isSuitableType( tree ) )
        {
            const int oldIndex = indexOf( tree );

            if( oldIndex >= 0 )
            {
                ObjectType* o;

                {
                    const ScopedLockType sl( arrayLock );
                    o = objects.removeAndReturn( oldIndex );
                }

                objectRemoved( o );
                deleteObject( o );
            }
        }
    }

    void valueTreeChildOrderChanged( juce::ValueTree& tree, int, int ) override
    {
        if( tree == parent )
        {
            {
                const ScopedLockType sl( arrayLock );
                sortArray();
            }

            objectOrderChanged();
        }
    }

    void valueTreePropertyChanged( juce::ValueTree&, const juce::Identifier& ) override {}
    void valueTreeParentChanged( juce::ValueTree& ) override {}
    void valueTreeRedirected( juce::ValueTree& ) override { jassertfalse; } // may need to add handling if this is hit

    juce::Array<ObjectType*> objects;
    CriticalSectionType      arrayLock;
    using ScopedLockType = typename CriticalSectionType::ScopedLockType;

protected:
    juce::ValueTree parent;

    void deleteAllObjects()
    {
        const ScopedLockType sl( arrayLock );

        while( objects.size() > 0 )
            deleteObject( objects.removeAndReturn( objects.size() - 1 ) );
    }

    bool isChildTree( juce::ValueTree& v ) const
    {
        return isSuitableType( v ) && v.getParent() == parent;
    }

    int indexOf( const juce::ValueTree& v ) const noexcept
    {
        for( int i = 0; i < objects.size(); ++i )
            if( objects.getUnchecked( i )->state == v )
                return i;

        return -1;
    }

    void sortArray()
    {
        objects.sort( *this );
    }

public:
    int compareElements( ObjectType* first, ObjectType* second ) const
    {
        int index1 = parent.indexOf( first->state );
        int index2 = parent.indexOf( second->state );

#if defined( ENABLE_ASSERTS ) && ( 0 < ENABLE_ASSERTS )
        jassert( index1 >= 0 && index2 >= 0 );
#endif
        return index1 - index2;
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( ValueTreeObjectList )
};
