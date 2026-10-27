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

namespace kick_ui
{
#define CV_PROG_DEBUG ( 0 )

    CvProgramEdit::CvProgramEdit()
        : ProgEditInterface( mt::MTProgramType::Cv ), cvInterface( mt::CV::getCVInterface() )
    {
        for( int i = 0; i < mt::CV::MaxCVPorts; ++i )
        {
            cv[i].setTitle( "CV OUT " + juce::String( i + 1 ) );
            cv[i].setEnabled( false );
            cv[i].setRange( 0.0, 5.0 );
            cv[i].setTextValueSuffix( " V" );
            cv[i].showTitle( true, juce::Justification::centredTop );
            cv[i].showValue( true, juce::Justification::centredBottom, juce::Slider::TextBoxBelow );
            //cv[i].setValue( 2.5, juce::dontSendNotification );
            addAndMakeVisible( cv[i] );
        }
    }

    CvProgramEdit::~CvProgramEdit()
    {
        stopTimer();
    }

    void CvProgramEdit::refresh()
    {
    }

    void CvProgramEdit::handleCommand( CPktMgr::HWMsg_t& )
    {
    }

    void CvProgramEdit::timerCallback()
    {
        cvInterface.readAllchannelVoltage( cvValues );
        auto max = juce::jmin<int>( mt::CV::MaxCVPorts, cvValues.size() );
        for( int i = 0; i < max; ++i )
        {
            cv[i].setValue( double( cvValues[i] ), juce::dontSendNotification );
        }

#if defined( CV_PROG_DEBUG ) && ( 0 < CV_PROG_DEBUG )
        for( int i = 0; i < mt::CV::MaxCVPorts; ++i )
            cv[i].setValue( juce::Random::getSystemRandom().nextDouble(), juce::dontSendNotification );
#endif
    }

    void CvProgramEdit::paint( juce::Graphics& g )
    {
        auto b = getLocalBounds().reduced( 10 ).toFloat();
        g.setColour( ui::Theme::backgroundColour( 1 ) );
        g.fillRoundedRectangle( b, 5.f );
    }

    void CvProgramEdit::resized()
    {
        cv[0].setBoundsRelative( .0517f, .2358f, .1562f, .2008f );
        cv[1].setBoundsRelative( .2919f, .2358f, .1562f, .2008f );
        cv[2].setBoundsRelative( .5322f, .2358f, .1562f, .2008f );
        cv[3].setBoundsRelative( .7724f, .2358f, .1562f, .2008f );
        cv[4].setBoundsRelative( .05174, .524f, .1562f, .2008f );
        cv[5].setBoundsRelative( .2919f, .524f, .1562f, .2008f );
        cv[6].setBoundsRelative( .5322f, .524f, .1562f, .2008f );
        cv[7].setBoundsRelative( .7724f, .524f, .1562f, .2008f );
    }

    void CvProgramEdit::visibilityChanged()
    {
        auto visible = isVisible();
        if( visible )
        {
            auto selected = ae.getSelectedProgram();

            startTimerHz( 20 );
        }
        else
            stopTimer();
    }
}
