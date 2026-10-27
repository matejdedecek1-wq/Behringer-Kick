#pragma once

#include <math.h>
#include <string.h>
#include <memory>
#include <random>
#include <stdexcept>
#include <type_traits>
#include <cassert>
#include <bitset>
#include <climits>
#include <cstring>

namespace AudIO
{
    template<typename Type>
    struct Math
    {
        static constexpr Type pi    = static_cast<Type>( 3.141592653589793238L ); // Pi
        static constexpr Type twoPi = static_cast<Type>( 2 * pi ); // Pi*2
        static constexpr Type pi_2  = static_cast<Type>( 1.5707963267948966 ); // Pi/2
        static constexpr Type pi_4  = static_cast<Type>( 0.7853981633974483 ); //pi/4

        static constexpr Type euler = static_cast<Type>( 2.71828182845904523536L );

        static constexpr Type sqrt2     = static_cast<Type>( 1.4142135623730950488L ); // sqrt(2)
        static constexpr Type sqrt2_2   = static_cast<Type>( 0.7071067811865476 ); //sqrt(2)/2
        static constexpr Type root_12_2 = static_cast<Type>( 1.0594630943592952645618 );

        static constexpr Type gain3dB   = static_cast<Type>( 0.7079457843841379 ); //10^(-3/20)
        static constexpr Type gain4_5dB = static_cast<Type>( 0.5956621435290105 ); //10^(-4.5/20)
        static constexpr Type gain6dB   = static_cast<Type>( 0.5011872336272722 ); //10^(-6/20)
    };

    namespace utils
    {
        static constexpr const char* emptyString{ "" };

        template<class T>
        using uniform_distribution =
            typename std::conditional<
                std::is_floating_point<T>::value,
                std::uniform_real_distribution<T>,
                typename std::conditional<std::is_integral<T>::value, std::uniform_int_distribution<T>, void>::type>::type;

        template<typename T>
        T random( T range_from = 0, T range_to = std::numeric_limits<T>::max() )
        {
            static thread_local std::mt19937_64 generator( std::random_device{}() );
            uniform_distribution<T>             distr( range_from, range_to );
            return distr( generator );
        }

        template<typename... Args>
        std::string string_format( const std::string& format, Args... args )
        {
            int size_s = std::snprintf( nullptr, 0, format.c_str(), args... ) + 1; // Extra space for '\0'
            if( size_s <= 0 )
            {
                throw std::runtime_error( "Error during formatting." );
            }
            auto                    size = static_cast<size_t>( size_s );
            std::unique_ptr<char[]> buf( new char[size] );
            std::snprintf( buf.get(), size, format.c_str(), args... );
            return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
        }

        template<typename EnumType>
        constexpr inline decltype( auto ) toInt( EnumType enumValue )
        {
            static_assert( std::is_enum<EnumType>::value, "Enum type required" );
            using EnumValueType = std::underlying_type_t<EnumType>;
            return static_cast<EnumValueType>( enumValue );
        }

        template<typename EnumType, typename IntegralType>
        constexpr inline EnumType toEnum( IntegralType value )
        {
            static_assert( std::is_enum<EnumType>::value, "Enum type required" );
            static_assert( std::is_integral<IntegralType>::value, "Integer required" );
            return static_cast<EnumType>( value );
        }

        enum ObjectStringConstants
        {
            kMaxParamStrLen   = 8, ///< used for #effGetParamLabel, #effGetParamDisplay, #effGetParamName
            kMaxObjectNameLen = 32 ///< used for #effGetEffectName
        };

        static float logToLinear( float db )
        {
            return static_cast<float>( pow( 10, ( db * 0.05 ) ) ); // 10 ^ (db/20)
        }

        static float linearTolog( float gain )
        {
            return gain > 0.f ? ( static_cast<float>( std::log10( gain ) ) * 20.0f ) : -100.f; //return (float)( 20 * log10( value ) );
        }

        static float millisecToSamples( float time, uint32_t sampleRate )
        {
            return time * ( sampleRate ) / 1000.0f;
        }

