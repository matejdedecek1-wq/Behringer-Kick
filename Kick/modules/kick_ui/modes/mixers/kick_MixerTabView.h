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
    /*
    * MixerTabWidget is a tab component used by all channel mixer tabs. the pad button displayed in a mixer tab is determined by padButtonType passed.
    * when ever a button value changes currentButtonChanged signal is emitted with ButtonData;
    */
    class MixerTabComponent : public juce::Component
    {
    public:
        MixerTabComponent();
        ~MixerTabComponent() = default;

        void setButtonType( int buttonRange, KickPadButton::PadButtonType type );
        void setButtonNames( const std::list<juce::String>& labels );
        void resized() override;

        KickPadButtonGroupComponent& getPads() { return padGroupComp; }
        KickIOComponent&             getIO() { return ioComp; }

    private:
        int                         program{ 0 };
        KickIOComponent             ioComp;
        KickPadButtonGroupComponent padGroupComp{ KickPadButton::PadButtonType::with1Row };

        void visibilityChanged() override;
    };

    struct MixerHandlerInterface;
    class MixerTabView : public KickTabbedComponent
    {
    public:
        MixerTabView( MixerHandlerInterface* hand = nullptr );
        virtual ~MixerTabView();

        void resetHandler( MixerHandlerInterface* hand );

        MixerTabComponent&           getTabView() { return tab; }
        KickPadButtonGroupComponent& getPads() { return tab.getPads(); }
        KickIOComponent&             getIO() { return tab.getIO(); }

    private:
        int                    currentIndex = 0;
        MixerHandlerInterface* handler      = nullptr;
        MixerTabComponent      tab;
        KickLabel              na{ "NotAvailable", "Not available for this program." };

        void visibilityChanged() override;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( MixerTabView )
    };
}
