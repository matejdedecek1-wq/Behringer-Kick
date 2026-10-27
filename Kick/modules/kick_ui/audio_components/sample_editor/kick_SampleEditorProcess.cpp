#include "kick_SampleEditorProcess.h"
namespace mt
{
    namespace ac
    {
#define SampleEditorProcessActionMargin       10
#define SampleEditorProcessActionButtonWidth  140
#define SampleEditorProcessActionButtonHeight 34

        class ProcessOptionsInterface
        {
        public:
            virtual ~ProcessOptionsInterface() {}
            virtual std::list<juce::var> getParameters() = 0;
        };

        class StereoMonoOptions : public juce::Component
            , public ProcessOptionsInterface
        {
        public:
            StereoMonoOptions();
            ~StereoMonoOptions() {}
            void                         paint( juce::Graphics& g );
            void                         resized() override;
            juce::String                 getSampleName() { return sampleNameEdit.getText(); }
            int                          getExtractMethod();
            virtual std::list<juce::var> getParameters();

        private:
            ui::KickLabel        source{ "Source" };
            ui::KickToggleButton left{ "Left" };
            ui::KickToggleButton right{ "Right" };
            ui::KickToggleButton sum{ "Sum" };
            ui::KickLabel        sampleName{ "Sample Name" };
            juce::Label          sampleNameEdit{ "SampleName", "Sample1" };
        };

        StereoMonoOptions::StereoMonoOptions()
        {
            left.setToggleState( true, juce::dontSendNotification );
            addAndMakeVisible( source );
            addAndMakeVisible( left );
            addAndMakeVisible( right );
            addAndMakeVisible( sum );
            addAndMakeVisible( sampleName );
            addAndMakeVisible( sampleNameEdit );

            left.setRadioGroupId( 1 );
            right.setRadioGroupId( 1 );
            sum.setRadioGroupId( 1 );

            sampleNameEdit.setEditable( true );
        }

        void StereoMonoOptions::resized()
        {
            source.setBounds( 0, 0, 264, 16 );
            left.setBounds( 0, source.getBottom() + 10, 81, 30 );
            right.setBounds( left.getRight() + 10, source.getBottom() + 10, 81, 30 );
            sum.setBounds( right.getRight() + 10, source.getBottom() + 10, 81, 30 );

            sampleName.setBounds( 0, sum.getBottom() + 30, 264, 16 );
            sampleNameEdit.setBounds( 0, sampleName.getBottom() + 10, 264, 30 );
        }

        int StereoMonoOptions::getExtractMethod()
        {
            if( left.getToggleState() )
                return 0;
            else if( right.getToggleState() )
                return 1;
            else if( sum.getToggleState() )
                return 2;

            return 0;
        }

        std::list<juce::var> StereoMonoOptions::getParameters()
        {
            std::list<juce::var> params;
            params.push_back( getExtractMethod() );
            params.push_back( getSampleName() );

            return params;
        }

        void StereoMonoOptions::paint( juce::Graphics& g )
        {
            g.setColour( ui::Theme::outlineColour( 2 ) );
            g.drawLine( 0.0f, left.getBottom() + 15.0f, (float)getWidth(), left.getBottom() + 15.0f );
        }

        class TimeStretchOptions : public juce::Component
            , public ProcessOptionsInterface
        {
        public:
            TimeStretchOptions();
            ~TimeStretchOptions() {}
            void                         resized() override;
            void                         paint( juce::Graphics& g ) override;
            virtual std::list<juce::var> getParameters();

        private:
            ui::KickNumber      fromBeat{ "Beat", 1, 200, 120, 1, true };
            ui::KickNumber      toBeat{ "Beat", 1, 200, 120, 1, true };
            ui::KickNumber      fromTempo{ "Original\nTempo", 1, 200, 120, 1, true };
            ui::KickNumber      toTempo{ "New\nTempo", 1, 200, 120, 1, true };
            ui::KickImageButton imageButton{ "directionbtn", "sEdit_Arrow" };
        };

        TimeStretchOptions::TimeStretchOptions()
        {
            addAndMakeVisible( fromBeat );
            addAndMakeVisible( toBeat );
            addAndMakeVisible( fromTempo );
            addAndMakeVisible( toTempo );
            addAndMakeVisible( imageButton );

            ui::themeChanged.connect( [&]() {
                imageButton.setImages( "sEdit_Arrow" );
            } );
        }

