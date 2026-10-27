#include "kick_SEScaleComponent.h"
namespace mt
{
    namespace ac
    {
#define seSamplesSpacing  30
#define seSpaceFromMarker 3
        SEScaleComponent::SEScaleComponent( SampleEditorData& data )
            : seData( data )
        {
            data.sampleChanged.connect( [this]() {
                calculateBeatWidth();
                repaint();
            } );

            data.settingsChanged.connect( [this]() {
                repaint();
            } );

            data.currentSampleModified.connect( [this]() {
                repaint();
            } );
        }

        SEScaleComponent::~SEScaleComponent()
        {
        }

        void SEScaleComponent::paint( juce::Graphics& g )
        {
            if( beatWidth == 0 )
                return;

            juce::Font f( 12 );
            g.setFont( f );
            g.setColour( ui::Theme::foregroundColour( 0 ) );

            auto tmUnits = seData.getTimeLineUnits();
            if( tmUnits == SampleEditorData::TimeLineUnits::unitsSamples )
            {
                paintSamples( g );
            }
            else if( tmUnits == SampleEditorData::TimeLineUnits::unitsBeats )
            {
                paintBeats( g );
            }
            else if( tmUnits == SampleEditorData::TimeLineUnits::unitsTime )
            {
                paintTime( g );
            }
        }

        void SEScaleComponent::resized()
        {
            if( defaultSize == 0 )
                defaultSize = static_cast<float>( getWidth() );

            calculateBeatWidth();
        }

        void SEScaleComponent::drawMarkingText( juce::Graphics& g, juce::String& markerText, float xpos )
        {
            float height = static_cast<float>( getHeight() );

            g.drawLine( xpos, 7.0f, xpos, height );
            g.drawSingleLineText( markerText, static_cast<int>( xpos + seSpaceFromMarker ), 11 );
        }

        void SEScaleComponent::paintSamples( juce::Graphics& g )
        {
            // calculate markings width
            juce::int64  totalSamples = seData.getSampleLengthInSamples();
            int          widgetWidth  = getLocalBounds().getWidth();
            juce::String totalSampleStr( totalSamples );
            float        sampleDisplayWidth   = static_cast<float>( g.getCurrentFont().getStringWidth( totalSampleStr ) + seSamplesSpacing + seSpaceFromMarker );
            float        samplesPerMarkingTmp = ( sampleDisplayWidth * totalSamples * 1.0f ) / widgetWidth;
            int          samplesPerMarking    = static_cast<int>( std::round( samplesPerMarkingTmp ) );
            sampleDisplayWidth                = ( sampleDisplayWidth * samplesPerMarking ) / samplesPerMarkingTmp;

            //draw markings
            juce::int64 currentMarkerValue  = samplesPerMarking;
            float       currentMarkerXValue = sampleDisplayWidth;
            while( currentMarkerXValue < widgetWidth )
            {
                juce::String currentMarkerText( currentMarkerValue );
                drawMarkingText( g, currentMarkerText, currentMarkerXValue );

                currentMarkerValue += samplesPerMarking;
                currentMarkerXValue += sampleDisplayWidth;
            }
        }

