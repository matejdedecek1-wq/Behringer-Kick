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
namespace kick_ui
{
    class Looper
        : public juce::Component
        , public kick_ui::IScreen
    {
    public:
        Looper();

        void handleCommand( CPktMgr::HWMsg_t& ) override;
        bool requiresHandleCommand() override { return true; }

    private:
        bool requiresStatusBar() override { return false; }

        void resized() override;
        void visibilityChanged() override;
        void paint( juce::Graphics& ) override;

    private:
        ui::KickImageButton clearBtn{ " Clear", "Song_Clear", "Song_Clear_Active" };
        ui::KickImageButton exportBtn{ "Export", "Song_Export", "Song_Export_Active" };

        kick_ui::KickModeHeader header{ "Looper", "Icon_Looper" };
        ui::KickIOComponent     options;
        kick_ui::LoopRecorder   recorder{ options };

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( Looper )
    };

    class looperDialog
        : public juce::Component
    {
    public:
        looperDialog()
        {
            setOpaque( true );
            setName( "Save Looper File" );
            setBounds( ui::layout::screen );
        }

        ~looperDialog()
        {
            DBG( "Dialog Destroyed" );
            removeAllChildren();
        }

    private:
        bool                 showYesNoButtons{ false };
        juce::Component*     pCont{ nullptr };
        juce::Rectangle<int> contentBounds{ ui::layout::dialog }, buttonArea, contentArea;
        juce::Colour         contentBackgroundColour{ ui::Theme::backgroundColour( 5 ).withAlpha( .25f ) };
        ui::KickLabel        title{ "Looper Save File" };
        ui::KickButton       apply{ "Apply" }, cancel{ "Cancel" };

        void paint( juce::Graphics& g )
        {
            //g.fillAll( ui::Theme::backgroundColour( 9 ).withAlpha( .75f ) ); // Dim background...
            g.setColour( ui::Theme::backgroundColour( 9 ).withAlpha( .75f ) );
            g.fillRect( getLocalBounds() ); // Dim background...

            g.setColour( contentBackgroundColour );
            g.fillRect( contentArea );

            if( showYesNoButtons )
            {
                g.setColour( ui::Theme::backgroundColour( 3 ) );
                g.fillRect( buttonArea );
            }
        }

        void mouseDown( const juce::MouseEvent& e )
        {
            if( !pCont->getBounds().contains( e.getPosition() ) )
                mt::utils::exitDialog( this, UI::CANCEL );
        }

        void resized() override
        {
            auto b = contentBounds.constrainedWithin( getLocalBounds() );
            b.setCentre( getLocalBounds().getCentre() );

            // Setup Title
            auto bTitle = b.removeFromTop( 50 );
            // Setup content
            if( pCont )
            {
                contentArea = b;
                pCont->setBounds( contentArea );
                mt::utils::addAndMakeVisible( this, *pCont );
            }
        }
    };

}
