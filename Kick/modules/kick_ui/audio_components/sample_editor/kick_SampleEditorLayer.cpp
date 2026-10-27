#include "kick_SampleEditorLayer.h"
namespace mt
{
    namespace ac
    {
#define selayerOptionsHeight         249 ///195 + 54
#define seLayerScaleHeight           15
#define seLayersMargin               10
#define seLayerSelectionHeight       54

#define seLayerButtonWidth           170
#define seLayerButtonHeight          34

#define selayerOptionsControlWidth   200
#define selayerOptionsControlHeight  44
#define seLayerControlSpacing        20
#define seLayerControlSpacing1       60
#define selayerOptionsControlHeight1 74
#define selayerOptionsControlHeight2 50

        class SampleEditorLayerOptionsBase : public SampleEditorLayerOptionsInterface
            , public juce::Button::Listener
        {
        public:
            SampleEditorLayerOptionsBase();
            ~SampleEditorLayerOptionsBase();
            void setSeData( SampleEditorData* data ) override;
            void resized() override;
            void buttonClicked( juce::Button* button ) override;
            void setSamplePath( const juce::String& sample ) override;
            void reload() override;

            void setLayerButtonText( SampleEditorLayerType lt );
            // TODO handle layer changes through ParameterQueryInterface
            void         setLayerType( SampleEditorLayerType type );
            void         deleteLayer( SampleEditorLayerType lt );
            juce::String getOrSetLayer( SampleEditorLayerType lt, const juce::String& layer = "" );

        protected:
            juce::ValueTree        getNote();
            juce::Rectangle<float> getLayerOptionsRect();
            juce::Rectangle<int>   getLayerRect();

        protected:
            ui::KickToggleButton layer1{ TRANS( "L1" ) };
            ui::KickToggleButton layer2{ TRANS( "L2" ) };
            ui::KickToggleButton layer3{ TRANS( "L3" ) };
            ui::KickToggleButton layer4{ TRANS( "L4" ) };

            ui::KickListBox  sampleListBox{ "Sample", 1 };
            ui::KickNumber64 start{ "Start", 1, 200, 1, 1, true, false };
            ui::KickNumber64 end{ "End", 1, 200, 1, 1, true, false };
            ui::KickNumber64 loop{ "Loop", 1, 200, 1, 1, true, false };

            ui::KickListBox slices{ "Slices", 1 };
            ui::KickNumber  tune{ "Semi", 1, 2000, 1, 1, true, false };
            ui::KickNumber  fine{ "Fine", 1, 2000, 1, 1, true, false };
            ui::KickButton  reverse{ "Reverse" };

            juce::String              samplePath;
            std::vector<juce::String> samples;
            SampleEditorData*         seData            = nullptr;
            bool                      ignoreValueChange = false;
            SampleEditorLayerType     layerType         = SampleEditorLayerType::layerType1;
            int                       currentNote       = 0;
        };

        SampleEditorLayerOptionsBase::SampleEditorLayerOptionsBase()
        {
            utils::addAndMakeVisible( this, sampleListBox, start, end, loop, slices, tune, fine );
            utils::addAndMakeVisible( this, reverse, layer1, layer2, layer3, layer4 );

            utils::addListener( this, reverse );
            utils::addListener( this, layer1, layer2, layer3, layer4 );

            utils::setRadioGroupID( 1, layer1, layer2, layer3, layer4 );

            layer1.setToggleState( true, juce::dontSendNotification );

            /*Controls::InputEvents::hardwareInputTriggered.connect([this](CPktMgr::HWMsg_t& f_HwMsg)
             {
                 if( (f_HwMsg.pktType == CPktMgr::enumPacketType::EN_PKT_TYPE_PAD) && (f_HwMsg.pktId == Controls::PadCmd::EN_PAD_STATE) )
                 {
                     int padNum = Controls::getPadNumber( f_HwMsg );
                     int padState = Controls::getPadState( f_HwMsg );

                     if( padState == Controls::BTN_PRESSED )
                     {
                         currentNote = Controls::PadSequence::EN_PAD1;
                         reload();
                     }
                 }
                        } );*/

            /*contracts::DclSignals::hardwareInputTriggered.connect([this](CPktMgr::HWMsg_t& f_HwMsg)
             {
                 if( (f_HwMsg.pktType == CPktMgr::enumPacketType::EN_PKT_TYPE_PAD) && (f_HwMsg.pktId == contracts::DeviceControlContracts::PadCmd::EN_PAD_STATE) )
                 {
                     int padNum = contracts::DclContractsHelper::getPadNumber( f_HwMsg );
                     int padState = contracts::DclContractsHelper::getPadState( f_HwMsg );

                     if( padState == Controls::BTN_PRESSED )
                     {
                         currentNote = contracts::DeviceControlContracts::PadSequence::EN_PAD1;
                         reload();
                     }
                 }
                        } );*/
        }