        void TimeStretchOptions::resized()
        {
            fromBeat.setBounds( 0, 0, 80, 63 );
            toBeat.setBounds( fromBeat.getRight() + 37, 0, 80, 63 );
            imageButton.setBounds( fromBeat.getRight() + 10, fromBeat.getBottom() + 1, 17, 18 );
            fromTempo.setBounds( 0, toBeat.getBottom() + 20, 80, 63 );
            toTempo.setBounds( fromTempo.getRight() + 37, toBeat.getBottom() + 20, 80, 63 );
        }

        void TimeStretchOptions::paint( juce::Graphics& g )
        {
            g.setColour( ui::Theme::outlineColour( 2 ) );
            g.drawLine( 0.0f, fromBeat.getBottom() + 10.0f, 80, fromBeat.getBottom() + 10.0f );
            g.drawLine( fromTempo.getRight() + 37.0f, toBeat.getBottom() + 10.0f, (float)toBeat.getRight(), toBeat.getBottom() + 10.0f );
        }

        std::list<juce::var> TimeStretchOptions::getParameters()
        {
            return std::list<juce::var>();
        }

        class ExtractCopyOptions : public juce::Component
            , public ProcessOptionsInterface
        {
        public:
            ExtractCopyOptions( int type );
            ~ExtractCopyOptions() {}
            void                         resized() override;
            void                         paint( juce::Graphics& g ) override;
            virtual std::list<juce::var> getParameters();

        private:
            ui::KickImageButton imageBtn{ "ImgBtn", "sEdit_Gextract" };
            ui::KickLabel       label{ "Sample Name" };
            ui::KickLabel       sampleName{ "Sample1" };
            int                 type = 0;
        };

        ExtractCopyOptions::ExtractCopyOptions( int t )
            : type( t )
        {
            if( type == 1 )
                imageBtn.setImages( "sEdit_Gcopy" );

            addAndMakeVisible( imageBtn );
            addAndMakeVisible( label );
            addAndMakeVisible( sampleName );

            ui::themeChanged.connect( [&]() {
                imageBtn.setImages( "sEdit_Gextract" );
            } );
        }

        void ExtractCopyOptions::resized()
        {
            if( type == 0 )
                imageBtn.setBounds( 0, 0, 338, 100 );
            else
                imageBtn.setBounds( 0, 0, 338, 147 );

            label.setBounds( 0, imageBtn.getBottom() + 30, 338, 16 );
            sampleName.setBounds( 0, label.getBottom() + 10, 338, 22 );
            sampleName.setEditable( true );
        }

        void ExtractCopyOptions::paint( juce::Graphics& g )
        {
            g.setColour( ui::Theme::outlineColour( 2 ) );
            g.drawLine( 0.0f, imageBtn.getBottom() + 15.0f, (float)getWidth(), imageBtn.getBottom() + 15.0f );
        }

        std::list<juce::var> ExtractCopyOptions::getParameters()
        {
            std::list<juce::var> params;
            params.push_back( sampleName.getText() );

            return params;
        }

        class FadeInFadeOutOptions : public juce::Component
            , public ProcessOptionsInterface
        {
        public:
            FadeInFadeOutOptions( int type );
            ~FadeInFadeOutOptions() {}
            void                         resized() override;
            void                         paint( juce::Graphics& g ) override;
            virtual std::list<juce::var> getParameters();

        private:
            ui::KickImageButton  imageBtn{ "ImgBtn", "sEdit_Gfadein" };
            ui::KickLabel        label{ "Sample Name" };
            ui::KickToggleButton linear{ "Linear" };
            ui::KickToggleButton log{ "Log" };
            ui::KickToggleButton exp{ "Exp" };
        };

        void FadeInFadeOutOptions::paint( juce::Graphics& g )
        {
            g.setColour( ui::Theme::outlineColour( 2 ) );
            g.drawLine( 0.0f, imageBtn.getBottom() + 15.0f, (float)getWidth(), imageBtn.getBottom() + 15.0f );
        }

        std::list<juce::var> FadeInFadeOutOptions::getParameters()
        {
            std::list<juce::var> params;
            int                  type = 0;

            if( linear.getToggleState() )
                type = 0;
            else if( log.getToggleState() )
                type = 1;
            else if( exp.getToggleState() )
                type = 2;

            params.push_back( type );

            return params;
        }

