#include "kick_IOComponent.h"

namespace ui
{

#define io_radius            5
#define io_margin            6
#define io_dropControlHeight 50
#define io_op1Height         44
#define io_insertsHeight     24

    KickIOComponent::KickIOComponent()
    {
        gainMeter.setKnobImage( "Icon_Fader" );
        inputList.setJustificationType( juce::Justification::left );

        monitorSlider.showTitle( true );
        monitorSlider.showValue( false );
        monitorSlider.setTitleJustification( juce::Justification::centred );
        monitorSlider.setTitleFontHeight( 14 );
        monitorSlider.setRange( 0, 100, 1 );
        monitorSlider.setValue( 80, juce::dontSendNotification );
        monitorSlider.setTitleProportion( 0.5f );

        setInputs( { "Input 1", "Input 2" }, 0 );
        setOutputs( { "Mono", "Stereo" }, 1 );
        setOptions1( { &monitorSlider, &monitor } );

        mt::utils::addAndMakeVisible( this, inputList, outputList, panB, recordB, ieff, gainMeter, levelMeter );

        // Input and Output
        inputList.onSelectionChanged  = [this]( int index, const juce::String& value ) { inputChanged( index + 1, value ); };
        outputList.onSelectionChanged = [this]( int index, const juce::String& value ) { outputChanged( index + 1, value ); };
        // Gain and Pan
        gainMeter.gainChanged.connect( [this]( float gainDb ) { gainChanged( gainDb ); } );
        panB.onValueChanged = [&]( double value ) { panSliderChanged( value ); };
        // Record
        recordB.onStateChange = [&]() { recordButtonStateChanged( recordB.getToggleState() ); };
        // Monitor
        monitor.onClick              = [&]() { monitorStateChanged( monitor.getToggleState() ); };
        monitorSlider.onValueChanged = [&]( double Value ) { monitorLevelChanged( getMonitorLevelNormalized() ); };

        ieff.onMouseDown = [&]() {
            if( onInsertsClicked )
                onInsertsClicked();
        };

        // Theme
        themeChanged.connect( [&]() {
            gainMeter.setKnobImage( "Icon_Fader" );
            recordB.setImages( "Looper_rec", "Looper_rec_active" );
            monitor.setImages( "sampler_Monitor", "sampler_Monitor_active" );
        } );
    }

    void KickIOComponent::resized()
    {
        auto bounds = getLocalBounds();

        inputList.setBounds( bounds.removeFromTop( 64 ).reduced( 10 ) );
        auto left  = bounds.removeFromLeft( 180 ).reduced( 10 );
        auto right = bounds.reduced( 10 );
        ieff.setBounds( left.removeFromTop( 212 ) );

        op1[0]->setBounds( left.removeFromBottom( 28 ) );
        op1[1]->setBounds( left.removeFromBottom( 54 ) );

        right.removeFromBottom( 10 );
        gainMeter.setBounds( right.removeFromLeft( (int)right.proportionOfWidth( 0.64 ) ) );
        right.removeFromLeft( 5 );
        levelMeter.setBounds( right );
    }

    int KickIOComponent::getNumChannels()
    {
        return outputList.getSelectedIndex() + 1;
    }

    void KickIOComponent::signalStrengthChanged( const std::vector<float>& strength )
    {
        levelMeter.setSignalStrength( strength );
    }

    void KickIOComponent::setSignalStrength( float lrms, float rrms, float lpeak, float rpeak )
    {
        levelMeter.setSignalStrength( lrms, rrms, lpeak, rpeak );
    }

    void KickIOComponent::setRangeAndDisplayValues( const juce::Point<float>& range, const std::vector<float>& displayValues )
    {
        gainMeter.setRangeAndDisplayValues( range, displayValues );
        levelMeter.setRange( range.getX(), range.getY() );
    }

    void KickIOComponent::setInputs( const std::vector<juce::String>& input, int Selected )
    {
        inputList.setListOptions( input, Selected );
    }

    void KickIOComponent::setOutputs( const std::vector<juce::String>& output, int Selected )
    {
        outputList.setListOptions( output, Selected );
    }

    void KickIOComponent::setOptions1( const std::vector<juce::Component*>& options )
    {
        op1 = options;
        for( juce::Component* comp : options )
            addAndMakeVisible( *comp );
    }

    void KickIOComponent::setInsertsList( std::vector<juce::String>& InsertsList )
    {
        ieff.setInsertList( InsertsList );
    }

    void KickIOComponent::knobCommand( int knob, int value )
    {
        //if( knob == contracts::DeviceControlContracts::KnobSequence::EN_KNOB_4 )
        //    monitorSlider.knobRotated( value );
        //else
        if( knob == contracts::DeviceControlContracts::KnobSequence::EN_KNOB_5 )
            inputList.knobRotated( value );
    }

    void KickIOComponent::knobRotated( double deltaValue )
    {
        DBG( deltaValue );
        double value = getGain() + deltaValue;
        setGain( value );
    }

    void KickIOComponent::reset()
    {
        setInputs( {}, 0 );
        setOutputs( {}, 0 );

        for( juce::Component* comp : op1 )
            removeChildComponent( comp );

        op1.clear();
    }

    void KickIOComponent::visibilityChanged()
    {
        bool visible = isVisible();
        for( auto it : getChildren() )
            it->setVisible( visible );
    }

    /// <summary>
    ///
    /// </summary>

    KickInsertEffects::KickInsertEffects()
    {
        title.setColours( ui::Theme::textColour( 1 ), ui::Theme::backgroundColour( 9 ) );
        title.setValueJustification( juce::Justification::centred );
        title.setValueFontHeight( 20 );
        addAndMakeVisible( title );

        for( auto& it : insertLabels )
        {
            mt::utils::addAndMakeVisible( this, it );
            it.setValueColour( ui::Theme::textColour( 2 ) );
            it.setValueBackgroundColour( ui::Theme::backgroundColour( 13 ) );
            it.setValueFontHeight( 18 );
            it.setValueJustification( juce::Justification::centred );
            it.setText( "None", juce::dontSendNotification );
            it.setInterceptsMouseClicks( false, false );
        }
    }

    KickInsertEffects::~KickInsertEffects() {}

    void KickInsertEffects::setInsertList( std::vector<juce::String>& List )
    {
        inserts = List;

        for( int i = 0; i < 4; ++i )
        {
            if( i < (int)inserts.size() )
                insertLabels[i].setText( inserts[i].isNotEmpty() ? inserts[i] : "None", juce::dontSendNotification );
            else
                insertLabels[i].setText( "None", juce::dontSendNotification );
        }
    };

    void KickInsertEffects::paint( juce::Graphics& g )
    {
    }

    void KickInsertEffects::resized()
    {
        auto b = getLocalBounds();
        title.setBounds( b.removeFromTop( 20 ) );
        b.removeFromTop( 4 );
        for( auto& it : insertLabels )
        {
            it.setBounds( b.removeFromTop( 44 ) );
            b.removeFromTop( 4 );
        }
    }

    void KickInsertEffects::mouseDown( const juce::MouseEvent& Event )
    {
        if( !title.getBounds().contains( Event.getPosition() ) )
        {
            if( onMouseDown )
                onMouseDown();
        }
    }

}
