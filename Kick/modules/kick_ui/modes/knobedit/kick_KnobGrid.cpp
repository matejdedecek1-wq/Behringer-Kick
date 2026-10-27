#include "kick_KnobGrid.h"
namespace kick_ui
{

#define qlinkGridHorSpacing 7
#define qlinkGridVerSpacing 16
#define qlinkTotalGridKnobs 16
#define qlinkTotalRows      4
#define qlinkTotalColumns   4
#define STEPDIV             10

    namespace knobgrid
    {
        constexpr int knobWidth  = 70;
        constexpr int knobHeight = 80;
        constexpr int startX     = 20;
        constexpr int startY     = 74;
    }

    KnobGrid::KnobGrid()
    {
        mt::utils::addAndMakeVisible( this, label, knobdata );
        mt::utils::makeHidden( knobdata );

        for( int i = 0; i < qlinkTotalGridKnobs; i++ )
        {
            std::unique_ptr<ui::KickRotary>& ptr = qlinkKnobs.emplace_back( std::make_unique<ui::KickRotary>( "OFF" ) );
            ptr->showTitleAndValue( true, juce::Slider::TextBoxBelow, juce::Justification::centred );
            ptr->setDisplayProportions( .6f, .2f, .2f );
            ptr->addMouseListener( this, true );
            ptr->setRange( 0, 1, 0 );
            ptr->setFocus( true );
            addAndMakeVisible( ptr.get() );
            ptr->onValueChanged = [this, i]( double value ) {
                auto data = juce::String( "QLINK - " ) + juce::String( i + 1 );
                knobdata.setText( data, juce::dontSendNotification );
                if( onknobSelection )
                    onknobSelection( i );

                if( !initialized )
                    return;
            };
        }
        label.setValueJustification( juce::Justification::centredLeft );
        label.setValueBackgroundColour( ui::Theme::backgroundColour( 9 ) );
        label.setValueColour( ui::Theme::textColour( 1 ) );
        knobdata.setFontHeight( 20 );
        knobdata.setValueJustification( juce::Justification::centredLeft );
    }
    KnobGrid::~KnobGrid()
    {
    }

    void KnobGrid::paint( juce::Graphics& g )
    {
        mt::utils::fillRoundedRect( g, { 0, 0, 342, 460 }, ui::Theme::backgroundColour( 4 ) );

        int y      = knobgrid::startY;
        int width  = ( knobgrid::knobWidth * qlinkTotalColumns ) + ( qlinkGridHorSpacing * ( qlinkTotalColumns - 1 ) );
        int height = knobgrid::knobHeight;

        if( state >= 1 && state <= qlinkTotalRows )
            y = knobgrid::startY + ( state - 1 ) * ( knobgrid::knobHeight + qlinkGridVerSpacing );

        if( !knob.getMode() )
            mt::utils::renderRoundedRect( g, { knobgrid::startX, y, width, height }, ui::Theme::backgroundColour( 6 ) );
    }

    void KnobGrid::setInfoText2( juce::String txt )
    {
        infoText = txt;
        label.setText( infoText, juce::dontSendNotification );
    }

    void KnobGrid::labelBounds( bool isScreenClicked )
    {
        knobdata.setVisible( isScreenClicked );
        screenClicked = !isScreenClicked;
        resized();
    }

    void KnobGrid::resized()
    {
        knobdata.setBoundsRelative( .4883, .0217f, .4824f, .0956f );
        if( screenClicked )
            label.setBoundsRelative( .0584f, .0543f, .883f, .0434f );
        else
            label.setBoundsRelative( .0584f, .0543f, .3128f, .0434f );

        int x = knobgrid::startX;
        int y = knobgrid::startY;
        for( unsigned int i = 0; i < qlinkTotalRows; i++ )
        {
            for( unsigned int j = 0; j < qlinkTotalColumns; j++ )
            {
                std::unique_ptr<ui::KickRotary>& qlinkK = qlinkKnobs[i * qlinkTotalColumns + j];
                qlinkK->setBounds( x, y, knobgrid::knobWidth, knobgrid::knobHeight );
                x += knobgrid::knobWidth + qlinkGridHorSpacing;
            }
            y += knobgrid::knobHeight + qlinkGridVerSpacing;
            x = knobgrid::startX;
        }
    }

    void KnobGrid::labelInfo( int knobnum, juce::String name )
    {
        qlinkKnobs[knobnum]->setTitle( name );
        qlinkKnobs[knobnum]->repaint();
    }

    void KnobGrid::setPadState( bool padState )
    {
        isPadState = padState;
    }

    void KnobGrid::setPadIndex( int bankIndex, int padIndex )
    {
        bankNum = bankIndex;
        padNum  = padIndex;
    }

    void KnobGrid::setIndex( int index )
    {
        indexNum = index;
    }

    int KnobGrid::getState( int knobNum )
    {
        return ( ( knobNum - 1 ) / qlinkTotalColumns ) + 1;
    }

    void KnobGrid::knobCommand( int Knob, int value )
    {
        state = getState( Knob );
        setValues( Knob - 1, value );
        repaint();
    }