        FadeInFadeOutOptions::FadeInFadeOutOptions( int type )
        {
            if( type == 1 )
                imageBtn.setImages( "sEdit_Gfadeout" );

            addAndMakeVisible( imageBtn );
            addAndMakeVisible( label );
            addAndMakeVisible( linear );
            addAndMakeVisible( log );
            addAndMakeVisible( exp );

            linear.setRadioGroupId( 1 );
            log.setRadioGroupId( 1 );
            exp.setRadioGroupId( 1 );

            linear.setToggleState( true, juce::dontSendNotification );

            ui::themeChanged.connect( [&]() {
                imageBtn.setImages( "sEdit_Gfadein" );
            } );
        }

        void FadeInFadeOutOptions::resized()
        {
            imageBtn.setBounds( 0, 0, 268, 102 );

            label.setBounds( 0, imageBtn.getBottom() + 30, 268, 16 );
            linear.setBounds( 0, label.getBottom() + 10, 81, 30 );
            log.setBounds( linear.getRight() + 10, label.getBottom() + 10, 81, 30 );
            exp.setBounds( log.getRight() + 10, label.getBottom() + 10, 81, 30 );
        }

        SampleEditorProcessAction::SampleEditorProcessAction( unsigned int aid, SampleEditorData& d )
            : actionId( aid )
            , seData( d )
        {
            addAndMakeVisible( ok );
            addAndMakeVisible( cancel );
            ok.addListener( this );
            cancel.addListener( this );

            if( actionId <= 4 )
            {
                juce::String imageName = "sEdit_Gdiscard";
                if( actionId == 1 )
                    imageName = "sEdit_Gdelete";
                else if( actionId == 2 )
                    imageName = "sEdit_Gdelete";
                else if( actionId == 3 )
                    imageName = "sEdit_Gnormalize";
                else if( actionId == 4 )
                    imageName = "sEdit_Greverse";

                actionComponent.reset( new ui::KickImageButton( "actCmp", imageName ) );
            }
            else if( actionId == 5 || actionId == 6 )
            {
                if( actionId == 5 )
                    actionComponent.reset( new FadeInFadeOutOptions( 0 ) );
                else
                    actionComponent.reset( new FadeInFadeOutOptions( 1 ) );
            }
            else if( actionId == 7 )
            {
                actionComponent.reset( new ExtractCopyOptions( 0 ) );
            }
            else if( actionId == 8 || actionId == 10 || actionId == 12 )
            {
                juce::String title;
                double       min = 0.0;
                double       max = 0.0;
                if( actionId == 8 )
                {
                    title = "Pitch";
                    min   = SEMI::MIN;
                    max   = SEMI::MAX;
                }
                else if( actionId == 10 )
                {
                    title = "Gain";
                    min   = -18.0;
                    max   = 18.0;
                }
                else if( actionId == 13 )
                {
                    title = "Bit Reduce";
                    min   = 1;
                    max   = 16;
                }
                ui::KickRotary* rotary = new ui::KickRotary( title );
                rotary->setRange( min, max );
                actionComponent.reset( rotary );
            }
            else if( actionId == 9 )
            {
                actionComponent.reset( new TimeStretchOptions() );
            }
            else if( actionId == 11 )
            {
                actionComponent.reset( new ExtractCopyOptions( 1 ) );
            }
            else if( actionId == 13 )
            {
                actionComponent.reset( new StereoMonoOptions() );
            }

            addAndMakeVisible( actionComponent.get() );
            setSize( 360, 316 );
        }

        const juce::String& SampleEditorProcessAction::getTitle()
        {
            return seData.getSupportedActions().at( actionId );
        }

        void SampleEditorProcessAction::resized()
        {
            if( actionId <= 4 )
                actionComponent.get()->setBounds( 10, 77, 338, 60 );
            else if( actionId == 5 || actionId == 6 )
                actionComponent.get()->setBounds( 46, 10, 268, 190 );
            else if( actionId == 7 )
                actionComponent.get()->setBounds( 10, 10, 338, 210 );
            else if( actionId == 8 || actionId == 10 || actionId == 12 )
                actionComponent.get()->setBounds( 125, 83, 112, 90 );
            else if( actionId == 9 )
                actionComponent.get()->setBounds( 80, 69, 200, 160 );
            else if( actionId == 11 )
                actionComponent.get()->setBounds( 10, 10, 338, 240 );
            else if( actionId == 13 )
                actionComponent.get()->setBounds( 48, 39, 264, 150 );

            ok.setBounds( SampleEditorProcessActionMargin, getHeight() - SampleEditorProcessActionMargin - SampleEditorProcessActionButtonHeight, SampleEditorProcessActionButtonWidth, SampleEditorProcessActionButtonHeight );
            cancel.setBounds( getWidth() - SampleEditorProcessActionMargin - SampleEditorProcessActionButtonWidth, getHeight() - SampleEditorProcessActionMargin - SampleEditorProcessActionButtonHeight, SampleEditorProcessActionButtonWidth, SampleEditorProcessActionButtonHeight );
        }

