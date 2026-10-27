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
    Looper::Looper()
        : kick_ui::IScreen( kick_ui::Screen::MenuLooper )
    {
        options.showPanSlider( false );
        options.showRecordButton( false );
        options.showOutputs( false );

        options.setInputs( { "IN L - OUT MONO", "IN R - OUT MONO", "IN L+R - OUT MONO", "IN L - OUT STEREO", "IN R - OUT STEREO", "IN L+R - OUT STEREO" }, 5 );

        clearBtn.setEnabled( false );
        exportBtn.setEnabled( false );

        mt::utils::addChildComponents( this, header, options, recorder, exportBtn, clearBtn );

        header.onBackButtonPressed = [this]( bool ) { closeScreen(); };

        // Recorder buttons...
        recorder.recordingStarted.connect( [this]( bool value ) {
            clearBtn.setEnabled( value );
            exportBtn.setEnabled( value );
        } );

        // Bottom panel
        exportBtn.onClick = [&]() {
            auto p = new KeepDiscardSampleComponent();
            mt::utils::showOptions( "EXPORT", p, [p, this]( int R ) {
                if( R == UI::BUTTONS::APPLY )
                {
                    auto& ae         = mt::getAudioInterface();
                    auto  filePath   = Sampler::getValidFileName( ae.getProjectDir().getSiblingFile( p->getSampleName() + ".wav" ).getFullPathName().toStdString() );
                    auto  sampleFile = ae.getProjectDir().getSiblingFile( filePath ).getFullPathName();
                    recorder.exportRecording( sampleFile );
                    ae.addAsset( sampleFile, mt::AssetTypes::Samples );
                    if( p->getSelectedProgram() != 0 )
                    {
                        auto progType = ae.getProgram( ae.getSelectedProgram() )->getType();
                        if( progType == mt::MTProgramType::Drum || progType == mt::MTProgramType::Keygroup || progType == mt::MTProgramType::Clip || progType == mt::MTProgramType::Audio )
                            ae.addSampleToProgram( ae.getSelectedProgram(), p->getPadIndex(), sampleFile );
                    }
                }
                if( p ) delete p;
                exportBtn.setToggleState( false, juce::dontSendNotification );
            } );
        };

        clearBtn.onClick = [&]() {
            recorder.clearRecording();
            clearBtn.setToggleState( false, juce::dontSendNotification );
            mt::utils::setEnabled( false, exportBtn, clearBtn );
        };

        options.onInsertsClicked = [&]() {
            kick_ui::signals::onScreenData( kick_ui::Screen::Inserts, (void*)( dynamic_cast<mt::MTIInsertsInterface*>( mt::getAudioInterface().getLooperInsertsInterface() ) ) );
            kick_ui::signals::sigScreenOpen( kick_ui::Screen::Inserts );
        };

        ui::themeChanged.connect( [&]() {
            exportBtn.setImages( "Song_Export", "Song_Export_Active" );
            clearBtn.setImages( "Song_Clear", "Song_Clear_Active" );
            header.setIcon( "Icon_Looper" );
        } );
    }

    void Looper::paint( juce::Graphics& g )
    {
        g.setColour( ui::Theme::backgroundColour( 1 ) );
        g.fillRoundedRectangle( options.getBounds().toFloat(), 5.0f );
        g.fillRoundedRectangle( recorder.getBounds().toFloat(), 5.0f );
    }

    void Looper::resized()
    {
        auto b = getLocalBounds();
        // Set header bar...
        header.setBounds( b.removeFromTop( ui::layout::header.getHeight() ) );
        exportBtn.setBoundsRelative( .7318f, .0299f, .0489f, .0823f );
        clearBtn.setBoundsRelative( .7847f, .0299f, .0489f, .0823f );

        options.setBounds( b.removeFromLeft( 362 ).reduced( 10 ) );
        b.removeFromRight( 10 );
        recorder.setBounds( b.reduced( 0, 10 ) );
    }

    inline void Looper::handleCommand( CPktMgr::HWMsg_t& hwMsg )
    {
        recorder.handleCommand( hwMsg ); // Forward
    }

    void Looper::visibilityChanged()
    {
        auto visible = isVisible();
        for( auto it : getChildren() )
            it->setVisible( visible );
        if( isVisible() )
        {
            std::vector<juce::String> insertList;
            auto                      pEffects = mt::getAudioInterface().getLooperInsertsInterface();
            pEffects->getInserts( insertList );
            options.setInsertsList( insertList );
        }
    }
}
