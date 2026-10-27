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
#define MI_NUM_BUTTONS ( 4 )

    struct TabData
    {
        juce::String                              tabName{ "" };
        int                                       multiTabCount{ 1 };
        std::vector<KickPadButton::PadButtonType> multiTabButtonTypes;
    };

    struct MixerHandlerInterface : public juce::Component
    {
        MixerHandlerInterface( MixerTabView* View )
            : view( View )
            , pads( view->getPads() )
            , io( view->getIO() )
        {
            for( auto i = 0; i < MI_NUM_BUTTONS; ++i )
            {
                buttons[i].setCornerRadius( 0.f );
                buttons[i].showBorder( true );
                buttons[i].setColours( ui::Theme::outlineColour( 3 ), ui::Theme::outlineColour( 3 ), ui::Theme::backgroundColour( 9 ), 1 );
            }
        }
        virtual ~MixerHandlerInterface() = default;

        std::vector<TabData> getTabData() { return tabData; };

        // Abstract functions...
        virtual std::vector<juce::Component*> getIoOptions1()  = 0;
        virtual std::vector<juce::String>     getInputs()      = 0;
        virtual std::vector<juce::String>     getOutputs()     = 0;
        virtual void                          refreshContent() = 0;

        // Virtual functions...
        virtual void                      handlePadParameterChange( /*PadData*/ ){};
        virtual void                      setCurrentProgram( const mt::MTIProgramData& /*currentProgram*/ ) {}
        virtual juce::Point<float>        getLevelRange() { return juce::Point<float>( -100, 0 ); }
        virtual std::vector<float>        getLevelDisplay() { return { 0.0f, -3.0f, -9.0f, -20.0f, -30.0f, -100.0f }; }
        virtual float                     getLevelCenter() { return -9; }
        virtual std::vector<juce::String> getButtonLabels() { return {}; }

        // Refresh Mixer contents...
        virtual void onLoad()
        {
            refreshContent();
            pads.setSelected( selectedPad, juce::sendNotification );
        }

        void visibilityChanged() override
        {
            bool visible = isVisible();
            for( auto it : getChildren() )
                it->setVisible( visible );

            if( visible )
                onLoad();
        }

        MixerTabView*                view{ nullptr };
        mt::MTAudioInterface&        ae{ mt::getAudioInterface() };
        KickPadButtonGroupComponent& pads;
        KickIOComponent&             io;

        int                  selectedPad{ 0 };
        KickToggleButton     buttons[MI_NUM_BUTTONS];
        std::vector<TabData> tabData;
    };
}
