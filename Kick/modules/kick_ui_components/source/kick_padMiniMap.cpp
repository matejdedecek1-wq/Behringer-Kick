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
    constexpr int pad_spacer = 10;

    // Single PAD, in minimap view...
    KickMiniPad::KickMiniPad( const juce::Colour Normal, const juce::Colour Highlighted, const juce::Colour DisabledColor )
    {
        setColours( Normal, Highlighted, DisabledColor );
        setInterceptsMouseClicks( true, false );
    }

    KickMiniPad::~KickMiniPad()
    {
    }

    void KickMiniPad::setActive( bool Active )
    {
        active = Active;
        repaint();
    }

    void KickMiniPad::setHighlight( bool Highlight )
    {
        highlight = Highlight;
        repaint();
    }

    void KickMiniPad::triggerClick()
    {
        if( onClicked )
            onClicked( id );
    }

    void KickMiniPad::makeVisible( Component* Parent )
    {
        Parent->addAndMakeVisible( dynamic_cast<Component&>( *this ) );
    }

    void KickMiniPad::setColours( const juce::Colour Normal, const juce::Colour Highlighted, const juce::Colour DisabledColor, const juce::Colour SoloColor, bool IsSolo )
    {
        normalColor    = Normal;
        highlightColor = Highlighted;
        disabledColor  = DisabledColor;
        soloColor      = SoloColor;
        solo           = IsSolo;
    }

    void KickMiniPad::paint( juce::Graphics& g )
    {
        juce::Path p;
        p.addRoundedRectangle( getLocalBounds(), cornerSize );
        g.setColour( ( active ? solo ? soloColor : ( highlight ? highlightColor : normalColor ) : disabledColor ) );
        g.fillPath( p );
    }

    void KickMiniPad::resized() {}

    void KickMiniPad::mouseDown( const juce::MouseEvent& event )
    {
        if( onClicked )
            onClicked( id );
    }

    // Mini Map of single PAD GROUP...
    KickMiniPadGroup::KickMiniPadGroup( juce::String Bank, juce::Justification::Flags Position )
        : bank_( Bank )
        , pos_( Position )
    {
        bank_.setValueJustification( juce::Justification::centred );
        bank_.setColours( ui::Theme::textColour( 2 ), juce::Colours::transparentBlack ); //
        mt::utils::addAndMakeVisible( this, bank_ );
        bank_.setInterceptsMouseClicks( false, false );
        bank_.setValueFontHeight( 20 );
        for( int i = 0; i < NUM_PADS_IN_BANK; ++i )
        {
            pads_.push_back( std::make_unique<KickMiniPad>( ui::Theme::outlineColour( 5 ), ui::Theme::outlineColour( 1 ) ) );
            pads_.back()->setId( i );
            pads_.back()->makeVisible( this );
            pads_.back()->onClicked = [&]( int Id ) {
                if( onClicked )
                    onClicked( bank_.getText(), Id );
            };
        }
    }

    KickMiniPadGroup::~KickMiniPadGroup()
    {
    }

    void KickMiniPadGroup::select( bool Select )
    {
        isSelected_ = Select;
        bank_.setColours( isSelected_ ? ui::Theme::textColour( 2 ) : ui::Theme::textColour( 2 ), juce::Colours::transparentBlack );
        repaint();
    }

    void KickMiniPadGroup::setJustification( juce::Justification::Flags Justification )
    {
        pos_ = Justification;
        resized();
    }

    void KickMiniPadGroup::setColours( int Index, const juce::Colour Normal, const juce::Colour Highlighted, const juce::Colour DisabledColor, const juce::Colour SoloColor, bool IsSolo )
    {
        if( juce::isPositiveAndBelow( Index, NUM_PADS_IN_BANK ) )
            pads_[Index]->setColours( Normal, Highlighted, DisabledColor, SoloColor, IsSolo );
        else
            for( auto& it : pads_ )
                it->setColours( Normal, Highlighted, DisabledColor, SoloColor, IsSolo );
    }

    void KickMiniPadGroup::setHighlight( int Index, bool Highlight )
    {
        if( juce::isPositiveAndBelow( Index, NUM_PADS_IN_BANK ) )
            pads_[Index]->setHighlight( Highlight );
        else
            for( auto& it : pads_ )
                it->setHighlight( Highlight );
    }

    void KickMiniPadGroup::setActive( int Index, bool Active )
    {
        if( juce::isPositiveAndBelow( Index, NUM_PADS_IN_BANK ) )
            pads_[Index]->setActive( Active );
        else
            for( auto& it : pads_ )
                it->setActive( Active );
    }

    void KickMiniPadGroup::clearAll()
    {
        isSelected_ = false;
        for( auto& it : pads_ )
            it->setHighlight( false );
    }

    bool KickMiniPadGroup::isHighlight( int Index )
    {
        if( juce::isPositiveAndBelow( Index, NUM_PADS_IN_BANK ) )
            return pads_[Index]->isHighlight();
        return false;
    }

    void KickMiniPadGroup::triggerClick( int Index )
    {
        if( juce::isPositiveAndBelow( Index, NUM_PADS_IN_BANK ) )
            return pads_[Index]->triggerClick();
    }

    void KickMiniPadGroup::paint( juce::Graphics& g )
    {
        juce::Path p;
        auto       b = getLocalBounds();
        p.addRoundedRectangle( b.getX(), b.getY(), b.getWidth(), b.getHeight(), 6.f, 6.f );
        g.setColour( isSelected_ ? ui::Theme::backgroundColour( 7 ) : ui::Theme::backgroundColour( 1 ) );
        g.fillPath( p );
        g.setColour( ui::Theme::outlineColour( 3 ) );
        g.strokePath( p, juce::PathStrokeType( 1.0f ) );

        if( isSelected_ )
        {
            g.setColour( ui::Theme::outlineColour( 1 ).withAlpha( 0.5f ) );
            g.drawRoundedRectangle( (float)b.getX(), (float)b.getY(), (float)b.getWidth(), (float)b.getHeight(), 6.f, 1.5f );
        }
    }

    void KickMiniPadGroup::resized()
    {
        auto                 bounds = getLocalBounds();
        juce::Rectangle<int> bankBounds;
        int                  removeFromLeft = 0;

        // Setup bounds for display of bank name
        if( pos_ == juce::Justification::left )
        {
            removeFromLeft = leftPadAlign;
            bankBounds     = bounds.removeFromLeft( labelWidth_ + leftTrim_ ).withTrimmedLeft( leftTrim_ );
        }

        if( pos_ == juce::Justification::right )
        {
            removeFromLeft = rightPadAlign;
            bankBounds     = bounds.removeFromRight( labelWidth_ + rightTrim_ ).withTrimmedRight( rightTrim_ );
        }
        bank_.setBounds( bankBounds.withTrimmedTop( topTrim_ ).withTrimmedBottom( bottomTrim_ ) );

        for( auto j = 0; j < NUM_ROWS_IN_BANK; j++ )
        {
            auto y = j * ( padHeight + padSpacer ) + removeFromTop;
            for( auto k = 0; k < NUM_COLS_IN_BANK; k++ )
            {
                auto x  = k * ( miniPadWidth + padSpacer ) + removeFromLeft;
                auto pi = k + ( NUM_ROWS_IN_BANK - 1 - j ) * NUM_COLS_IN_BANK;
                pads_[pi]->setBounds( x, y, miniPadWidth, padHeight );
            }
        }
    }

    void KickMiniPadGroup::mouseDown( const juce::MouseEvent& e )
    {
        if( onClicked )
            onClicked( bank_.getText(), -1 );
    }

    // Mini Map of PAD GROUPS...
    KickPadMiniMap::KickPadMiniMap( const juce::Colour& Normal, const juce::Colour& Highlighted, const juce::Colour& Disabled, Layout Lay )
    {
        layout_ = Lay;
        for( int i = 0; i < NUM_BANKS; ++i )
        {
            if( Vertical == layout_ )
                banks_.push_back( std::make_unique<KickMiniPadGroup>( mt::Kick::padBankNames[(size_t)i], i & 1 ? juce::Justification::right : juce::Justification::left ) );
            else
                banks_.push_back( std::make_unique<KickMiniPadGroup>( mt::Kick::padBankNames[(size_t)i], juce::Justification::top ) );
            mt::utils::addAndMakeVisible( this, banks_.back().get() );
            banks_.back().get()->setColours( -1, Normal, Highlighted, Disabled );
            banks_.back().get()->onClicked = [&]( juce::String Bank, int PadID ) {
                if( onClicked )
                    onClicked( Bank, PadID );
            };
        }
        setInterceptsMouseClicks( false, true );
    }

    void KickPadMiniMap::select( const juce::String& Bank, bool Selection )
    {
        auto idx = ( (int)*Bank.getCharPointer() - (int)'A' );
        select( idx, Selection );
    }

    void KickPadMiniMap::select( int BankIndex, bool Selection )
    {
        for( int i = 0; i < NUM_BANKS; ++i )
            banks_[i]->select( i == BankIndex ? Selection : false );
    }

    void KickPadMiniMap::setColours( int Index, const juce::Colour Normal, const juce::Colour Highlighted, const juce::Colour DisabledColor, const juce::Colour SoloColor, bool IsSolo )
    {
        if( juce::isPositiveAndBelow( Index, ( NUM_BANKS * NUM_PADS_IN_BANK ) ) )
        {
            auto b = Index / NUM_PADS_IN_BANK;
            auto i = Index % NUM_PADS_IN_BANK;
            banks_[b]->setColours( i, Normal, Highlighted, DisabledColor, SoloColor, IsSolo );
        }
        else
            for( auto& it : banks_ )
                it->setColours( -1, Normal, Highlighted, DisabledColor, SoloColor, IsSolo );
    }

    void KickPadMiniMap::setActive( int Index, bool Active )
    {
        if( juce::isPositiveAndBelow( Index, ( NUM_BANKS * NUM_PADS_IN_BANK ) ) )
        {
            auto b = Index / NUM_PADS_IN_BANK;
            auto i = Index % NUM_PADS_IN_BANK;
            banks_[b]->setActive( i, Active );
        }
        else
            for( auto& it : banks_ )
                it->setActive( -1, Active );
    }

    void KickPadMiniMap::setHighlight( int Index, bool Highlight )
    {
        if( juce::isPositiveAndBelow( Index, ( NUM_BANKS * NUM_PADS_IN_BANK ) ) )
        {
            auto b = Index / NUM_PADS_IN_BANK;
            auto i = Index % NUM_PADS_IN_BANK;
            banks_[b]->setHighlight( i, Highlight );
        }
        else
            for( auto& it : banks_ )
                it->setHighlight( -1, Highlight );
    }

    void KickPadMiniMap::highlightAll( bool Highlight )
    {
        setHighlight( -1, Highlight );
    }

    void KickPadMiniMap::clearAll()
    {
        for( auto& it : banks_ )
            it->clearAll();
    }

    bool KickPadMiniMap::isHighlight( int Index )
    {
        if( juce::isPositiveAndBelow( Index, ( NUM_BANKS * NUM_PADS_IN_BANK ) ) )
        {
            auto b = Index / NUM_PADS_IN_BANK;
            auto i = Index % NUM_PADS_IN_BANK;
            return banks_[b]->isHighlight( i );
        }
        return false;
    }

    void KickPadMiniMap::triggerClick( int Index )
    {
        if( juce::isPositiveAndBelow( Index, ( NUM_BANKS * NUM_PADS_IN_BANK ) ) )
        {
            auto b = Index / NUM_PADS_IN_BANK;
            auto i = Index % NUM_PADS_IN_BANK;
            banks_[b]->triggerClick( i );
        }
    }

    void KickPadMiniMap::makeVisible( Component* Parent )
    {
        Parent->addAndMakeVisible( dynamic_cast<Component&>( *this ) );
    }

    void KickPadMiniMap::setLayout( Layout layout )
    {
        layout_ = layout;

        for( int i = 0; i < NUM_BANKS; ++i )
        {
            if( Vertical == layout_ )
            {
                banks_[i]->setJustification( ( i & 1 ? juce::Justification::right : juce::Justification::left ) );
            }
            else
            {
                banks_[i]->setJustification( juce::Justification::top );
            }
        }
        resized();
    }

    void KickPadMiniMap::resized()
    {
        auto numRows = ( Vertical == layout_ ) ? 4 : 2;
        auto numCols = ( Vertical == layout_ ) ? 2 : 4;
        int  i       = 0;

        auto b = getLocalBounds();
        auto w = (int)( ( b.getWidth() - ( ( numCols - 1 ) * groupPadSpacer ) - leftRightSpace ) / numCols );
        auto h = (int)( ( b.getHeight() - ( ( numRows - 1 ) * groupPadSpacer ) - topBottomSpace ) / numRows );
        for( auto row = 0; row < numRows; ++row )
        {
            auto y = row * ( h + groupPadSpacer ) + topSpacer;

            for( auto col = 0; col < numCols; ++col )
            {
                auto x = col * ( w + groupPadSpacer ) + leftSpacer;
                banks_[i++]->setBounds( x, y, w, h );
            }
        }
    }
}
