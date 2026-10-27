#include "kick_AHDSView.h"


namespace kick_ui
{
#define ahdsDrawingStart 50
#define topBottomMargin 25
#define ahdsMarkinsWidth 10
#define controlsHeight  80

    KickEnvelopViewBase::KickEnvelopViewBase( const std::vector<int>& v, int noPoints )
        : values( v )
        , points( noPoints )
    {
        setSize( 850, 491 );
        std::vector<juce::String> names;
        if( points == 2 )
            names = { "Attack", "Decay" };
        else
            names = { "Attack", "Hold", "Decay", "Sustain" };

        auto readValues = [this]( int )
        {
            unsigned int counter = 0;
            for( std::unique_ptr<ui::KickNumber>& ctrl : controls )
            {
                values[counter] = ctrl->getValue();
                counter++;
            }
            updateValues();
            repaint();
        };
        for( unsigned int i = 0; i < (unsigned)points; i++ )
        {
            std::unique_ptr<ui::KickNumber>& numCtrl = controls.emplace_back( std::make_unique<ui::KickNumber>( names[i], MIDI::MIN, MIDI::MAX, values[i], 1, true ) );
            addAndMakeVisible( numCtrl.get() );
            numCtrl->onValueChanged = readValues;
        }
    }

    KickEnvelopViewBase::~KickEnvelopViewBase()
    {
    }

    void KickEnvelopViewBase::paint( juce::Graphics& g )
    {
        drawScale( g, 20 );
        drawScale( g, xEnd + 10 );
    }

    void KickEnvelopViewBase::updateControls()
    {
        unsigned int counter = 0;
        for( std::unique_ptr<ui::KickNumber>& ctrl : controls )
        {
            ctrl->setValue( values[counter] );
            counter++;
        }
    }

    void KickEnvelopViewBase::drawScale( juce::Graphics& g, float xValue )
    {
        int totalMarkings = 32;
        float delta = (yEnd - yStart) / totalMarkings;
        float yVal = yStart;

        g.setColour( ui::Theme::textColour( 1 ) );
        while( yVal <= yEnd )
        {
            g.drawLine( xValue, yVal, xValue + ahdsMarkinsWidth, yVal );
            yVal += delta;
        }
    }

    juce::Rectangle<int> KickEnvelopViewBase::getControlsRect()
    {
        return juce::Rectangle<int>( 0, getHeight() - controlsHeight, getWidth(), controlsHeight );
    }

    void KickEnvelopViewBase::resized()
    {
        xStart = (float)ahdsDrawingStart;
        yStart = (float)topBottomMargin;
        xEnd = (float)(getWidth() - ahdsDrawingStart);
        yEnd = (float)(getHeight() - topBottomMargin - controlsHeight);

        juce::Rectangle<int> controlsRect = getControlsRect();
        int prev = controlsRect.getX() + ahdsDrawingStart;
        for( std::unique_ptr<ui::KickNumber>& ctrl : controls )
        {
            ctrl->setBounds( prev, controlsRect.getY() + 5, 80, 60 );
            prev = ctrl->getRight() + 5;
        }
    }

    juce::Path KickEnvelopViewBase::getLogPath( float x1, float x2, float y1, float y2, bool reverse = false )
    {
        juce::Path curvePath;
        curvePath.startNewSubPath( x1, y1 );
        float diff = std::abs( x2 - x1 );
        float height = std::abs( y2 - y1 );

        float delta = diff / 127.0f;
        if( delta == 0 )
        {
            curvePath.lineTo( x2, y2 );
            return curvePath;
        }

        float xVal = delta;
        float yVal = 0.0f;
        while( xVal <= diff )
        {
            yVal = std::log10( xVal );
            if( yVal >= 0 )
            {
                yVal = (height)*yVal / std::log10( diff );
                if( reverse )
                    curvePath.lineTo( xVal + x1, y1 - yVal );
                else
                    curvePath.lineTo( xVal + x1, y1 + yVal );
            }
            xVal += delta;
        }
        return curvePath;
    }

    int KickEnvelopViewBase::evalueatePointValue( float p1, float p2, int totalPoints, bool yAxis )
    {
        float diff = yAxis ? yEnd - yStart : xEnd - xStart;
        float value = (127 * std::abs( p2 - p1 )) / (diff / totalPoints);
        return static_cast<int>(std::roundf( value ));
    }

    float KickEnvelopViewBase::convertValueToPoint( int value, int totalPoints, bool yAxis )
    {
        float diff = yAxis ? yEnd - yStart : xEnd - xStart;
        float point = ((diff / totalPoints) * value) / 127;

        return point;
    }