        juce::ValueTree SampleEditorLayerOptionsBase::getNote()
        {
            MTAudioInterface& ai = getAudioInterface();
            juce::ValueTree   note;
            auto              pProg           = ai.getSelectedProgramData();
            juce::String      currProgramName = pProg ? pProg->name.get() : "";
            if( currProgramName.isEmpty() )
                return note;
            juce::ValueTree vt      = ai.getValueTree();
            juce::ValueTree program = vt.getChildWithProperty( IDs::name, currProgramName );
            if( !program.isValid() )
                return note;

            note = program.getChildWithProperty( "keyNote", currentNote );
            return note;
        }

        void SampleEditorLayerOptionsBase::setSeData( SampleEditorData* data )
        {
            seData  = data;
            samples = seData->getSampleList();
            samples.insert( samples.begin(), "None" );

            sampleListBox.setListOptions( samples );
            sampleListBox.onSelectionChanged = [this]( int selIndex, const juce::String& value ) {
                if( selIndex != 0 && ignoreValueChange == false )
                {
                    getOrSetLayer( layerType, value );
                    seData->initialize( value );
                    setLayerButtonText( layerType );
                }
                else if( selIndex == 0 )
                {
                    deleteLayer( layerType );
                    seData->initialize( "" );
                    setLayerButtonText( layerType );
                }
            };

            seData->regionChanged.connect( [this]( const Region& region ) {
                if( region.isSelected() )
                {
                    start.setValue( region.getStart() );
                    end.setValue( region.getEnd() );
                    const juce::String& value = region.getIndex() == -1 ? juce::String( "All" ) : juce::String( region.getIndex() );
                    if( slices.getSelectedValue() != value )
                    {
                        slices.setSelected( value );
                    }
                }
            } );

            seData->sampleChanged.connect( [this]() {
                std::vector<juce::String> options;
                auto                      totalRegions = seData->getTotalRegions();
                if( slices.getSize() == (int)( totalRegions + 1 ) )
                    return;
                options.resize( totalRegions + 1 );
                options[0] = "All";
                for( unsigned int i = 1; i <= totalRegions; i++ )
                    options[i] = juce::String( i );

                slices.setListOptions( options, seData->getSelectedRegion().getIndex() + 1 );
                start.setValueBounds( 1, seData->getSampleLengthInSamples(), 1 );
                end.setValueBounds( 1, seData->getSampleLengthInSamples(), 1 );
            } );

            slices.onSelectionChanged = [this]( int selIndex, const juce::String& /*value*/ ) {
                seData->selectRegion( selIndex - 1 );
            };
        }

        SampleEditorLayerOptionsBase::~SampleEditorLayerOptionsBase() {}

        void SampleEditorLayerOptionsBase::resized()
        {
            layer1.setBounds( 0, seLayersMargin, seLayerButtonWidth, seLayerButtonHeight );
            layer2.setBounds( layer1.getRight() + seLayersMargin, seLayersMargin, seLayerButtonWidth, seLayerButtonHeight );
            layer3.setBounds( layer2.getRight() + seLayersMargin, seLayersMargin, seLayerButtonWidth, seLayerButtonHeight );
            layer4.setBounds( layer3.getRight() + seLayersMargin, seLayersMargin, seLayerButtonWidth, seLayerButtonHeight );
        }

        void SampleEditorLayerOptionsBase::buttonClicked( juce::Button* button )
        {
            if( button == &reverse )
            {
            }
            else
            {
                if( !button->getToggleState() )
                    return;

                SampleEditorLayerType lt = SampleEditorLayerType::layerType1;
                if( button == &layer1 )
                    lt = SampleEditorLayerType::layerType1;
                else if( button == &layer2 )
                    lt = SampleEditorLayerType::layerType2;
                else if( button == &layer3 )
                    lt = SampleEditorLayerType::layerType3;
                else if( button == &layer4 )
                    lt = SampleEditorLayerType::layerType4;

                setLayerType( lt );
            }
        }

