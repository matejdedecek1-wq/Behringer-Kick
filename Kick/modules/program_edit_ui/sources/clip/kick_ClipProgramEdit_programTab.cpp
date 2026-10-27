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

    ClipProgramTab::ClipProgramTab( int& SelectedPad, const std::shared_ptr<mt::MTClipProgram>& PClip )
        : selectedPad( SelectedPad ), pClip( PClip )
    {
        pads.setPadButtonSpacer( 9 );
        pads.setSelectionEnabled( false );
        pads.getBankComponent().setSelectionEnabled( false );

        launchQuantize.setJustificationType( juce::Justification::centred );
        launchQuantize.setColours( ui::Theme::textColour( 2 ), ui::Theme::textColour( 1 ) );

        mt::utils::addAndMakeVisible( this, semi, fine, launchQuantize, pads );

        semi.onValueChanged = [&]( double Value ) {
            auto p = pClip.get();
            if( p ) p->semi = ( (int)Value );
        };
        fine.onValueChanged = [&]( double Value ) {
            auto p = pClip.get();
            if( p ) p->fine = ( (int)Value );
        };
        launchQuantize.onSelectionChanged = [&]( int Index, const juce::String& ) {
            auto p = pClip.get();
            if( p ) p->launchQuantize = mt::Kick::QuantizeMultipler[Index];
        };

        pads.OnButtonClicked = [this]( int Id ) {
            selectedPad = Id;
            refresh();
        };

        pads.OnDoubleClicked = [this]( int Id ) {
            selectedPad = Id;
            fileList.setContent( ( files ), -1 );
            fileList.setSelected( pads.getRow1Text( Id ), juce::dontSendNotification );
            fileList.show( "Pad - " + juce::String( Id ), [this, Id]( int R ) {
                if( R != UI::CANCEL && 0 <= R )
                {
                    if( R == 0 )
                    {
                        ae.removeSampleFromProgram( ae.getSelectedProgram(), Id );
                        pads.setRow1Text( Id, "", juce::dontSendNotification );
                    }
                    else
                    {
                        auto sample = ae.getAsset( mt::AssetTypes::Samples, R - 1 );
                        ae.addSampleToProgram( ae.getSelectedProgram(), Id, sample, 0 );
                        pads.setRow1Text( Id, juce::File( sample ).getFileNameWithoutExtension(), juce::dontSendNotification );
                    }
                }
            } );
        };
    }

    void ClipProgramTab::handleCommand( CPktMgr::HWMsg_t& f_HwMsg )
    {
        if( contracts::DclContractsHelper::isPadButtonMessage( f_HwMsg ) )
        {
            auto padNum = contracts::DclContractsHelper::getPadNumber( f_HwMsg );
            selectedPad = padNum + mt::getAudioInterface().getBankOffset();
            refresh();
        }
    }

    void ClipProgramTab::refresh()
    {
        files = ae.getAssetNames( mt::AssetTypes::Samples );
        files.insert( files.begin(), "Clear Assignment" );

        pads.clearAllPadData();
        pads.setActive( -1, true );
        pads.setPrefix( "Pad " );
        //pads.setBorderColor( ui::Theme::selectionColour( 1 ), 2.5f, juce::dontSendNotification );

        auto semiHandler   = [&]() {
            auto p = pClip.get();
            if( p ) semi.setValue( (double)p->semi.get(), juce::dontSendNotification ); };
        auto fineHandler   = [&]() {
            auto p = pClip.get();
            if( p ) fine.setValue( (double)p->fine.get(), juce::dontSendNotification ); };
        auto launchHandler = [&]() {
            auto p = pClip.get();
            if( p )
            {
                auto q   = p->launchQuantize.get();
                auto sel = 0; // Try to locate index in the list...
                for( auto& i : mt::Kick::QuantizeMultipler )
                {
                    if( juce::approximatelyEqual<float>( i, q ) )
                        break;
                    sel++;
                }
                launchQuantize.setSelected( sel, juce::dontSendNotification );
            }
        };

        scope.reset();
        if( auto p = pClip.get() )
        {
            // Setup scope tracker.
            scope = std::make_shared<mt::Kick::ScopeTracker>();

            std::vector<mt::MTISample*> samples;
            p->getSamples( samples );
            for( const auto& sample : samples )
            {
                auto text = juce::File( sample->layers[0]->source.get() ).getFileNameWithoutExtension();
                pads.setRow1Text( sample->keyNote.get(), text, juce::dontSendNotification );
            }

            // Setup handlers to updates values.
            p->semi.onValueChange.connect( semiHandler, scope );
            p->fine.onValueChange.connect( fineHandler, scope );
            p->launchQuantize.onValueChange.connect( launchHandler, scope );
            // Refresh values.
            p->semi.onValueChange();
            p->fine.onValueChange();
            p->launchQuantize.onValueChange();
        }
    }

    void ClipProgramTab::paint( juce::Graphics& g )
    {
        g.setColour( ui::Theme::backgroundColour( 4 ) );
        g.fillRoundedRectangle( 10.f, 10.f, 342.f, 384.f, 6.f );
        mt::utils::renderRoundedRect( g, { 10, 10, 342, 384 }, ui::Theme::backgroundColour( 0 ), 1.f, 6.f );
    }

    void ClipProgramTab::resized()
    {
        auto b = getLocalBounds().toFloat();

        semi.setBoundsRelative( 101.f / b.getWidth(), 51.f / b.getHeight(), 160.f / b.getWidth(), 60.f / b.getHeight() );
        fine.setBoundsRelative( 101.f / b.getWidth(), 171.f / b.getHeight(), 160.f / b.getWidth(), 60.f / b.getHeight() );

        launchQuantize.setBoundsRelative( 101.f / b.getWidth(), 291.f / b.getHeight(), 160.f / b.getWidth(), 60.f / b.getHeight() );

        pads.setBoundsRelative( 362.f / b.getWidth(), ( 10.f / b.getHeight() ), 648.f / b.getWidth(), ( b.getHeight() - 10.f ) / b.getHeight() );
    }

    void ClipProgramTab::visibilityChanged()
    {
        if( isVisible() )
            refresh();
        else
            scope.reset();
    }

}
