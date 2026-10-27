#include "kick_SEViewComponent.h"

namespace mt
{
    namespace ac
    {
#define splitManualRegionHeight 40
#define positionerTriWidth      20
#define positionerTriHeight     14

        SEViewComponent::SEViewComponent( SampleEditorData& data, ui::KickViewport* p )
            : seData( data )
            , parent( p )
        {
            setWantsKeyboardFocus( true );
            seData.loopLockChanged.connect( [this]( bool /*locked*/ ) {
                repaint();
            } );

            seData.cuePositionChanged.connect( [this]( juce::int64 /*pos*/ ) {
                repaint();
            } );

            data.modeChanged.connect( [this]( SampleEditorData::Mode /*mode*/ ) {
                update();
                repaint();
            } );

            data.regionChanged.connect( [this]( const Region& region ) {
                if( region.isSelected() )
                {
                    for( RegionComponent* regionComp : regions )
                    {
                        if( regionComp->hasRegion( region ) && selectedRegion != regionComp )
                        {
                            selectedRegion = regionComp;
                        }
                    }
                }
                update();
                repaint();
            } );

            data.sampleAboutToBeChanged.connect( [this]() {
                for( auto it = regions.begin(); it != regions.end(); it++ )
                    delete *it;

                regions.clear();
            } );

            data.sampleChanged.connect( [this]() {
                resizeRegion            = nullptr;
                selectedRegion          = nullptr;
                playHeadPos             = 0;
                const Region& selRegion = seData.getSelectedRegion();
                for( RegionComponent* regionComp : regions )
                {
                    if( regionComp->hasRegion( selRegion ) && selectedRegion != regionComp )
                    {
                        selectedRegion = regionComp;
                    }
                }

                update();
                repaint();
            } );

            data.regionAdded.connect( [this]( const Region& region ) {
                bool hasRegion = false;
                for( auto it = regions.begin(); it != regions.end(); it++ )
                    if( ( *it )->hasRegion( region ) )
                    {
                        hasRegion = true;
                        break;
                    }

                if( !hasRegion )
                {
                    regions.push_back( RegionComponent::createRegion( region, seData, getLocalBounds() ) );

                    update();
                    repaint();
                }
            } );

            data.regionRemoved.connect( [this]( const Region& /*delRegion*/ ) {
                update();
                repaint();
            } );

            data.regionAboutToBeRemoved.connect( [this]( const Region& delRegion ) {
                for( auto it = regions.begin(); it != regions.end(); it++ )
                {
                    if( ( *it )->hasRegion( delRegion ) )
                    {
                        if( ( *it ) == selectedRegion )
                        {
                            selectedRegion = nullptr;
                        }
                        delete( *it );
                        regions.erase( it );
                        repaint();
                        break;
                    }
                }
            } );

            data.playHeadChanged.connect( [this]( juce::int64 pos ) {
                playHeadPos = pos;
                repaint();
            } );

            data.playbackStopped.connect( [this]() {
                // setting the value to zero also makes the playhead invisible
                playHeadPos = 0;
                repaint();
            } );

            data.currentSampleModified.connect( [this]() {
                repaint();
            } );
        }

        SEViewComponent::~SEViewComponent()
        {
            for( RegionComponent* region : regions )
                delete region;
        }

        void SEViewComponent::paint( juce::Graphics& g )
        {
            if( seData.getNumberOfBeats() == 0 )
                return;
            g.setColour( juce::Colour( 21, 21, 21 ) );
            g.fillAll();

            juce::AudioThumbnailBase* thumbnail = seData.getThumbnail();
            if( !thumbnail )
                return;

            if( selectedRegion )
            {
                selectedRegion->paint( g, true );
            }

            juce::Rectangle<int> rectL;
            juce::Rectangle<int> rectR;

            if( thumbnail->getNumChannels() == 1 )
            {
                rectL = juce::Rectangle( 0, 0, getWidth(), getHeight() );

                // draw zero line
                float mid = ( rectL.getHeight() * 1.0f ) / 2;
                g.drawLine( 0.0f, mid, static_cast<float>( rectL.getWidth() ), mid, 0.2f );
            }
            else
            {
                rectL = juce::Rectangle( 0, 0, getWidth(), ( getHeight() / 2 ) - 1 );
                rectR = juce::Rectangle( 0, ( getHeight() / 2 ) + 1, getWidth(), ( getHeight() / 2 ) - 1 );

                // draw zero line
                float mid = ( rectL.getHeight() * 1.0f ) / 2;
                g.drawLine( 0.0f, mid, static_cast<float>( rectL.getWidth() ), mid, 0.2f );

                mid = ( rectR.getHeight() * 1.0f ) / 2;
                mid += rectR.getY();
                g.drawLine( 0.0f, mid, static_cast<float>( rectL.getWidth() ), mid, 0.2f );
            }

            g.setColour( ui::Theme::waveformColour( 1 ) );
            if( thumbnail->getNumChannels() == 1 )
            {
                thumbnail->drawChannel( g, rectL, 0.0, thumbnail->getTotalLength(), 0, 1.0f );
            }
            else
            {
                thumbnail->drawChannel( g, rectL, 0.0, thumbnail->getTotalLength(), 0, 1.0f );
                thumbnail->drawChannel( g, rectR, 0.0, thumbnail->getTotalLength(), 1, 1.0f );
            }

            auto localBnds = getLocalBounds();
            for( RegionComponent* region : regions )
                region->paint( g );

            //draw cue
            cue.paint( g );
            loopPositioner.paint( g );

            if( !thumbnail->isFullyLoaded() )
            {
                startTimer( 100 );
            }

            // draw playhead
            float posInPixel = seData.getPosInPixels( playHeadPos );
            if( posInPixel > 0 )
            {
                setNextScrollPosition( posInPixel );
                g.setColour( juce::Colours::white );
                g.drawLine( posInPixel, 0.0f, posInPixel, static_cast<float>( getHeight() ), 2.0f );
            }
        }

