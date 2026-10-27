#include "kick_padButtonGroup.h"
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

namespace ui
{
    KickPadButtonData invalidPadButtonData{ juce::Colours::transparentBlack };
    KickPadButtonGroupComponent::KickPadButtonGroupComponent( KickPadButton::PadButtonType Type )
    {
        setLookAndFeel( KickLooknFeel::getLnf() );

        beathingPads.clear();
        beathingPads.ensureStorageAllocated( 128 );

        bank.onSelectionChanged = [this]( int Selection, const juce::String& Value ) {
            juce::ignoreUnused( Selection );
            setBank( Value );
        };

        for( auto i = 0; i < _NUM_PADS_IN_COMPONENT; ++i )
        {
            pads[i] = std::make_unique<KickPadButton>( prefix + juce::String( i + 1 ), Type );
            addChildComponent( pads[i].get() );

            pads[i]->onColorChanged  = [&]( KickPadButton* Pad, const juce::Colour& Col, float Val ) { if( onColorChanged ) onColorChanged( Pad->getID(), Col, Val ); };
            pads[i]->onHeaderChanged = [&]( KickPadButton* Pad, const juce::String& Val ) { if( onHeaderChanged ) onHeaderChanged( Pad->getID(), Val ); };
            pads[i]->onGroupChanged  = [&]( KickPadButton* Pad, const juce::String& Val ) { if( onGroupChanged ) onGroupChanged( Pad->getID(), Val ); };
            pads[i]->onRow1Changed   = [&]( KickPadButton* Pad, const juce::String& Val ) { if( onRow1Changed ) onRow1Changed( Pad->getID(), Val ); };
            pads[i]->onRow2Changed   = [&]( KickPadButton* Pad, const juce::String& Val ) { if( onRow2Changed ) onRow2Changed( Pad->getID(), Val ); };
            pads[i]->onRow3Changed   = [&]( KickPadButton* Pad, const juce::String& Val ) { if( onRow3Changed ) onRow3Changed( Pad->getID(), Val ); };
            pads[i]->onMuteChanged   = [&]( KickPadButton* Pad, bool Val ) { if( onMuteChanged ) onMuteChanged( Pad->getID(), Val ); };
            pads[i]->onSliderChanged = [&]( KickPadButton* Pad, double Val ) { if( onSliderChanged ) onSliderChanged( Pad->getID(), Val ); };
            pads[i]->onPanChanged    = [&]( KickPadButton* Pad, double Val ) { if( onPanChanged ) onPanChanged( Pad->getID(), Val ); };
            pads[i]->setTextColour( ui::Theme::backgroundColour( 8 ) );

            pads[i]->onClick = [&, i]() {
                pads[i]->setFocus( true );
                setSelected( i, juce::dontSendNotification );
                if( pads[i]->isActive() && OnButtonClicked )
                    OnButtonClicked( pads[i]->getID() );
            };
            pads[i]->onDoubleClicked       = [&]( KickPadButton* Pad, int ID ) { if( OnDoubleClicked ) OnDoubleClicked(ID); };
            pads[i]->onKnobClickedCallback = [&, i]() {if(onKnobClicked) onKnobClicked(i); };
        }
        onBankChanged();
    }

    KickListBox& KickPadButtonGroupComponent::getBankComponent() { return bank; }

    KickPadButtonData& KickPadButtonGroupComponent::getPadData( int Index )
    {
        if( juce::isPositiveAndBelow( Index, _NUM_PADS ) )
            return padData[(size_t)Index];
        return invalidPadButtonData;
    }

    const juce::Colour& KickPadButtonGroupComponent::getBorderColor( int Index )
    {
        if( juce::isPositiveAndBelow( Index, _NUM_PADS ) )
            return pads[Index]->getBorderColor();
        return juce::Colours::transparentBlack;
    }