        void SampleEditorLayerOptionsBase::setLayerButtonText( SampleEditorLayerType lt )
        {
            ui::KickToggleButton* btn      = nullptr;
            const juce::String&   filePath = juce::File( getOrSetLayer( lt ) ).getFileName();
            juce::String          buttonText;
            if( lt == SampleEditorLayerType::layerType1 )
            {
                buttonText = "L1 ";
                btn        = &layer1;
            }
            else if( lt == SampleEditorLayerType::layerType2 )
            {
                buttonText = "L2 ";
                btn        = &layer2;
            }
            else if( lt == SampleEditorLayerType::layerType3 )
            {
                buttonText = "L3 ";
                btn        = &layer3;
            }
            else if( lt == SampleEditorLayerType::layerType4 )
            {
                buttonText = "L4 ";
                btn        = &layer4;
            }
            buttonText += filePath;
            btn->setToggleButtonText( buttonText, buttonText );
        }

        juce::Rectangle<float> SampleEditorLayerOptionsBase::getLayerOptionsRect()
        {
            return juce::Rectangle<float>( 0.0f, seLayerSelectionHeight, (float)getWidth(), (float)getHeight() );
        }

        juce::Rectangle<int> SampleEditorLayerOptionsBase::getLayerRect()
        {
            return juce::Rectangle<int>( 0, 0, getWidth(), seLayerSelectionHeight );
        }

        juce::String SampleEditorLayerOptionsBase::getOrSetLayer( SampleEditorLayerType lt, const juce::String& newSamplePath )
        {
            juce::String    layerSample;
            int             layerIndex = (int)lt;
            juce::ValueTree note       = getNote();
            if( !note.isValid() )
                return layerSample;

            juce::ValueTree sound;
            int             numChild = note.getNumChildren();
            for( int i = 0; i < numChild; i++ )
            {
                juce::ValueTree child = note.getChild( i );
                auto            type  = child.getType();
                if( type != juce::Identifier( "CLIP" ) )
                    continue;

                if( (int)child.getProperty( IDs::layerType ) == layerIndex )
                {
                    sound = child;
                    break;
                }
            }

            if( !newSamplePath.isEmpty() )
            {
                if( sound.isValid() )
                {
                    sound.setProperty( "source", newSamplePath, nullptr );
                }
                else
                {
                    sound = juce::ValueTree( "CLIP" );
                    sound.setProperty( "source", newSamplePath, nullptr );
                    sound.setProperty( IDs::layerType, juce::String( layerIndex ), nullptr );
                    note.appendChild( sound, nullptr );
                }
            }

            if( sound.isValid() )
                return sound.getProperty( "source" );

            return juce::String();
        }

        void SampleEditorLayerOptionsBase::setLayerType( SampleEditorLayerType type )
        {
            layerType              = type;
            juce::String layerPath = getOrSetLayer( type );
            ignoreValueChange      = true;

            seData->initialize( layerPath );
            if( layerPath.isEmpty() )
                setSamplePath( "None" );
            else
                setSamplePath( layerPath );

            setLayerButtonText( layerType );
            ignoreValueChange = false;
        }

        void SampleEditorLayerOptionsBase::deleteLayer( SampleEditorLayerType lt )
        {
            int             layerIndex = (int)lt;
            juce::ValueTree note       = getNote();
            if( !note.isValid() )
                return;

            int numChild = note.getNumChildren();
            for( int i = 0; i < numChild; i++ )
            {
                juce::ValueTree child = note.getChild( i );
                auto            type  = child.getType();
                if( type != juce::Identifier( "CLIP" ) )
                    continue;

                if( layerIndex == (int)child.getProperty( IDs::layerType ) )
                {
                    note.removeChild( i, nullptr );
                    break;
                }
            }
        }

        void SampleEditorLayerOptionsBase::setSamplePath( const juce::String& sample )
        {
            int index = 0;
            for( auto it = samples.begin(); it != samples.end(); it++, index++ )
            {
                if( *it == sample )
                {
                    sampleListBox.setSelected( index );
                    break;
                }
            }
        }

