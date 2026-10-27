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

//
// This file is exact adaptation from juce::CachedValue; except for ability to add listeners
// to notify whenever underlying value changes.
//

namespace mt
{
    //==============================================================================
    /**
    This class acts as a typed wrapper around a property inside a ValueTree.

    A CachedValue provides an easy way to read and write a ValueTree property with
    a chosen type. So for example a CachedValue<int> allows you to read or write the
    property as an int, and a CachedValue<String> lets you work with it as a String.

    It also allows efficient access to the value, by caching a copy of it in the
    type that is being used.

    You can give the CachedValue an optional UndoManager which it will use when writing
    to the underlying ValueTree.

    If the property inside the ValueTree is missing, the CachedValue will automatically
    return an optional default value, which can be specified when initialising the CachedValue.

    To create one, you can either use the constructor to attach the CachedValue to a
    ValueTree, or can create an uninitialised CachedValue with its default constructor and
    then attach it later with the referTo() methods.

    Common types like String, int, double which can be easily converted to a var should work
    out-of-the-box, but if you want to use more complex custom types, you may need to implement
    some template specialisations of VariantConverter which this class uses to convert between
    the type and the ValueTree's internal var.

    @tags{DataStructures}
*/
    template<typename Type>
    class CachedValue : private juce::ValueTree::Listener
    {
    public:
        sigslot::signal<> onValueChange = {};

        //==============================================================================
        // Default constructor.
        // Creates a default CachedValue not referring to any property. To initialise the  object, call one of the referTo() methods.
        CachedValue()
        {
            targetTree.addListener( this );
        }

        /** Constructor.

        Creates a default Cached Value referring to a Value property inside a ValueTree,
        and specifies a fallback value to use if the property does not exist.

        @param tree          The ValueTree containing the property
        @param propertyID    The identifier of the property
        @param undoManager   The UndoManager to use when writing to the property
        @param defaultToUse  The fallback default value to use.
        */
        CachedValue( const juce::ValueTree& tree, const juce::Identifier& propertyID, juce::UndoManager* um, const Type& defaultToUse = Type() )
        {
            targetTree.addListener( this );
            referTo( tree, propertyID, um, defaultToUse );
        }

        //==============================================================================
        /** Returns the current value of the property. If the property does not exist, returns the fallback default value.

        This is the same as calling get().
        */
        operator Type() const noexcept { return cachedValue; }

        /** Returns the current value of the property. If the property does not exist, returns the fallback default value. */
        Type get() const noexcept { return cachedValue; }

        /** Dereference operator. Provides direct access to the property.  */
        const Type& operator*() const noexcept { return cachedValue; }

        /** Dereference operator. Provides direct access to members of the property if it is of object type. */
        const Type* operator->() const noexcept { return &cachedValue; }

        /** Returns true if the current value of the property (or the fallback value) is equal to other. */
        template<typename OtherType>
        bool operator==( const OtherType& other ) const { return cachedValue == other; }

        /** Returns true if the current value of the property (or the fallback value) is not equal to other. */
        template<typename OtherType>
        bool operator!=( const OtherType& other ) const { return !operator==( other ); }

        //==============================================================================
        /** Returns the current property as a Value object. */
        juce::Value getPropertyAsValue() { return targetTree.getPropertyAsValue( targetProperty, undoManager ); }

        /** Returns true if the current property does not exist and the CachedValue is using the fallback default value instead. */
        bool isUsingDefault() const { return !targetTree.hasProperty( targetProperty ); }

        /** Returns the current fallback default value. */
        Type getDefault() const { return defaultValue; }

        //==============================================================================
        /** Sets the property. This will actually modify the property in the referenced ValueTree. */
        CachedValue& operator=( const Type& newValue )
        {
            setValue( newValue, undoManager );
            return *this;
        }

        /** Sets the property. This will actually modify the property in the referenced ValueTree. */
        void setValue( const Type& newValue, juce::UndoManager* undoManagerToUse )
        {
            if( !juce::exactlyEqual( cachedValue, newValue ) || isUsingDefault() )
            {
                beginTransactionForUndo();
                targetTree.setProperty( targetProperty, juce::VariantConverter<Type>::toVar( newValue ), undoManagerToUse );
            }
            forceUpdateOfCachedValue();
        }

        /** Removes the property from the referenced ValueTree and makes the CachedValue return the fallback default value instead. */
        void resetToDefault() { resetToDefault( undoManager ); }

        /** Removes the property from the referenced ValueTree and makes the CachedValue return the fallback default value instead. */
        void resetToDefault( juce::UndoManager* undoManagerToUse )
        {
            targetTree.removeProperty( targetProperty, undoManagerToUse );
            forceUpdateOfCachedValue();
        }

        /** Resets the fallback default value. */
        void setDefault( const Type& value ) { defaultValue = value; }

        //==============================================================================
        /** Makes the CachedValue refer to the specified property inside the given ValueTree, and specifies a fallback value to use if the property does not exist. */
        void referTo( const juce::ValueTree& tree, const juce::Identifier& property, juce::UndoManager* um, const Type& defaultVal = Type() )
        {
            referToWithDefault( tree, property, um, defaultVal );
        }

        /** Force an update in case the referenced property has been changed from elsewhere.

        Note: The CachedValue is a ValueTree::Listener and therefore will be informed of changes of the referenced property
        anyway (and update itself). But this may happen asynchronously. forceUpdateOfCachedValue() forces an update immediately. */
        void forceUpdateOfCachedValue()
        {
            cachedValue = getTypedValue();
            onValueChange();
        }

        //==============================================================================
        /** Returns a reference to the ValueTree containing the referenced property. */
        juce::ValueTree& getValueTree() noexcept { return targetTree; }

        /** Returns the property ID of the referenced property. */
        const juce::Identifier& getPropertyID() const noexcept { return targetProperty; }

        /** Returns the UndoManager that is being used. */
        juce::UndoManager* getUndoManager() noexcept { return undoManager; }

    private:
        //==============================================================================
        juce::ValueTree    targetTree{ "Temp" };
        juce::Identifier   targetProperty{ "Dummy" };
        juce::UndoManager* undoManager = nullptr;
        Type               defaultValue;
        Type               cachedValue;

        //==============================================================================
        void referToWithDefault( const juce::ValueTree& v, const juce::Identifier& i, juce::UndoManager* um, const Type& defaultVal )
        {
            targetTree     = v;
            targetProperty = i;
            undoManager    = um;
            defaultValue   = defaultVal;

            auto prevValue = cachedValue;
            cachedValue    = getTypedValue();
            if( prevValue != cachedValue )
                onValueChange();
        }

        void beginTransactionForUndo()
        {
            if( undoManager )
                undoManager->beginNewTransaction();
        }

        Type getTypedValue() const
        {
            if( const juce::var* property = targetTree.getPropertyPointer( targetProperty ) )
                return juce::VariantConverter<Type>::fromVar( *property );

            return defaultValue;
        }

        void valueTreePropertyChanged( juce::ValueTree& changedTree, const juce::Identifier& changedProperty ) override
        {
            if( changedProperty == targetProperty && targetTree == changedTree )
                forceUpdateOfCachedValue();
        }

        //==============================================================================
        JUCE_DECLARE_WEAK_REFERENCEABLE( CachedValue )
        JUCE_DECLARE_NON_COPYABLE( CachedValue )
    };

    //==============================================================================

} // namespace mt