    void KickPadButtonGroupComponent::setBank( const int BankNum )
    {
#if defined( __APPLE_CPP__ ) || defined( __APPLE_CC__ ) || defined( JUCE_MAC )
        const char buildKick = 'A' + BankNum;
#endif
        if( juce::isPositiveAndBelow( BankNum, _NUM_BANKS ) )
        {
#if defined( __APPLE_CPP__ ) || defined( __APPLE_CC__ ) || defined( JUCE_MAC )
            char ch[2] = { buildKick, '\0' };
#else
            char ch[2] = { static_cast<char>( 'A' + BankNum ), '\0' };
#endif
            _bank      = juce::String( ch );
            bankOffset = 16 * BankNum;

            uint8_t bankID;
            switch( BankNum )
            {
                case 0: bankID = contracts::DeviceControlContracts::BtnLedSequence::EN_BANK_A; break;
                case 1: bankID = contracts::DeviceControlContracts::BtnLedSequence::EN_BANK_B; break;
                case 2: bankID = contracts::DeviceControlContracts::BtnLedSequence::EN_BANK_C; break;
                case 3: bankID = contracts::DeviceControlContracts::BtnLedSequence::EN_BANK_D; break;
                case 4: bankID = contracts::DeviceControlContracts::BtnLedSequence::EN_BANK_E; break;
                case 5: bankID = contracts::DeviceControlContracts::BtnLedSequence::EN_BANK_F; break;
                case 6: bankID = contracts::DeviceControlContracts::BtnLedSequence::EN_BANK_G; break;
                case 7: bankID = contracts::DeviceControlContracts::BtnLedSequence::EN_BANK_H; break;
            }            
          
            contracts::DclSignals::sendCommandToDcl( contracts::DclContractsHelper::setBankMessage( BankNum ) );
           
            bank.setSelected( _bank, juce::dontSendNotification );
            onBankChanged();
        }
    }

    void KickPadButtonGroupComponent::setBank( const juce::String& Bank )
    {
        auto b     = Bank.toUpperCase();
        auto index = (int)( *b.getCharPointer() - 'A' );

        if( isBankEnabled )
            setBank( index );
    }

    void KickPadButtonGroupComponent::incBank( const int Rotation )
    {
        auto bankIdx = getBankIndex() + Rotation;
        setBank( bankIdx );
    }

    int KickPadButtonGroupComponent::getBankIndex()
    {
        auto b     = _bank.toUpperCase();
        auto index = (int)( *b.getCharPointer() - 'A' );
        return index;
    }

    juce::String KickPadButtonGroupComponent::getBank( int BankIndex )
    {
        return juce::String::charToString( static_cast<char>( 'A' + BankIndex ) );
    }

    void KickPadButtonGroupComponent::setActive( int Index, bool isActive )
    {
        if( juce::isPositiveAndBelow( Index, _NUM_PADS ) )
        {
            padData[(size_t)Index].flags.active = isActive;

            auto i = Index - bankOffset;
            if( juce::isPositiveAndBelow( i, _NUM_PADS_IN_COMPONENT ) )
                pads[i]->setActive( padData[(size_t)Index].flags.active );
        }
        else
        {
            for( auto i = 0; i < _NUM_PADS; i++ )
                setActive( i, isActive );
        }
    }

    void KickPadButtonGroupComponent::setHeaderColour( juce::Colour headerColour )
    {
        for( auto i = 0; i < _NUM_PADS_IN_COMPONENT; ++i )
            pads[i]->setHeaderColour( headerColour );
    }

    void KickPadButtonGroupComponent::setGroupColour( juce::Colour groupColour )
    {
        for( auto i = 0; i < _NUM_PADS_IN_COMPONENT; ++i )
            pads[i]->setGroupColour( groupColour );
    }

    void KickPadButtonGroupComponent::setRow1Colour( juce::Colour row1Colour )
    {
        for( auto i = 0; i < _NUM_PADS_IN_COMPONENT; ++i )
            pads[i]->setRow1Colour( row1Colour );
    }

