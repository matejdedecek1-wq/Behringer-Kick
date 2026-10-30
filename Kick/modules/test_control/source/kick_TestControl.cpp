/*
 * Copyright (c) 2024 MUSIC Tribe
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

#include <typeinfo>
#if defined( __GNUC__ ) || defined( __clang__ )
    #include <cxxabi.h>
#endif

namespace testcontrol
{
    namespace
    {
        void log( const juce::String& S ) { juce::Logger::writeToLog( "TestControl: " + S ); }

        juce::String typeName( const juce::Component& C )
        {
            const char* raw = typeid( C ).name();
#if defined( __GNUC__ ) || defined( __clang__ )
            int  status = 0;
            auto* name  = abi::__cxa_demangle( raw, nullptr, nullptr, &status );
            if( status == 0 && name != nullptr )
            {
                juce::String s( name );
                std::free( name );
                return s;
            }
#endif
            return raw;
        }

        juce::String nameOf( const juce::Component& C )
        {
            return C.getTitle().isNotEmpty() ? C.getTitle() : C.getName();
        }

        juce::String rectText( juce::Rectangle<int> R )
        {
            return juce::String( R.getX() ) + " " + juce::String( R.getY() ) + " " + juce::String( R.getWidth() ) + " " + juce::String( R.getHeight() );
        }

        /// Depth-first search of showing components, topmost (last-added) children first.
        juce::Component* findIn( juce::Component& Root, const std::function<bool( juce::Component& )>& Match )
        {
            for( int i = Root.getNumChildComponents(); --i >= 0; )
            {
                auto* c = Root.getChildComponent( i );
                if( c == nullptr || !c->isShowing() )
                    continue;
                if( auto* hit = findIn( *c, Match ) )
                    return hit;
                if( Match( *c ) )
                    return c;
            }
            return nullptr;
        }
    } // namespace

    std::unique_ptr<TestControl> TestControl::createFromEnvironment( juce::Component& Screen )
    {
        const auto path = juce::SystemStats::getEnvironmentVariable( "KICK_TEST_CONTROL", {} ).trim();
        if( path.isEmpty() )
            return nullptr;
        return std::make_unique<TestControl>( Screen, juce::File::getCurrentWorkingDirectory().getChildFile( path ) );
    }

    TestControl::TestControl( juce::Component& Screen, const juce::File& File )
        : screen( &Screen ), file( File )
    {
        if( !file.existsAsFile() )
            file.create();
        readPos = file.getSize(); // only lines written after start-up
        FocusFallback::enabled() = true;
        log( "listening on " + file.getFullPathName() );
        startTimer( 50 );
    }

    TestControl::~TestControl()
    {
        stopTimer();
        FocusFallback::clear();
        FocusFallback::enabled() = false;
    }

    void TestControl::timerCallback()
    {
        if( busy )
            return;

        if( queue.isEmpty() )
        {
            const auto size = file.getSize();
            if( size < readPos )
                readPos = 0; // the file was truncated
            if( size == readPos )
                return;
            juce::FileInputStream in( file );
            if( !in.openedOk() )
                return;
            in.setPosition( readPos );
            partial += in.readString();
            readPos = in.getPosition();
            while( partial.containsChar( '\n' ) )
            {
                queue.add( partial.upToFirstOccurrenceOf( "\n", false, false ).trim() );
                partial = partial.fromFirstOccurrenceOf( "\n", false, false );
            }
        }

        if( !queue.isEmpty() )
        {
            auto line = queue[0];
            queue.remove( 0 );
            if( line.isNotEmpty() )
                run( line );
        }
    }

    juce::Array<juce::Component*> TestControl::desktopWindows()
    {
        // The main window first, then every other showing desktop window.
        juce::Array<juce::Component*> list;
        if( screen != nullptr )
            list.add( screen.getComponent() );
        auto& desk = juce::Desktop::getInstance();
        for( int i = 0; i < desk.getNumComponents(); ++i )
            if( auto* c = desk.getComponent( i ); c != nullptr && c != screen.getComponent() && c->isShowing() )
                list.add( c );
        return list;
    }

    void TestControl::run( const juce::String& Line )
    {
        juce::StringArray tok;
        tok.addTokens( Line, " ", "\"" );
        tok.trim();
        tok.removeEmptyStrings();
        for( auto& t : tok )
            t = t.unquoted();
        if( tok.size() < 2 )
        {
            log( "error ? malformed line: " + Line );
            return;
        }
        const auto seq = tok[0];
        const auto cmd = tok[1];
        tok.removeRange( 0, 2 );

        auto ok   = [seq]() { log( "ok " + seq ); };
        auto fail = [seq]( const juce::String& Why ) { log( "error " + seq + " " + Why ); };

        if( screen == nullptr )
            return fail( "no screen" );

        if( cmd == "touch" )
        {
            if( tok.size() != 2 )
                return fail( "usage: touch X Y" );
            auto r = peerTouch( { tok[0].getFloatValue(), tok[1].getFloatValue() }, ok, fail );
            if( r.isNotEmpty() )
                fail( r );
            return;
        }

        if( cmd == "tap" )
        {
            if( tok.size() != 2 )
                return fail( "usage: tap X Y" );
            auto r = touch( *screen, { tok[0].getFloatValue(), tok[1].getFloatValue() }, ok );
            if( r.isNotEmpty() )
                fail( r );
            return;
        }

        if( cmd == "click" || cmd == "clickat" )
        {
            // clickat <name> <fx> <fy>: a touch at that fraction of the component's size, for
            // controls with untitled parts (the On half of an Off / On toggle).
            if( cmd == "clickat" && tok.size() < 3 )
                return fail( "usage: clickat NAME FX FY" );
            const auto name   = cmd == "click" ? tok.joinIntoString( " " ) : juce::StringArray( tok.begin(), tok.size() - 2 ).joinIntoString( " " );
            auto*      target = findComponent( name );
            if( target == nullptr )
                return fail( "no showing component named: " + name );
            auto pos = target->getLocalBounds().toFloat().getCentre();
            if( cmd == "clickat" )
                pos = { tok[tok.size() - 2].getFloatValue() * (float)target->getWidth(), tok[tok.size() - 1].getFloatValue() * (float)target->getHeight() };
            auto r = touch( *target, pos, ok );
            if( r.isNotEmpty() )
                fail( r );
            return;
        }

        if( cmd == "shot" )
        {
            if( tok.isEmpty() )
                return fail( "usage: shot OUT.png" );
            auto r = snapshot( juce::File::getCurrentWorkingDirectory().getChildFile( tok.joinIntoString( " " ) ) );
            return r.isEmpty() ? ok() : fail( r );
        }

        if( cmd == "windows" )
        {
            auto& s = *screen;
            log( "window main\t" + typeName( s ) + "\tscreen " + rectText( s.getLocalBounds() ) + "\tdisplay " + rectText( s.getScreenBounds() ) );
            for( auto* c : s.getChildren() )
                if( c->isVisible() && ( c->isCurrentlyModal( false ) || dynamic_cast<juce::BubbleComponent*>( c ) != nullptr ) )
                    log( "window overlay\t" + typeName( *c ) + "\tscreen " + rectText( c->getBounds() ) + "\tdisplay " + rectText( c->getScreenBounds() ) );
            auto wins = desktopWindows();
            for( int i = 1; i < wins.size(); ++i )
                log( "window desktop\t" + typeName( *wins[i] ) + "\tscreen " + rectText( s.getLocalArea( nullptr, wins[i]->getScreenBounds() ) ) + "\tdisplay " + rectText( wins[i]->getScreenBounds() ) );
            return ok();
        }

        if( cmd == "list" )
        {
            const auto filter = tok.joinIntoString( " " );
            for( auto* w : desktopWindows() )
                findIn( *w, [&]( juce::Component& C ) {
                    const auto n = nameOf( C );
                    if( n.isNotEmpty() && ( filter.isEmpty() || n.containsIgnoreCase( filter ) ) )
                    {
                        // "popup": inside another desktop window, or inside a modal popup or a bubble on the screen.
                        bool inPopup = w != screen.getComponent();
                        for( auto* p = &C; !inPopup && p != nullptr && p != screen.getComponent(); p = p->getParentComponent() )
                            inPopup = p->isCurrentlyModal( false ) || dynamic_cast<juce::BubbleComponent*>( p ) != nullptr;
                        log( "item " + typeName( C ) + "\t" + n + "\t" + rectText( screen->getLocalArea( &C, C.getLocalBounds() ) ) + ( inPopup ? "\tpopup" : "\tscreen" ) );
                    }
                    return false;
                } );
            return ok();
        }

        if( cmd == "status" )
        {
            log( "status " + ( statusProvider ? statusProvider() : juce::String( "n/a" ) ) );
            return ok();
        }

        if( cmd == "echo" )
        {
            log( "echo " + tok.joinIntoString( " " ) );
            return ok();
        }

        fail( "unknown command " + cmd );
    }

    juce::Component* TestControl::findComponent( const juce::String& Name )
    {
        // Zero-size components are skipped: some dialog contents carry unused 0x0 "Apply" /
        // "Cancel" buttons of the same name as the dialog's real ones.
        auto match = [&]( juce::Component& C ) { return nameOf( C ) == Name && !C.getLocalBounds().isEmpty(); };
        // Other desktop windows sit above the main window; within it, popups are its last children
        // and are searched first.
        auto wins = desktopWindows();
        for( int i = wins.size(); --i >= 0; )
            if( auto* hit = findIn( *wins[i], match ) )
                return hit;
        return nullptr;
    }

    juce::String TestControl::peerTouch( juce::Point<float> Pos, std::function<void()> Done, std::function<void( const juce::String& )> Fail )
    {
        auto* top = screen->getTopLevelComponent();
        if( top->getPeer() == nullptr )
            return "the main window has no peer";
        const auto inTop = top->getLocalPoint( screen.getComponent(), Pos );

        busy        = true;
        auto finish = [this, Done, Fail, weak = std::weak_ptr<bool>( alive )]( const juce::String& Error ) {
            if( weak.expired() )
                return;
            busy = false;
            if( Error.isEmpty() )
                Done();
            else
                Fail( Error );
        };

        // JUCE's own test: Component::contains asks the peer whether the window is the topmost
        // one at that point. If not, bring the window to the front (that activates the app, as a
        // finger on the unit's screen would find it in front anyway) and look again.
        juce::Component::SafePointer<juce::Component> safeTop( top );
        auto dispatch = [this, safeTop, inTop, Pos, finish]() {
            if( safeTop == nullptr || safeTop->getPeer() == nullptr )
                return finish( "the main window went away" );
            if( !safeTop->contains( inTop ) )
                return finish( "a real touch needs the window to be the topmost one at " + juce::String( Pos.x ) + " " + juce::String( Pos.y )
                               + " (screen locked, or another window covers it)" );
            if( auto* c = safeTop->getComponentAt( inTop.roundToInt() ) )
                log( "touch " + nameOf( *c ) + " (" + typeName( *c ) + ") through the window" );
            auto* peer  = safeTop->getPeer();
            const auto raw   = inTop * safeTop->getDesktopScaleFactor(); // the peer's own (unscaled) coordinates
            // The mouse source: JUCE creates touch sources only on platforms that report touch
            // input (not macOS), so a touch-type event would have no source and be dropped.
            auto       event = [peer, raw]( bool Down ) {
                peer->handleMouseEvent( juce::MouseInputSource::InputSourceType::mouse, raw,
                                        Down ? juce::ModifierKeys().withFlags( juce::ModifierKeys::leftButtonModifier ) : juce::ModifierKeys(),
                                        Down ? 1.0f : juce::MouseInputSource::defaultPressure, juce::MouseInputSource::defaultOrientation,
                                        juce::Time::currentTimeMillis() );
            };
            event( true );
            juce::Timer::callAfterDelay( 80, [safeTop, peer, event, finish]() {
                if( safeTop != nullptr && juce::ComponentPeer::isValidPeer( peer ) )
                    event( false );
                finish( {} );
            } );
        };
        if( top->contains( inTop ) )
            dispatch();
        else
        {
            // Float the window above the others for the touch (no activation, no focus taken
            // from whatever app is in front), then put it back.
            log( "touch: floating the window above the others for the touch" );
            const bool wasOnTop = top->isAlwaysOnTop();
            top->setAlwaysOnTop( true );
            juce::Timer::callAfterDelay( 150, [dispatch, safeTop, wasOnTop]() {
                dispatch();
                juce::Timer::callAfterDelay( 400, [safeTop, wasOnTop]() {
                    if( safeTop != nullptr && !wasOnTop )
                        safeTop->setAlwaysOnTop( false );
                } );
            } );
        }
        return {};
    }

    juce::String TestControl::touch( juce::Component& Target, juce::Point<float> Pos, std::function<void()> Done )
    {
        // Hit-test from the window, like a finger: whatever is on top at that point (a popup, a
        // dialog, a notice) gets the touch, not necessarily the component that was named.
        auto* top = Target.getTopLevelComponent();
        auto* c   = top->getComponentAt( top->getLocalPoint( &Target, Pos ).roundToInt() );
        if( c == nullptr )
            c = &Target;
        const auto touchedPos = c->getLocalPoint( &Target, Pos );
        log( "touched " + nameOf( *c ) + " (" + typeName( *c ) + ")" );
        if( c->isCurrentlyBlockedByAnotherModalComponent() )
        {
            auto* modal = juce::Component::getCurrentlyModalComponent();
            return "blocked by modal component " + ( modal != nullptr ? nameOf( *modal ) + " (" + typeName( *modal ) + ")" : juce::String( "?" ) );
        }

        busy        = true;
        auto finish = [this, Done, weak = std::weak_ptr<bool>( alive )]() {
            if( weak.expired() )
                return; // the module was destroyed (app shutting down)
            busy = false;
            Done();
        };

        // Buttons: triggerClick, which is what an accessibility press does.
        auto* button = dynamic_cast<juce::Button*>( c );
        if( button == nullptr )
            button = c->findParentComponentOfClass<juce::Button>();
        if( button != nullptr )
        {
            button->triggerClick();
            juce::Timer::callAfterDelay( 200, finish );
            return {};
        }

        // Anything else: what JUCE does on a click first, keyboard focus to the touched component
        // or the nearest parent that wants it. That is how a touched control receives the jog
        // wheel, + / -, PUSH and the slider. JUCE refuses focus while the window is not the key
        // window (screen locked), so it is also recorded in the fallback.
        {
            juce::Component* f = c;
            while( f != nullptr && !f->getWantsKeyboardFocus() )
                f = f->getParentComponent();
            if( f != nullptr )
            {
                f->grabKeyboardFocus();
                FocusFallback::note( f, true );
                log( "focus " + nameOf( *f ) + " (" + typeName( *f ) + ")" );
            }
        }

        // Then mouseDown now and mouseUp 80 ms later, at the touched point.
        const auto pos  = touchedPos;
        const auto now  = juce::Time::getCurrentTime();
        auto       make = [c, pos, now]( juce::ModifierKeys Mods ) {
            return juce::MouseEvent( juce::Desktop::getInstance().getMainMouseSource(), pos, Mods,
                                     juce::MouseInputSource::defaultPressure, juce::MouseInputSource::defaultOrientation,
                                     juce::MouseInputSource::defaultRotation, juce::MouseInputSource::defaultTiltX,
                                     juce::MouseInputSource::defaultTiltY, c, c, juce::Time::getCurrentTime(), pos, now, 1, false );
        };
        c->mouseDown( make( juce::ModifierKeys( juce::ModifierKeys::leftButtonModifier ) ) );
        juce::Component::SafePointer<juce::Component> safe( c );
        juce::Timer::callAfterDelay( 80, [safe, make, finish]() {
            if( safe != nullptr )
                safe->mouseUp( make( {} ) );
            finish();
        } );
        return {};
    }

    juce::String TestControl::snapshot( const juce::File& Out )
    {
        auto write = []( juce::Component& C, const juce::File& F ) -> juce::String {
            auto img = C.createComponentSnapshot( C.getLocalBounds(), true, 1.0f );
            F.deleteFile();
            F.getParentDirectory().createDirectory();
            juce::FileOutputStream os( F );
            if( !os.openedOk() || !juce::PNGImageFormat().writeImageToStream( img, os ) )
                return "cannot write " + F.getFullPathName();
            log( "shot " + F.getFullPathName() + " " + juce::String( img.getWidth() ) + "x" + juce::String( img.getHeight() ) + " " + typeName( C ) );
            return {};
        };

        // OUT.png: the main window with everything on it (popups, bubbles) at its native size;
        // OUT-<n>.png: any other showing desktop window.
        auto wins = desktopWindows();
        auto r    = write( *wins[0], Out );
        for( int i = 1; i < wins.size() && r.isEmpty(); ++i )
            r = write( *wins[i], Out.getSiblingFile( Out.getFileNameWithoutExtension() + "-" + juce::String( i ) + ".png" ) );
        return r;
    }
} // namespace testcontrol