        void SampleEditorLayerOptionsBase::reload()
        {
            setLayerType( SampleEditorLayerType::layerType1 );

            setLayerButtonText( SampleEditorLayerType::layerType1 );
            setLayerButtonText( SampleEditorLayerType::layerType2 );
            setLayerButtonText( SampleEditorLayerType::layerType3 );
            setLayerButtonText( SampleEditorLayerType::layerType4 );
        }

        class SampleEditorLayerOptions : public SampleEditorLayerOptionsBase
        {
        public:
            SampleEditorLayerOptions();
            ~SampleEditorLayerOptions() {}
            void paint( juce::Graphics& g ) override;
            void resized() override;
            void setSeData( SampleEditorData* data ) override;

        private:
            ui::KickImageButton padLoop{ "PadLoop" };

            ui::KickButton warp{ "Warp" };
            ui::KickNumber stretch{ "Stretch", 1, 200, 1, 1, true, false };
            ui::KickNumber bpm{ "Bpm", 1, 200, 1, 1, true, false };
            ui::KickButton bpmSync{ "BPM Sync" };
        };

        SampleEditorLayerOptions::SampleEditorLayerOptions()
        {
            padLoop.setEnabled( false );

            utils::addAndMakeVisible( this, padLoop, warp, bpmSync, stretch, bpm );
            utils::addListener( this, padLoop );
            utils::addListener( this, warp, bpmSync );
        }

        void SampleEditorLayerOptions::paint( juce::Graphics& g )
        {
            g.setColour( ui::Theme::backgroundColour( 1 ) );
            const juce::Rectangle<float>& backRect = getLayerOptionsRect();
            g.fillRoundedRectangle( backRect, 2 );

            // draw marking lines
            g.setColour( ui::Theme::outlineColour( 2 ) );
            g.drawLine( (float)seLayersMargin, sampleListBox.getBottom() + seLayerControlSpacing / 2.0f, (float)( getWidth() - seLayersMargin ), sampleListBox.getBottom() + seLayerControlSpacing / 2.0f, 0.5f );
            //g.drawLine( seLayersMargin, slices.getBottom() + seLayerControlSpacing / 2, getWidth() - seLayersMargin, slices.getBottom() + seLayerControlSpacing / 2, 0.5f );

            float y = (float)seLayerSelectionHeight;
            g.drawLine( sampleListBox.getRight() + seLayerControlSpacing / 2.0f, y + seLayersMargin, sampleListBox.getRight() + seLayerControlSpacing / 2.0f, y + seLayersMargin + selayerOptionsControlHeight, 0.5f );
            g.drawLine( start.getRight() + seLayerControlSpacing / 2.0f, y + seLayersMargin, start.getRight() + seLayerControlSpacing / 2.0f, y + seLayersMargin + selayerOptionsControlHeight, 0.5f );
            g.drawLine( end.getRight() + seLayerControlSpacing / 2.0f, y + seLayersMargin, end.getRight() + seLayerControlSpacing / 2.0f, y + seLayersMargin + selayerOptionsControlHeight, 0.5f );

            g.drawLine( slices.getRight() + seLayerControlSpacing / 2.0f, (float)slices.getY(), slices.getRight() + seLayerControlSpacing / 2.0f, (float)( getHeight() - seLayersMargin ), 0.5f );
            g.drawLine( fine.getRight() + seLayerControlSpacing / 2.0f, (float)fine.getY(), fine.getRight() + seLayerControlSpacing / 2.0f, (float)( getHeight() - seLayersMargin ), 0.5f );
            g.drawLine( tune.getRight() + seLayerControlSpacing / 2.0f, (float)tune.getY(), tune.getRight() + seLayerControlSpacing / 2.0f, (float)( tune.getY() + selayerOptionsControlHeight ), 0.5f );

            g.drawLine( (float)slices.getX(), slices.getBottom() + seLayerControlSpacing / 2.0f, (float)( slices.getX() + selayerOptionsControlWidth ), slices.getBottom() + seLayerControlSpacing / 2.0f, 0.5f );
            g.drawLine( (float)tune.getX(), slices.getBottom() + seLayerControlSpacing / 2.0f, (float)( tune.getX() + selayerOptionsControlWidth ), slices.getBottom() + seLayerControlSpacing / 2.0f, 0.5f );

            SampleEditorLayerOptionsBase::paint( g );
        }