        void SEViewComponent::timerCallback()
        {
            repaint();
        }

        void SEViewComponent::mouseMove( const juce::MouseEvent& event )
        {
            resizeRegion = nullptr;
            cue.mouseMove( event );
            loopPositioner.mouseMove( event );
            for( RegionComponent* region : regions )
            {
                if( region->mouseMove( event ) )
                {
                    resizeRegion = region;
                    break;
                }
            }
            if( resizeRegion )
            {
                setMouseCursor( juce::MouseCursor::LeftRightResizeCursor );
            }
            else if( isSplitManual( juce::Point<int>( event.x, event.y ) ) )
            {
                setMouseCursor( juce::MouseCursor::IBeamCursor );
            }
            else
            {
                setMouseCursor( juce::MouseCursor::NormalCursor );
            }
        }

        void SEViewComponent::mouseDrag( const juce::MouseEvent& event )
        {
            setViewportIgnoreDragFlag( true );

            if( cue.hasMouseFocus() )
            {
                cue.mouseDrag( event );
                repaint();
            }
            else if( loopPositioner.hasMouseFocus() )
            {
                loopPositioner.mouseDrag( event );
                repaint();
            }
            else if( resizeRegion )
            {
                resizeRegion->mouseDrag( event );
                repaint();
                seData.regionMoved( resizeRegion->getRegion() );
            }
            else
            {
                setViewportIgnoreDragFlag( false );
            }
        }

        void SEViewComponent::mouseUp( const juce::MouseEvent& event )
        {
            if( resizeRegion )
                resizeRegion->mouseUp( event );

            if( seData.isListenTool() && seData.isTrimMode() )
                seData.stop();

            cue.mouseUp( event );
            loopPositioner.mouseUp( event );
            repaint();
        }

        void SEViewComponent::mouseDown( const juce::MouseEvent& event )
        {
            if( isSplitManual( event.getMouseDownPosition() ) && resizeRegion == nullptr ) // edit tool
            {
                juce::int64 posInSec = seData.getPosInSeconds( event.position.getX() );
                seData.splitAtTime( posInSec );
            }
            else if( seData.getTool() == SampleEditorData::SeTool::seToolErase && resizeRegion ) //erase tool
            {
                seData.deleteRegion( resizeRegion->getRegion() );
                resizeRegion = nullptr;
            }
            else if( seData.isListenTool() && resizeRegion == nullptr ) //lsiten tool
            {
                DBG( "play called" );
                seData.playLoop();
            }
            else if( seData.isChopMode() )
            {
                if( cue.hasMouseFocus() )
                    return;
                juce::int64 posInSec = seData.getPosInSeconds( event.position.getX() );
                for( RegionComponent* region : regions )
                {
                    if( region->select( posInSec ) )
                    {
                        selectedRegion = region;
                        seData.regionSelected( region->getRegion() );
                        break;
                    }
                }
            }
        }

        void SEViewComponent::resized()
        {
            if( zoomAt1 == 0 )
                zoomAt1 = static_cast<float>( getWidth() );

            updateArea();
        }

        void SEViewComponent::mouseWheelMove( const juce::MouseEvent&, const juce::MouseWheelDetails& wheel )
        {
            if( seData.getTool() != SampleEditorData::SeTool::seToolZoom )
                return;

            setViewArea( wheel.deltaY );
        }

