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
    template<typename DataType, typename ControlType, typename AttachmentType>
    struct AbstractControlAttachmentBase : public ControlType
    {
        template<typename... Params>
        explicit AbstractControlAttachmentBase( Params&&... params )
            : ControlType( std::forward<Params>( params )... ) {}

        using BoundsChecker = std::function<DataType( DataType )>;
        void attachTo( juce::Value ID, BoundsChecker Checker = nullptr )
        {
            this->attachment.attachTo( std::forward<juce::Value>( ID ), std::forward<BoundsChecker>( Checker ) );
        }

        void removeAttachement() { this->attachment.removeAttachement(); }

        DataType getAttachedValue() { return (DataType)attachment->getAttachedValue(); }
        void     setAttachedValue( DataType Value ) { attachment->setAttachedValue( Value ); }

    private:
        AttachmentType attachment{ *this };
    };

    //
    // Kick Number
    //

    template<typename T>
    using KickNumberWithAttachedValue = AbstractControlAttachmentBase<T, KickNumberT<T>, NumberAttachment<T>>;

    using KickSliderWithAttachedValue = AbstractControlAttachmentBase<double, KickSlider, SliderAttachment>;
    using KickRotaryWithAttachedValue = AbstractControlAttachmentBase<double, KickRotary, SliderAttachment>;

    using KickToggleButtonWithAttachedValue = AbstractControlAttachmentBase<bool, KickToggleButton, ButtonAttachment>;
    using KickImageButtonWithAttachedValue  = AbstractControlAttachmentBase<bool, KickImageButton, ButtonAttachment>;

    using KickLabelWithAttachedValue          = AbstractControlAttachmentBase<juce::String, KickLabel, LabelAttachment>;
    using KickNameLabelWithAttachedValue      = AbstractControlAttachmentBase<juce::String, KickNameLabel, LabelAttachment>;
    using KickTitleLabelWithAttachedValue     = AbstractControlAttachmentBase<juce::String, KickTitleLabel, LabelAttachment>;
    using Kick2LineNameLabelWithAttachedValue = AbstractControlAttachmentBase<juce::String, Kick2LineNameLabel, LabelAttachment>;

    using KickTempoWithAttachedValue = AbstractControlAttachmentBase<int, KickTempo, TempoAttachment>;

    //
    // Kick Toggle Options
    //

    //using KickToggleOptionsWithAttachedValue = AbstractControlAttachmentBase<int, KickToggleOptions, ToggleOptionsAttachment>;
    class KickToggleOptionsWithAttachedValue : public KickToggleOptions
    {
    public:
        explicit KickToggleOptionsWithAttachedValue( const juce::String& Title = "", const std::vector<juce::String>& Options = {} )
            : KickToggleOptions( Title, Options )
            , attachment( *this ) {}

        void attachTo( juce::Value ID ) { attachment.resetAttachment( ID ); }
        void removeAttachement() { attachment.removeAttachement(); }

    private:
        ToggleOptionsAttachment attachment;
    };

}
