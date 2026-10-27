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

#include "kick_padButton.h"

namespace ui
{
#define _NUM_PADS_IN_ROW       ( 4 )
#define _NUM_PADS_IN_COL       ( 4 )
#define _NUM_BANKS             ( 8 )
#define _NUM_PADS_IN_COMPONENT ( _NUM_PADS_IN_COL * _NUM_PADS_IN_ROW )
#define _NUM_PADS              ( _NUM_PADS_IN_COMPONENT * _NUM_BANKS )

    struct KickPadButtonData
    {
        juce::Colour color{ DEFAULT_BORDER_COLOR };
        float        thickness{ DEFAULT_BORDER_THICKNESS };
        juce::String hdr1{ "" }, group{ "" }, r1{ "" }, r2{ "" }, r3{ "" };
        double       slider{ 0.0 }, pan{ 0.0 };
        int          id{ 0 };

        struct _flags
        {
            bool         highlight : 1;
            bool         selected  : 1;
            bool         active    : 1;
            bool         mute      : 1;
            bool         solo      : 1;
            bool         enable    : 1;
            juce::Colour highlightColour;
        } flags;

        juce::Justification jr1{ juce::Justification::centredLeft }, jr2{ juce::Justification::centredLeft }, jr3{ juce::Justification::centredLeft };

        void clear()
        {
            color     = DEFAULT_BORDER_COLOR;
            thickness = DEFAULT_BORDER_THICKNESS;
            hdr1 = group = r1 = r2 = r3 = "";
            slider = pan    = 0.0;
            flags.highlight = flags.mute = flags.solo = false;
        }
    };
    using KickPadDataArray = std::array<KickPadButtonData, _NUM_PADS>;

