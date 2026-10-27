namespace mt
{
    namespace ac
    {
#define resizeRectangleWidth 20
#define resizeRectWidth      40
#define resizeRectHeight     34

        SeCuePositioner::SeCuePositioner( SampleEditorData& d )
            : seData( d )
        {
            seData.sampleChanged.connect( [this]() {
                //hide   = seData.isTrimMode();
                hide   = !seData.isChopMode();
                cuePos = seData.getCuePosition();
            } );
            seData.modeChanged.connect( [this]( SampleEditorData::Mode /*mode*/ ) {
                // hide = seData.isTrimMode();
                hide = !seData.isChopMode();
            } );
            seData.cuePositionChanged.connect( [this]( juce::int64 pos ) {
                if( cuePos != pos )
                    cuePos = pos;
            } );
            hide = !seData.isChopMode();
        }

        SeCuePositioner::~SeCuePositioner() {}

        void SeCuePositioner::paint( juce::Graphics& g )
        {
            if( hide )
                return;

            juce::Rectangle<float> cpr = getCuePositioningRect();
            g.setColour( ui::Theme::backgroundColour( 8 ) );
            g.drawLine( cpr.getTopLeft().getX(), 0, cpr.getTopLeft().getX(), static_cast<float>( totalArea.getHeight() ) );
            g.fillRect( cpr );
            g.setColour( ui::Theme::textColour( 0 ) );
            g.drawFittedText( "C", cpr.toNearestInt(), juce::Justification::centred, 1 );
        }

        juce::Rectangle<float> SeCuePositioner::getCuePositioningRect()
        {
            float                  posInPixels = seData.getPosInPixels( cuePos );
            juce::Point<float>     topLeft( posInPixels, ( totalArea.getHeight() / 2.0f - ( resizeRectangleWidth / 2.0f ) ) );
            juce::Rectangle<float> resizeRect( topLeft.getX(), topLeft.getY(), static_cast<float>( resizeRectangleWidth ), static_cast<float>( resizeRectHeight ) );

            return resizeRect;
        }

        bool SeCuePositioner::mouseMove( const juce::MouseEvent& event )
        {
            mouseFocus = false;
            if( hide )
                return mouseFocus;

            auto  resizeR  = getCuePositioningRect();
            float posInPix = seData.getPosInPixels( cuePos );

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

        void SeCuePositioner::mouseDrag( const juce::MouseEvent& event )
        {
            if( hide )
                return;

            juce::int64 cuePosTmp = seData.getPosInSeconds( event.position.getX() );
            if( cuePosTmp < 0 || cuePosTmp > seData.getSampleLengthInSamples() )
            {
                if( cuePosTmp < 0 )
                    cuePosTmp = 0;
                else
                    cuePosTmp = seData.getSampleLengthInSamples();
            }
            if( cuePosTmp == cuePos )
                return;

            cuePos = cuePosTmp;
            seData.setCuePosition( cuePos );
            if( mouseDragStart && seData.getCuePreview() == SampleEditorData::CuePreview::cuePreviewBefore )
            {
                MTSampleRegion r( cuePos, cuePos );
                seData.play( r, MTSamplePlayMode::samplePlayModeCue );
                mouseDragStart = false;
            }
        }

        void SeCuePositioner::mouseUp( const juce::MouseEvent& )
        {
            mouseDragStart = true;
            if( hide || !mouseFocus )
                return;

            if( seData.getCuePreview() == SampleEditorData::CuePreview::cuePreviewAfter )
            {
                MTSampleRegion r( cuePos, cuePos );
                seData.play( r, MTSamplePlayMode::samplePlayModeCue );
            }
        }

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////// Loop positioner//////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        SeLoopPositioner::SeLoopPositioner( SampleEditorData& d )
            : seData( d )
        {
            seData.sampleChanged.connect( [this]() {
                loopPos = seData.getLoopPosition();
            } );

            seData.regionChanged.connect( [this]( const Region& region ) {
                if( region.isSelected() )
                    selectedRegion = region;
                hide = seData.getLoopLock() || seData.getLoopMode() == LoopMode::loopModeNone;
                if( hide || !region.isSelected() )
                    return;

                if( region.getLoopPos() != loopPos )
                    loopPos = region.getLoopPos();
            } );

            seData.loopLockChanged.connect( [this]( bool /*locked*/ ) {
                hide    = seData.getLoopLock() || seData.getLoopMode() == LoopMode::loopModeNone;
                loopPos = seData.getLoopPosition();
            } );

            seData.modeChanged.connect( [this]( SampleEditorData::Mode /*mode*/ ) {
                hide = seData.getLoopLock() || seData.getLoopMode() == LoopMode::loopModeNone;
                if( !hide )
                {
                    const Region& selRegion = seData.getSelectedRegion();
                    if( loopPos < selRegion.getStart() || loopPos > selRegion.getEnd() )
                        loopPos = selRegion.getStart();
                }
            } );
            seData.loopPositionChanged.connect( [this]( juce::int64 pos ) {
                if( loopPos != pos )
                    loopPos = pos;
            } );
            hide = seData.getLoopLock() || seData.getLoopMode() == LoopMode::loopModeNone;
        }

        SeLoopPositioner::~SeLoopPositioner() {}

        void SeLoopPositioner::paint( juce::Graphics& g )
        {
            float posInPixS = seData.getPosInPixels( selectedRegion.getStart() );
            float posInPixE = seData.getPosInPixels( selectedRegion.getEnd() );

            // loop marking
            LoopMode mode = selectedRegion.getLoopMode();
            if( mode != LoopMode::loopModeNone )
            {
                posInPixS = seData.getPosInPixels( seData.getLoopPosition() );

                g.setColour( juce::Colours::blue );
                g.drawLine( posInPixS, 0, posInPixE, 0, 5 );
            }

            if( hide )
                return;

            juce::Rectangle<float> cpr = getCuePositioningRect();
            g.setColour( juce::Colours::blue );
            g.drawLine( cpr.getTopLeft().getX(), 0, cpr.getTopLeft().getX(), static_cast<float>( totalArea.getHeight() ) );
            g.fillRect( cpr );
        }

        juce::Rectangle<float> SeLoopPositioner::getCuePositioningRect()
        {
            float                  posInPixels = seData.getPosInPixels( loopPos );
            juce::Point<float>     topLeft( posInPixels, ( totalArea.getHeight() / 2.0f - ( resizeRectangleWidth / 2.0f ) ) );
            juce::Rectangle<float> resizeRect( topLeft.getX(), topLeft.getY(), static_cast<float>( resizeRectangleWidth ), static_cast<float>( resizeRectHeight ) );

            return resizeRect;
        }

        bool SeLoopPositioner::mouseMove( const juce::MouseEvent& event )
        {
            mouseFocus = false;
            if( hide )
                return mouseFocus;

            auto  resizeR  = getCuePositioningRect();
            float posInPix = seData.getPosInPixels( loopPos );

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

        void SeLoopPositioner::mouseDrag( const juce::MouseEvent& event )
        {
            if( hide )
                return;

            juce::int64 loopPosTmp = seData.getPosInSeconds( event.position.getX() );
            if( seData.isZeroCrossingEnabled() )
                loopPosTmp = seData.getNextZeroSnapPos( loopPosTmp, loopPosTmp > loopPos );

            const Region& selRegion = seData.getSelectedRegion();

            if( loopPosTmp > selRegion.getEnd() || loopPosTmp < selRegion.getStart() )
            {
                if( loopPosTmp < selRegion.getStart() )
                    loopPosTmp = selRegion.getStart();
                else
                    loopPosTmp = selRegion.getEnd();
            }

            if( loopPosTmp == loopPos )
                return;

            loopPos = loopPosTmp;
            seData.setLoopPosition( loopPos );
        }

        void SeLoopPositioner::mouseUp( const juce::MouseEvent& )
        {
            if( hide || !mouseFocus )
                return;

            seData.setLoopPosition( loopPos, true );
        }
    }
}