    KickAHDSView::KickAHDSView( const std::vector<int>& initValues )
        : KickEnvelopViewBase( initValues, 4 )
    {
    }

    void KickAHDSView::paint( juce::Graphics& g )
    {
        // dont paint before initialization
        if( attack == 0.0f )
            return;

        // draw background and circle
        g.setColour( ui::Theme::backgroundColour( 1 ) );
        g.fillAll();
        g.setColour( ui::Theme::backgroundColour( 0 ) );
        const juce::Rectangle<float>& aRect = getARect();
        const juce::Rectangle<float>& hRect = getHRect();
        const juce::Rectangle<float>& dRect = getDRect();
        const juce::Rectangle<float>& sRect = getSRect();
        g.fillEllipse( aRect );
        g.fillEllipse( hRect );
        g.fillEllipse( dRect );
        g.fillEllipse( sRect );

        // draw rectangle
        g.setColour( ui::Theme::textColour( 1 ) );
        g.drawText( "A", aRect, juce::Justification::centred );
        g.drawText( "H", hRect, juce::Justification::centred );
        g.drawText( "D", dRect, juce::Justification::centred );
        g.drawText( "S", sRect, juce::Justification::centred );

        // draw curve
        g.strokePath( getAPath(), juce::PathStrokeType( 1 ) );

        // only ahds
        g.strokePath( getHPath(), juce::PathStrokeType( 1 ) );
        g.drawLine( attack, yStart, hold, yStart, 1 );
        g.drawLine( decay, sustain, xEnd, sustain, 1 );

        KickEnvelopViewBase::paint( g );
    }

    void KickAHDSView::resized()
    {
        KickEnvelopViewBase::resized();

        updateValues();
        controlPointDistance = (yEnd - yStart) / 3;
    }

    void KickAHDSView::mouseDrag( const juce::MouseEvent& event )
    {
        if( mouseDragpoint == MouseDragPoint::dragPointNone )
        {
            const auto& aRect = getARect();
            const auto& hRect = getHRect();
            const auto& dRect = getDRect();
            const auto& sRect = getSRect();

            if( aRect.contains( event.mouseDownPosition ) )
                mouseDragpoint = MouseDragPoint::dragPointA;
            else if( hRect.contains( event.mouseDownPosition ) )
                mouseDragpoint = MouseDragPoint::dragPointH;
            else if( dRect.contains( event.mouseDownPosition ) )
                mouseDragpoint = MouseDragPoint::dragPointD;
            else if( sRect.contains( event.mouseDownPosition ) )
                mouseDragpoint = MouseDragPoint::dragPointS;
        }

        float mouseX = event.position.getX();
        float mouseY = event.position.getY();
        setPointToMaxIfExceeds( mouseX, mouseY );

        float attackOld = attack;
        float holdOld = hold;
        if( mouseDragpoint == MouseDragPoint::dragPointA )
        {
            if( checkDragCondition( mouseDragpoint, mouseX, mouseY ) )
            {
                attack = mouseX;
                hold += attack - attackOld;
                decay += attack - attackOld;
            }
        } else if( mouseDragpoint == MouseDragPoint::dragPointH )
        {
            if( checkDragCondition( mouseDragpoint, mouseX, mouseY ) )
            {
                hold = mouseX;
                if( hold < attack )
                    hold = attack;
                decay += hold - holdOld;
            }
        } else if( mouseDragpoint == MouseDragPoint::dragPointD )
        {
            if( checkDragCondition( mouseDragpoint, mouseX, mouseY ) )
            {
                decay = mouseX;
                if( decay < hold )
                    decay = hold;
            }
        } else if( mouseDragpoint == MouseDragPoint::dragPointS )
        {
            if( checkDragCondition( mouseDragpoint, mouseX, mouseY ) )
                sustain = mouseY;
        }

        if( mouseDragpoint != MouseDragPoint::dragPointNone )
        {
            getAHDSValues();
            updateControls();
        }

        repaint();
    }

    void KickAHDSView::mouseUp( const juce::MouseEvent& )
    {
        mouseDragpoint = MouseDragPoint::dragPointNone;
    }