        void SEScaleComponent::paintBeats( juce::Graphics& g )
        {
            juce::String maxTick;
            int          totalBeats        = static_cast<int>( std::floor( seData.getNumberOfBeats() ) );
            int          maxTickDisplayLen = g.getCurrentFont().getStringWidth( juce::String().formatted( "%d.%d.%d", totalBeats, 4, 960 ) ) + seSamplesSpacing + seSpaceFromMarker;
            int          maxBeatDisplayLen = g.getCurrentFont().getStringWidth( juce::String().formatted( "%d.%d.%d", totalBeats, 4, 0 ) );
            float        barWidth          = 4 * beatWidth;

            // total beat markings
            int totalDigits      = 1;
            int noOfBeatMarkings = 0;
            if( std::floor( ( 4 * beatWidth ) / maxBeatDisplayLen ) > 1 )
            {
                noOfBeatMarkings = static_cast<int>( std::floor( ( 4 * beatWidth ) / maxBeatDisplayLen ) );
                if( noOfBeatMarkings >= 3 )
                    noOfBeatMarkings = 4;
                else if( noOfBeatMarkings == 2 )
                    noOfBeatMarkings = 1;
            }
            // total tick markings
            int noOfTickMarkings = 0;
            if( std::floor( ( beatWidth - maxBeatDisplayLen ) / maxTickDisplayLen ) > 0 )
            {
                noOfTickMarkings = static_cast<int>( std::floor( beatWidth / maxTickDisplayLen ) );
            }

            if( noOfBeatMarkings > 0 )
                totalDigits += 1;
            if( noOfTickMarkings > 0 )
                totalDigits += 1;

            // todo get ppq from settings
            int ppqResolution = 960;
            if( noOfTickMarkings > 0 )
                ppqResolution = 960 / ( noOfTickMarkings + 1 );

            float currentPosX = 0;
            float tickWidth   = beatWidth / ( noOfTickMarkings + 1 );
            float barInc      = barWidth / ( noOfBeatMarkings + 1 );
            if( noOfBeatMarkings == 4 )
                barInc = barWidth / noOfBeatMarkings;

            juce::String markingText;
            for( int i = 1; i <= totalBeats; i += 4 )
            {
                int bar = static_cast<int>( std::ceil( i / 4 ) + 1 );
                getDisplayText( markingText, bar, 1, 0, totalDigits );
                drawMarkingText( g, markingText, currentPosX );

                for( int j = 0; j < noOfBeatMarkings; j++ )
                {
                    if( noOfTickMarkings )
                        currentPosX += tickWidth;
                    else
                        currentPosX += barInc;

                    int beat = j + 2;
                    if( noOfBeatMarkings == 1 )
                        beat = 3;

                    for( int k = 0; k < noOfTickMarkings; k++ )
                    {
                        markingText = juce::String().formatted( "%d.%d.%d", bar, beat - 1, ( k + 1 ) * ppqResolution );
                        drawMarkingText( g, markingText, currentPosX );
                        currentPosX += tickWidth;
                    }
                    if( j != 3 )
                    {
                        getDisplayText( markingText, bar, beat, 0, totalDigits );
                        drawMarkingText( g, markingText, currentPosX );
                    }
                    if( noOfBeatMarkings == 1 )
                        currentPosX += barInc;
                }
                if( noOfBeatMarkings == 0 )
                    currentPosX += barWidth;
            }
        }

        void SEScaleComponent::paintTime( juce::Graphics& g )
        {
            int                 viewWidth          = getLocalBounds().getWidth();
            double              sampleLength       = seData.getSampleLength();
            const juce::String& str                = juce::String().formatted( "%.3f", sampleLength );
            float               sampleDisplayWidth = static_cast<float>( g.getCurrentFont().getStringWidth( str ) + seSamplesSpacing + seSpaceFromMarker );

            double       sampleMarkerInc = ( sampleLength * sampleDisplayWidth ) / getLocalBounds().getWidth();
            float        xPos            = sampleDisplayWidth;
            juce::String markerText;
            double       currentSampleMarkerValue = sampleMarkerInc;

            while( xPos < viewWidth )
            {
                markerText = markerText.formatted( "%.3f", currentSampleMarkerValue );
                drawMarkingText( g, markerText, xPos );
                xPos += sampleDisplayWidth;
                currentSampleMarkerValue += sampleMarkerInc;
            }
        }

        void SEScaleComponent::calculateBeatWidth()
        {
            beatWidth         = 0;
            double totalBeats = seData.getNumberOfBeats();
            if( totalBeats == 0 )
                return;

            beatWidth = static_cast<float>( defaultSize / totalBeats );
            beatWidth = ( beatWidth * getLocalBounds().getWidth() ) / defaultSize;
        }

        void SEScaleComponent::getDisplayText( juce::String& markerText, int bar, int beat, int tick, int numDigits )
        {
            if( numDigits == 1 )
                markerText = markerText.formatted( "%d", bar );
            else if( numDigits == 2 )
                markerText = markerText.formatted( "%d.%d", bar, beat );
            else if( numDigits == 3 )
                markerText = markerText.formatted( "%d.%d.%d", bar, beat, tick );
        }
    }
}
