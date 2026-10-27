#include "ADSR.h"

namespace AudIO
{
    ADSRObj::ADSRObj()
        : Object( 1, 0, ADSRObj::kParamMax )
    {
        recalculateCoeffs();
    }

    void ADSRObj::prepareToPlay( uint32_t samplerate, uint32_t blocksize )
    {
        Object::prepareToPlay( samplerate, blocksize );
        recalculateCoeffs();
    }

    void ADSRObj::setParameter( int index, float value )
    {
        switch( index )
        {
            case kParamBypass:
                bypass = value >= 0.5 ? true : false;
                break;
            case kParamAttackTimeSec:
                setAttackTime( value );
                break;
            case kParamDecayTimeSec:
                setDecayTime( value );
                break;
            case kParamSustainLevel:
                setSustainLevel( value );
                break;
            case kParamReleaseTimeSec:
                setReleaseTime( value );
                break;
            case kParamGate:
            {
                if( value == 1.0 )
                    gateOn();
                else if( value == 0.0 )
                    gateOff();
            }
            break;
            default:
                break; // unknown parameter
        }
    }

    float ADSRObj::getParameter( int index )
    {
        switch( index )
        {
            case kParamBypass: return bypass;
            case kParamAttackTimeSec: return attack;
            case kParamDecayTimeSec: return decay;
            case kParamSustainLevel: return sustain;
            case kParamReleaseTimeSec: return release;
            default: break;
        }
        return 0.0;
    }

    const char* ADSRObj::getType() const
    {
        return "ADSRObj";
    }

    void ADSRObj::getParameterLabel( int index, char* text )
    {
        switch( index )
        {
            case kParamAttackTimeSec:
            case kParamDecayTimeSec:
            case kParamReleaseTimeSec:
                if( text )
                    strcpy( text, "Sec" );
                break;
            default:
                break;
        }
        if( text )
            strcpy( text, utils::emptyString );
    }

    void ADSRObj::getParameterName( int index, char* text )
    {
        if( text )
        {
            switch( index )
            {
                case kParamBypass: strcpy( text, "Bypass" ); break;
                case kParamAttackTimeSec: strcpy( text, "Attack" ); break;
                case kParamDecayTimeSec: strcpy( text, "Decay" ); break;
                case kParamSustainLevel: strcpy( text, "Sustain" ); break;
                case kParamReleaseTimeSec: strcpy( text, "Release" ); break;
                case kParamGate: strcpy( text, "Gate" ); break;
                default: break;
            }
            strcpy( text, utils::emptyString );
        }
    }

    float ADSRObj::getMin( uint32_t index ) const
    {
        switch( index )
        {
            case kParamBypass: return 0.0;
            case kParamAttackTimeSec: return 0.0;
            case kParamDecayTimeSec: return 0.0;
            case kParamSustainLevel: return 0.0;
            case kParamReleaseTimeSec: return 0.0;
            case kParamGate: return 0.0;
            default: break; // unknown parameter
        }
        return 0.0;
    }

    float ADSRObj::getMax( uint32_t index ) const
    {
        switch( index )
        {
            case kParamBypass: return 0.0;
            case kParamAttackTimeSec: return 10.0;
            case kParamDecayTimeSec: return 10.0;
            case kParamSustainLevel: return 1.0;
            case kParamReleaseTimeSec: return 10.0;
            case kParamGate: return 1.0;
            default: break; // unknown parameter
        }
        return 0.0;
    }

    void ADSRObj::setAttackTime( double time )
    {
        attack = (float)time;
        recalulateAttack();
    }

    void ADSRObj::setDecayTime( double time )
    {
        decay = (float)time;
        recalulateDecay();
    }

    void ADSRObj::setReleaseTime( double timeMs )
    {
        release = (float)timeMs;
        recalulateRelease();
    }

    void ADSRObj::recalulateAttack()
    {
        attackCoef = getRate( 1.0, attack );
        if( State::attack == state && attackCoef <= 0.0f )
        {
            goToNextState();
        }
    }

    void ADSRObj::recalulateDecay()
    {
        decayCoef = getRate( 1.0f - sustain, decay );
        if( State::decay == state && decayCoef <= 0.0f )
        {
            goToNextState();
        }
    }

    void ADSRObj::recalulateRelease()
    {
        releaseCoef = getRate( sustain, release );
        if( State::release == state && releaseCoef <= 0.0f )
        {
            goToNextState();
        }
    }

    void ADSRObj::setSustainLevel( double level )
    {
        sustain = (float)level;
        recalulateDecay();
        recalulateRelease();
    }

    inline void ADSRObj::gateOn()
    {
        if( attackCoef > 0.0f )
        {
            state = State::attack;
        }
        else if( decayCoef > 0.0f )
        {
            output = 1.0f;
            state  = State::decay;
        }
        else
        {
            output = sustain;
            state  = State::sustain;
        }
    }

    inline void ADSRObj::gateOff()
    {
        if( state != State::idle )
        {
            if( release > 0.0f )
            {
                releaseCoef = (float)( output / ( release * sampleRate() ) );
                state       = State::release;
            }
            else
            {
                reset();
            }
        }
    }

    void ADSRObj::setBypass( bool Bypass )
    {
        bypass = Bypass;
    }

    void ADSRObj::process( AudIO::Buffer<float>& buffer )
    {
        if( bypass.load() )
        {
            buffer.copyInputsToOutputs();
            return;
        }

        auto pIns  = buffer.getInputs();
        auto pOuts = buffer.getOutputs();

        for( uint32_t sample = 0; sample < blockSize(); ++sample )
        {
            auto multiplier = processADSR();
            for( uint32_t channel = 0; channel < buffer.getNumInputChannels(); channel++ )
            {
                pOuts[channel][sample] = pIns[channel][sample] * multiplier;
            }
        }
    }

    inline float ADSRObj::processADSR()
    {
        if( bypass )
            return 1.f;

        switch( state )
        {
            case State::idle:
                return 0.f;
                break;
            case State::attack:
                output += attackCoef;
                if( output >= 1.0 )
                {
                    output = 1.0;
                    goToNextState();
                }
                break;
            case State::decay:
                output -= decayCoef;
                if( output <= sustain )
                {
                    output = sustain;
                    goToNextState();
                }
                break;
            case State::sustain:
                output = sustain;
                break;
            case State::release:
            {
                output -= releaseCoef;
                if( output <= 0.0 )
                {
                    goToNextState();
                }
            }
            break;
        }
        return output;
    }

    inline float ADSRObj::getRate( float distance, float timeInSeconds ) const
    {
        return timeInSeconds > 0.0f ? (float)( distance / ( timeInSeconds * sampleRate() ) ) : -1.0f;
    }

    inline void ADSRObj::goToNextState()
    {
        if( state == State::attack )
        {
            state = ( decayCoef > 0.0f ? State::decay : State::sustain );
        }
        else if( state == State::decay )
        {
            state = State::sustain;
        }
        else if( state == State::release )
        {
            reset();
        }
    }

    inline void ADSRObj::reset()
    {
        state  = State::idle;
        output = 0.0;
    }

    inline void ADSRObj::recalculateCoeffs()
    {
        recalulateAttack();
        recalulateDecay();
        recalulateRelease();
    }
};