    void KickPadButtonGroupComponent::setRow2Colour( juce::Colour row2Colour )
    {
        for( auto i = 0; i < _NUM_PADS_IN_COMPONENT; ++i )
            pads[i]->setRow2Colour( row2Colour );
    }

    void KickPadButtonGroupComponent::setRow3Colour( juce::Colour row3Colour )
    {
        for( auto i = 0; i < _NUM_PADS_IN_COMPONENT; ++i )
            pads[i]->setRow3Colour( row3Colour );
    }

    void KickPadButtonGroupComponent::setPrefix( const juce::String& Text )
    {
        prefix = Text;
        refreshAllPads();
    }

    void KickPadButtonGroupComponent::setHeaderText( int Index, const juce::String& Text, juce::NotificationType Notify )
    {
        if( juce::isPositiveAndBelow( Index, _NUM_PADS ) )
        {
            padData[(size_t)Index].hdr1 = Text;
            refreshPad( Index );

            if( juce::dontSendNotification != Notify )
                onHeaderChanged( Index, Text );
        }
    }

    void KickPadButtonGroupComponent::setGroupText( int Index, const juce::String& Text, juce::NotificationType Notify )
    {
        if( juce::isPositiveAndBelow( Index, _NUM_PADS ) )
        {
            padData[(size_t)Index].group = Text;
            refreshPad( Index );
            if( juce::dontSendNotification != Notify )
                onGroupChanged( Index, Text );
        }
    }

    void KickPadButtonGroupComponent::setPadId( int id, int Index )
    {
        if( juce::isPositiveAndBelow( id, _NUM_PADS ) )
        {
            padData[(size_t)Index].id = id;
            refreshPad( Index );
        }
    }

    int KickPadButtonGroupComponent::getGroupNumber( int Index )
    {
        if( juce::isPositiveAndBelow( Index, _NUM_PADS ) )
        {
            return padData[(size_t)Index].group.getIntValue();
        }
        return -1;
    }

    void KickPadButtonGroupComponent::setRow1Text( int Index, const juce::String& Text, juce::NotificationType Notify )
    {
        if( juce::isPositiveAndBelow( Index, _NUM_PADS ) )
        {
            padData[(size_t)Index].r1 = Text;
            refreshPad( Index );
            if( juce::dontSendNotification != Notify )
                onRow1Changed( Index, Text );
        }
    }

    void KickPadButtonGroupComponent::setRow2Text( int Index, const juce::String& Text, juce::NotificationType Notify )
    {
        if( juce::isPositiveAndBelow( Index, _NUM_PADS ) )
        {
            padData[(size_t)Index].r2 = Text;
            refreshPad( Index );
            if( juce::dontSendNotification != Notify )
                onRow2Changed( Index, Text );
        }
    }

    void KickPadButtonGroupComponent::setRow3Text( int Index, const juce::String& Text, juce::NotificationType Notify )
    {
        if( juce::isPositiveAndBelow( Index, _NUM_PADS ) )
        {
            padData[(size_t)Index].r3 = Text;
            refreshPad( Index );
            if( juce::dontSendNotification != Notify )
                onRow3Changed( Index, Text );
        }
    }

    const juce::String& KickPadButtonGroupComponent::getRow1Text( int Index ) { return padData[(size_t)Index].r1; }
    const juce::String& KickPadButtonGroupComponent::getRow2Text( int Index ) { return padData[(size_t)Index].r2; }
    const juce::String& KickPadButtonGroupComponent::getRow3Text( int Index ) { return padData[(size_t)Index].r3; }

    void KickPadButtonGroupComponent::setRow1Justification( int Index, juce::Justification Just )
    {
        if( juce::isPositiveAndBelow( Index, _NUM_PADS ) )
        {
            padData[(size_t)Index].jr1 = Just;
            refreshPad( Index );
        }
        else
        {
            for( auto i = 0; i < _NUM_PADS; i++ )
                padData[i].jr1 = Just;
            refreshAllPads();
        }
    }

