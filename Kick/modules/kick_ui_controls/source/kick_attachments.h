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

namespace ui
{
    using Value = juce::Value;

    template<typename DataType>
    class AbstractAttachmentBase : private Value::Listener
    {
    public:
        // Callback...
        std::function<void()>               onAttachedValueChanged{ nullptr };
        std::function<DataType( DataType )> boundsChecker{ nullptr };

        // Setup change listenets...
        explicit AbstractAttachmentBase()
        {
            // Setup change listenets...
            value.addListener( this );
        }

        // Virtual Methods to get/set values in `Control`
        virtual DataType get() = 0;
        virtual void     set() = 0;

        // Getter & Setter
        juce::var getAttachedValue() { return value.getValue(); }
        void      setAttachedValue( juce::var V ) { value.setValue( V ); }

        // Attachments...
        void attachTo( const Value& ID, std::function<DataType( DataType )> Checker = nullptr )
        {
            boundsChecker = Checker;
            value.referTo( ID );
            set();
        }

        void removeAttachement() { attachTo( Value( juce::var( UNKNOWN_VALUE ) ) ); }
        void resetAttachment( const Value& ID ) { attachTo( ID ); }

    private:
        void valueChanged( Value& V ) override
        {
            auto v = V.getValue();
            if( !v.equalsWithSameType( get() ) )
            {
                set();
            }
        }

    private:
        Value value{ juce::var( UNKNOWN_VALUE ) };
    };

    class SliderAttachment : public AbstractAttachmentBase<double>
        , private KickSlider::Listener
    {
    public:
        explicit SliderAttachment( KickSlider& Parameter )
            : param( Parameter ), AbstractAttachmentBase()
        {
            param.addListener( this );
        }

        ~SliderAttachment() { param.removeListener( this ); }

        double get() override
        {
            auto val = static_cast<double>( this->param.getValue() );
            if( boundsChecker ) val = boundsChecker( val );
            return val;
        }

        void set() override
        {
            auto val = static_cast<double>( getAttachedValue() );
            if( boundsChecker ) val = boundsChecker( val );
            this->param.setValue( val, juce::dontSendNotification );
        }

    private:
        void sliderValueChanged( juce::Slider* S ) override
        {
            setAttachedValue( get() ); // Notify
            if( this->onAttachedValueChanged ) this->onAttachedValueChanged();
        }

        KickSlider& param;
    };

    class ToggleOptionsAttachment : public AbstractAttachmentBase<int>
        , private KickToggleOptions::Listener
    {
    public:
        explicit ToggleOptionsAttachment( KickToggleOptions& Parameter )
            : param( Parameter ), AbstractAttachmentBase()
        {
            param.addListener( this );
        }

        ~ToggleOptionsAttachment() { param.removeListener( this ); }

        int  get() override { return static_cast<int>( this->param.getSelected() ); }
        void set() override { this->param.setSelected( std::forward<int>( getAttachedValue() ), juce::dontSendNotification ); }

    private:
        void toggleOptionsChanged( KickToggleOptions* N ) override
        {
            setAttachedValue( get() ); // Notify
            if( this->onAttachedValueChanged ) this->onAttachedValueChanged();
        }

        KickToggleOptions& param;
    };

    class ButtonAttachment : public AbstractAttachmentBase<bool>
        , private juce::Button::Listener
    {
    public:
        explicit ButtonAttachment( juce::Button& Parameter )
            : param( Parameter ), AbstractAttachmentBase()
        {
            param.addListener( this );
        }

        ~ButtonAttachment() { param.removeListener( this ); }

        bool get() override { return static_cast<bool>( this->param.getToggleState() ); }
        void set() override { this->param.setToggleState( std::forward<bool>( getAttachedValue() ), juce::dontSendNotification ); }

    private:
        void buttonClicked( juce::Button* B ) override
        {
            setAttachedValue( get() ); // Notify
            if( this->onAttachedValueChanged ) this->onAttachedValueChanged();
        }

        juce::Button& param;
    };

    template<typename Type>
    class NumberAttachment : public AbstractAttachmentBase<Type>
        , private KickNumberT<Type>::Listener
    {
    public:
        explicit NumberAttachment( KickNumberT<Type>& Parameter )
            : param( Parameter ), AbstractAttachmentBase<Type>()
        {
            param.addListener( this );
        }

        ~NumberAttachment() { param.removeListener( this ); }

        Type get() override
        {
            auto val = static_cast<Type>( this->param.getValue() );
            if( this->boundsChecker ) val = this->boundsChecker( val );
            return val;
        }

        void set() override
        {
            auto val = static_cast<Type>( this->getAttachedValue() );
            if( this->boundsChecker ) val = this->boundsChecker( val );
            this->param.setValue( val, juce::dontSendNotification );
        }

    private:
        void numberValueChanged( KickNumberT<Type>* N ) override
        {
            this->setAttachedValue( get() ); // Notify
            if( this->onAttachedValueChanged ) this->onAttachedValueChanged();
        }

        KickNumberT<Type>& param;
    };

    class TempoAttachment : public AbstractAttachmentBase<int>
        , private KickTempo::Listener
    {
    public:
        explicit TempoAttachment( KickTempo& Parameter )
            : param( Parameter ), AbstractAttachmentBase<int>()
        {
            param.addListener( this );
        }

        ~TempoAttachment() { param.removeListener( this ); }

        int get() override
        {
            auto val = this->param.getValue();
            if( this->boundsChecker ) val = this->boundsChecker( val );
            return val;
        }

        void set() override
        {
            auto val = static_cast<int>( this->getAttachedValue() );
            if( this->boundsChecker ) val = this->boundsChecker( val );
            this->param.setValue( val, juce::dontSendNotification );
        }

    private:
        void tempoChanged( KickTempo* T ) override
        {
            this->setAttachedValue( get() ); // Notify
            if( this->onAttachedValueChanged ) this->onAttachedValueChanged();
        }

        KickTempo& param;
    };

    class LabelAttachment : public AbstractAttachmentBase<juce::String>
        , private juce::Label::Listener
    {
    public:
        explicit LabelAttachment( juce::Label& Parameter )
            : param( Parameter ), AbstractAttachmentBase()
        {
            param.addListener( this );
        }

        ~LabelAttachment() { param.removeListener( this ); }

        juce::String get() override { return this->param.getText(); }
        void         set() override { this->param.setText( std::forward<juce::String>( getAttachedValue() ), juce::dontSendNotification ); }

    private:
        void labelTextChanged( juce::Label* labelThatHasChanged ) override
        {
            setAttachedValue( get() ); // Notify
            if( this->onAttachedValueChanged ) this->onAttachedValueChanged();
        }

        juce::Label& param;
    };

}
