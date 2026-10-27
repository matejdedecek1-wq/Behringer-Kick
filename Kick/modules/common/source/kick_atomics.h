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

    ///
    /// Property value source, Juce Internal implementation does not provide ability to return value with default.
    ///
    struct VTPropertyValueSource
        : public juce::Value::ValueSource
        , private juce::ValueTree::Listener
    {
        VTPropertyValueSource( const juce::ValueTree& vt, const juce::Identifier& prop, juce::UndoManager* um, bool sync, juce::var defValue )
            : tree( vt ), property( prop ), undoManager( um ), updateSynchronously( sync ), defaultValue( defValue )
        {
            tree.addListener( this );
        }

        ~VTPropertyValueSource() override
        {
            tree.removeListener( this );
        }

        juce::var    getValue() const override { return tree.getProperty( property, defaultValue ); }
        virtual void setValue( const juce::var& newValue ) { tree.setProperty( property, newValue, undoManager ); }

    private:
        juce::ValueTree          tree;
        const juce::Identifier   property;
        juce::UndoManager* const undoManager;
        juce::var                defaultValue;
        const bool               updateSynchronously;

        void valueTreePropertyChanged( juce::ValueTree& changedTree, const juce::Identifier& changedProperty ) override
        {
            if( tree == changedTree && property == changedProperty )
                sendChangeMessage( updateSynchronously );
        }

        void valueTreeChildAdded( juce::ValueTree&, juce::ValueTree& ) override {}
        void valueTreeChildRemoved( juce::ValueTree&, juce::ValueTree&, int ) override {}
        void valueTreeChildOrderChanged( juce::ValueTree&, int, int ) override {}
        void valueTreeParentChanged( juce::ValueTree& ) override {}
    };

    ///
    /// Ranged Property value source. Extends earlier implementation with ability to set value range.
    ///
    struct VTRangedPropertyValueSource
        : public VTPropertyValueSource
    {
        VTRangedPropertyValueSource( const juce::ValueTree& vt, const juce::Identifier& prop, juce::UndoManager* um, bool sync, juce::var defValue, juce::var minValue, juce::var maxValue )
            : VTPropertyValueSource( vt, prop, um, sync, defValue ), min( minValue ), max( maxValue )
        {}

        void setValue( const juce::var& newValue ) override
        {
            auto value = juce::jlimit( min, max, newValue );
            VTPropertyValueSource::setValue( value );
        }

    private:
        juce::var min;
        juce::var max;
    };

    ///
    /// Synchronizer
    ///
    template<typename Type>
    struct AtomicValueSynchronizer : public juce::Value::Listener
    {
        std::atomic<Type>&     atomic;
        std::function<void()>& valueChangeCallback;

        explicit AtomicValueSynchronizer( std::atomic<Type>& AtomicVar, std::function<void()>& onValueChanged )
            : atomic( AtomicVar ), valueChangeCallback( onValueChanged ) {}

        void valueChanged( juce::Value& value ) override
        {
            atomic = juce::VariantConverter<Type>::fromVar( value.getValue() );
            valueChangeCallback();
        }
    };

    ///
    /// Synchronizer
    ///
    struct StringValueSynchronizer : public juce::Value::Listener
    {
        juce::String&          atomic;
        std::function<void()>& valueChangeCallback;

        explicit StringValueSynchronizer( juce::String& AtomicVar, std::function<void()>& onValueChanged )
            : atomic( AtomicVar ), valueChangeCallback( onValueChanged ) {}

        void valueChanged( juce::Value& value ) override
        {
            atomic = value.getValue().toString();
            valueChangeCallback();
        }
    };

    ///
    /// Atomic ( reference to attached juce::Value)
    ///
    template<typename Type>
    class Atomic
    {
    public:
        std::function<void()> onValueChanged = []() {};

        Atomic()
        {
            value.addListener( &sync );
        }

        void attachTo( juce::ValueTree& Tree, juce::Identifier Property, juce::UndoManager* Undo = nullptr, juce::var DefaultValue = {} )
        { // Setup initial values...
            value.referTo( juce::Value( new VTPropertyValueSource( Tree, Property, Undo, true, DefaultValue ) ) );
            forceUpdate();
        }

        void attachTo( juce::ValueTree& Tree, juce::Identifier Property, juce::UndoManager* Undo, juce::var DefaultValue, juce::var MinValue, juce::var MaxValue )
        { // Setup initial values...
            value.referTo( juce::Value( new VTRangedPropertyValueSource( Tree, Property, Undo, true, DefaultValue, MinValue, MaxValue ) ) );
            forceUpdate();
        }

        void attachTo( juce::Value ID )
        { // Setup initial values...
            value.referTo( ID );
            forceUpdate();
        }

        void removeAttachement()
        { // Remove change listeners...
            value.referTo( juce::Value() );
            store( (Type)0 );
        }

        void forceUpdate()
        {
            sync.valueChanged( value );
        }

        // Return the current value of the property.
        Type get() const { return atomic.load(); }
        Type load() const { return atomic.load(); }
        Type operator()() const noexcept { return atomic.load(); }

        // Set value of property.
        void set( Type Val ) { value.setValue( juce::VariantConverter<Type>::toVar( Val ) ); }
        void store( Type Val ) { value.setValue( juce::VariantConverter<Type>::toVar( Val ) ); }

        // Returns internal state
        juce::Value getValue() { return value; }

        void operator=( const Type Value ) noexcept { store( Value ); }
        bool operator==( const Atomic& other ) const noexcept { return atomic.load() == other.atomic.load(); }
        bool operator!=( const Atomic& other ) const noexcept { return atomic.load() != other.atomic.load(); }
        Type operator++() noexcept
        {
            ++atomic;
            store( atomic.load() );
            return atomic.load();
        }
        Type operator--() noexcept
        {
            --atomic;
            store( atomic.load() );
            return atomic.load();
        }

    private:
        std::atomic<Type>             atomic{ Type( 0 ) };
        juce::Value                   value;
        AtomicValueSynchronizer<Type> sync{ atomic, onValueChanged };
    };
    //

    ///
    /// AtomicString ( reference to attached juce::Value)
    ///
    class AtomicString
    {
    public:
        std::function<void()> onValueChanged = []() {};

        AtomicString()
        {
            value.addListener( &sync );
        }
        ~AtomicString() = default;

        void attachTo( juce::ValueTree& Tree, juce::Identifier Property, juce::UndoManager* Undo = nullptr, juce::var DefaultValue = { "" } )
        { // Setup initial values...
            value.referTo( juce::Value( new VTPropertyValueSource( Tree, Property, Undo, true, DefaultValue ) ) );
        }

        void attachTo( juce::Value ID )
        { // Setup initial values...
            value.referTo( ID );
        }

        void removeAttachement()
        { // Remove change listeners...
            value.referTo( juce::Value() );
            store( "" );
        }

        // Return the current value of the property.
        const juce::String& get() const { return atomic; }
        const juce::String& load() const { return atomic; }
        const juce::String& operator()() const noexcept { return atomic; }

        // Set value of property.
        void set( juce::String Val ) { value.setValue( Val ); }
        void store( juce::String Val ) { value.setValue( Val ); }

        // Returns internal state
        juce::Value getValue() { return value; }

        void operator=( const juce::String Value ) noexcept { store( Value ); }
        bool operator==( const AtomicString& other ) const noexcept { return atomic == other.atomic; }
        bool operator!=( const AtomicString& other ) const noexcept { return atomic != other.atomic; }

    private:
        juce::String            atomic;
        juce::Value             value;
        StringValueSynchronizer sync{ atomic, onValueChanged };
    };

}