    void KickPadButtonGroupComponent::setRow2Justification( int Index, juce::Justification Just )
    {
        if( juce::isPositiveAndBelow( Index, _NUM_PADS ) )
        {
            padData[(size_t)Index].jr2 = Just;
            refreshPad( Index );
        }
        else
        {
            for( auto i = 0; i < _NUM_PADS; i++ )
                padData[i].jr2 = Just;
            refreshAllPads();
        }
    }

    void KickPadButtonGroupComponent::setRow3Justification( int Index, juce::Justification Just )
    {
        if( juce::isPositiveAndBelow( Index, _NUM_PADS ) )
        {
            padData[(size_t)Index].jr3 = Just;
            refreshPad( Index );
        }
        else
        {
            for( auto i = 0; i < _NUM_PADS; i++ )
                padData[i].jr3 = Just;
            refreshAllPads();
        }
    }

    void KickPadButtonGroupComponent::resetBorderColor( int Index )
    {
        if( juce::isPositiveAndBelow( Index, _NUM_PADS ) )
        {
            padData[(size_t)Index].color     = DEFAULT_BORDER_COLOR;
            padData[(size_t)Index].thickness = DEFAULT_BORDER_THICKNESS;
            refreshPad( Index );
        }
    }

    void KickPadButtonGroupComponent::setBorderColor( int Index, const juce::Colour& Col, float Thickness, juce::NotificationType Notify )
    {
        if( juce::isPositiveAndBelow( Index, _NUM_PADS ) )
        {
            padData[(size_t)Index].color     = Col;
            padData[(size_t)Index].thickness = Thickness;
            refreshPad( Index );
            if( juce::dontSendNotification != Notify && onColorChanged )
                onColorChanged( Index, Col, Thickness );
        }
    }

    void KickPadButtonGroupComponent::setBorderColor( const juce::Colour& Col, float Thickness, juce::NotificationType Notify )
    {
        for( auto i = 0; i < _NUM_PADS; ++i )
            setBorderColor( i, Col, Thickness, Notify );
    }

    void KickPadButtonGroupComponent::setSelected( int Index, juce::NotificationType Notify )
    {
        for( auto i = 0; i < _NUM_PADS; ++i )
        {
            padData[(size_t)i].flags.selected = ( i == Index );
            refreshPad( i );
        }
        selectedPad = Index;
        if( juce::dontSendNotification != Notify && OnButtonClicked )
            OnButtonClicked( selectedPad );
    }

    void KickPadButtonGroupComponent::setHighlight( int Index, juce::Colour highlightedColour )
    {
        for( auto i = 0; i < _NUM_PADS; ++i )
        {
            padData[(size_t)i].flags.highlight = ( i == Index );
            if( i == Index )
            {
                padData[(size_t)i].flags.highlightColour = highlightedColour;
            }
            refreshPad( i );
        }
    }

    int KickPadButtonGroupComponent::getHighlightedPad()
    {
        for( auto i = 0; i < _NUM_PADS; ++i )
        {
            if( padData[(size_t)i].flags.highlight )
                return i;
        }
        return -1;
    }

    void KickPadButtonGroupComponent::setMute( int Index, bool Mute, juce::NotificationType Notify )
    {
        if( juce::isPositiveAndBelow( Index, _NUM_PADS ) )
        {
            padData[(size_t)Index].flags.mute = Mute;
            refreshPad( Index );
            if( juce::dontSendNotification != Notify )
                onMuteChanged( Index, Mute );
        }
    }

    bool KickPadButtonGroupComponent::getMute( int Index )
    {
        if( juce::isPositiveAndBelow( Index, _NUM_PADS ) )
            return padData[(size_t)Index].flags.mute;
        return false;
    }