        // These functions are intended for 32-bit floating point numbers only and should work a bit faster than the regular ones.
        // fastabs() gives you the absolute value of a float
        static inline float fastabs( float f )
        {
            int i = ( ( *(int*)&f ) & 0x7fffffff );
            return ( *(float*)&i );
        }

        // fastneg() gives you the negative number (faster than multiplying with -1)
        static inline float fastneg( float f )
        {
            int i = ( ( *(int*)&f ) ^ 0x80000000 );
            return ( *(float*)&i );
        }

        // fastsgn() gives back +1 for 0 or positive numbers, -1 for negative numbers
        static inline int fastsgn( float f )
        {
            return 1 + ( ( ( *(int*)&f ) >> 31 ) << 1 );
        }

        static inline double fasttanh( float x )
        {
            const double ax = fastabs( x );
            const double x2 = x * x;

            return ( x * ( 2.45550750702956 + 2.45550750702956 * ax + ( 0.893229853513558 + 0.821226666969744 * ax ) * x2 )
                     / ( 2.44506634652299 + ( 2.44506634652299 + x2 ) * fastabs( x + 0.814642734961073 * x * ax ) ) );
        }

        /** Returns 1.0 if the input Value is >= 0.
        Returns -1.0 if the input Value is < 0. */
        template<typename TYPE>
        const TYPE sign( const TYPE& Value )
        {
            return static_cast<TYPE>( 1 ) - ( Value < static_cast<TYPE>( 0 ) ) * static_cast<TYPE>( 2 );
        }

        template<typename T>
        static T max( const T& val1, const T& val2 )
        {
            return std::max( val1, val2 );
        }

        template<typename T>
        static T min( const T& val1, const T& val2 )
        {
            return std::min( val1, val2 );
        }

        template<typename T>
        static inline T limit( const T& lower, const T& upper, const T& value )
        {
            return ( value < lower ) ? lower : ( ( value > upper ) ? upper : value );
        }

        template<typename T>
        static T clamp( const T& value, const T& lower, const T& upper )
        {
            return std::max( lower, std::min( value, upper ) );
        }

        static bool isBelow( unsigned int Value, unsigned int Max )
        {
            return ( ( Value < Max ) ? true : false );
        }

        static bool isBetween( int Value, int Min, int Max )
        {
            return ( ( Min < Value ) && ( Value < Max ) ) ? true : false;
        }

        static inline bool isPositive( int Value )
        {
            return ( -1 < Value );
        }

        static bool isPositiveAndBelow( int Value, int Max )
        {
            return isBetween( Value, -1, Max );
        }

        static float peak( float peak, float sample )
        {
            auto abs = std::abs( sample );
            return abs > peak ? abs : peak;
        }

        // Floating point comparision utilities...
        static constexpr float EPSILON = 0.0001f; // 1e-4
        // scaled epsilon float epsilon = std::max(std::fabs(a), std::fabs(b)) * 1e-4;

        template<typename T>
        static bool isEqual( T a, T b, T epsilon = (T)EPSILON )
        {
            return ( ( a - b ) < epsilon ) && ( ( b - a ) < epsilon );
        }

        template<typename T>
        static bool isNotEqual( T a, T b, T epsilon = (T)EPSILON )
        {
            return !isEqual( a, b, epsilon );
        }

        template<typename T>
        static bool isZero( T a )
        {
            return isEqual<T>( a, 0.0 );
        }

        template<typename T>
        static bool isGreaterThan( T a, T b, T epsilon = (T)EPSILON )
        {
            return a > b + epsilon;
        }
        template<typename T>
        static bool isGreaterOrEuqal( T a, T b, T epsilon = (T)EPSILON )
        {
            return a > b - epsilon;
        }
        template<typename T>
        static bool isLessThan( T a, T b, T epsilon = (T)EPSILON )
        {
            return a < b - epsilon;
        }
        template<typename T>
        static bool isLessOrEuqal( T a, T b, T epsilon = (T)EPSILON )
        {
            return a < b + epsilon;
        }