        void SampleEditorLayerOptions::resized()
        {
            SampleEditorLayerOptionsBase::resized();

            int y = seLayerSelectionHeight;
            sampleListBox.setBounds( seLayersMargin, y + seLayersMargin, selayerOptionsControlWidth, selayerOptionsControlHeight );
            start.setBounds( sampleListBox.getRight() + seLayerControlSpacing, y + seLayersMargin, selayerOptionsControlWidth, selayerOptionsControlHeight );
            end.setBounds( start.getRight() + seLayerControlSpacing, y + seLayersMargin, selayerOptionsControlWidth, selayerOptionsControlHeight );
            loop.setBounds( end.getRight() + seLayerControlSpacing, y + seLayersMargin, selayerOptionsControlWidth, selayerOptionsControlHeight );

            int x = getWidth() - ( 2 * selayerOptionsControlWidth ) - ( seLayersMargin + seLayerControlSpacing );
            slices.setBounds( seLayersMargin, start.getBottom() + seLayerControlSpacing, selayerOptionsControlWidth, selayerOptionsControlHeight );
            tune.setBounds( slices.getRight() + seLayerControlSpacing, start.getBottom() + seLayerControlSpacing, selayerOptionsControlWidth / 2 - seLayerControlSpacing, selayerOptionsControlHeight );
            fine.setBounds( tune.getRight() + seLayerControlSpacing, start.getBottom() + seLayerControlSpacing, selayerOptionsControlWidth / 2, selayerOptionsControlHeight );
            reverse.setBounds( x, start.getBottom() + seLayerControlSpacing, selayerOptionsControlWidth, selayerOptionsControlHeight );
            padLoop.setBounds( reverse.getRight() + seLayerControlSpacing, start.getBottom() + seLayerControlSpacing, selayerOptionsControlWidth, selayerOptionsControlHeight );

            warp.setBounds( seLayersMargin, slices.getBottom() + seLayerControlSpacing, selayerOptionsControlWidth, selayerOptionsControlHeight );
            stretch.setBounds( warp.getRight() + seLayerControlSpacing, slices.getBottom() + seLayerControlSpacing, selayerOptionsControlWidth, selayerOptionsControlHeight );
            bpm.setBounds( stretch.getRight() + seLayerControlSpacing, slices.getBottom() + seLayerControlSpacing, selayerOptionsControlWidth, selayerOptionsControlHeight );

            x = getWidth() - selayerOptionsControlWidth - seLayersMargin;
            bpmSync.setBounds( x, slices.getBottom() + seLayerControlSpacing, selayerOptionsControlWidth, selayerOptionsControlHeight );
        }