    void KickAHDSView::setPointToMaxIfExceeds( float& mouseX, float& mouseY )
    {
        if( mouseX < xStart )
            mouseX = xStart;
        if( mouseX > xEnd )
            mouseX = xEnd;

        if( mouseY < yStart )
            mouseY = yStart;
        if( mouseY > yEnd )
            mouseY = yEnd;

        if( mouseDragpoint == MouseDragPoint::dragPointA )
        {
            if( mouseX > (xStart + ((xEnd - xStart) / 4)) )
            {
                mouseX = (xStart + ((xEnd - xStart) / 4));
            }
        } else if( mouseDragpoint == MouseDragPoint::dragPointH )
        {
            if( mouseX > (attack + ((xEnd - xStart) / 4)) )
            {
                mouseX = (attack + ((xEnd - xStart) / 4));
            }

        } else if( mouseDragpoint == MouseDragPoint::dragPointD )
        {
            if( mouseX > (hold + ((xEnd - xStart) / 4)) )
            {
                mouseX = (hold + ((xEnd - xStart) / 4));
            }
        } else if( mouseDragpoint == MouseDragPoint::dragPointS )
        {
        }

    }

    void KickAHDSView::updateValues()
    {
        attack = xStart + convertValueToPoint( values[0], 4 );
        hold = attack + convertValueToPoint( values[1], 4 );
        decay = hold + convertValueToPoint( values[2], 4 );
        sustain = yEnd - convertValueToPoint( values[3], 1, true );
    }

    bool KickAHDSView::checkDragCondition( MouseDragPoint dragPoint, float mouseX, float mouseY )
    {
        bool canDrag = false;
        if( dragPoint == MouseDragPoint::dragPointA )
        {
            canDrag = mouseX <= (xStart + ((xEnd - xStart) / 4));
        } else if( dragPoint == MouseDragPoint::dragPointD )
        {
            canDrag = (decay >= hold) && (mouseX - hold) <= (xEnd - xStart) / 4;
        } else if( dragPoint == MouseDragPoint::dragPointH )
        {
            canDrag = (hold >= attack) && (mouseX - attack) <= (xEnd - xStart) / 4;
        } else if( dragPoint == MouseDragPoint::dragPointS )
        {
            canDrag = mouseY >= yStart && mouseY <= yEnd;
        }

        return canDrag;
    }

    juce::Rectangle<float> KickAHDSView::getARect()
    {
        return juce::Rectangle<float>( attack - 16, yStart - 16, 36.0f, 36.0f );
    }

    juce::Rectangle<float> KickAHDSView::getHRect()
    {
        return juce::Rectangle<float>( hold - 16, controlPointDistance + yStart - 16, 36.0f, 36.0f );
    }

    juce::Rectangle<float> KickAHDSView::getDRect()
    {
        return juce::Rectangle<float>( decay - 16, 2 * controlPointDistance + yStart - 16, 36.0f, 36.0f );
    }

    juce::Rectangle<float> KickAHDSView::getSRect()
    {
        return juce::Rectangle<float>( xEnd - 16, sustain - 16, 36.0f, 36.0f );
    }

    juce::Path KickAHDSView::getAPath()
    {
        return getLogPath( xStart, attack, yEnd, yStart, true );
    }

    juce::Path KickAHDSView::getHPath()
    {
        return getLogPath( hold, decay, yStart, sustain );
    }

    std::vector<int> KickAHDSView::getAHDSValues()
    {
        values[0] = evalueatePointValue( xStart, attack, 4 );
        values[1] = evalueatePointValue( attack, hold, 4 );
        values[2] = evalueatePointValue( hold, decay, 4 );
        values[3] = 127 - evalueatePointValue( yStart, sustain, 1, true );

        return values;
    }

    /// //////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ////////////////////////////////////// KickADView ////////////////////////////////////////////////////
    /// /////////////////////////////////////////////////////////////////////////////////////////////////////
    KickADView::KickADView( const std::vector<int>& initValues )
        : KickEnvelopViewBase( initValues, 2 )
    {
    }

    KickADView::~KickADView()
    {
    }

    void KickADView::resized()
    {
        KickEnvelopViewBase::resized();
        updateValues();
    }

    void KickADView::paint( juce::Graphics& g )
    {
        if( attack == 0.0f )
            return;

        g.setColour( ui::Theme::backgroundColour( 1 ) );
        g.fillAll();
        g.setColour( ui::Theme::backgroundColour( 0 ) );
        const juce::Rectangle<float>& aRect = getARect();
        const juce::Rectangle<float>& dRect = getDRect();
        g.fillEllipse( aRect );
        g.fillEllipse( dRect );

        g.setColour( ui::Theme::textColour( 1 ) );
        g.drawText( "A", aRect, juce::Justification::centred );
        g.drawText( "D", dRect, juce::Justification::centred );

        // draw curve 0 to attack
        g.strokePath( getAPath(), juce::PathStrokeType( 1 ) );

        //draw line A to D
        if( envDecaypMode == EnvelopDecayMode::decayModeEnd )
            g.drawLine( attack, yStart, decay, yStart );

        // draw decay
        g.strokePath( getDPath(), juce::PathStrokeType( 1 ) );

        KickEnvelopViewBase::paint( g );
    }

