#pragma once

namespace ui
{
    class WaveRenderer : public juce::Component
    {
    public:
        WaveRenderer();
        ~WaveRenderer() = default;

        void setSource( const juce::AudioSampleBuffer& buffer, double sampleRate );
        void setSource( float* const* dataReferer, int numChannels, int numSamples, int sampleRate );
        void setZoomFactor( double amount );
        void setRange( juce::Range<double> newRange );
        void changeAmplitude( juce::AudioSampleBuffer& buffer, double Level );
        void setLevelOrReverseSample( double Level, bool IsReverse );
        void setSelected( bool IsSelected )
        {
            isSelected = IsSelected;
        }
        void setMute( bool IsMuted )
        {
            mute = IsMuted;
        }
        void clearCache();
        void reRender( const juce::AudioSampleBuffer& Data, int NumSamples, juce::int64 SampleNumberInSource = 0, int StartOffsetInBuffer = 0 );
        void setBackgroundColour( juce::Colour newColour )
        {
            backgroundColour = newColour;
            repaint(); // Repaint the component to reflect the new colourl
        }

    private:
        juce::Colour backgroundColour;
        bool         isSelected{ false };
        bool         mute{ false };

        void paint( juce::Graphics& g ) override
        {
            auto b = getLocalBounds().reduced( 1 );
            // Draw Background...
            g.setColour( backgroundColour );
            g.fillRoundedRectangle( b.toFloat(), 5.f );

            if( isSelected )
            {
                g.setColour( ui::Theme::backgroundColour( 12 ) );
                g.fillRoundedRectangle( b.toFloat(), 1.f );
                g.setColour( ui::Theme::backgroundColour( 5 ) );
                g.drawRoundedRectangle( b.toFloat(), 2.f, 2.f );
            }

            mute ? g.setColour( juce::Colours::grey ) : g.setColour( juce::Colours::orange );
            thumbnail.drawChannels( g, b.reduced( 5 ), visibleRange.getStart(), visibleRange.getEnd(), vZoom );
        }

        inline double xToTime( const float x ) const
        {
            return ( x / (float)getWidth() ) * ( visibleRange.getLength() ) + visibleRange.getStart();
        }

        void setSource( const juce::AudioSampleBuffer& );

    private:
        float  hZoom{ 0.f };
        float  vZoom{ 1.f };
        double level{ 0 };
        double sampleRate{ 44100 };

        juce::AudioThumbnailCache thumbnailCache; // [1]
        juce::AudioThumbnail      thumbnail; // [2]
        juce::AudioFormatManager  formatManager; // [3]
        juce::Range<double>       visibleRange;
        juce::AudioSampleBuffer   sampleBuffer;
    };
}
