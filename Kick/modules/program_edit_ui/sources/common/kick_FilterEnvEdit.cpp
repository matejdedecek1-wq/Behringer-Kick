#include "kick_FilterEnvEdit.h"

namespace kick_ui
{
    FilterEnvEdit::FilterEnvEdit( std::vector<int>& Pads )
        : pads( Pads ), KickGrid( 4, 1 )
    {
        addComponent( filterLabel, 0, 0, 215 );
        addComponent( msLabel, 1, 0, 215 );
        addComponent( feOptions, 2, 0, 215 );
        addComponent( ampEnvOptions, 3, 0, 215 );

        addComponent( filterType, 0, 0, 130 );
        addComponent( filterCutoff, 0, 0, 130 );
        addComponent( filterReso, 0, 0, 130 );
        addComponent( filterEnv, 0, 0, 130 );

        addComponent( msVelStat, 1, 0, 130 );
        addComponent( msVelAtk, 1, 0, 130 );
        addComponent( msVelEnv, 1, 0, 130 );
        addComponent( msVelFlt, 1, 0, 130 );

        addComponent( feAttack, 2, 0, 130 );
        addComponent( feHold, 2, 0, 130 );
        addComponent( feDecay, 2, 0, 130 );
        addComponent( feSustain, 2, 0, 130 );
        addComponent( feAhdsrBtn, 2, 0, 130 );

        addComponent( ampAttack, 3, 0, 130 );
        addComponent( ampHold, 3, 0, 130 );
        addComponent( ampDecay, 3, 0, 130 );
        addComponent( ampSustain, 3, 0, 130 );
        addComponent( ampAhdsrBtn, 3, 0, 130 );

        ampAhdsrBtn.addListener( this );
        feAhdsrBtn.addListener( this );

        ui::themeChanged.connect( [&]() {
            feAhdsrBtn.setImages( "effect3" );
            ampAhdsrBtn.setImages( "effect3" );
        } );
    }

    FilterEnvEdit::~FilterEnvEdit()
    {
    }

    void FilterEnvEdit::buttonClicked( juce::Button* button )
    {
        KickEnvelopViewBase* dialog = nullptr;
        std::vector<int>     values;
        if( button == &feAhdsrBtn )
        {
            values.resize( 4 );
            values[0] = feAttack.getValue();
            values[1] = feHold.getValue();
            values[2] = feDecay.getValue();
            values[3] = feSustain.getValue();

            KickAHDSView* p = new KickAHDSView( values );
            dialog          = p;
        }
        else if( button == &ampAhdsrBtn )
        {
            values.resize( 4 );
            values[0] = ampAttack.getValue();
            values[1] = ampHold.getValue();
            values[2] = ampDecay.getValue();
            values[3] = ampSustain.getValue();

            KickAHDSView* p = new KickAHDSView( values );
            dialog          = p;
        }

        mt::utils::showDialog(
            "FILTER ENVELOPE", dialog, [this, dialog, button]( int /*r*/ ) {
                if( 1 /*r != UI::BUTTONS::CANCEL*/ )
                {
                    const std::vector<int>& initValues = dialog->getAHDSValues();
                    if( button == &feAhdsrBtn )
                    {
                        feAttack.setValue( initValues[0] );
                        feHold.setValue( initValues[1] );
                        feDecay.setValue( initValues[2] );
                        feSustain.setValue( initValues[3] );
                    }
                    else if( button == &ampAhdsrBtn )
                    {
                        ampAttack.setValue( initValues[0] );
                        ampHold.setValue( initValues[1] );
                        ampDecay.setValue( initValues[2] );
                        ampSustain.setValue( initValues[3] );
                    }
                }
                delete dialog;
            },
            dialog->getLocalBounds() );
    }

    void FilterEnvEdit::refresh()
    {
    }

