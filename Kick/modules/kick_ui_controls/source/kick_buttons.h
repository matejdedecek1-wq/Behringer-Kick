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

    class KickButton
        : public juce::TextButton
        , public mt::MTProperties
        , protected juce::Timer
    {
    public:
        explicit KickButton( const juce::String& Name = "", const juce::String& buttonText = "" );

        void        setColours( juce::Colour TextColour, juce::Colour OutlineColour, juce::Colour BackgroundColour, int OutlineThickness );
        inline void setBackgroundColour( juce::Colour Colour ) { setValueBackgroundColour( Colour ); }

        void  setNormalized( float Value );
        float getNormalized();

    protected:
        void mouseDown( const juce::MouseEvent& ) override;
        void mouseExit( const juce::MouseEvent& ) override;
        void timerCallback() override;

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( KickButton )
    };

    class KickToggleButton : public KickButton
    {
    public:
        explicit KickToggleButton( const juce::String& Name = "", const juce::String& OnText = "", const juce::String& OffText = "" );
        void setToggleButtonText( const juce::String& OnText, const juce::String& OffText );

    private:
        void buttonStateChanged() override;

    private:
        juce::String onText{ "" };
        juce::String offText{ "" };
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( KickToggleButton )
    };

    class KickCheckBoxButton
        : public juce::ToggleButton
        , public mt::MTProperties
    {
    public:
        explicit KickCheckBoxButton( const juce::String& buttonName, const juce::String& buttonText = "" );
        void  setNormalized( float Value );
        float getNormalized();

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( KickCheckBoxButton )
    };

    class KickImageButton : public KickButton
    {
    public:
        std::function<void()> onDoubleClick;

        explicit KickImageButton( const juce::String& buttonName = "", const juce::String& NormalImage = "", const juce::String& DownImage = "", const juce::String& HoverImage = "", const juce::String& DisabledImage = "" );

        void setImages( const juce::String& NormalImage, const juce::String& DownImage = "", const juce::String& HoverImage = "", const juce::String& DisabledImage = "" );
        void setTemplateDemoImage( const juce::File& file );
        void setOverlayColours( const juce::Colour& Down, const juce::Colour& Normal = ui::Theme::highlightColour( 0 ), const juce::Colour& Hover = ui::Theme::highlightColour( 1 ).withAlpha( 0.2f ) );

        inline void setPlacementFlag( int Flags ) { placementFlags = Flags; }
        inline void setOpacity( float Opacity ) { imgOpacity = Opacity; }

    private:
        void paintButton( juce::Graphics& g, bool asHighlighed, bool asDown ) override;
        void mouseDoubleClick( const juce::MouseEvent& e ) override;

    private:
        std::unique_ptr<juce::Drawable> imgDown;
        std::unique_ptr<juce::Drawable> imgNormal;
        std::unique_ptr<juce::Drawable> imgHover;
        std::unique_ptr<juce::Drawable> imgDisabled;

        juce::Colour normalColour{ juce::Colours::transparentBlack };
        juce::Colour downColour{ juce::Colours::transparentBlack };
        juce::Colour hoverColour{ juce::Colours::transparentBlack };
        juce::Colour dimColour{ juce::Colours::transparentBlack };

        int   placementFlags{ juce::RectanglePlacement::centred | juce::RectanglePlacement::onlyIncreaseInSize };
        float imgOpacity{ 1.0 };

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( KickImageButton )
    };

    class KickColorButton : public KickToggleButton
    {
    public:
        explicit KickColorButton( const juce::Colour& onColor = ui::Theme::highlightColour( 1 ), const juce::Colour& offColor = ui::Theme::highlightColour( 1 ) );

        void setColor( const juce::Colour& onColor, const juce::Colour& offColor );

    private:
        juce::Colour onCol;
        juce::Colour offCol;

        void paint( juce::Graphics& g ) override;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( KickColorButton )
    };

    class KickTextEditor
        : public juce::TextEditor
        , public mt::MTProperties
    {
    public:
        std::function<void( int )> onMouseDown;
        explicit KickTextEditor( const juce::String& value = "" );

    private:
        void mouseDown( const juce::MouseEvent& e )
        {
            juce::TextEditor::mouseDown( e );
            if( onMouseDown ) onMouseDown( getID() );
        }
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( KickTextEditor )
    };

    class KickRecordButton : public KickToggleButton
    {
    public:
        KickRecordButton();
        ~KickRecordButton() = default;

    private:
        int currentIndex = -1;

        std::unique_ptr<juce::Drawable> imgOff;
        std::unique_ptr<juce::Drawable> imgRecording;
        std::unique_ptr<juce::Drawable> imgArm;

        void mouseDown( const juce::MouseEvent& ) override;
        void paintButton( juce::Graphics&, bool, bool ) override;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( KickRecordButton )
    };

} // namespace ui