    void KnobGrid::setValues( int knobnum, int value )
    {
        juce::String title    = qlinkKnobs[knobnum]->getTitle();
        auto&        ae       = mt::getAudioInterface();
        auto         pProg    = ae.getProgramDataForTrack( ae.getSelectedSequence(), indexNum );
        auto         newValue = value;

        if( pProg )
        {
            auto  program = ae.getProgram( pProg->name.get() );
            auto* inserts = program->getInserts();

            for( int insertIndex = 0; insertIndex < 4; ++insertIndex )
            {
                auto insertData = program->getInsertsInfo( insertIndex );
                auto index      = 0;
                for( const auto& param : insertData.params )
                {
                    if( title == param.name )
                    {
                        auto value1     = (float)value / STEPDIV;
                        auto paramValue = program->getParameterValue( insertIndex, index );
                        auto paramVal   = paramValue + value1;
                        paramVal        = juce::jlimit( 0.f, 1.f, paramVal );
                        newValue        = paramVal;
                        qlinkKnobs[knobnum]->setRange( GAIN::MIN, GAIN::MAX, 0.01f );
                        program->setParameterValue( insertIndex, index, paramVal );
                    }
                    index++;
                }
            }
            if( program->type.get() == mt::MTProgramType::Midi )
            {
                qlinkKnobs[knobnum]->setRange( MIDI::MIN, MIDI::MAX, MIDI::STEP );
                if( parameterSelection )
                {
                    auto updatedValue = parameterSelection( title, value );
                    qlinkKnobs[knobnum]->setValue( updatedValue );
                }
                return;
            }

            if( program->type.get() == mt::MTProgramType::Drum || program->type.get() == mt::MTProgramType::Clip || program->type.get() == mt::MTProgramType::Keygroup )
            {
                if( isPad )
                {
                    if( title == "Pan" )
                    {
                        newValue = program->getNotePan( padNum + bankNum * 16 ) + value;
                        qlinkKnobs[knobnum]->setRange( PAN::MIN, PAN::MAX, PAN::STEP );
                        program->setNotePan( padNum, newValue );
                    }
                    else if( title == "Level" )
                    {
                        newValue = program->getNoteGain( padNum ) + value;
                        qlinkKnobs[knobnum]->setRange( MIDI::MIN, MIDI::LEVEL_DEFAULT, MIDI::STEP );
                        program->setNoteGain( padNum, newValue );
                    }
                    else if( title == "SemiTune" )
                    {
                        newValue = program->getNoteSemi( padNum ) + value;
                        qlinkKnobs[knobnum]->setRange( MIDI::MIN, MIDI::LEVEL_DEFAULT, MIDI::STEP );
                        program->setNoteSemi( padNum, newValue );
                    }
                    else if( title == "FineTune" )
                    {
                        newValue = program->getNoteFine( padNum ) + value;
                        qlinkKnobs[knobnum]->setRange( MIDI::MIN, MIDI::LEVEL_DEFAULT, MIDI::STEP );
                        program->setNoteFine( padNum, newValue );
                    }

                    else if( title == "PadSolo" )
                    {
                        ( value ) ? program->setNoteSolo( padNum, true ) : program->setNoteSolo( padNum, false );
                    }
                    else if( title == "PadMute" )
                    {
                        ( value ) ? program->setNoteMute( padNum, true ) : program->setNoteMute( padNum, false );
                    }
                }
                else
                {
                    if( title == "Pan" )
                    {
                        newValue = program->getPan() + value;
                        qlinkKnobs[knobnum]->setRange( PAN::MIN, PAN::MAX, PAN::STEP );
                        program->setPan( newValue );
                    }
                    else if( title == "Level" )
                    {
                        newValue = program->getLevel() + value;
                        qlinkKnobs[knobnum]->setRange( MIDI::MIN, MIDI::LEVEL_DEFAULT, MIDI::STEP );
                        program->setLevel( newValue );
                    }
                    else if( title == "Velocity" )
                    {
                        newValue = program->getLevel() + value;
                        qlinkKnobs[knobnum]->setRange( MIDI::MIN, MIDI::LEVEL_DEFAULT, MIDI::STEP );
                        program->setLevel( newValue );
                    }
                    else if( title == "Fine" || title == "GlobalFineTune" )
                    {
                        newValue = program->getFine() + value;
                        qlinkKnobs[knobnum]->setRange( FINE::MIN, FINE::MAX, FINE::STEP );
                        program->setFine( newValue );
                    }
                    else if( title == "Poly" )
                    {
                        newValue = program->getPolyphony() + value;
                        qlinkKnobs[knobnum]->setRange( POLYPHONY::MIN, POLYPHONY::MAX, POLYPHONY::STEP );
                        program->setPolyphony( newValue );
                    }
                    else if( title == "Semi" || title == "GlobalSemiTune" )
                    {
                        newValue = program->getSemi() + value;
                        qlinkKnobs[knobnum]->setRange( SEMI::MIN, SEMI::MAX, SEMI::STEP );
                        program->setSemi( newValue );
                    }
                    else if( title == "Mute" )
                    {
                        ( value ) ? program->setMute( true ) : program->setMute( false );
                    }
                    else if( title == "Solo" )
                    {
                        ( value ) ? program->setSolo( true ) : program->setSolo( false );
                    }
                }
            }
        }
        if( title == "OFF" ) return;

        qlinkKnobs[knobnum]->setValue( newValue );

        for( int i = 0; i < qlinkKnobs.size(); ++i )
        {
            if( qlinkKnobs[i]->getTitle() == title )
            {
                qlinkKnobs[i]->setValue( newValue );
            }
        }
    }
#undef qlinkGridHorSpacing
#undef qlinkGridVerSpacing
#undef qlinkTotalGridKnobs
#undef qlinkTotalRows
#undef qlinkTotalColumns
#undef STEPDIV

}