    void KickPadButtonGroupComponent::setSolo( int Index, bool Solo, juce::NotificationType Notify )
    {
        if( juce::isPositiveAndBelow( Index, _NUM_PADS ) )
        {
            padData[(size_t)Index].flags.solo = Solo;
            refreshPad( Index );
            if( juce::dontSendNotification != Notify )
                onSoloChanged( Index, Solo );
        }
    }

    bool KickPadButtonGroupComponent::getSolo( int Index )
    {
        if( juce::isPositiveAndBelow( Index, _NUM_PADS ) )
            return padData[(size_t)Index].flags.solo;
        return false;
    }

    void KickPadButtonGroupComponent::setValue( int Index, double Value, juce::NotificationType Notify )
    {
        if( juce::isPositiveAndBelow( Index, _NUM_PADS ) )
        {
            padData[(size_t)Index].slider = Value;
            refreshPad( Index );
            if( juce::dontSendNotification != Notify )
                onSliderChanged( Index, Value );
        }
    }

    void KickPadButtonGroupComponent::setPan( int Index, double Value, juce::NotificationType Notify )
    {
        if( juce::isPositiveAndBelow( Index, _NUM_PADS ) )
        {
            padData[(size_t)Index].pan = Value;
            refreshPad( Index );
            if( juce::dontSendNotification != Notify )
                onPanChanged( Index, Value );
        }
    }

    double KickPadButtonGroupComponent::getValue( int Index )
    {
        if( juce::isPositiveAndBelow( Index, _NUM_PADS ) )
        {
            return padData[(size_t)Index].slider;
        }
        return 0.f;
    }

    double KickPadButtonGroupComponent::getPan( int Index )
    {
        if( juce::isPositiveAndBelow( Index, _NUM_PADS ) )
        {
            return padData[(size_t)Index].pan;
        }
        return 0.f;
    }

    KickPadButton::PadButtonType KickPadButtonGroupComponent::getButtonType( int Index )
    {
        if( juce::isPositiveAndBelow( Index, _NUM_PADS ) )
            return pads[Index]->getType();

        return KickPadButton::PadButtonType::with1Row;
    }

    void KickPadButtonGroupComponent::setButtonType( KickPadButton::PadButtonType type )
    {
        for( auto& button : pads )
            button->setType( type );
    }

    void KickPadButtonGroupComponent::setButtonType( int buttonRange, KickPadButton::PadButtonType type )
    {
        auto range = juce::jlimit( 0, _NUM_PADS_IN_COMPONENT, buttonRange );
        for( int i = 0; i < range; i++ )
            pads[i]->setType( type );
    }

    void KickPadButtonGroupComponent::setButtonNames( const std::list<juce::String>& names )
    {
        int i = 0;
        for( auto it = names.begin(); it != names.end(); it++ )
        {
            padData[(size_t)i].hdr1 = *it;
            i++;
            refreshPad( i );
        }
    }

    void KickPadButtonGroupComponent::clearAllPadData()
    {
        for( auto i = 0; i < _NUM_PADS; ++i )
        {
            padData[i].clear();
            resetBorderColor( i );
        }
        for( auto i = bankOffset; i < ( bankOffset + _NUM_PADS_IN_COMPONENT ); ++i )
        {
            refreshPad( i );
        }
    }

    void KickPadButtonGroupComponent::resized()
    {
        auto b = getLocalBounds().reduced( 3 );

        const auto numRows = ( _NUM_PADS_IN_COMPONENT / _NUM_PADS_IN_ROW );
        const auto numCols = ( _NUM_PADS_IN_COMPONENT / _NUM_PADS_IN_COL );

        auto w = (int)( ( b.getWidth() - ( ( numCols - 1 ) * padButtonSpacer ) ) / numCols );
        auto h = (int)( ( b.getHeight() - ( ( numRows - 1 ) * padButtonSpacer ) ) / numRows );

        for( auto row = 0; row < numRows; ++row )
        {
            auto x = 0;
            auto y = row * ( h + padButtonSpacer );
            for( auto col = 0; col < numCols; ++col )
            {
                x        = col * ( w + padButtonSpacer ) + 3;
                auto idx = ( numRows - row - 1 ) * _NUM_PADS_IN_ROW + col;
                pads[idx]->setBounds( x, y, w, h );
            }
        }
    }