    KickFilterEnvelope::KickFilterEnvelope( std::vector<int>& Keys, mt::MTKeygroupProgram*& PKeyGroup )
        : keys( Keys ), pKeyGroup( PKeyGroup ), ui::KickGrid( 3, 7 )
    {
        setRowTitles( { "AMPLITUDE", "FILTER", "PITCH" } );

        for( auto i = 0; i < 3; ++i )
        {
            //env[i].showTitle( true );
            env[i].setTitle( "TYPE" );
            env[i].setListOptions( { "AD", "ADSR", "AHD", "AHDS", "AHDSR" } );
            env[i].setJustificationType( juce::Justification::centred );

            a[i].setTitle( "ATTACK" );
            h[i].setTitle( "HOLD" );
            d[i].setTitle( "DECAY" );
            s[i].setTitle( "SUSTAIN" );
            dep[i].setTitle( "DEPTH" );

            a[i].showTitleAndValue( true, juce::Slider::TextBoxAbove, juce::Justification::centred );
            h[i].showTitleAndValue( true, juce::Slider::TextBoxAbove, juce::Justification::centred );
            d[i].showTitleAndValue( true, juce::Slider::TextBoxAbove, juce::Justification::centred );
            s[i].showTitleAndValue( true, juce::Slider::TextBoxAbove, juce::Justification::centred );
            dep[i].showTitleAndValue( true, juce::Slider::TextBoxAbove, juce::Justification::centred );

            a[i].setRange( 0.0, 127.0, 1.0 );
            h[i].setRange( 0.0, 127.0, 1.0 );
            d[i].setRange( 0.0, 127.0, 1.0 );
            s[i].setRange( 0.0, 127.0, 1.0 );
            dep[i].setRange( 0.0, 127.0, 0.01 );

            a[i].setDoubleClickReturnValue( true, 0, 0 );
            h[i].setDoubleClickReturnValue( true, 0, 0 );
            d[i].setDoubleClickReturnValue( true, 0, 0 );
            s[i].setDoubleClickReturnValue( true, 127, 0 );
            dep[i].setDoubleClickReturnValue( true, 0, 0 );

            addComponent( env[i], i, 1 );
            addComponent( a[i], i, 2 );
            addComponent( h[i], i, 3 );
            addComponent( d[i], i, 4 );
            addComponent( s[i], i, 5 );
            addComponent( dep[i], i, 6 );

            env[i].onSelectionChanged = [&, i]( int Index, const juce::String& Option ) {
                h[i].setVisible( Index >= 2 && Index <= 4 );
                s[i].setVisible( Index == 1 || Index == 3 || Index == 4 );
                dep[i].setVisible( Index == 1 || Index == 4 );
                if( pKeyGroup )
                    pKeyGroup->setEnvType( keys[0], toEnum<mt::ADSRType>( Index ), toEnum<mt::EnvelopeType>( i ) );
            };

            a[i].onValueChanged = [&, i]( juce::int64 Value ) {
                if( pKeyGroup )
                    pKeyGroup->setEnvelopeAttack( keys[0], toEnum<mt::EnvelopeType>( i ), Value );
            };

            h[i].onValueChanged = [&, i]( juce::int64 Value ) {
                if( pKeyGroup )
                    pKeyGroup->setEnvelopeHold( keys[0], toEnum<mt::EnvelopeType>( i ), Value );
            };

            d[i].onValueChanged = [&, i]( juce::int64 Value ) {
                if( pKeyGroup )
                    pKeyGroup->setEnvelopeDecay( keys[0], toEnum<mt::EnvelopeType>( i ), Value );
            };

            s[i].onValueChanged = [&, i]( juce::int64 Value ) {
                if( pKeyGroup )
                    pKeyGroup->setEnvelopeSustain( keys[0], toEnum<mt::EnvelopeType>( i ), Value );
            };

            dep[i].onValueChanged = [&, i]( juce::int64 Value ) {
                if( pKeyGroup )
                    pKeyGroup->setEnvelopeDepth( keys[0], toEnum<mt::EnvelopeType>( i ), Value );
            };
        }
    }

    void KickFilterEnvelope::refresh()
    {
        if( pKeyGroup && keys.size() )
            refreshPads();
    }

    void KickFilterEnvelope::refreshPads()
    {
        if( pKeyGroup && keys.size() )
        {
            for( auto index = 0; index < 3; ++index )
            {
                env[index].setSelected( pKeyGroup->getEnvType( keys[0], toEnum<mt::EnvelopeType>( index ) ), juce::sendNotification );
                a[index].setValue( pKeyGroup->getEnvelopeAttack( keys[0], toEnum<mt::EnvelopeType>( index ) ), juce::dontSendNotification );
                h[index].setValue( pKeyGroup->getEnvelopeHold( keys[0], toEnum<mt::EnvelopeType>( index ) ), juce::dontSendNotification );
                d[index].setValue( pKeyGroup->getEnvelopeDecay( keys[0], toEnum<mt::EnvelopeType>( index ) ), juce::dontSendNotification );
                s[index].setValue( pKeyGroup->getEnvelopeSustain( keys[0], toEnum<mt::EnvelopeType>( index ) ), juce::dontSendNotification );
                dep[index].setValue( pKeyGroup->getEnvelopeDepth( keys[0], toEnum<mt::EnvelopeType>( index ) ), juce::dontSendNotification );
            }
        }
    }

    void KickFilterEnvelope::visibilityChanged()
    {
        auto visible = isVisible();
        if( visible )
            refresh();
    }

    void KickFilterEnvelope::handleCommand( CPktMgr::HWMsg_t& f_HwMsg )
    {
        if( contracts::DclContractsHelper::isPadButtonMessage( f_HwMsg ) )
        {
            auto padNum = contracts::DclContractsHelper::getPadNumber( f_HwMsg );
            keys[0]     = padNum + mt::getAudioInterface().getBankOffset();
            refresh();
        }
    }
}