        // Bloolean utilities...
        template<typename Type>
        inline Type toType( bool state ) noexcept
        {
            return state ? static_cast<Type>( 1 ) : static_cast<Type>( 0 );
        }

        template<typename Type>
        inline bool toBool( Type value ) noexcept
        {
            return value != static_cast<Type>( 0 );
        }

        inline std::string toString( bool state, bool useYesNo = false )
        {
            return state ? ( useYesNo ? "Yes" : "True" ) : ( useYesNo ? "No" : "False" );
        }

        static bool isPowerOfTwo( uint32_t x )
        {
            return x && ( !( x & ( x - 1 ) ) );
        }

        static uint32_t previousPowerOfTwo( uint32_t x )
        {
            x = x | ( x >> 1 );
            x = x | ( x >> 2 );
            x = x | ( x >> 4 );
            x = x | ( x >> 8 );
            x = x | ( x >> 16 );
            return x - ( x >> 1 );
        }

        template<typename Type, typename UnaryFunction>
        static inline void forEachItem( const std::vector<Type>& items, const UnaryFunction& fn )
        {
            for( auto f : items )
                fn( f );
        }

        template<typename T>
        void clear( T* Dst, uint32_t Len )
        {
            assert( nullptr != Dst );
            assert( 0 != Len );

            //std::copy( std::execution::par, Src, Src + Len, Dst );
            std::memset( Dst, 0, sizeof( T ) * Len );
        }

        template<typename T>
        void copy( T* Dst, const T* Src, uint32_t Len )
        {
            assert( nullptr != Dst );
            assert( nullptr != Src );
            assert( 0 != Len );

            //std::copy( std::execution::par, Src, Src + Len, Dst );
            std::copy( Src, Src + Len, Dst );
        }

        template<typename T>
        void copy( T* Dst, uint32_t Len, T Src )
        {
            assert( nullptr != Dst );
            assert( 0 != Len );

            std::memset( Dst, Src, sizeof( T ) * Len );
        }

        template<typename T, typename U>
        void copyWithGain( T* Dst, const T* Src, uint32_t Len, U Gain )
        {
            assert( nullptr != Dst );
            assert( nullptr != Src );
            assert( 0 != Len );

            if( isEqual<T>( Gain, (T)0.0 ) )
            {
                std::memset( Dst, 0, sizeof( T ) * Len );
            }
            else if( isEqual<T>( Gain, (T)1.0 ) )
            {
                std::copy( Src, Src + Len, Dst );
            }
            else
            {
                for( uint32_t i = 0; i < Len; ++i )
                    Dst[i] = Src[i] * Gain;
            }
        }

        template<typename T>
        inline T addSample( T Dst, T Src )
        {
            return ( Dst + Src );
            //return (Dst + Src ) / 2;
            //return ( 0.707 )( Dst + Src);
            //return std::tanh( Dst + Src );
        }

        template<typename T>
        void add( T* Dst, T* Src, uint32_t Len )
        {
            assert( nullptr != Dst );
            assert( nullptr != Src );
            assert( 0 != Len );

            for( uint32_t i = 0; i < Len; ++i )
                Dst[i] += Src[i];
        }

        template<typename T>
        void add( T* Dst, uint32_t Len, T Factor )
        {
            assert( nullptr != Dst );
            assert( 0 != Len );

            for( uint32_t i = 0; i < Len; ++i )
                Dst[i] += Factor;
        }

        template<typename T>
        void substract( T* Dst, T* Src, uint32_t Len )
        {
            assert( nullptr != Dst );
            assert( nullptr != Src );
            assert( 0 != Len );

            for( uint32_t i = 0; i < Len; ++i )
                Dst[i] -= Src[i];
        }

        template<typename T>
        void substract( T* Dst, uint32_t Len, T Factor )
        {
            assert( nullptr != Dst );
            assert( 0 != Len );

            for( uint32_t i = 0; i < Len; ++i )
                Dst[i] -= Factor;
        }

