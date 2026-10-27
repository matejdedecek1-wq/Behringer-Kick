#pragma once

#if !defined( JUCE_VERSION )
    #include "../Includes/AudioObject.h"
#endif

namespace AudIO
{
    class ADSRObj : public Object
    {
    public:
        enum
        {
            kParamBypass = 0,
            kParamAttackTimeSec,
            kParamDecayTimeSec,
            kParamSustainLevel,
            kParamReleaseTimeSec,
            kParamGate,
            kParamMax
        };
        ADSRObj();

        void        setParameter( int index, float value ) override;
        float       getParameter( int index ) override;
        const char* getType() const override;
        void        getParameterLabel( int index, char* text ) override;
        void        getParameterName( int index, char* text ) override;
        int         getNumParameters() override { return ADSRObj::kParamMax; }
        float       getMin( uint32_t index ) const override;
        float       getMax( uint32_t index ) const override;
        void        setAttackTime( double time );
        void        setDecayTime( double time );
        void        setReleaseTime( double time );
        void        setSustainLevel( double level );
        void        reset( void );
        void        gateOn();
        void        gateOff();

        void setBypass( bool Bypass );

        enum class State
        {
            idle = 0,
            attack,
            decay,
            sustain,
            release,
        };

        inline float processADSR( void );

        void prepareToPlay( uint32_t samplerate, uint32_t blocksize ) override;

    private:
        void process( Buffer<float>& buffer ) override;

        inline float getRate( float distance, float timeInSeconds ) const;
        inline void  recalulateAttack();
        inline void  recalulateDecay();
        inline void  recalulateRelease();
        inline void  recalculateCoeffs();
        inline void  goToNextState();
        State        state = State::idle;
        float        output;

        float attack{ 0.1f };
        float decay{ 0.1f };
        float release{ 0.1f };
        float sustain{ 1.0f };

        std::atomic<bool>  bypass{ false };
        std::atomic<float> attackCoef{ 0.0f };
        std::atomic<float> decayCoef{ 0.0f };
        std::atomic<float> releaseCoef{ 0.0f };
    };
    REGISTER_OBJECT( "ADSR", ADSRObj );
}