        void SampleEditorProcessAction::buttonClicked( juce::Button* btn )
        {
            juce::DialogWindow* dw = findParentComponentOfClass<juce::DialogWindow>();
            if( dw == nullptr )
                return;

            if( btn == &ok )
            {
                if( actionId == seActionFadeIn || actionId == seActionFadeOut || actionId == seActionExtract || actionId == seActionTimeStretch || actionId == seActionCopy || actionId == seActionStereoToMono )
                {
                    ProcessOptionsInterface* intf = dynamic_cast<ProcessOptionsInterface*>( actionComponent.get() );
                    params                        = intf->getParameters();
                }
                else if( actionId == seActionGainChange || actionId == seActionPitchShift || actionId == seActionBitReduce )
                {
                    ui::KickRotary* rotary = static_cast<ui::KickRotary*>( actionComponent.get() );
                    params.push_back( rotary->getValue() );
                }
                dw->exitModalState( UI::BUTTONS::APPLY );
            }
            else
            {
                dw->exitModalState( UI::BUTTONS::CANCEL );
            }
        }

        void SampleEditorProcessAction::performAction( unsigned int actionId, SampleEditorData& data )
        {
            SampleEditorProcessAction* p = new SampleEditorProcessAction( actionId, data );
            utils::showDialog(
                p->getTitle(), p, [p, &data]( int r ) {
                    if( r != UI::BUTTONS::CANCEL )
                        data.performAction( p->getId(), p->getParams() );

                    delete p;
                },
                p->getLocalBounds() );
        }

        SampleEditorProcess::SampleEditorProcess( SampleEditorData& d )
            : KickMenuComponent( "SmpOptions", 4 )
        {
            std::vector<ui::MenuItem> sliceActions( 12 );
            sliceActions[0]  = ui::MenuItem( "N", "sEdit_Gdiscard" );
            sliceActions[1]  = ui::MenuItem( "N", "sEdit_Gdelete" );
            sliceActions[2]  = ui::MenuItem( "N", "sEdit_Gsilence" );
            sliceActions[3]  = ui::MenuItem( "N", "sEdit_Gnormalize" );
            sliceActions[4]  = ui::MenuItem( "N", "sEdit_Greverse" );
            sliceActions[5]  = ui::MenuItem( "N", "sEdit_Gfadein" );
            sliceActions[6]  = ui::MenuItem( "N", "sEdit_Gfadeout" );
            sliceActions[7]  = ui::MenuItem( "N", "sEdit_Gextract" );
            sliceActions[8]  = ui::MenuItem( "N", "sEdit_pitchshift" );
            sliceActions[9]  = ui::MenuItem( "N", "sEdit_timestretch" );
            sliceActions[10] = ui::MenuItem( "N", "sEdit_gainchange" );
            sliceActions[11] = ui::MenuItem( "N", "sEdit_copy" );

            std::vector<ui::MenuItem> sliceActions1( 2 );
            sliceActions1[0] = ui::MenuItem( "", "sEdit_bitreduce" );
            sliceActions1[1] = ui::MenuItem( "", "sEdit_steromono" );

            // disable inactive actions
            const std::vector<unsigned int>& disabledActions = d.getDisabledAction();
            for( unsigned int disAct : disabledActions )
            {
                if( disAct > 11 )
                    sliceActions1[disAct - 12].enabled = false;
                else
                    sliceActions[disAct].enabled = false;
            }

            addOptions( sliceActions, "Slices" );
            addOptions( sliceActions1, "Sample" );

            setOpaque( true );

            onMenuOptionClicked = [this]( const juce::String& /*option*/, int index, const juce::String& group ) {
                if( group == "Slices" )
                    selectedId = (unsigned int)index;
                else
                    selectedId = 12 + (unsigned int)index;

                juce::DialogWindow* dw = findParentComponentOfClass<juce::DialogWindow>();
                if( dw == nullptr )
                    return;

                dw->exitModalState( UI::BUTTONS::APPLY );
            };
        }
    }
}
