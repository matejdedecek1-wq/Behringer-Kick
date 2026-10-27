#pragma once

#include <stdint.h>
#include <string>
#include <atomic>
#include "../Includes/Utils.h"

namespace AudIO
{
    //
    // Time Constants.
    static constexpr int secondsPerMinute = 60;
    static constexpr int minutesPerHour   = 60;
    static constexpr int hoursPerDay      = 24;

    static constexpr int msPerSecond = 1000;
    static constexpr int msPerMinute = ( msPerSecond * secondsPerMinute );

    static constexpr int secondsPerHour = ( secondsPerMinute * minutesPerHour );
    static constexpr int minutesPerDay  = ( minutesPerHour * hoursPerDay );
    static constexpr int secondsPerDay  = ( secondsPerHour * hoursPerDay );

    struct BarBeats;
    struct TimeSignature;
    struct Tempo;

    enum class TimeBase
    { //! DO NOT CHANGE VALUES OF TIMEBASE...
        _1   = 1,
        _2   = 2,
        _2T  = 3,
        _4   = 4,
        _4T  = 6,
        _8   = 8,
        _8T  = 12,
        _16  = 16,
        _16T = 24,
        _32  = 32,
        _32T = 48,
        _64  = 64,
        _64T = 96,
    };

    const static std::vector<std::string> TimeBaseStrings{ "1", "2", "2T", "4", "4T", "8", "8T", "16", "16T", "32", "32T" };
    const static std::vector<float>       TimeBaseMultiplier{ 4.f, 2.f, 1.33333334f, 1.f, 0.66666667f, 0.5f, 0.3333333334f, 0.25f, 0.1666666667f, 0.125f, 0.0833333334f, 0.0625f, 0.0416666667f };

    constexpr double determineTempo( double secondsPerQuarterNote ) noexcept
    {
        return secondsPerQuarterNote > 0.0 ? ( secondsPerMinute / secondsPerQuarterNote ) : 0.0;
    }

    constexpr double secondsPerTempo( double tempo ) noexcept { return tempo > 0.0 ? ( secondsPerMinute / tempo ) : 0.0; }
    constexpr double msPerTempo( double tempo ) noexcept { return tempo > 0.0 ? ( msPerMinute / tempo ) : 0.0; }

    constexpr double secondsPerBeat( double tempo, int denominator ) noexcept { return tempo > 0.0 ? ( 240.0 / ( denominator * tempo ) ) : 0.0; }
    constexpr double msPerBeat( double tempo, int denominator ) noexcept { return msPerSecond * secondsPerBeat( tempo, denominator ); }

    constexpr double secondsPerBar( double tempo, int numerator, int denominator ) noexcept { return numerator * secondsPerBeat( tempo, denominator ); }
    constexpr double msPerBar( double tempo, int numerator, int denominator ) noexcept { return numerator * msPerBeat( tempo, denominator ); }

    constexpr double secondsToQuarterNotes( double seconds, double tempo ) noexcept
    {
        return ( seconds >= 0.0 && tempo >= 0.0 ) ? ( seconds / msPerTempo( tempo ) ) : 0.0;
    }

    constexpr double ticksPerBeat( int denominator, int ppq ) noexcept { return ( ( (double)ppq * 4.0 ) / (double)denominator ); }
    constexpr double ticksPerBar( int numerator, int denominator, int ppq ) noexcept { return ( numerator * ticksPerBeat( denominator, ppq ) ); }
    constexpr double ticksPerSecond( double tempo, int denominator, int ppqn ) noexcept { return ( ( tempo * ppqn ) * ( 15 * denominator ) ); }
    constexpr double ticksPerMilliSecond( double tempo, int denominator, int ppq ) noexcept { return ( ppq * tempo ) / ( 15000 * denominator ); }

    constexpr double msPerTick( double tempo, int denominator, int ppq ) noexcept { return ( 15000 * denominator ) / ( ppq * tempo ); }
    constexpr double secondsPerTick( double tempo, int denominator, int ppq ) noexcept { return ( 15 * denominator ) / ( ppq * tempo ); }

