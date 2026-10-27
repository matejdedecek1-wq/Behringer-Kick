/*
 * Copyright (c) 2022 MUSIC Tribe
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

#include "_playback/_playback.h"

//==============================================================================
class AudioEngineApplication : public juce::JUCEApplication
{
public:
    //==============================================================================
    AudioEngineApplication() {}

    const juce::String getApplicationName() override
    {
        return "Audio Engine";
    }

    const juce::String getApplicationVersion() override
    {
        return "0.0.1";
    }

    bool moreThanOneInstanceAllowed() override
    {
        return false;
    }

    //==============================================================================
    void initialise( const juce::String& /*commandLine*/ ) override
    {
        // This method is where you should put your application's initialisation code..
        w = std::make_unique<HomeWindow>( getApplicationName() );
    }

    void shutdown() override
    {
        // Add your application's shutdown code here..
        w = nullptr;
    }

    //==============================================================================
    void systemRequestedQuit() override
    {
        // This is called when the app is being asked to quit: you can ignore this request and let the app
        // carry on running, or call quit() to allow the app to close.
        auto mgr = juce::ModalComponentManager::getInstance();
        if( mgr->cancelAllModalComponents() )
            juce::Timer::callAfterDelay( 100, [this] { quit(); } );
        else
            quit();
    }

    void anotherInstanceStarted( const juce::String& /*commandLine*/ ) override
    {
        // When another instance of the app is launched while this one is running, this method is invoked,
        // and the commandLine parameter tells you what the other instance's command-line arguments were.
    }

    //==============================================================================
    // This class implements the desktop window that contains an instance of our MainComponent class.
    class HomeWindow : public juce::DocumentWindow
    {
    public:
        HomeWindow( juce::String name )
            : juce::DocumentWindow( name, juce::Colours::black, juce::DocumentWindow::closeButton )
        {
            setUsingNativeTitleBar( false );
            setTitleBarHeight( 0 );
            setTopLeftPosition( 0, 0 );
            setSize( ui::layout::getAppWidth(), ui::layout::getAppHeight() );
#if( JUCE_IOS || JUCE_ANDROID || JUCE_LINUX || __linux__ )
            juce::Desktop::getInstance().setKioskModeComponent( this, false );
#endif
            setVisible( true );
            toFront( true );
            //setContentOwned( AppHome::Instance(), true );
            setContentOwned( new kick_ui::AppWindow{}, true );
        }

        ~HomeWindow()
        {
        }

        void closeButtonPressed() override { juce::JUCEApplication::getInstance()->systemRequestedQuit(); }
    };

private:
    std::unique_ptr<juce::DocumentWindow> w;
};

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION( AudioEngineApplication )