    class KickPadButtonGroupComponent : public juce::Component
        , private juce::Timer
    {
    public:
        std::function<void( int )>                 OnButtonClicked;
        std::function<void( int )>                 OnDoubleClicked;
        std::function<void( const juce::String& )> OnBankChanged;
        std::function<void( int )>                 onKnobClicked;
        std::function<void( int )>                 onShiftPadPressed;

        std::function<void( int, const juce::Colour&, float )> onColorChanged;
        std::function<void( int, const juce::String& )>        onHeaderChanged;
        std::function<void( int, const juce::String& )>        onGroupChanged;
        std::function<void( int, const juce::String& )>        onRow1Changed;
        std::function<void( int, const juce::String& )>        onRow2Changed;
        std::function<void( int, const juce::String& )>        onRow3Changed;
        std::function<void( int, bool )>                       onMuteChanged;
        std::function<void( int, bool )>                       onSoloChanged;
        std::function<void( int, double )>                     onSliderChanged;
        std::function<void( int, double )>                     onPanChanged;

        explicit KickPadButtonGroupComponent( KickPadButton::PadButtonType Type );

        KickListBox& getBankComponent();

        bool                isLastBank() { return ( ( _NUM_BANKS - 1 ) == getBankIndex() ); }
        int                 getBankOffset() { return bankOffset; }
        int                 getBankIndex();
        const juce::String& getBank() { return _bank; }
        juce::String        getBank( int BankIndex );
        void                setBank( const juce::String& Bank );
        void                setBank( const int BankNum );
        void                incBank( const int Rotation );

        KickPadButtonData& getPadData( int Index );
        KickPadDataArray&  getPadsData() { return padData; };

        bool                isPadVisible( int Index );
        bool                isBreathing( int Index );
        bool                getMute( int Index );
        bool                getSolo( int Index );
        int                 getGroupNumber( int Index );
        int                 getSelected() noexcept { return selectedPad; }
        double              getValue( int Index );
        double              getPan( int Index );
        const juce::String& getRow1Text( int Index );
        const juce::String& getRow2Text( int Index );
        const juce::String& getRow3Text( int Index );
        const juce::Colour& getBorderColor( int Index );

        bool isSelectionEnabled() const noexcept { return interceptKeys; }
        void setSelectionEnabled( bool Intercept ) noexcept { interceptKeys = Intercept; }

        void setActive( int Index, bool isAvtive );

        void setHeaderColour( juce::Colour headerColour );
        void setGroupColour( juce::Colour groupColour );
        void setRow1Colour( juce::Colour headerColour );
        void setRow2Colour( juce::Colour headerColour );
        void setRow3Colour( juce::Colour headerColour );

        void setPrefix( const juce::String& Text );
        void setHeaderText( int Index, const juce::String& Text, juce::NotificationType Notify );
        void setGroupText( int Index, const juce::String& Text, juce::NotificationType Notify );
        void setPadId( int id, int Index );

        void setRow1Text( int Index, const juce::String& Text, juce::NotificationType Notify );
        void setRow2Text( int Index, const juce::String& Text, juce::NotificationType Notify );
        void setRow3Text( int Index, const juce::String& Text, juce::NotificationType Notify );

        void setRow1Justification( int Index, juce::Justification Just );
        void setRow2Justification( int Index, juce::Justification Just );
        void setRow3Justification( int Index, juce::Justification Just );

        void resetBorderColor( int Index );
        void setBorderColor( int Index, const juce::Colour& Col, float Thickness, juce::NotificationType Notify );
        void setBorderColor( const juce::Colour& Col, float Thickness, juce::NotificationType Notify );

        void setSelected( int Index, juce::NotificationType Notify );
        void setHighlight( int Index, juce::Colour highlightedColour = ui::Theme::selectionColour( 2 ) );
        int  getHighlightedPad();

        void setMute( int Index, bool Mute, juce::NotificationType Notify );
        void setSolo( int Index, bool Solo, juce::NotificationType Notify );
        void setValue( int Index, double Value, juce::NotificationType Notify );
        void setPan( int Index, double Value, juce::NotificationType Notify );

        void setBreathing( int Index );
        void removeBreathing( int Index );
        void clearAllBreathing();

        KickPadButton::PadButtonType getButtonType( int Index );
        void                         setButtonType( KickPadButton::PadButtonType type );
        void                         setButtonType( int buttonRange, KickPadButton::PadButtonType type );

        void setButtonNames( const std::list<juce::String>& names );

        void clearAllPadData();
        void refreshPad( int Index );
        void refreshAllPads();
        void setPadButtonSpacer( int );
        void setElementsEnable( int Index, const bool state );

        void setEnableTrackPrefix( bool Enable ) { enableTrackPrefix = Enable; }
        void enableBank( bool State ) { isBankEnabled = State; }

        void setKnobBorder( int Row );
        void setRowBorder( int Row );
        void setColumnBorder( int Column );
        void updateLevelMeter( int padNo, float lRms, float rRms, float lPeak, float rPeak );
        void setFocusOutline( bool State );

    private:
        bool  knobHighlight{ false };
        bool  interceptKeys{ true };
        bool  enableTrackPrefix{ false };
        bool  isBankEnabled{ true };
        int   padButtonSpacer{ 4 };
        int   bankOffset{ 0 };
        int   selectedPad{ 0 };
        float boarderThickness;

        juce::String     prefix{ "" };
        juce::String     _bank{ "A" };
        juce::Colour     invalidColour;
        juce::Colour     borderColour;
        juce::Array<int> beathingPads;

        std::array<std::unique_ptr<KickPadButton>, _NUM_PADS_IN_COMPONENT> pads;

        KickPadDataArray       padData;
        KickListBox            bank{ KickListBox::BoxType::withoutTitleBarWithNavigationButtons, "Bank", mt::Kick::padBankNames, 0, 2 };
        juce::Rectangle<float> rowBounds;
        juce::Rectangle<float> columnBounds;

        juce::Rectangle<float> getRowBounds( int row );
        juce::Rectangle<float> getColumnBounds( int column );

        void onBankChanged();
        void setupBreathingTimer();
        void resized() override;
        void visibilityChanged() override;
        void timerCallback() override;
        void paint( juce::Graphics& g );

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( KickPadButtonGroupComponent )
    };
}
