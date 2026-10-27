#include "DawTime.h"
#include "../Includes/Utils.h"

namespace AudIO
{
    Tempo::Tempo( double TempoValue, double Ramp ) noexcept
        : bpm( snap( TempoValue ) ), bpmRampDuration( Ramp ) {}

    double Tempo::snap( double Value ) const noexcept
    {
        return utils::clamp( Value, minTempo, maxTempo );
    }

    bool Tempo::operator==( const Tempo& other ) const noexcept
    {
        return utils::isEqual( bpm, other.bpm );
    }

    bool Tempo::operator!=( const Tempo& other ) const noexcept
    {
        return !operator==( other );
    }

    bool Tempo::operator<( const Tempo& other ) const noexcept
    {
        return bpm < other.bpm;
    }

    bool Tempo::operator<=( const Tempo& other ) const noexcept
    {
        return bpm <= other.bpm;
    }

    bool Tempo::operator>( const Tempo& other ) const noexcept
    {
        return bpm > other.bpm;
    }

    bool Tempo::operator>=( const Tempo& other ) const noexcept
    {
        return bpm >= other.bpm;
    }

    Tempo Tempo::operator+( const Tempo& other ) const noexcept
    {
        return Tempo( bpm + other.bpm );
    }

    Tempo Tempo::operator-( const Tempo& other ) const noexcept
    {
        return Tempo( bpm - other.bpm );
    }

    Tempo Tempo::operator*( const Tempo& other ) const noexcept
    {
        return Tempo( bpm * other.bpm );
    }

    Tempo Tempo::operator*( double multiplier ) const noexcept
    {
        return Tempo( bpm * multiplier );
    }

    Tempo Tempo::operator/( double divisor ) const noexcept
    {
        return Tempo( bpm / divisor );
    }

    Tempo& Tempo::operator+=( const Tempo& other ) noexcept
    {
        bpm = snap( bpm + other.bpm );
        return *this;
    }

    Tempo& Tempo::operator-=( const Tempo& other ) noexcept
    {
        bpm = snap( bpm - other.bpm );
        return *this;
    }

    Tempo& Tempo::operator*=( const Tempo& other ) noexcept
    {
        bpm = snap( bpm * other.bpm );
        return *this;
    }

    Tempo& Tempo::operator*=( double multiplier ) noexcept
    {
        bpm = snap( bpm * multiplier );
        return *this;
    }

    Tempo& Tempo::operator/=( double divisor ) noexcept
    {
        bpm = snap( bpm / divisor );
        return *this;
    }

    //
    // TimeSignature...
    //
    TimeSignature::TimeSignature( int Num, int Den ) noexcept
    {
        withNumerator( Num ).withDenominator( Den );
    }

    // Comparators...
    bool TimeSignature::operator==( const TimeSignature& TS ) const noexcept
    {
        return numerator == TS.numerator && denominator == TS.denominator;
    }

    bool TimeSignature::operator!=( const TimeSignature& TS ) const noexcept
    {
        return !operator==( TS );
    }

    bool TimeSignature::operator<( const TimeSignature& TS ) const noexcept
    {
        return numerator < TS.numerator && denominator < TS.denominator;
        ;
    }

    bool TimeSignature::operator<=( const TimeSignature& TS ) const noexcept
    {
        return numerator <= TS.numerator && denominator <= TS.denominator;
        ;
    }

    bool TimeSignature::operator>( const TimeSignature& TS ) const noexcept
    {
        return numerator > TS.numerator && denominator > TS.denominator;
        ;
    }

    bool TimeSignature::operator>=( const TimeSignature& TS ) const noexcept
    {
        return numerator >= TS.numerator && denominator >= TS.denominator;
        ;
    }

    // Helpers...
    TimeSignature& TimeSignature::withNumerator( const int Num )
    {
        numerator = utils::clamp( Num, minNumerator, maxNumerator );
        return *this;
    }

    TimeSignature& TimeSignature::withDenominator( const int Den )
    {
        if( !utils::isPowerOfTwo( Den ) )
            denominator = utils::previousPowerOfTwo( Den );
        denominator = utils::clamp( denominator, minDenominator, maxDenominator );
        return *this;
    }

    TimeSignature& TimeSignature::withPPQN( const int PPQN )
    {
        ppqn = utils::clamp( PPQN, minPPQN, maxPPQN );
        return *this;
    }

    std::string TimeSignature::toString() const noexcept
    {
        return std::to_string( numerator ) + "/" + std::to_string( denominator );
    }

    //TimeSignature TimeSignature::fromString( const std::string& ){ return {};}

    double TimeSignature::getNumQuarterNotesPerBar() const noexcept
    {
        return ( 4.0 / (double)numerator * (double)denominator );
    }

    double TimeSignature::getNumSecondsPerBar( const Tempo& T ) const noexcept
    {
        return secondsPerBar( T, numerator, denominator );
    }

    //
    // BarBeats ...
    //

    BarBeats::BarBeats( int Bar, int Beat, int Tick ) noexcept
        : bar( Bar ), beat( Beat ), tick( Tick ){};

    //==============================================================================
    // Comparators...
    bool BarBeats::operator==( const BarBeats& Other ) const noexcept
    {
        return ( bar == Other.bar ) && ( beat == Other.beat ) && ( tick == Other.tick );
    }

    bool BarBeats::operator!=( const BarBeats& Other ) const noexcept
    {
        return !operator==( Other );
    }

    //==============================================================================
    bool BarBeats::isValid() const
    {
        return ( -1 < bar ) && ( -1 < beat ) && ( -1 < tick );
    }

    void BarBeats::clear()
    {
        bar = beat = tick = 0;
    }

    std::string BarBeats::toString() const
    {
        return utils::string_format( "%3d:%2d:%3d", bar, beat, tick );
    }

    int BarBeats::toTicks( int numerator, int denominator, int ppqn ) const
    {
        auto tpb      = ticksPerBeat( denominator, ppqn );
        auto numTicks = ( beat * tpb );
        numTicks += ( numerator * tpb );
        numTicks += tick;
        return (int)numTicks;
    }

    double BarBeats::toSeconds( double tempo, int denominator, int ppqn ) const
    {
        auto ticks = toTicks( 0, denominator, ppqn );
        return ticksToSeconds( ticks, tempo, denominator, ppqn );
    };

    BarBeats BarBeats::fromTicks( unsigned int Ticks, int numerator, int denominator, int ppqn )
    {
        auto tpm  = ticksPerBar( numerator, denominator, ppqn );
        auto tpb  = ticksPerBeat( denominator, ppqn );
        auto tick = Ticks;

        auto bar = (int)( tick / tpm );
        tick -= (unsigned int)( bar * tpm );
        auto beat = (int)( tick / tpb );
        tick -= (unsigned int)( beat * tpb );
        return { bar, beat, (int)tick };
    }

    BarBeats BarBeats::fromSeconds( double Seconds, double Tempo, int Numerator, int Denominator, int PPQN )
    {
        auto ticks = (unsigned int)secondsToTicks( Seconds, Tempo, Denominator, PPQN );
        return fromTicks( ticks, Numerator, Denominator, PPQN );
    }

    BarBeats BarBeats::fromSeconds( double Seconds, const Tempo& tempo, const TimeSignature& ts, int ppqn )
    {
        auto ticks = (unsigned int)secondsToTicks( Seconds, tempo, ts.denominator, ppqn );
        return fromTicks( ticks, ts.numerator, ts.denominator, ppqn );
    }

};