    void KickPadButtonGroupComponent::paint( juce::Graphics& g )
    {
        if( knobHighlight )
            mt::utils::renderRoundedRect( g, columnBounds.toNearestInt(), ui::Theme::backgroundColour( 6 ) );
        else
            mt::utils::renderRoundedRect( g, rowBounds.toNearestInt(), ui::Theme::backgroundColour( 6 ) );
    }

    void KickPadButtonGroupComponent::setKnobBorder( int index )
    {
        if( contracts::DclContractsHelper::isShiftPressed() )
            setColumnBorder( index );
        else
            setRowBorder( ( ( index - 1 ) / 4 ) + 1 );
    }

    void KickPadButtonGroupComponent::setRowBorder( int Row )
    {
        const auto numRows = ( _NUM_PADS_IN_COMPONENT / _NUM_PADS_IN_ROW );
        if( Row >= 1 && Row <= numRows )
        {
            rowBounds     = getRowBounds( numRows - Row + 1 );
            knobHighlight = false;
            repaint();
        }
    }

    juce::Rectangle<float> KickPadButtonGroupComponent::getRowBounds( int row )
    {
        auto b = getLocalBounds().toFloat();
        DBG( "ROW IN BOUNDS: " << row );
        const auto numRows = ( _NUM_PADS_IN_COMPONENT / _NUM_PADS_IN_ROW );
        auto       bounds  = pads[( numRows - row ) * _NUM_PADS_IN_ROW]->getBounds().toFloat();
        return { 0, bounds.getY() - 3, b.getWidth(), bounds.getHeight() + 6 };
    }

    void KickPadButtonGroupComponent::setColumnBorder( int Column )
    {
        const auto numCols = _NUM_PADS_IN_ROW;
        if( Column >= 1 && Column <= numCols )
        {
            columnBounds  = getColumnBounds( Column );
            knobHighlight = true;
            repaint();
        }
    }

    juce::Rectangle<float> KickPadButtonGroupComponent::getColumnBounds( int column )
    {
        auto       b          = getLocalBounds().toFloat();
        const auto numColumns = ( _NUM_PADS_IN_COMPONENT / _NUM_PADS_IN_COL );
        auto       bounds     = pads[( column ) + 11]->getBounds().toFloat();
        return { bounds.getX() - 3, 0, bounds.getWidth() + 6, b.getHeight() };
    }

