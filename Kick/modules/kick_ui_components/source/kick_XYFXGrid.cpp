#include "kick_XYFXGrid.h"
namespace ui
{
#define totalHorizontalCells 8
#define totalVerticalCells   8
#define spacing              2
    XYFXGrid::XYFXGrid()
        : selectedCells( 64 )
    {
    }

    void XYFXGrid::paint( juce::Graphics& g )
    {
        if( isEnabled() )
            paintCells( g );
        else
            g.fillAll( ui::Theme::XYFXCellColour() );
    }

    void XYFXGrid::paintCells( juce::Graphics& g )
    {
        for( unsigned int row = 0; row < totalHorizontalCells; row++ )
        {
            for( unsigned column = 0; column < totalVerticalCells; column++ )
            {
                unsigned int index = row * totalHorizontalCells + column;
                if( selectedCells[index] > 100 )
                {
                    g.setColour( ui::Theme::XYFXCellSelectColour( selectedCells[index] ) );
                }
                else
                {
                    g.setColour( ui::Theme::XYFXCellColour() );
                }

                const juce::Rectangle<float> cellRect = getCellRect( (int)row, (int)column );
                g.fillRect( cellRect );
            }
        }
        if( isLatch && !isTimerRunning() )
        {
            const juce::Rectangle<float> cellRect = getCellRect( (int)currentIndex / 8, (int)currentIndex % 8 );
            g.setColour( ui::Theme::XYFXCellSelectColour( 255 ) );
            g.fillRect( cellRect );
        }

        float yAxisPos = cellHeight * 4.0f + spacing * 3.0f;
        mt::utils::fillRoundedRect( g, { 0, (int)yAxisPos - 1, getWidth(), 3 }, ui::Theme::XYFXAxisColour(), 0, false, false, false, false );
        float xAxisPos = cellWidth * 4.0f + spacing * 3.0f;
        mt::utils::fillRoundedRect( g, { (int)xAxisPos, 0, 3, getHeight() }, ui::Theme::XYFXAxisColour(), 0, false, false, false, false );

        float xpos = (float)cellWidth;
        for( int i = 0; i < 7; i++ ) //xaxis  markings
        {
            mt::utils::fillRoundedRect( g, { (int)xpos, (int)yAxisPos - 14, 2, 30 }, ui::Theme::XYFXAxisColour(), 0, false, false, false, false );
            xpos += cellWidth + 2;
        }

        float ypos = (float)cellHeight;
        for( int i = 0; i < 7; i++ ) //xaxis  markings
        {
            mt::utils::fillRoundedRect( g, { (int)xAxisPos - 14, (int)ypos, 30, 2 }, ui::Theme::XYFXAxisColour(), 0, false, false, false, false );
            ypos += cellHeight + 2;
        }

        g.setColour( ui::Theme::textColour( 1 ) );
        g.setFont( 15.0f );
        g.drawText( xLabel.getText(), 0, yAxisPos - 20, 100, 20, juce::Justification::left );
        g.drawText( xLeft, 0, yAxisPos, 100, 20, juce::Justification::centredLeft );
        g.drawText( xLabel.getText(), getWidth() - 100, yAxisPos - 20, 100, 20, juce::Justification::right );
        g.drawText( xRight, getWidth() - 100, yAxisPos, 100, 20, juce::Justification::centredRight );
        g.drawText( yLabel.getText(), xAxisPos + 10, getHeight() - 40, 100, 20, juce::Justification::left );
        g.drawText( yBottom, xAxisPos + 10, getHeight() - 20, 100, 20, juce::Justification::left );
        g.drawText( yLabel.getText(), xAxisPos + 10, 0, 100, 20, juce::Justification::left );
        g.drawText( yTop, xAxisPos + 10, 20, 100, 20, juce::Justification::left );
    }

    void XYFXGrid::resized()
    {
        cellWidth  = ( getWidth() - ( totalHorizontalCells - 1.0f ) * spacing ) / totalHorizontalCells;
        cellHeight = ( getHeight() - ( totalVerticalCells - 1.0f ) * spacing ) / totalVerticalCells;
    }

    juce::Rectangle<float> XYFXGrid::getCellRect( int row, int column )
    {
        float xStart = column * cellWidth + ( column * spacing );
        float yStart = row * cellHeight + ( row * spacing );
        return juce::Rectangle<float>( xStart, yStart, cellWidth, cellHeight );
    }

    void XYFXGrid::mouseDown( const juce::MouseEvent& event )
    {
        if( isEnabled() )
            updateSelection( event );
    }

    void XYFXGrid::mouseUp( const juce::MouseEvent& )
    {
        if( isEnabled() )
            currentIndex = -1;
    }

    void XYFXGrid::mouseDrag( const juce::MouseEvent& event )
    {
        if( isEnabled() )
            updateSelection( event );
    }

    void XYFXGrid::timerCallback()
    {
        bool valueChanged = false;
        for( unsigned int i = 0; i < 64; i++ )
        {
            if( selectedCells[i] > 100 )
            {
                selectedCells[i] = selectedCells[i] - 5;
                valueChanged     = true;
            }
        }
        if( !valueChanged )
        {
            stopTimer();
            if( isLatch )
            {
                selectedCells[prevSel] = 255;
                repaint();
            }
        }
        else
            repaint();
    }

    void XYFXGrid::updateSelection( const juce::MouseEvent& event )
    {
        if( !getLocalBounds().toFloat().contains( event.position ) )
            return;

        float cw     = getWidth() * 1.0f / totalHorizontalCells;
        float ch     = getHeight() * 1.0f / totalVerticalCells;
        int   column = (int)( event.position.getX() / cw );
        int   row    = (int)( event.position.getY() / ch );
        int   index  = row * totalHorizontalCells + column;

        if( index < 0 )
            jassertfalse;
        xCoord = event.position.getX();
        yCoord = event.position.getY();
        setXYCoordinateValue( xCoord, yCoord );

        if( index != currentIndex )
        {
            currentIndex = prevSel             = index;
            selectedCells[(unsigned int)index] = 255;
            if( !isTimerRunning() )
            {
                startTimer( 30 );
            }
            repaint();
        }
    }

    void XYFXGrid::setXYCoordinates( std::vector<juce::String> data )
    {
        xLabel.setText( data[0], juce::dontSendNotification );
        yLabel.setText( data[1], juce::dontSendNotification );
        if( xLabel.getText() == "Beat Division" )
        {
            xLeft  = "2 Bar";
            xRight = "1/64";
        }
        else
        {
            xLeft  = "0.000";
            xRight = "1.000";
        }
        yTop    = "1.000";
        yBottom = "0.000";

        setXYCoordinateValue( xCoord, yCoord );
        repaint();
    }

    void XYFXGrid::setXYCoordinateValue( float XCoordinate, float YCoordinate )
    {
        /// X Axis Coordinates
        if( xLabel.getText() == "Beat Division" )
        {
            float cw     = getWidth() * 1.0f / totalHorizontalCells;
            int   column = juce::jlimit( 0, totalHorizontalCells, (int)( XCoordinate / cw ) );
            xCoordinate  = column;
        }
        else
            xCoordinate = juce::jlimit( 0.f, 1.f, XCoordinate * 1.0f / getWidth() );

        /// Y Axis Coordinates
        {
            yCoordinate = juce::jlimit( 0.f, 1.f, 1.0f - ( YCoordinate * 1.0f / getHeight() ) );
        }

        if( onValueChanged )
            onValueChanged( xCoordinate, yCoordinate );
    }
}