        void SEViewComponent::setViewArea( float viewPoint )
        {
            float tempZoom = zoomLevel;
            if( tempZoom + viewPoint < 1 )
            {
                tempZoom = 1;
            }
            else if( tempZoom + viewPoint > 100 )
            {
                tempZoom = 100;
            }
            else
            {
                tempZoom += viewPoint;
            }

            if( tempZoom != zoomLevel || isReset )
            {
                zoomLevel = tempZoom;
                setSize( static_cast<int>( std::round( zoomAt1 * zoomLevel ) ), getHeight() );
                sizeChanged();
                updateArea();
                parent->setScrollBarsShown( false, !isReset );
            }
        }

        void SEViewComponent::resetZoomFactor()
        {
            zoomLevel = 1;
            isReset   = true;
            setViewArea( 0 );
            isReset = false;
        }

        void SEViewComponent::updateArea()
        {
            auto area = getLocalBounds();
            for( RegionComponent* rc : regions )
            {
                rc->updateArea( area );
            }

            cue.setViewArea( area );
            loopPositioner.setViewArea( area );
        }

        bool SEViewComponent::isSplitManual( juce::Point<int> point )
        {
            auto area = getLocalBounds();
            area.setHeight( splitManualRegionHeight );

            return ( area.contains( point ) && seData.isSplitModeManual() );
        }

        void SEViewComponent::update()
        {
            for( RegionComponent* region : regions )
            {
                region->update();
            }
        }

        bool SEViewComponent::setNextScrollPosition( float playHeadPosPix )
        {
            if( seData.getAutoScroll() == SampleEditorData::AutoScroll::autoScrollOff )
                return false;

            bool changed  = false;
            auto viewArea = parent->getViewArea();
            if( playHeadPosPix > viewArea.getTopRight().getX() )
            {
                int xChange = 0;
                changed     = true;

                if( seData.getAutoScroll() == SampleEditorData::AutoScroll::autoScrollFollow )
                    xChange = juce::jmin<int>( viewArea.getX() + viewArea.getWidth() / 2, getWidth() );
                else
                    xChange = juce::jmin<int>( viewArea.getX() + viewArea.getWidth(), getWidth() );

                parent->setViewPosition( xChange, 0 );
            }
            else if( !viewArea.contains( juce::Point( static_cast<int>( playHeadPosPix ), 0 ) ) )
            {
                parent->setViewPosition( static_cast<int>( playHeadPosPix ), 0 );
            }

            return changed;
        }
        //
        // region selector
        //
        RegionSelector::RegionSelector( const Region& p, SampleEditorData& data, bool e )
            : pos( p )
            , seData( data )
            , end( e )
        {
        }

        void RegionSelector::paint( juce::Graphics& g )
        {
            if( enabled == false && pos.isSelected() == false )
                return;

            //draw rect
            g.setColour( ui::Theme::backgroundColour( 5 ) );
            juce::Rectangle<float> resizeR = getResizeRect();
            g.fillRect( resizeR );

            juce::Colour drawColour = end ? juce::Colour( 255, 0, 0 ) : juce::Colour( 0, 170, 238 );
            g.setColour( drawColour );
            //draw line
            if( end )
            {
                juce::Point p = resizeR.getBottomRight();
                g.drawLine( p.getX(), p.getY(), p.getX(), 0 );
            }
            else
            {
                juce::Point p = resizeR.getTopLeft();
                g.drawLine( p.getX(), p.getY(), p.getX(), static_cast<float>( totalArea.getHeight() ) );
            }

            //float resizeTriDelta = resizeR.getCentreX() - (resizeRectangleWidth / 2.0);
            //g.drawLine()
            // draw text
            juce::String label = end ? pos.getEndLable() : pos.getStartLabel();
            g.drawText( label, resizeR, juce::Justification::centred );
        }

        bool RegionSelector::mouseDrag( const juce::MouseEvent& event )
        {
            juce::int64 currentPos = end ? pos.getEnd() : pos.getStart();
            juce::int64 posInSec   = seData.getPosInSeconds( event.position.getX() );
            if( seData.isZeroCrossingEnabled() )
                posInSec = seData.getNextZeroSnapPos( posInSec, posInSec > currentPos );

            if( seData.moveTime( pos, posInSec, end ) )
            {
                if( mouseDragStart && seData.getSlicePreview() == SampleEditorData::SlicePreview::slicePreviewBefore )
                {
                    juce::int64    cuePos = end ? pos.getEnd() : pos.getStart();
                    MTSampleRegion r( cuePos, cuePos );
                    seData.play( r, MTSamplePlayMode::samplePlayModeCue );
                    mouseDragStart = false;
                }

                return true;
            }

            return false;
        }

