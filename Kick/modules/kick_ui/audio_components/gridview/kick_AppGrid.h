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
    /////////////////////////////////////////////////////////////////////
    // Grid Edit Options
    /////////////////////////////////////////////////////////////////////
    const static std::vector<juce::String> gridEditOptionList = { "Clear", "Double Length With Events", "Half Speed", "Double Speed", "Trim To Length", "Pitch Quantize", "Humanize", "Random Events", "Split Events", "Convert To Progression", "Bounce To Sample", "Bounce To Audio Track" };

    class GridEditOptions : public juce::Component
    {
    public:
        std::function<void( int )> onOptionClicked;
        GridEditOptions();
        void close();

    private:
        void resized() override;

        std::array<ui::KickImageButton, 12> options;

        ui::KickImageButton clear{ "Clear" };
        ui::KickImageButton doubleLength{ "Double Length With Events" };
        ui::KickImageButton halfSpeed{ "Half Speed" };
        ui::KickImageButton doubleSpeed{ "Double Speed" };
        ui::KickImageButton trimToLength{ "Trim To Length" };
        ui::KickImageButton pitchQuantize{ "Pitch Quantize" };
        ui::KickImageButton humanize{ "Humanize" };
        ui::KickImageButton randomEvents{ "Random Events" };
        ui::KickImageButton splitEvents{ "Split Events" };
        ui::KickImageButton convertToProgression{ "Convert To Progression" };
        ui::KickImageButton bounceToSample{ "Bounce To Sample" };
        ui::KickImageButton bounceToAudioTrack{ "Bounce To Audio Track" };
    };

    ///////////////////////////////////////////////////////////////////////
    // Grid Playhead
    ///////////////////////////////////////////////////////////////////////
    class GridPlayhead : public juce::Component
        , juce::AsyncUpdater
    {
    public:
        GridPlayhead();
        ~GridPlayhead() = default;

        void  drawAt( float X, int MaxWidth );
        float getPlayHeadPosition() const { return normX; }

    private:
        void handleAsyncUpdate() override;
        void paint( juce::Graphics& g ) override;

    private:
        float normX{ 0 };
    };

    class NotesView : public juce::Component
    {
    public:
        std::function<void( int SelectedNote, bool State )> noteSelectionChanged;
        enum class Type
        {
            Drum,
            Keys,
        };

        NotesView() = default;

        void setType( Type T )
        {
            noteType = T;
            repaint();
        }

        void paint( juce::Graphics& g ) override
        {
            auto b = getLocalBounds().toFloat();

            // Write markers...
            g.setColour( ui::Theme::backgroundColour( 5 ) );
            g.setFont( juce::jmin( 26.0f, ( 0.65f * noteHeight ) ) );
            int n = mt::ScaleAndNotes::maxPianoKeys;
            while( n >= 0 )
            {
                auto tArea    = b.removeFromTop( noteHeight );
                auto semiTone = n % 12;
                bool isBlack  = mt::Note::colours[semiTone];
                if( Type::Drum == noteType )
                {
                    g.setColour( isBlack ? blackColour : whiteColour );
                    g.fillRect( tArea );

                    g.setColour( ui::Theme::outlineColour( 5 ) );
                    g.drawRect( tArea.toFloat(), 0.6f );

                    g.setColour( isBlack ? whiteColour : blackColour );
                    auto drawArea = tArea.removeFromRight( (float)( tArea.getWidth() * .25f ) );
                    g.drawFittedText( mt::Kick::getPadName( n ), drawArea.toNearestInt(), juce::Justification::centred, 1 );
                }
                else
                {
                    if( isBlack )
                    {
                        auto bArea = tArea.withTrimmedRight( 0.25 * tArea.getWidth() );
                        auto wArea = tArea.withTrimmedLeft( 0.75 * tArea.getWidth() );
                        g.setColour( whiteColour );
                        g.fillRect( wArea );

                        g.setColour( blackColour );
                        g.fillRect( bArea );

                        g.setColour( ui::Theme::outlineColour( 5 ) );
                        g.drawHorizontalLine( wArea.getCentreY(), wArea.getX(), wArea.getX() + wArea.getWidth() );
                    }
                    else
                    {
                        g.setColour( whiteColour );
                        g.fillRect( tArea );

                        if( ( 4 == semiTone ) || ( 11 == semiTone ) )
                        {
                            g.setColour( ui::Theme::outlineColour( 5 ) );
                            g.drawHorizontalLine( ( tArea.getY() ), tArea.getX(), tArea.getX() + tArea.getWidth() );
                        }
                    }

                    if( 0 == semiTone )
                    {
                        g.setColour( isBlack ? whiteColour : blackColour );
                        g.drawFittedText( mt::ScaleAndNotes::midiNoteNames[n], tArea.toNearestInt(), juce::Justification::centredRight, 1 );
                    }
                }
                n--;
            }
        }

        void mouseUp( const juce::MouseEvent& event )
        {
            selectedNote = (int)( (float)event.position.getY() / getNoteHeight() ) + 1;
            selectedNote = getMaxNotes() - selectedNote;

            if( noteSelectionChanged )
                noteSelectionChanged( selectedNote, true );
        }

        inline float getNoteHeight() const { return noteHeight; }
        inline int   getMaxNotes() const { return mt::ScaleAndNotes::maxNotes; }
        void         setNoteHeight( float Height )
        {
            noteHeight = Height;
            repaint();
        }

        void selectNote( int PadNum, bool State )
        {
            selectedNote = PadNum;
            if( noteSelectionChanged )
                noteSelectionChanged( selectedNote, State );
        }

    private:
        Type         noteType{ Type::Drum };
        int          selectedNote{ 0 };
        float        noteHeight{ 26 };
        juce::Colour blackColour{ ui::Theme::backgroundColour( 5 ) };
        juce::Colour whiteColour{ ui::Theme::backgroundColour( 8 ) };
    };

    class ScaleComponent : public juce::Component
    {
    public:
        std::function<void( float Value )> onMouseClick;

        ScaleComponent()
        {
            gridData.gridChanged.connect( [&]() {
                repaint();
            } );
        }

        void paint( juce::Graphics& g ) override
        {
            g.fillAll( ui::Theme::backgroundColour( 4 ) );
            auto offset = 3;
            auto b      = getLocalBounds();
            b.removeFromTop( (int)( 0.35 * b.getHeight() ) );

            const auto beatColour = ui::Theme::gridColour( 2 );
            const auto barColour  = ui::Theme::gridColour( 3 );

            juce::String text;
            g.setFont( juce::jmin( 26.0f, ( 0.9f * b.getHeight() ) ) );

            int   bar  = 1;
            int   beat = 0;
            float xPos = 0.f;
            float x    = 0.f;

            juce::Rectangle<float> tArea = b.toFloat();

            auto numBars  = gridData.getNumBars();
            auto barWidth = gridData.getBarWidth();
            if( mt::getAudioInterface().getNumSequences() <= 0 )
            {
                barWidth = getWidth();
                numBars  = 1;
            }

            for( int i = 1; i <= numBars; ++i )
            {
                auto beatsPerBar = gridData.getBeatsPerBar( ( i - 1 ) );
                auto beatWidth   = barWidth / beatsPerBar;

                // Draw Bar Marker...
                x = xPos;
                g.setColour( barColour );
                g.drawVerticalLine( xPos, b.getY(), b.getY() + b.getHeight() );

                text = text.formatted( "%d", bar );
                tArea.setHorizontalRange( { ( xPos + offset ), ( xPos + beatWidth ) } );
                g.drawFittedText( text, tArea.toNearestInt(), juce::Justification::centredLeft, 1 );

                // Draw Beat Marker...
                beat = 1;
                g.setColour( beatColour );
                for( int q = 0; q < beatsPerBar - 1; ++q )
                {
                    x += beatWidth;
                    g.drawVerticalLine( x, b.getY(), b.getY() + b.getHeight() );

                    text = text.formatted( "%d.%d", bar, ++beat );
                    tArea.setHorizontalRange( { ( x + offset ), ( x + beatWidth ) } );
                    g.drawFittedText( text, tArea.toNearestInt(), juce::Justification::centredLeft, 1 );
                }
                xPos += barWidth;
                ++bar;
            }
            g.setColour( barColour );
            g.drawVerticalLine( xPos, b.getY(), b.getY() + b.getHeight() );
        }

    private:
        void mouseUp( const juce::MouseEvent& e ) override
        {
            if( e.mouseWasClicked() )
            {
                auto normX = juce::jlimit( 0.f, (float)getWidth(), (float)e.getMouseDownX() );
                if( onMouseClick )
                    onMouseClick( normX );
            }
        }

    public:
        GridData& gridData{ GridData::Instance() };
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( ScaleComponent )
    };

    class AppGrid : public juce::Component
        , public ui::KickViewport::Listener
        , public kick_ui::IScreen
        , public juce::Timer
    {
    public:
        AppGrid();
        ~AppGrid() = default;

        bool requiresControlBar() override { return false; };
        bool requiresStatusBar() override { return false; };
        bool requiresHandleCommand() override { return true; };

        void handleCommand( CPktMgr::HWMsg_t& ) override;
        void knobCommand( int knob, int value );

        void updateOnViewPositionChange( ui::KickViewport* pViewport );
        void setViewType( mt::MTProgramType Program );

        void redraw( int Seq, int Trk );

    private:
        void resized() override;
        void visibilityChanged() override;

        void transpseSelectedNotes( juce::Array<juce::ValueTree>&, int );
        void nudgeSelectedNotes( juce::Array<juce::ValueTree>&, int, double, bool );
        void editStartForSelectedNotes( juce::Array<juce::ValueTree>&, int, double, bool );
        void editEndForSelectedNotes( juce::Array<juce::ValueTree>&, int, double, bool );
        void setZoomInZoomOut( int, int, bool );
        void timerCallback() override;
        void updatePlayhead();
        void updateViewPosition( int, bool );

    private:
        const mt::MTSelected& aeSel{ mt::getAudioInterface().getSelected() };
        mt::MTAudioInterface& ae{ mt::getAudioInterface() };
        GridData&             gridData{ GridData::Instance() };

        kick_ui::GridHeader     hdrComp;
        kick_ui::KickModeHeader header{ "Grid View", "Icon_Grid" };
        ui::KickListBox         timeDivListBox{ ui::KickListBox::BoxType::withoutTitleBar, "TIME DIV", { mt::Kick::TimeDivisionStrings }, 5, 3 };

        ui::KickViewport scaleViewport;
        ui::KickViewport gridViewport;
        ui::KickViewport parameterViewport;
        ui::KickViewport noteViewport;
        ui::KickViewport phViewport;

        GridComponent        gridComponent{ gridViewport };
        ParameterViewControl parameterControl;
        ParameterView        parameterView{ parameterControl };
        ParameterViewScale   parameterScale;
        ScaleComponent       scaleComp;
        NotesView            notesComp;

        GridEditOptions editOptions;
        MetronomePopup& metronome{ MetronomePopup::Instance() };
        GridSettings&   gridSettings{ GridSettings::Instance() };
        int             gridPage{ 0 };

        juce::Array<juce::ValueTree>& selectedNotes{ ae.getSelectedNoteList() };

        GridPlayhead ph;
        GridViewType viewType{ GridViewType::DrumView };

        bool              isSeek{ false };
        bool              initialized{ false };
        bool              parameterViewExpanded{ false };
        juce::Point<int>  bottomLeftNote;
        juce::ProgressBar progressBar{ ae.getPlayhead().normized };

        void openPitchQuantizePopUp();
        void openHumanizePopUp();
        void openRandomEventsPopUp();
        void openSplitEventsPopUp();
        void openWarningPopUp( const juce::String& Text );

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( AppGrid )
    };

} //namespace kick_ui