    void KickPadButtonGroupComponent::refreshPad( int Index )
    {
        auto i = Index - bankOffset;
        //if( bankOffset <= Index && Index < (bankOffset + _NUM_PADS_IN_COMPONENT) )
        if( juce::isPositiveAndBelow( i, _NUM_PADS_IN_COMPONENT ) )
        {
            auto& data = padData[(size_t)Index];

            pads[i]->setActive( true );
            pads[i]->setSelected( data.flags.selected );
            pads[i]->setHighlight( data.flags.highlight, data.flags.highlightColour );
            pads[i]->setBorderColor( data.color, data.thickness );
            juce::String text;
            if( !enableTrackPrefix )
            {
                if( isBankEnabled )
                {
                    text = prefix.length() != 0 ?
                               prefix + juce::String( " " ) + _bank + juce::String( " " ) + juce::String( bankOffset + i + 1 ).paddedLeft( '0', 2 ) :
                               _bank + juce::String( " " ) + juce::String( i + 1 ).paddedLeft( '0', 2 );
                }
                else
                {
                    text = prefix.length() != 0 ?
                               prefix + juce::String( " " ) + juce::String( bankOffset + i + 1 ).paddedLeft( '0', 2 ) :
                               juce::String( i + 1 ).paddedLeft( '0', 2 );
                }
            }
            else
            {
                text = "T" + juce::String( " " ) + juce::String( bankOffset + i + 1 ).paddedLeft( '0', 2 );
            }

            pads[i]->setID( Index );
            pads[i]->setHeaderText( data.hdr1.length() != 0 ? data.hdr1 : text );
            pads[i]->setGroupText( data.group );
            pads[i]->setRow1Text( data.r1, data.jr1 );
            pads[i]->setRow2Text( data.r2, data.jr2 );
            pads[i]->setRow3Text( data.r3, data.jr3 );
            pads[i]->setSliderValue( data.slider );
            pads[i]->setPanSliderValue( data.pan );
            pads[i]->setMute( data.flags.mute, juce::dontSendNotification );
            pads[i]->setSolo( data.flags.solo, juce::dontSendNotification );
            pads[i]->setElementsEnable( data.flags.enable );
            pads[i]->setActive( data.flags.active );
        }
    }
    void KickPadButtonGroupComponent::refreshAllPads()
    {
        for( auto i = 0; i < _NUM_PADS_IN_COMPONENT; ++i )
            refreshPad( bankOffset + i );
    }

    void KickPadButtonGroupComponent::onBankChanged()
    {
        refreshAllPads();
        if( OnBankChanged )
            OnBankChanged( getBank() );
    }

    bool KickPadButtonGroupComponent::isPadVisible( int Index )
    {
        if( ( bankOffset <= Index ) && ( Index < ( bankOffset + _NUM_PADS_IN_COMPONENT ) ) )
            return true;
        return false;
    }

    bool KickPadButtonGroupComponent::isBreathing( int Index )
    {
        return beathingPads.contains( Index );
    }

    void KickPadButtonGroupComponent::setBreathing( int Index )
    {
        if( juce::isPositiveAndBelow( Index, _NUM_PADS ) )
        {
            beathingPads.addIfNotAlreadyThere( Index );
            setupBreathingTimer();
        }
    }

    void KickPadButtonGroupComponent::removeBreathing( int Index )
    {
        if( juce::isPositiveAndBelow( Index, _NUM_PADS ) )
        {
            beathingPads.removeAllInstancesOf( Index );
            refreshPad( Index );
            setupBreathingTimer();
        }
    }
    void KickPadButtonGroupComponent::clearAllBreathing()
    {
        beathingPads.clearQuick();
        setupBreathingTimer();
        refreshAllPads();
    }

    void KickPadButtonGroupComponent::updateLevelMeter( int PadNo, float lRms, float rRms, float lPeak, float rPeak )
    {
        pads[PadNo]->updateLevelMeter( lRms, rRms, lPeak, rPeak );
    }

