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
    void         setProgramIcon( ui::KickImageButton& Prog, mt::MTProgramType Type );
    juce::String getProgramIcon( mt::MTProgramType Type );
    juce::String getDisplayProgramIcon( mt::MTProgramType Type );
    juce::String getEditProgramIcon( mt::MTProgramType Type );

#define NUM_OF_PROGRAMS 7

    //////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////    Header Bar - Program Selector
    //////////////////////////////////////////////////////////////////////////////////////////
    class KickProgramSelector : public juce::Component
    {
    public:
        KickProgramSelector( const juce::String& Title = "SELECT PROGRAM" );
        ~KickProgramSelector();

        const juce::String&                                    getSelectedProgram();
        const mt::MTProgramType                                getSelectedProgramType();
        void                                                   setValues( const juce::String& Program, const mt::MTProgramType Type );
        void                                                   refreshContent();
        std::unique_ptr<ui::KickTableView>                     table;
        mt::MTProgramType                                      getprogramType( int Index );
        std::map<mt::MTProgramType, std::vector<juce::String>> programs;
        void                                                   updateRowSelection( bool basedOnTrackProgram = true );

    private:
        bool                      isBasedOnTrackProgram{ true };
        juce::Rectangle<int>      tableBounds = { 10, 8, 342, 445 };
        ui::KickTableViewProvider provider;
        ui::KickList              progList{ "" };

        ui::KickImageButton   image;
        mt::MTAudioInterface& ae{ mt::getAudioInterface() };
        juce::String          selectedProgram{ "" };
        mt::MTProgramType     selectedType{ mt::MTProgramType::Unknown }, tempType{ mt::MTProgramType::Unknown };

        void paint( juce::Graphics& g ) override;
        void resized() override;
        void visibilityChanged() override;

        void setupListContent( std::vector<juce::String>& Options );
    };

    class ProgramSelectorDisplay : public juce::Component
    {
    public:
        ProgramSelectorDisplay( int rowId );
        ~ProgramSelectorDisplay(){};
        void setProperties( const juce::String& image, const juce::String& labeltext );
        void setDisableComp( bool );

    private:
        ui::KickImageButton imagebutton;
        ui::KickLabel       label;
        void                paint( juce::Graphics& g ){};
        void                resized();
    };

    class KickProgramSelectorWithDisplay : public juce::Component
    {
    public:
        std::function<void( const juce::String& Program, const mt::MTProgramType Type )> onSelectionChanged;

        KickProgramSelectorWithDisplay( bool ShowName = true );

        inline void showName();
        inline void showIcon();

        void setValues( const juce::String& Program, const mt::MTProgramType Type, juce::NotificationType Notification );
        void setValueBackgroundColor( juce::Colour colour ) { progLabel.setValueBackgroundColour( colour ); }

    private:
        void refresh();
        void resized() override;
        void visibilityChanged() override;

    private:
        bool                                 bShowName{ true };
        ui::KickNameLabel                    progLabel{ "" };
        ui::KickImageButton                  progIcon{ "Program" };
        std::unique_ptr<KickProgramSelector> selector;
    };

}
