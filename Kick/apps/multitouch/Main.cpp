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

#include <juce_core/juce_core.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_audio_devices/juce_audio_devices.h>

class ButtonComponent : public juce::Component
{
public:
    ButtonComponent() = default;

private:
    void paint( juce::Graphics& g ) override
    {
        auto width  = getWidth();
        auto height = getHeight();
        auto x1 = 0, x2 = width, y1 = 0;
        auto vericalGap = height / 16;
        g.setColour( juce::Colours::darkgrey );
        for( ; y1 <= height; )
        {
            g.drawHorizontalLine( y1, x1, x2 );
            y1 += vericalGap;
        }
        x1 = y1            = 0;
        auto y2            = height;
        auto horizantalGap = width / 16;
        for( ; x1 <= width; )
        {
            g.drawVerticalLine( x1, y1, y2 );
            x1 += horizantalGap;
        }
    }

    void mouseDown( const juce::MouseEvent& event )
    {
        if( event.source.isTouch() )
        {
            activeTouches[event.source.getIndex()] = event.position;
            if( activeTouches.size() == 2 )
            {
                initialDistance   = calculateDistance();
                initialZoomFactor = zoomFactor;
            }
        }
    }

    void mouseDrag( const juce::MouseEvent& event )
    {
        if( event.source.isTouch() )
        {
            activeTouches[event.source.getIndex()] = event.position;
            if( activeTouches.size() == 2 )
            {
                float currentDistance = calculateDistance();
                zoomFactor            = initialZoomFactor * ( currentDistance / initialDistance );
                DBG( "Zoom Factor  " << zoomFactor );
                updateViewPosition();
            }
        }
    }

    void mouseUp( const juce::MouseEvent& event )
    {
        if( event.source.isTouch() )
        {
            activeTouches.erase( event.source.getIndex() );
        }
    }

    void updateViewPosition()
    {
        auto bounds = getBounds();

        if( zoomFactor < 1.f )
            zoomFactor = 1.f;

        setSize( 1020 * zoomFactor, 596 * zoomFactor );
    }

    float calculateDistance()
    {
        auto it = activeTouches.begin();
        auto p1 = it->second;
        ++it;
        auto p2 = it->second;
        return p1.getDistanceFrom( p2 );
    }

    std::array<juce::ToggleButton, 4> buttons;

    std::map<int, juce::Point<float>> activeTouches;
    float                             zoomFactor{ 1.f };
    float                             initialDistance   = 0.0f;
    float                             initialZoomFactor = 1.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( ButtonComponent )
};

class MainComponent : public juce::Component
{
public:
    MainComponent()
    {
        addAndMakeVisible( multiTouchViewPort );

        multiTouchViewPort.setViewedComponent( &btnComp, false );
        multiTouchViewPort.setScrollBarsShown( false, false, true, true );
    }

    void resized() override
    {
        multiTouchViewPort.setBounds( 0, 0, 1020, 596 );
        btnComp.setSize( 1020, 596 );
    }

    void paint( juce::Graphics& g )
    {
        g.fillAll( juce::Colours::grey );
        g.setColour( juce::Colours::gold );
        g.drawRoundedRectangle( multiTouchViewPort.getBounds().toFloat().expanded( 2 ), 5.f, 2.f );
    }

private:
    juce::Viewport  multiTouchViewPort;
    ButtonComponent btnComp;
};

// Main window for the JUCE application
class MainWindow : public juce::DocumentWindow
{
public:
    MainWindow( juce::String name )
        : DocumentWindow( name,
                          juce::Colours::darkgrey,
                          DocumentWindow::allButtons )
    {
        setUsingNativeTitleBar( true );
        auto mainComp = new MainComponent();
        mainComp->setSize( getScreenBounds().getWidth(), getScreenBounds().getHeight() );
        setContentOwned( mainComp, true );

        setFullScreen( true ); // Set the window to full screen

        setVisible( true );
    }

    void closeButtonPressed() override
    {
        // Exit the application when the close button is pressed
        juce::JUCEApplication::getInstance()->systemRequestedQuit();
    }
};

// Main application class
class MultiTouchDemoApplication : public juce::JUCEApplication
{
public:
    MultiTouchDemoApplication() {}

    const juce::String getApplicationName() override { return "MultiTouch Demo"; }
    const juce::String getApplicationVersion() override { return "1.0"; }

    void initialise( const juce::String& commandLine ) override
    {
        mainWindow.reset( new MainWindow( getApplicationName() ) );
    }

    void shutdown() override
    {
        mainWindow = nullptr; // Clean up
    }

private:
    std::unique_ptr<MainWindow> mainWindow;
};

// This macro generates the main() function that launches the application
START_JUCE_APPLICATION( MultiTouchDemoApplication )