        template<typename T, typename U>
        void addWithGain( T* Dst, T* Src, uint32_t Len, U Gain )
        {
            assert( nullptr != Dst );
            assert( nullptr != Src );
            assert( 0 != Len );

            if( isEqual<U>( 1.0, Gain ) )
                for( uint32_t i = 0; i < Len; ++i )
                    Dst[i] = addSample( Dst[i], Src[i] );
            else if( isNotEqual<U>( 0.0, Gain ) )
                for( uint32_t i = 0; i < Len; ++i )
                    Dst[i] = addSample( Dst[i], ( Src[i] * Gain ) );
        }

        template<typename T>
        void multiply( T* Dst, T* Src, uint32_t Len )
        {
            assert( nullptr != Dst );
            assert( nullptr != Src );
            assert( 0 != Len );

            for( uint32_t i = 0; i < Len; ++i )
                Dst[i] *= Src[i];
        }

        template<typename T>
        void multiply( T* Dst, uint32_t Len, T Gain )
        {
            assert( nullptr != Dst );
            assert( 0 != Len );

            auto out = Dst;
            for( uint32_t i = 0; i < Len; ++i, ++out )
            {
                *out *= Gain;
            }
        }

        template<typename T>
        void divide( T* Dst, T* Src, uint32_t Len )
        {
            assert( nullptr != Dst );
            assert( nullptr != Src );
            assert( 0 != Len );

            for( uint32_t i = 0; i < Len; ++i )
                Dst[i] /= Src[i];
        }

        template<typename T>
        void divide( T* Dst, uint32_t Len, T Div )
        {
            assert( nullptr != Dst );
            assert( 0 != Len );

            auto out = Dst;
            for( uint32_t i = 0; i < Len; ++i, ++out )
            {
                *out /= Div;
            }
        }

        template<typename T>
        void compressor( T* Buffer, uint32_t Len )
        {
            assert( nullptr != Buffer );
            assert( 0 != Len );

            float val = 0;
            auto  out = Buffer;
            for( uint32_t i = 0; i < Len; ++i, ++out )
            {
                val = *out;
                //Buffer[i] = std::tanh( val );
                Buffer[i] = fasttanh( val );
            }
        }

        template<typename T>
        static inline T rms( T* Buffer, uint32_t Len )
        {
            assert( nullptr != Buffer );
            assert( 0 != Len );

            double rms = 0.0;
            float  val = 0;
            auto   out = Buffer;
            for( uint32_t i = 0; i < Len; ++i, ++out )
            {
                val = *out;
                rms += ( val * val );
            }

            return static_cast<T>( std::sqrt( rms / Len ) );
        }

        template<typename T>
        static inline void rmsAndPeak( const T* Buffer, uint32_t Len, T& Rms, T& Peak )
        {
            assert( nullptr != Buffer );
            assert( 0 != Len );

            double rms = 0.0;
            float  val = 0;
            auto   out = Buffer;
            for( uint32_t i = 0; i < Len; ++i, ++out )
            {
                val = *out;
                rms += ( val * val );
                Peak = max( Peak, fastabs( val ) );
            }
            Rms = std::sqrt( rms / Len );
        }

        static inline void rmsMinAndMax( const float* Buffer, uint32_t Len, float& Rms, float& Min, float& Max )
        {
            assert( nullptr != Buffer );
            assert( 0 != Len );

            double rms = 0.0;
            float  val = 0;
            auto   out = Buffer;
            for( uint32_t i = 0; i < Len; ++i, ++out )
            {
                val = *out;
                rms += ( val * val );
                Min = min( Min, val );
                Max = max( Max, val );
            }
            Rms = (float)std::sqrt( rms / Len );
        }

        static inline int semiToCents( int Semi )
        {
            return Semi * 100;
        }

        static inline int centsToSemi( int Cents )
        {
            return ( Cents / 100 );
        }

        static inline double calculatePitchFactor( int cents )
        {
            return pow( 2, ( cents / 1200.0 ) );
        }

        static inline double calculatePitchFactor( int semi, int cents )
        {
            int cents_ = semiToCents( semi ) + cents;
            return calculatePitchFactor( cents_ );
        }
    } // namespace utils
} // namespace AudIO
