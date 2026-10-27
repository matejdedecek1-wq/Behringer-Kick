#pragma once

#include <cmath>
#include <algorithm>

#define USE_STATIC_RAMPER ( 0 )

#if defined( USE_STATIC_RAMPER ) && ( 0 < USE_STATIC_RAMPER )
    #include "../3rdparty/phmap/phmap.h"
#endif

namespace AudIO
{
    struct RampedParameterBase
    {
        virtual bool isRamping() const   = 0;
        virtual void ramp( int samples ) = 0;
    };

#if defined( USE_STATIC_RAMPER ) && ( 0 < USE_STATIC_RAMPER )
    struct SRamper
    {
        static struct SRamper& get()
        {
            static struct SRamper _s;
            return _s;
        }

        void ramp( int samples = 1 )
        {
            return;

            // Merge added/updated elements...
            processSet.merge( _add );
            _add.clear();

            // Remove destroyed elements...
            for( auto& it : _remove )
                processSet.erase( it );
            _remove.clear();

            // Process ramp quque...
            if( processSet.size() )
            {
                auto itr = processSet.begin();
                while( itr != processSet.end() )
                {
                    ( *itr )->ramp( samples );
                    if( !( *itr )->isRamping() )
                        itr = processSet.erase( itr );
                    else
                        ++itr;
                }
            }
        }

        void add( RampedParameterBase* p )
        {
            if( p )
            {
                _add.emplace( p );
            }
        }
        void remove( RampedParameterBase* p )
        {
            if( p )
            {
                _remove.emplace( p );
            }
        }

        phmap::flat_hash_set<RampedParameterBase*> processSet, _remove, _add;
    };
#endif

    template<typename T>
    class RampedParameter : public RampedParameterBase
    {
    public:
        explicit RampedParameter( T V = .0f, int Steps = 12000, bool IsLinear = true )
            : // default ramp duration of 10ms @48K
            isLinear( IsLinear )
            , current( V )
            , target( V )
            , nSteps( Steps )
        {}

#if defined( USE_STATIC_RAMPER ) && ( 0 < USE_STATIC_RAMPER )
        virtual ~RampedParameter()
        {
            SRamper::get().remove( this );
        }
#else
        ~RampedParameter() = default;
#endif

        inline T value() const { return target; }
        inline T getTarget() const { return target; }
        inline T getCurrent() const { return current; }
        inline T load() const { return current; }

        inline T getNext( int Samples = 1 )
        {
            ramp( Samples );
            return current;
        }

        void operator=( const T val )
        {
            store( val );
        }

        void setRampDuration( float sampleRate, float rampLengthInSeconds ) noexcept
        {
            nSteps = std::floor( rampLengthInSeconds * sampleRate );
        }

        void setRampDuration( int numSteps ) noexcept
        {
            nSteps = numSteps;
        }

        void store( const T val, bool force = false )
        {
            target = val;

            if( force || utils::isEqual( target, current ) )
            {
                current = val;
#if defined( USE_STATIC_RAMPER ) && ( 0 < USE_STATIC_RAMPER )
                SRamper::get().remove( this );
#endif
                return;
            }

            if( isLinear )
                stepSize = ( target - current ) / (T)nSteps;
            //std::cout << "\n";
            // Add to ramper...
            nStepCounter = 0;
#if defined( USE_STATIC_RAMPER ) && ( 0 < USE_STATIC_RAMPER )
            SRamper::get().add( this );
#endif
        }

        bool isRamping() const override
        {
            return ( nStepCounter < nSteps );
        }

    private:
        bool isLinear{ true };
        int  nSteps;
        int  nStepCounter{ 0 };
        T    current{ 0 };
        T    target{ 0 };
        T    stepSize{ 0 };

        void ramp( int samples = 1 ) override
        {
            if( isRamping() )
            {
                auto availSteps = nSteps - nStepCounter;
                auto incSteps   = ( samples < availSteps ) ? samples : availSteps;

                current = isLinear ? linearInterpolator( incSteps ) : logInterpolator( incSteps );
                nStepCounter += incSteps;
            }
            else
                current = target;
        }

        T linearInterpolator( int Samples )
        {
            return current += stepSize * Samples;
        }

        T logInterpolator( int /*Samples*/ )
        {
            auto step = std::exp( ( std::log( std::abs( target ) ) - std::log( std::abs( current ) ) ) / nSteps );
            return static_cast<T>( current * std::pow( step, nStepCounter ) );
        }
    };
}