        void SampleEditorLayerOptions::setSeData( SampleEditorData* data )
        {
            SampleEditorLayerOptionsBase::setSeData( data );
            seData->sampleChanged.connect( [this]() {
                auto loopMode = seData->getLoopMode();
                if( loopMode == loopModeNone )
                    padLoop.setImages( "sEdit_buttonloop" );
                else if( loopMode == loopModeNone )
                    padLoop.setImages( "sEdit_buttonforward" );
                else if( loopMode == loopModeNone )
                    padLoop.setImages( "sEdit_buttonalternate" );
                else if( loopMode == loopModeNone )
                    padLoop.setImages( "sEdit_buttonbackward" );
            } );
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // ///SampleEditorLayer
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        SampleEditorLayerBase::SampleEditorLayerBase( SampleEditorLayerOptionsInterface* opt )
        {
            layerOptions.reset( opt );
            layerOptions->setSeData( &seData );

            seData.setView( &editor );
            scaleViewport.setViewedComponent( &scale, false );
            seViewport.setViewedComponent( &editor, false );

            addAndMakeVisible( scaleViewport );
            addAndMakeVisible( seViewport );
            addAndMakeVisible( layerOptions.get() );
        }

        SampleEditorLayerBase::~SampleEditorLayerBase() {}

        void SampleEditorLayerBase::resized()
        {
            int height = ui::_scaledY( getHeight() ) - ui::_scaledY( selayerOptionsHeight + seLayerScaleHeight );
            scaleViewport.setBounds( 0, 0, ui::layout::getAppWidth(), ui::_scaledY( seLayerScaleHeight ) );
            seViewport.setBounds( 0, ui::_scaledY( seLayerScaleHeight ), ui::layout::getAppWidth(), height );

            scale.setBounds( 0, 0, ui::layout::getAppWidth(), ui::_scaledY( seLayerScaleHeight ) );
            editor.setBounds( 0, ui::_scaledY( seLayerScaleHeight ), ui::layout::getAppWidth(), height );

            int y            = height + seLayerScaleHeight;
            int optionsWidth = ui::layout::getAppWidth() - seLayersMargin;
            layerOptions->setBounds( 0, y, optionsWidth, selayerOptionsHeight - 2 );
        }

        juce::Component* SampleEditorLayerBase::getSampleLayerSettingsComponent()
        {
            return &settings;
        }

        void SampleEditorLayerBase::loadSample( const juce::String& )
        {
            layerOptions->reload();
        }

        SampleEditorLayer::SampleEditorLayer()
            : SampleEditorLayerBase( new SampleEditorLayerOptions() )
        {
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // ///SampleEditorLayerKG
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        class SampleEditorLayerOptionsKG : public SampleEditorLayerOptionsBase
        {
        public:
            SampleEditorLayerOptionsKG();
            void paint( juce::Graphics& g );
            void resized() override;

        private:
            ui::KickCyclicStateControl loopf;
        };

        SampleEditorLayerOptionsKG::SampleEditorLayerOptionsKG()
        {
            addAndMakeVisible( loopf );
            loopf.setStates( { "sEdit_buttonloop", "sEdit_buttonforward", "sEdit_buttonalternate", "sEdit_buttonbackward" } );
        }

        void SampleEditorLayerOptionsKG::paint( juce::Graphics& g )
        {
            g.setColour( ui::Theme::backgroundColour( 1 ) );
            const juce::Rectangle<float>& backRect = getLayerOptionsRect();
            g.fillRoundedRectangle( backRect, 2 );

            // draw marking lines
            g.setColour( ui::Theme::outlineColour( 2 ) );

            float y = (float)seLayerSelectionHeight;
            g.drawLine( sampleListBox.getRight() + seLayerControlSpacing / 2.0f, y + seLayersMargin, sampleListBox.getRight() + seLayerControlSpacing / 2.0f, y + seLayersMargin + selayerOptionsControlHeight1, 0.5f );
            g.drawLine( start.getRight() + seLayerControlSpacing / 2.0f, y + seLayersMargin, start.getRight() + seLayerControlSpacing / 2.0f, y + seLayersMargin + selayerOptionsControlHeight1, 0.5f );

            g.drawLine( slices.getRight() + seLayerControlSpacing / 2.0f, (float)slices.getY(), slices.getRight() + seLayerControlSpacing / 2.0f, (float)( getHeight() - seLayersMargin ), 0.5f );
            g.drawLine( fine.getRight() + seLayerControlSpacing / 2.0f, (float)fine.getY(), fine.getRight() + seLayerControlSpacing / 2.0f, (float)( getHeight() - seLayersMargin ), 0.5f );
            g.drawLine( tune.getRight() + seLayerControlSpacing / 2.0f, (float)( tune.getY() + 32 ), tune.getRight() + seLayerControlSpacing / 2.0f, (float)( tune.getY() + selayerOptionsControlHeight1 ), 0.5f );
        }

        void SampleEditorLayerOptionsKG::resized()
        {
            int y = seLayerSelectionHeight;
            sampleListBox.setBounds( seLayersMargin, y + seLayersMargin, selayerOptionsControlWidth, selayerOptionsControlHeight1 );
            start.setBounds( sampleListBox.getRight() + seLayerControlSpacing, y + seLayersMargin, selayerOptionsControlWidth, selayerOptionsControlHeight1 );
            end.setBounds( start.getRight() + seLayerControlSpacing, y + seLayersMargin, selayerOptionsControlWidth, selayerOptionsControlHeight1 );

            slices.setBounds( seLayersMargin, start.getBottom() + seLayerControlSpacing, selayerOptionsControlWidth, selayerOptionsControlHeight1 );
            tune.setBounds( slices.getRight() + seLayerControlSpacing, start.getBottom() + seLayerControlSpacing, selayerOptionsControlWidth / 2 - seLayerControlSpacing, selayerOptionsControlHeight1 );
            fine.setBounds( tune.getRight() + seLayerControlSpacing, start.getBottom() + seLayerControlSpacing, selayerOptionsControlWidth / 2, selayerOptionsControlHeight1 );
            juce::Rectangle<int> revRect( fine.getRight() + seLayerControlSpacing + 10, start.getBottom() + seLayerControlSpacing, selayerOptionsControlWidth, selayerOptionsControlHeight1 );
            reverse.setBounds( 0, 0, selayerOptionsControlWidth, 32 );
            reverse.setBoundsToFit( revRect, juce::Justification::centred, true );

            int x = getWidth() - selayerOptionsControlWidth - 2 * seLayersMargin;
            loopf.setBounds( x, reverse.getY(), 170, 32 );

            SampleEditorLayerOptionsBase::resized();
        }

        SampleEditorLayerKG::SampleEditorLayerKG()
            : SampleEditorLayerBase( new SampleEditorLayerOptionsKG() )
        {
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // ///SampleEditorLayerKG
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        class SampleEditorLayerOptionsClip : public SampleEditorLayerOptionsInterface
        {
        public:
            SampleEditorLayerOptionsClip();
            void setSeData( SampleEditorData* data ) override { seData = data; }
            void setSamplePath( const juce::String& sample )
            {
                if( seData )
                    seData->initialize( sample );
            }
            void paint( juce::Graphics& g ) override;
            void resized() override;

        private:
            SampleEditorData* seData = nullptr;
            ui::KickNumber64  start{ "Start", 1, 200, 1, 1, true, false };
            ui::KickNumber64  end{ "End", 1, 200, 1, 1, true, false };
            ui::KickButton    warp{ "Warp" };

            ui::KickNumberf bpm{ "Bpm", 20, 1000, 20, 1, true, false };
            ui::KickListBox padPlay{ "Pad Play", 1 };
            ui::KickListBox padQuantize{ "Pad Quantize", 1 };
            ui::KickButton  reverse{ "Reverse" };

            ui::KickOffSetControl semi{ 99, "Semi" };
            ui::KickOffSetControl coarse{ 99, "Coarse" };
            ui::KickListBox       muteGroup{ "Mute Group", 3 };
            ui::KickButton        fade{ "Fade" };
        };

        SampleEditorLayerOptionsClip::SampleEditorLayerOptionsClip()
        {
            addAndMakeVisible( start );
            addAndMakeVisible( end );
            addAndMakeVisible( warp );

            addAndMakeVisible( bpm );
            addAndMakeVisible( padPlay );
            addAndMakeVisible( padQuantize );
            addAndMakeVisible( reverse );

            addAndMakeVisible( semi );
            addAndMakeVisible( coarse );
            addAndMakeVisible( muteGroup );
            addAndMakeVisible( fade );

            padPlay.setListOptions( { "Toggle", "One Shot" } );
            padQuantize.setListOptions( { "Program", "Off", "8 Bars", "4 Bars", "2 Bars", "1 Bar", "1/2 Bar", "1/4 Bar", "1/8 Bar", "1/16 Bar", "1/32 Bar", "1/64 Bar" } );

            std::vector<juce::String> muteGroupOpt( 32 );
            for( int i = 0; i < 32; i++ )
                muteGroupOpt[(size_t)i] = juce::String( i + 1 );
            muteGroup.setListOptions( muteGroupOpt );
        }

        void SampleEditorLayerOptionsClip::paint( juce::Graphics& g )
        {
            g.setColour( ui::Theme::backgroundColour( 1 ) );
            const juce::Rectangle<int> backRect( 0, 10, getWidth(), getHeight() );
            g.fillRoundedRectangle( backRect.toFloat(), 3 );

            g.setColour( ui::Theme::outlineColour( 2 ) );
            float y = 10.0f;
            g.drawLine( start.getRight() + seLayerControlSpacing1 / 2.0f, y + seLayersMargin, start.getRight() + seLayerControlSpacing1 / 2.0f, y + seLayersMargin + selayerOptionsControlHeight2, 0.5f );
            g.drawLine( bpm.getRight() + seLayerControlSpacing1 / 2.0f, (float)bpm.getY(), start.getRight() + seLayerControlSpacing1 / 2.0f, (float)bpm.getY() + selayerOptionsControlHeight2, 0.5f );
            g.drawLine( semi.getRight() + seLayerControlSpacing1 / 2.0f, (float)semi.getY(), semi.getRight() + seLayerControlSpacing1 / 2.0f, (float)semi.getY() + selayerOptionsControlHeight2, 0.5f );
            g.drawLine( coarse.getRight() + seLayerControlSpacing1 / 2.0f, (float)coarse.getY(), coarse.getRight() + seLayerControlSpacing1 / 2.0f, (float)coarse.getY() + selayerOptionsControlHeight2, 0.5f );
            g.drawLine( end.getRight() + seLayerControlSpacing1 / 2.0f, (float)end.getY(), end.getRight() + seLayerControlSpacing1 / 2.0f, (float)end.getY() + selayerOptionsControlHeight2, 0.5f );
            g.drawLine( padPlay.getRight() + seLayerControlSpacing1 / 2.0f, (float)padPlay.getY(), padPlay.getRight() + seLayerControlSpacing1 / 2.0f, (float)padPlay.getY() + selayerOptionsControlHeight2, 0.5f );
            g.drawLine( padQuantize.getRight() + seLayerControlSpacing1 / 2.0f, (float)padQuantize.getY(), padQuantize.getRight() + seLayerControlSpacing1 / 2.0f, (float)padQuantize.getY() + selayerOptionsControlHeight2, 0.5f );
            g.drawLine( muteGroup.getRight() + seLayerControlSpacing1 / 2.0f, (float)muteGroup.getY(), muteGroup.getRight() + seLayerControlSpacing1 / 2.0f, (float)muteGroup.getY() + selayerOptionsControlHeight2, 0.5f );
        }

        void SampleEditorLayerOptionsClip::resized()
        {
            int y = 10;
            int x = getWidth() - 170 - ( 2 * seLayersMargin );
            start.setBounds( seLayersMargin, y + seLayersMargin, selayerOptionsControlWidth, selayerOptionsControlHeight2 );
            end.setBounds( start.getRight() + seLayerControlSpacing1, y + seLayersMargin, selayerOptionsControlWidth, selayerOptionsControlHeight2 );
            juce::Rectangle<int> buttonRect( x, y + seLayersMargin, 170, selayerOptionsControlHeight2 );
            warp.setBounds( 0, 0, 170, 34 );
            warp.setBoundsToFit( buttonRect, juce::Justification::centred, true );

            bpm.setBounds( seLayersMargin, y + start.getBottom() + seLayerControlSpacing, selayerOptionsControlWidth, selayerOptionsControlHeight2 );
            padPlay.setBounds( bpm.getRight() + seLayerControlSpacing1, y + start.getBottom() + seLayerControlSpacing, selayerOptionsControlWidth, selayerOptionsControlHeight2 );
            padQuantize.setBounds( padPlay.getRight() + seLayerControlSpacing1, y + start.getBottom() + seLayerControlSpacing, selayerOptionsControlWidth, selayerOptionsControlHeight2 );
            buttonRect.setY( bpm.getY() );
            reverse.setBounds( 0, 0, 170, 34 );
            reverse.setBoundsToFit( buttonRect, juce::Justification::centred, true );

            semi.setBounds( seLayersMargin, y + padPlay.getBottom() + seLayerControlSpacing, selayerOptionsControlWidth, selayerOptionsControlHeight2 );
            coarse.setBounds( semi.getRight() + seLayerControlSpacing1, y + padPlay.getBottom() + seLayerControlSpacing, selayerOptionsControlWidth, selayerOptionsControlHeight2 );
            muteGroup.setBounds( coarse.getRight() + seLayerControlSpacing1, y + padPlay.getBottom() + seLayerControlSpacing, selayerOptionsControlWidth, selayerOptionsControlHeight2 );
            buttonRect.setY( semi.getY() );
            fade.setBounds( 0, 0, 170, 34 );
            fade.setBoundsToFit( buttonRect, juce::Justification::centred, true );
        }

        SampleEditorLayerClip::SampleEditorLayerClip()
            : SampleEditorLayerBase( new SampleEditorLayerOptionsClip )
        {
        }

        REGISER_FACTORY( SampleEditorLayerInterface, SampleEditorLayer, SampleLayerType::sampleLayerDrum )
        REGISER_FACTORY( SampleEditorLayerInterface, SampleEditorLayerKG, SampleLayerType::sampleLayerKeyGroup )
        REGISER_FACTORY( SampleEditorLayerInterface, SampleEditorLayerClip, SampleLayerType::sampleLayerClip )
    }
}