    void KickADView::mouseDrag( const juce::MouseEvent& event )
    {
        if( mouseDragpoint == MouseDragPoint::dragPointNone )
        {
            const auto& aRect = getARect();
            const auto& dRect = getDRect();
            if( aRect.contains( event.mouseDownPosition ) )
                mouseDragpoint = MouseDragPoint::dragPointA;
            else if( dRect.contains( event.mouseDownPosition ) )
                mouseDragpoint = MouseDragPoint::dragPointD;
        }

        float mouseX = event.position.getX();
        setPointToMaxIfExceeds( mouseX );

        float attackOld = attack;
        if( mouseDragpoint == MouseDragPoint::dragPointA )
        {
            if( checkDragCondition( mouseDragpoint, mouseX ) )
            {
                attack = mouseX;
                if( envDecaypMode == EnvelopDecayMode::decayModeStart )
                    decay += attack - attackOld;
            }
        } else if( mouseDragpoint == MouseDragPoint::dragPointD )
        {
            if( checkDragCondition( mouseDragpoint, mouseX ) )
            {
                decay = mouseX;
                if( decay < attack )
                    decay = attack;
            }
        }

        if( mouseDragpoint != MouseDragPoint::dragPointNone )
        {
            getAHDSValues();
            updateControls();
        }
        repaint();
    }

    void KickADView::mouseUp( const juce::MouseEvent& )
    {
        mouseDragpoint = MouseDragPoint::dragPointNone;
    }

    std::vector<int> KickADView::getAHDSValues()
    {
        values[0] = evalueatePointValue( xStart, attack, 2 );
        if( envDecaypMode == EnvelopDecayMode::decayModeEnd )
            values[1] = evalueatePointValue( decay, xEnd, 2 );
        else
            values[1] = evalueatePointValue( attack, decay, 2 );

        return values;
    }

    void KickADView::setPointToMaxIfExceeds( float& mouseX )
    {
        if( mouseX < xStart )
            mouseX = xStart;
        if( mouseX > xEnd )
            mouseX = xEnd;

        if( mouseDragpoint == MouseDragPoint::dragPointA )
        {
            if( mouseX > (xStart + ((xEnd - xStart) / 2)) )
                mouseX = (xStart + ((xEnd - xStart) / 2));
        } else if( mouseDragpoint == MouseDragPoint::dragPointD )
        {
            if( envDecaypMode == EnvelopDecayMode::decayModeStart )
            {
                if( mouseX > (attack + ((xEnd - xStart) / 2)) )
                    mouseX = (attack + ((xEnd - xStart) / 2));
            } else
            {
                if( mouseX < (xEnd - ((xEnd - xStart) / 2)) )
                    mouseX = xEnd - ((xEnd - xStart) / 2);
            }
        }
    }

    void KickADView::updateValues()
    {
        attack = xStart + convertValueToPoint( values[0], 2 );
        if( envDecaypMode == EnvelopDecayMode::decayModeStart )
            decay = attack + convertValueToPoint( values[1], 2 );
        else
            decay = xEnd - convertValueToPoint( values[1], 2 );
    }

    bool KickADView::checkDragCondition( MouseDragPoint dragPoint, float mouseX )
    {
        bool canDrag = false;
        if( dragPoint == MouseDragPoint::dragPointA )
        {
            canDrag = mouseX <= (xStart + ((xEnd - xStart) / 2));
        }
        if( dragPoint == MouseDragPoint::dragPointD )
        {
            if( envDecaypMode == EnvelopDecayMode::decayModeEnd )
                canDrag = mouseX >= (xStart + ((xEnd - xStart) / 2));
            else
                canDrag = (decay >= attack) && (mouseX - attack) <= (xEnd - xStart) / 2;
        }

        return canDrag;
    }

    juce::Rectangle<float> KickADView::getARect()
    {
        return juce::Rectangle<float>( attack - 16, yStart - 16, 36.0f, 36.0f );
    }
    juce::Rectangle<float> KickADView::getDRect()
    {
        return juce::Rectangle<float>( decay - 16, yEnd - 16, 36.0f, 36.0f );
    }

    juce::Path KickADView::getAPath()
    {
        return getLogPath( xStart, attack, yEnd, yStart, true );
    }

    juce::Path KickADView::getDPath()
    {
        juce::Path curvePath;
        if( envDecaypMode == EnvelopDecayMode::decayModeStart )
            curvePath = getLogPath( attack, decay, yStart, yEnd );
        else
            curvePath = getLogPath( decay, xEnd, yStart, yEnd );

        return curvePath;
    }
}