    constexpr double msToTicks( double timeInMs, double tempo, int denominator, int ppq ) noexcept { return msPerTick( tempo, denominator, ppq ) * timeInMs; }
    constexpr double secondsToTicks( double timeInSec, double tempo, int denominator, int ppq ) noexcept { return secondsPerTick( tempo, denominator, ppq ) * timeInSec; }

    constexpr double ticksToMs( double ticks, double tempo, int denominator, int ppq ) noexcept { return msPerTick( tempo, denominator, ppq ) * ticks; }
    constexpr double ticksToSeconds( double ticks, double tempo, int denominator, int ppq ) noexcept { return secondsPerTick( tempo, denominator, ppq ) * ticks; }

    struct Tempo
    {
        // Constants...(in beats per minute)
        static constexpr double minTempo     = 20.0;
        static constexpr double defaultTempo = 120.0;
        static constexpr double maxTempo     = 300.0;

        // Constant multipliers...
        static constexpr double perNote             = 4.0;
        static constexpr double perHalfNote         = 2.0;
        static constexpr double perQuarterNote      = 1.0;
        static constexpr double perEighthNote       = 0.5;
        static constexpr double perSixteenthNote    = 0.25;
        static constexpr double perThirtySecondNote = 0.125;
        static constexpr double perSixtyForthNote   = 0.0625;

        double bpm{ defaultTempo };
        double bpmRampDuration{ 0 };

        Tempo() noexcept  = default;
        ~Tempo() noexcept = default;
        explicit Tempo( double TempoValue, double Ramp = 0 ) noexcept;

        // Copy and Move constructors...
        Tempo( const Tempo& ) noexcept            = default;
        Tempo( Tempo&& ) noexcept                 = default;
        Tempo& operator=( const Tempo& ) noexcept = default;
        Tempo& operator=( Tempo&& ) noexcept      = default;

        // Getters...
        constexpr operator double() const noexcept { return bpm; }

        // Arithmetic operations...
        Tempo  operator+( const Tempo& ) const noexcept;
        Tempo& operator+=( const Tempo& ) noexcept;
        Tempo  operator-( const Tempo& ) const noexcept;
        Tempo& operator-=( const Tempo& ) noexcept;
        Tempo  operator*( const Tempo& ) const noexcept;
        Tempo& operator*=( const Tempo& ) noexcept;
        Tempo  operator*( double multiplier ) const noexcept;
        Tempo& operator*=( double multiplier ) noexcept;
        Tempo  operator/( double divisor ) const noexcept;
        Tempo& operator/=( double divisor ) noexcept;

        // Comparators...
        bool operator==( const Tempo& ) const noexcept;
        bool operator!=( const Tempo& ) const noexcept;
        bool operator<( const Tempo& ) const noexcept;
        bool operator<=( const Tempo& ) const noexcept;
        bool operator>( const Tempo& ) const noexcept;
        bool operator>=( const Tempo& ) const noexcept;

        inline double beatsPerSecond() const noexcept { return ( bpm / secondsPerMinute ); }
        inline double secondsPerBeat() const noexcept { return ( secondsPerMinute / bpm ); }
        inline double msPerBeat() const noexcept { return ( msPerMinute / bpm ); }

        inline double msPerNote() const noexcept { return msPerBeat() * perNote; };
        inline double msPerHalfNote() const noexcept { return msPerBeat() * perHalfNote; };
        inline double msPerQuarterNote() const noexcept { return msPerBeat(); };
        inline double msPerEigthNote() const noexcept { return msPerBeat() * perEighthNote; };
        inline double msPerSixteenthNote() const noexcept { return msPerBeat() * perSixteenthNote; };
        inline double msPerThirtySecondNote() const noexcept { return msPerBeat() * perThirtySecondNote; };
        inline double msPerSixtyFourthNote() const noexcept { return msPerBeat() * perSixtyForthNote; };

        double snap( double Value ) const noexcept;
    };

    struct TimeSignature
    {
        // Numerators and denominators in beats.
        static constexpr int defaultNumerator   = 4;
        static constexpr int minNumerator       = 1;
        static constexpr int maxNumerator       = 16;
        static constexpr int defaultDenominator = 4;
        static constexpr int minDenominator     = 4;
        static constexpr int maxDenominator     = 32;