    void KickPadButtonGroupComponent::visibilityChanged()
    {
        auto handle_Bank = [this]( CPktMgr::HWMsg_t& f_HwMsg ) {
            if( contracts::DclContractsHelper::isPadButtonMessage( f_HwMsg ) )
            {
                if( contracts::DclContractsHelper::isShiftPressed() )
                {
                    int padVelocity = contracts::DclContractsHelper::getPadVelocity( f_HwMsg );

                    if( contracts::DeviceControlContracts::BTN_PRESSED == contracts::DclContractsHelper::getPadState( f_HwMsg ) )
                    {
                        DBG( "Shift + Pad Number:" );
                        if( onShiftPadPressed )
                            onShiftPadPressed( contracts::DclContractsHelper::getPadNumber( f_HwMsg ) );
                    }
                }
                else
                {
                    int padVelocity = contracts::DclContractsHelper::getPadVelocity( f_HwMsg );
#if( DEBUG_PRINTS && DEBUG_PRINTS > 0 )
                    DBG( "Pad Number:" );
#endif
                    if( contracts::DeviceControlContracts::BTN_PRESSED == contracts::DclContractsHelper::getPadState( f_HwMsg ) )
                    {
                        switch( contracts::DclContractsHelper::getPadNumber( f_HwMsg ) )
                        {
                            case contracts::DeviceControlContracts::PadSequence::EN_PAD1: pads[0]->triggerClick(); break;
                            case contracts::DeviceControlContracts::PadSequence::EN_PAD2: pads[1]->triggerClick(); break;
                            case contracts::DeviceControlContracts::PadSequence::EN_PAD3: pads[2]->triggerClick(); break;
                            case contracts::DeviceControlContracts::PadSequence::EN_PAD4: pads[3]->triggerClick(); break;
                            case contracts::DeviceControlContracts::PadSequence::EN_PAD5: pads[4]->triggerClick(); break;
                            case contracts::DeviceControlContracts::PadSequence::EN_PAD6: pads[5]->triggerClick(); break;
                            case contracts::DeviceControlContracts::PadSequence::EN_PAD7: pads[6]->triggerClick(); break;
                            case contracts::DeviceControlContracts::PadSequence::EN_PAD8: pads[7]->triggerClick(); break;
                            case contracts::DeviceControlContracts::PadSequence::EN_PAD9: pads[8]->triggerClick(); break;
                            case contracts::DeviceControlContracts::PadSequence::EN_PAD10: pads[9]->triggerClick(); break;
                            case contracts::DeviceControlContracts::PadSequence::EN_PAD11: pads[10]->triggerClick(); break;
                            case contracts::DeviceControlContracts::PadSequence::EN_PAD12: pads[11]->triggerClick(); break;
                            case contracts::DeviceControlContracts::PadSequence::EN_PAD13: pads[12]->triggerClick(); break;
                            case contracts::DeviceControlContracts::PadSequence::EN_PAD14: pads[13]->triggerClick(); break;
                            case contracts::DeviceControlContracts::PadSequence::EN_PAD15: pads[14]->triggerClick(); break;
                            case contracts::DeviceControlContracts::PadSequence::EN_PAD16: pads[15]->triggerClick(); break;
                        };
                    }
                }
            }
        };

        bool visible = isVisible();
        if( visible && isSelectionEnabled() )
        {
            contracts::DclSignals::hardwareInputTriggered.connect( handle_Bank );
        }
        else
        {
            contracts::DclSignals::hardwareInputTriggered.disconnect( handle_Bank );
       }

        for( auto it : getChildren() )
            it->setVisible( visible );

        setupBreathingTimer();
    }

    void KickPadButtonGroupComponent::setupBreathingTimer()
    {
        if( isShowing() && beathingPads.size() )
        {
            if( !isTimerRunning() )
                startTimerHz( 4 );
        }
        else
        {
            stopTimer();
        }
    }

    void KickPadButtonGroupComponent::timerCallback()
    {
        for( auto it : beathingPads )
        {
            auto id = it - getBankOffset();
            if( id >= 0 && id < 16 )
            {
                pads[id]->setHighlight( !pads[id]->isHighlighted() );
            }
        }
    }

    void KickPadButtonGroupComponent::setPadButtonSpacer( int spacer )
    {
        padButtonSpacer = spacer;
        resized();
    }

    void KickPadButtonGroupComponent::setElementsEnable( int Index, const bool state )
    {
        if( juce::isPositiveAndBelow( Index, _NUM_PADS ) )
        {
            padData[(size_t)Index].flags.enable = state;
            refreshPad( Index );
        }
    }

    void KickPadButtonGroupComponent::setFocusOutline( bool State )
    {
        for( auto Index = 0; Index < _NUM_PADS; ++Index )
        {
            auto i = Index - bankOffset;
            if( juce::isPositiveAndBelow( i, _NUM_PADS_IN_COMPONENT ) )
                pads[i]->setFocusOutline( State );
        }
    }
}