        bool RegionSelector::mouseMove( const juce::MouseEvent& event )
        {
            mouseFocus = false;
            if( !enabled )
                return false;

            juce::Rectangle<float> resizeR  = getResizeRect();
            float                  posInPix = end ? resizeR.getBottomRight().getX() : resizeR.getTopLeft().getX();

            if( event.position.getX() < ( posInPix + 5 ) && event.position.getX() > ( posInPix - 5 ) )
            {
                mouseFocus = true;
            }
            else if( resizeR.contains( event.position ) )
            {
                mouseFocus = true;
            }

            return mouseFocus;
        }

        void RegionSelector::mouseUp( const juce::MouseEvent& )
        {
            if( mouseFocus )
            {
                juce::int64 posInSec = end ? pos.getEnd() : pos.getStart();
                seData.moveTime( pos, posInSec, end, true );
                mouseDragStart = true;

                if( seData.getSlicePreview() == SampleEditorData::SlicePreview::slicePreviewAfter )
                {
                    juce::int64    cuePos = end ? pos.getEnd() : pos.getStart();
                    MTSampleRegion r( cuePos, cuePos );
                    seData.play( r, MTSamplePlayMode::samplePlayModeCue );
                }
            }
        }

        juce::Rectangle<float> RegionSelector::getResizeRect()
        {
            juce::int64 posv     = end ? pos.getEnd() : pos.getStart();
            float       posInPix = seData.getPosInPixels( posv );

            juce::Rectangle<float> resizeRect;
            if( !end )
            {
                resizeRect = juce::Rectangle<float>( posInPix, 0, ui::_scaledXF( resizeRectWidth ), ui::_scaledXF( resizeRectHeight ) );
            }
            else
            {
                resizeRect = juce::Rectangle<float>( posInPix - ui::_scaledXF( resizeRectWidth ), ui::_scaledYF( totalArea.getHeight() - resizeRectHeight ), ui::_scaledXF( resizeRectWidth ), ui::_scaledYF( resizeRectHeight ) );
            }

            return resizeRect;
        }

        RegionComponent* RegionComponent::createRegion( const Region& region, SampleEditorData& data, juce::Rectangle<int> area )
        {
            RegionSelector* startS = new RegionSelector( region, data );
            startS->setArea( area );
            RegionSelector* endS = new RegionSelector( region, data, true );
            endS->setArea( area );

            return new RegionComponent( startS, endS );
        }

        RegionComponent::RegionComponent( RegionSelector* s, RegionSelector* e )
            : start( s )
            , end( e )
        {
        }

        bool RegionComponent::mouseDrag( const juce::MouseEvent& event )
        {
            if( start->hasMouseFocus() )
                return start->mouseDrag( event );

            if( end->hasMouseFocus() )
                return end->mouseDrag( event );

            return false;
        }

        void RegionComponent::updateArea( juce::Rectangle<int>& area )
        {
            start->setArea( area );
            end->setArea( area );
        }

        void RegionComponent::update()
        {
            const Region& region = start->getRegion();
            start->setEnabled( true );
            end->setEnabled( true );

            if( start->getdata().isTrimMode() )
            {
                if( !region.isSelected() )
                {
                    start->setEnabled( false );
                    end->setEnabled( false );
                }
            }
            else
            {
                end->setEnabled( !start->getdata().getLinkSlices() || region.isSelected() );
            }
        }

        bool RegionComponent::hasRegion( const Region& region )
        {
            return ( region == start->getRegion() );
        }

        bool RegionComponent::mouseMove( const juce::MouseEvent& event )
        {
            bool move = start->mouseMove( event ) || end->mouseMove( event );

            return move;
        }

        bool RegionComponent::select( juce::int64 time )
        {
            if( time >= start->getRegion().getStart() && time <= start->getRegion().getEnd() )
            {
                start->getdata().selectRegion( start->getRegion() );
                return true;
            }
            return false;
        }

        void RegionComponent::mouseUp( const juce::MouseEvent& event )
        {
            start->mouseUp( event );
            end->mouseUp( event );
        }

        RegionComponent::~RegionComponent()
        {
            delete start;
            delete end;
        }

        void RegionComponent::paint( juce::Graphics& g, bool selection )
        {
            if( selection )
            {
                g.setColour( ui::Theme::backgroundColour( 11 ) );
                juce::Rectangle<int>&  area      = start->getTotalArea();
                SampleEditorData&      data      = start->getdata();
                float                  posInPixS = data.getPosInPixels( start->getRegion().getStart() );
                float                  posInPixE = data.getPosInPixels( start->getRegion().getEnd() );
                juce::Rectangle<float> selRegion( posInPixS, 0, posInPixE - posInPixS, static_cast<float>( area.getHeight() ) );
                g.fillRect( selRegion );
            }
            else
            {
                start->paint( g );
                end->paint( g );
            }
        }
    }
}