        static constexpr int minPPQN     = 16;
        static constexpr int defaultPPQN = 96;
        static constexpr int maxPPQN     = 960;

        static constexpr TimeBase defaultTimebase = TimeBase::_4;
        static constexpr TimeBase defaultQuantize = TimeBase::_16;

        int numerator{ defaultNumerator };
        int denominator{ defaultDenominator };
        int ppqn{ defaultPPQN };

        TimeBase quantTimebase{ defaultTimebase };

        TimeSignature() noexcept = default;
        TimeSignature( int Num, int Den ) noexcept;
        ~TimeSignature() noexcept = default;

        // Copy and Move constructors...
        TimeSignature& operator=( const TimeSignature& ) noexcept = default;
        TimeSignature& operator=( TimeSignature&& ) noexcept      = default;
        TimeSignature( const TimeSignature& ) noexcept            = default;
        TimeSignature( TimeSignature&& ) noexcept                 = default;

        // Comparators...
        bool operator==( const TimeSignature& ) const noexcept;
        bool operator!=( const TimeSignature& ) const noexcept;
        bool operator<( const TimeSignature& ) const noexcept;
        bool operator<=( const TimeSignature& ) const noexcept;
        bool operator>( const TimeSignature& ) const noexcept;
        bool operator>=( const TimeSignature& ) const noexcept;

        // Other functions...
        TimeSignature& withNumerator( const int Num );
        TimeSignature& withDenominator( const int Den );
        TimeSignature& withPPQN( const int PPQN );

        std::string toString() const noexcept;
        //static TimeSignature fromString( const std::string& S );

        inline int      getNumerator() const { return numerator; }
        inline int      getDenominator() const { return denominator; }
        inline int      getPPQN() const { return ppqn; }
        inline TimeBase getQuantizationTimebase() const { return quantTimebase; }
        inline void     setQuantizationTimebase( TimeBase Timebase ) { quantTimebase = Timebase; }

        double getNumQuarterNotesPerBar() const noexcept;
        double getNumSecondsPerBar( const Tempo& ) const noexcept;
        double getNumSecondsPerBeat( const Tempo& ) const noexcept;

        inline double getTicksPerSecond( const Tempo& ) const noexcept { return ( 4.0 * ppqn ); };

        inline int getTicksPerBeat() const noexcept { return ppqn; }
        inline int getTicksInBeat( int Beats ) const noexcept { return ( Beats * ppqn ); }
        inline int getTicksPerBar() const noexcept;
        inline int getTicksInBars( int Bars ) const noexcept;

        BarBeats toBnB() const noexcept;
        void     fromBnB( const BarBeats& ) noexcept;
    };

    struct BarBeats
    {
        int bar{ -1 };
        int beat{ 0 };
        int tick{ 0 };

        BarBeats( int Bar = -1, int Beat = 0, int Tick = 0 ) noexcept;
        ~BarBeats() noexcept = default;

        //==============================================================================
        // Copy and Move constructors...
        BarBeats& operator=( const BarBeats& ) noexcept = default;
        BarBeats& operator=( BarBeats&& ) noexcept      = default;
        BarBeats( const BarBeats& ) noexcept            = default;
        BarBeats( BarBeats&& ) noexcept                 = default;

        // Comparators...
        bool operator==( const BarBeats& Other ) const noexcept;
        bool operator!=( const BarBeats& Other ) const noexcept;
        bool operator<( const BarBeats& Other ) const noexcept;
        bool operator<=( const BarBeats& Other ) const noexcept;
        bool operator>( const BarBeats& Other ) const noexcept;
        bool operator>=( const BarBeats& Other ) const noexcept;

        //==============================================================================
        bool isValid() const;
        void clear();

        std::string toString() const;
        int         toTicks( int numerator, int denominator, int ppqn ) const;
        double      toSeconds( double tempo, int denominator, int ppqn ) const;

        static BarBeats fromTicks( unsigned int Ticks, int Numerator, int Denominator, int PPQN );
        static BarBeats fromSeconds( double Seconds, double Tempo, int Numerator, int Denominator, int PPQN );
        static BarBeats fromSeconds( double Seconds, const Tempo& tempo, const TimeSignature& ts, int ppqn );
    };

};
