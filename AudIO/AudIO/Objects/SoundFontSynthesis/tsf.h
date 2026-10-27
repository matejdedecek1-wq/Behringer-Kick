/* TinySoundFont - v0.8 - SoundFont2 synthesizer - https://github.com/schellingb/TinySoundFont
                                     no warranty implied; use at your own risk

   NOTE: Modified version to reduce memory usage and with ARM optimization.

   NOT YET IMPLEMENTED
     - Better low-pass filter without lowering performance too much
     - Support for modulators

   LICENSE (MIT)

   Copyright (C) 2017, 2018 Bernhard Schelling
   Based on SFZero, Copyright (C) 2012 Steve Folta (https://github.com/stevefolta/SFZero)

   Permission is hereby granted, free of charge, to any person obtaining a copy of this
   software and associated documentation files (the "Software"), to deal in the Software
   without restriction, including without limitation the rights to use, copy, modify, merge,
   publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons
   to whom the Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
   INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
   PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
   LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
   USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#pragma once

#include "stdio.h"
#include <array>
#include <math.h>
#include <stdint.h>
#include <string.h>
#include <vector>
struct tsf;

struct IStream
{
public:
    virtual int32_t read( void* ptr, uint32_t size ) = 0;
    virtual int32_t skip( uint32_t count ) = 0;
    virtual int32_t tell() = 0;
    virtual int32_t seek( uint32_t offset ) = 0;
    virtual ~IStream() {}
};

#if defined( _MSC_VER )
#define FILE_OPEN( p, f, m ) fopen_s( p, f, m )
#define FILE_CLOSE( f )      fclose( f )
#else
#define FILE_OPEN( p, f, m ) fopen( f, m )
#define FILE_CLOSE( f )      fclose( f )
#endif

class FileStream : public IStream
{
public:
    FileStream( const char* filename )
    {
        FILE_OPEN( &f, filename, "rb" );
    }
    ~FileStream()
    {
        fclose( f );
    }
    int32_t read( void* ptr, uint32_t size ) override
    {
        return (int32_t)fread( ptr, 1, size, f );
    }
    int32_t skip( uint32_t count ) override
    {
        return !fseek( f, count, SEEK_CUR );
    }
    int32_t tell() override
    {
        return ftell( f );
    }
    int32_t seek( uint32_t count ) override
    {
        return fseek( f, count, SEEK_SET );
    }

private:
    FILE* f;
};

class MemoryStream : public IStream
{
public:
    MemoryStream( const uint8_t* memory, size_t size )
        : _memory( memory )
        , _size( size )
    {}
    int32_t read( void* ptr, uint32_t size ) override
    {
        if( (uint32_t)(_pos + size) > (uint32_t)_size )
            size = (uint32_t)(_size - _pos);

        memcpy( ptr, _memory + _pos, size );

        _pos += size;
        return size;
    }
    int32_t skip( uint32_t count ) override
    {
        if( count + _pos < _size )
            _pos += count;
        return 1;
    }
    int32_t tell() override
    {
        return (int32_t)_pos;
    }
    int32_t seek( uint32_t count ) override
    {
        if( count < _size )
            _pos = count;
        return 0;
    }

private:
    const uint8_t* _memory = nullptr;
    size_t         _pos = 0;
    size_t         _size = 0;
};

// Supported output modes by the render methods
enum OutputMode
{
    // Two channels with single left/right samples one after another
    StereoInterleaved,
    // Two channels with all samples for the left channel first then right
    StereoUnweaved,
    // A single channel (stereo instruments are mixed into center)
    Mono,
};

// Thread safety:
// Your audio output which calls the tsf_render* functions will most likely
// run on a different thread than where the playback tsf_note* functions
// are called. In which case some sort of concurrency control like a
// mutex needs to be used so they are not called at the same time.

// Grace release time for quick voice off (avoid clicking noise)
#define TSF_FASTRELEASETIME 0.01f

// Reuse voice < level
#define TSF_REUSE_LEVEL 0.1f

// execute GC every TSF_GC_F render
#define TSF_GC_F 1024

#if !defined( TSF_MUTEX_TYPEDEF ) || !defined( TSF_MUTEX_INIT ) || !defined( TSF_MUTEX_DEINIT ) || !defined( TSF_MUTEX_LOCK ) || !defined( TSF_MUTEX_UNLOCK )
#define TSF_MUTEX_TYPEDEF uint32_t
#define TSF_MUTEX_INIT    0
#define TSF_MUTEX_DEINIT
#define TSF_MUTEX_LOCK
#define TSF_MUTEX_UNLOCK
#endif

#ifndef TSF_NO_STDIO
#include <stdio.h>
#endif

#define TSF_PI                3.14159265358979323846264338327950288

#define float_to_fixed( v )   ( int32_t )( (v)*32768.0f )
#define fixed_to_float( v )   (float)( ( v ) / 32768.0f )

#define float_to_fixed64( v ) ( uint64_t )( (v)*4294967296.0f )
#define fixed64_to_float( v ) (float)( ( v ) / 4294967296.0f )

typedef char     tsf_fourcc[4];
typedef int8_t   tsf_s8;
typedef uint8_t  tsf_u8;
typedef uint16_t tsf_u16;
typedef int16_t  tsf_s16;
typedef uint32_t tsf_u32;
typedef char     tsf_char20[20];

static float timecents2Secsf( float timecents ) { return powf( 2.0f, timecents / 1200.0f ); }
static float cents2Hertz( float cents ) { return 8.176f * powf( 2.0f, cents / 1200.0f ); }
static float decibelsToGain( float db ) { return (db > -100.f ? powf( 10.0f, db * 0.05f ) : 0); }
static float gainToDecibels( float gain ) { return (gain <= .00001f ? -100.f : (float)(20.0 * log10( gain ))); }

#define TSF_FourCCEquals( value1, value2 ) ( value1[0] == value2[0] && value1[1] == value2[1] && value1[2] == value2[2] && value1[3] == value2[3] )

struct Hydra
{
    int32_t phdrNum, pbagNum, pmodNum, pgenNum, instNum, ibagNum, imodNum, igenNum, shdrNum;
    int32_t phdrPos, pbagPos, pmodPos, pgenPos, instPos, ibagPos, imodPos, igenPos, shdrPos;
};

enum LoopMode
{
    none,
    continuous,
    sustain
};

enum
{
    phdrSizeInFile = 38,
    pbagSizeInFile = 4,
    pmodSizeInFile = 10,
    pgenSizeInFile = 4,
    instSizeInFile = 22,
    ibagSizeInFile = 4,
    imodSizeInFile = 10,
    igenSizeInFile = 4,
    shdrSizeInFile = 46
};

struct Voice;
struct TSFChannel
{
    uint16_t presetIndex = 0, bank = 0, pitchWheel = 8192, midiPan = 8192, midiVolume = 16383, midiExpression = 16383, midiRPN = 0xFFFF, midiData = 0;
    float    reverb = 0.0f, chorus = 0.0f;
    float    panOffset = 0.0f, gainDB = 0.0f, pitchRange = 2.0f, tuning = 0.0f;

    int32_t getPresetIndex()
    {
        return presetIndex;
    }

    void setPresetIndex( uint16_t index )
    {
        presetIndex = index;
    }

    int32_t getPresetBank()
    {
        return bank & 0x7FFF;
    }

    float getPan()
    {
        return panOffset - 0.5f;
    }

    float getVolume()
    {
        return decibelsToGain( gainDB );
    }

    int32_t getPitchWheel()
    {
        return pitchWheel;
    }

    float getPitchRange()
    {
        return pitchRange;
    }

    float getTuning()
    {
        return tuning;
    }
};

struct VoiceLfo
{
    int32_t samplesUntil;
    float   level, delta;
    void    setup( float delay, float freq, float sampleRate )
    {
        samplesUntil = (int32_t)(delay * sampleRate);
        delta = (4.0f * cents2Hertz( freq ) / sampleRate);
        level = 0;
    }
    void process( int32_t blockSamples )
    {
        if( samplesUntil > blockSamples )
        {
            samplesUntil -= blockSamples;
            return;
        }
        level += delta * blockSamples;
        if( level > 1.0f )
        {
            delta = -delta;
            level = 2.0f - level;
        } else if( level < -1.0f )
        {
            delta = -delta;
            level = -2.0f - level;
        }
    }
};

//struct VoiceLowPass
//{
//    void setQ( float Q )
//    {
//        QInv = Q;
//        z1 = 0;
//        z2 = 0;
//    }
//    void setCutOff( float Fc )
//    {
//#ifndef TSF_NO_LOWPASS
//        active = (Fc < 0.499f);
//        if( active )
//        {
//            // Lowpass filter from http://www.earlevel.com/main/2012/11/26/biquad-c-source-code/
//            double K = tan( TSF_PI * Fc ), KK = K * K;
//            double norm = 1 / (1 + K * QInv + KK);
//            a0 = (KK * norm);
//            a1 = (2 * KK * norm);
//            b1 = (2 * (KK - 1) * norm);
//            b2 = ((1 - K * QInv + KK) * norm);
//        }
//        // printf("LOW-PASS a0:%d a1:%d b1:%d b2:%d\n",a0, a1, b1, b2);
//#endif
//    }
//
//    inline float process( double In )
//    {
//        double Out = In * a0 + z1;
//        z1 = In * a1 + z2 - b1 * Out;
//        z2 = In * a0 - b2 * Out;
//        return (float)Out;
//    }
//
//    bool isActive() { return active; }
//
//private:
//    float  QInv = 0.0f;
//    double a0 = 0, a1 = 0, b1 = 0, b2 = 0;
//    double z1 = 0, z2 = 0;
//    bool   active = false;
//};

struct envelopeParams
{
    float delay = 0.0f;
    float attack = 0.0f;
    float hold = 0.0f;
    float decay = 0.0f;
    float sustain = 0.0f;
    float release = 0.0f;
    float keynumToHold = 0.0f;
    float keynumToDecay = 0.0f;
    void  clear()
    {
        delay = 0.0f;
        attack = 0.0f;
        hold = 0.0f;
        decay = 0.0f;
        sustain = 0.0f;
        release = 0.0f;
        keynumToHold = 0.0f;
        keynumToDecay = 0.0f;
    }
    void tosecs( bool sustainIsGain )
    {
        // EG times need to be converted from timecents to seconds.
        // Pin very short EG segments.  Timecents don't get to zero, and our EG is
        // happier with zero values.
        delay = (delay < -11950.0f ? 0.0f : timecents2Secsf( delay ));
        attack = (attack < -11950.0f ? 0.0f : timecents2Secsf( attack ));
        release = (release < -11950.0f ? 0.0f : timecents2Secsf( release ));

        // If we have dynamic hold or decay times depending on key number we need
        // to keep the values in timecents so we can calculate it during startNote
        if( !keynumToHold )
            hold = (hold < -11950.0f ? 0.0f : timecents2Secsf( hold ));
        if( !keynumToDecay )
            decay = (decay < -11950.0f ? 0.0f : timecents2Secsf( decay ));

        if( sustain < 0.0f )
            sustain = 0.0f;
        else if( sustainIsGain )
            sustain = decibelsToGain( -sustain / 10.0f );
        else
            sustain = 1.0f - (sustain / 1000.0f);
    }
};

struct VoiceEnvelope
{
    enum EnvelopeState
    {
        delay = 0,
        attack,
        hold,
        decay,
        sustain,
        release,
        idle
    };
    float          level = 0.0f, slope = 0.0f;
    int32_t        samplesUntilNextSegment = 0;
    int16_t        segment = EnvelopeState::idle, midiVelocity;
    envelopeParams parameters;
    float          outSampleRate = 0.0f;
    bool           segmentIsExponential, isAmpEnv;

    bool isPostAttack()
    {
        return segment > EnvelopeState::attack && level < TSF_REUSE_LEVEL;
    }

    bool isReleased()
    {
        return segment >= EnvelopeState::release;
    }

    bool notReleased()
    {
        return segment < EnvelopeState::release;
    }

    bool isIdle()
    {
        return segment == EnvelopeState::idle;
    }

    void initialize( envelopeParams* new_parameters, int32_t midiNoteNumber, int16_t velocity, bool AmpEnv, float sampleRate )
    {
        parameters = *new_parameters;
        if( parameters.keynumToHold )
        {
            parameters.hold += parameters.keynumToHold * (60.0f - midiNoteNumber);
            parameters.hold = (parameters.hold < -10000.0f ? 0.0f : timecents2Secsf( parameters.hold ));
        }
        if( parameters.keynumToDecay )
        {
            parameters.decay += parameters.keynumToDecay * (60.0f - midiNoteNumber);
            parameters.decay = (parameters.decay < -10000.0f ? 0.0f : timecents2Secsf( parameters.decay ));
        }
        midiVelocity = velocity;
        isAmpEnv = AmpEnv;
        outSampleRate = sampleRate;
        GateOn();
    }

    void GateOn()
    {
        segment = EnvelopeState::idle;
        goToNextState();
    }

    void GateOff()
    {
        segment = EnvelopeState::sustain;
        goToNextState();
    }

    void process( int32_t blocksize )
    {
        if( slope )
        {
            if( segmentIsExponential )
                level *= powf( slope, (float)blocksize );
            else
                level += (slope * blocksize);
        }
        if( (samplesUntilNextSegment -= blocksize) <= 0 )
        {
            goToNextState();
        }
    }

    void goToNextState()
    {
        switch( segment )
        {
            case EnvelopeState::idle:
            {
                samplesUntilNextSegment = (int32_t)(parameters.delay * outSampleRate);
                if( samplesUntilNextSegment > 0 )
                {
                    segment = EnvelopeState::delay;
                    segmentIsExponential = false;
                    level = 0.0f;
                    slope = 0.0f;
                    return;
                }
            }
            case EnvelopeState::delay:
            {
                samplesUntilNextSegment = (int32_t)(parameters.attack * outSampleRate);
                if( samplesUntilNextSegment > 0 )
                {
                    if( !isAmpEnv )
                    {
                        // mod env attack duration scales with velocity (velocity of 1 is full duration, max velocity is 0.125 times duration)
                        samplesUntilNextSegment = (int32_t)(parameters.attack * ((145 - midiVelocity) / 144.0f) * outSampleRate);
                    }
                    segment = EnvelopeState::attack;
                    segmentIsExponential = false;
                    level = 0.0f;
                    slope = 1.0f / samplesUntilNextSegment;
                    return;
                }
            }
            case EnvelopeState::attack:
            {
                samplesUntilNextSegment = (int32_t)(parameters.hold * outSampleRate);
                if( samplesUntilNextSegment > 0 )
                {
                    segment = EnvelopeState::hold;
                    segmentIsExponential = false;
                    level = 1.0f;
                    slope = 0.0f;
                    return;
                }
            }
            case EnvelopeState::hold:
            {
                samplesUntilNextSegment = (int32_t)(parameters.decay * outSampleRate);
                if( samplesUntilNextSegment > 0 )
                {
                    segment = EnvelopeState::decay;
                    level = 1.0f;
                    if( isAmpEnv )
                    {
                        // I don't truly understand this; just following what LinuxSampler does.
                        float mysterySlope = -9.226f / samplesUntilNextSegment;
                        slope = expf( mysterySlope );
                        segmentIsExponential = true;
                        if( parameters.sustain > 0.0f )
                        {
                            // Again, this is following LinuxSampler's example, which is similar to
                            // SF2-style decay, where "decay" specifies the time it would take to
                            // get to zero, not to the sustain level.  The SFZ spec is not that
                            // specific about what "decay" means, so perhaps it's really supposed
                            // to specify the time to reach the sustain level.
                            samplesUntilNextSegment = (int32_t)(log( parameters.sustain ) / mysterySlope);
                        }
                    } else
                    {
                        slope = -1.0f / samplesUntilNextSegment;
                        samplesUntilNextSegment = (int32_t)(parameters.decay * (1.0f - parameters.sustain) * outSampleRate);
                        segmentIsExponential = false;
                    }
                    return;
                }
            }
            case EnvelopeState::decay:
            {
                segment = EnvelopeState::sustain;
                level = parameters.sustain;
                slope = 0.0f;
                samplesUntilNextSegment = 0x7FFFFFFF;
                segmentIsExponential = false;
                return;
            }
            case EnvelopeState::sustain:
            {
                segment = EnvelopeState::release;
            }
            case EnvelopeState::release:
                samplesUntilNextSegment = (int32_t)( ( parameters.release <= 0 ? TSF_FASTRELEASETIME : parameters.release ) * outSampleRate );
                if( isAmpEnv )
                {
                    // I don't truly understand this; just following what LinuxSampler does.
                    float mysterySlope   = -9.226f / samplesUntilNextSegment;
                    slope                = expf( mysterySlope );
                    segmentIsExponential = true;
                }
                else
                {
                    slope                = -level / samplesUntilNextSegment;
                    segmentIsExponential = false;
                }
                return;
            default:
            segment = EnvelopeState::idle;
            segmentIsExponential = false;
            level = slope = 0.0f;
            samplesUntilNextSegment = 0x7FFFFFF;
        };
    }
};

#define TSFR( FIELD ) stream->read( &FIELD, sizeof( FIELD ) );
struct HydraPhdr
{
    tsf_char20 presetName;
    tsf_u16    preset, bank, presetBagNdx;
    tsf_u32    library, genre, morphology;

    void read( struct IStream* stream )
    {
        TSFR( presetName );
        TSFR( preset );
        TSFR( bank );
        TSFR( presetBagNdx );
        TSFR( library );
        TSFR( genre );
        TSFR( morphology );
    }
};
struct HydraPbag
{
    tsf_u16 genNdx, modNdx;
    void    read( IStream* stream )
    {
        TSFR( genNdx );
        TSFR( modNdx );
    }
};

struct HydraPmod
{
    tsf_u16 modSrcOper, modDestOper;
    tsf_s16 modAmount;
    tsf_u16 modAmtSrcOper, modTransOper;
    void    read_pmod( IStream* stream )
    {
        TSFR( modSrcOper )
            TSFR( modDestOper )
            TSFR( modAmount )
            TSFR( modAmtSrcOper )
            TSFR( modTransOper )
    }
};

union HydraGenAmount
{
    struct
    {
        tsf_u8 lo, hi;
    } range;
    tsf_s16 shortAmount;
    tsf_u16 wordAmount;
};
// static void tsf_hydra_read_imod(struct HydraIMod* i, struct IStream* stream) { TSFR(modSrcOper) TSFR(modDestOper) TSFR(modAmount) TSFR(modAmtSrcOper) TSFR(modTransOper) }

struct Riffchunk
{
    tsf_fourcc id;
    tsf_u32    size;
};

struct HydraPgen
{
    tsf_u16        genOper;
    HydraGenAmount genAmount;
    void           read( IStream* stream )
    {
        TSFR( genOper )
            TSFR( genAmount )
    }
};

struct HydraInst
{
    tsf_char20 instName;
    tsf_u16    instBagNdx;
    void       read( IStream* stream )
    {
        TSFR( instName )
            TSFR( instBagNdx )
    }
};

struct HydraIBag
{
    tsf_u16 instGenNdx, instModNdx;
    void    read( IStream* stream )
    {
        TSFR( instGenNdx )
            TSFR( instModNdx )
    }
};

struct HydraIMod
{
    tsf_u16 modSrcOper, modDestOper;
    tsf_s16 modAmount;
    tsf_u16 modAmtSrcOper, modTransOper;
};

struct HydraIGen
{
    tsf_u16        genOper;
    HydraGenAmount genAmount;
    void           read( IStream* stream )
    {
        TSFR( genOper )
            TSFR( genAmount )
    }
};

struct HydraSHdr
{
    tsf_char20 sampleName;
    tsf_u32    start, end, startLoop, endLoop, sampleRate;
    tsf_u8     originalPitch;
    tsf_s8     pitchCorrection;
    tsf_u16    sampleLink, sampleType;
    void       read( IStream* stream )
    {
        TSFR( sampleName )
            TSFR( start )
            TSFR( end )
            TSFR( startLoop )
            TSFR( endLoop )
            TSFR( sampleRate )
            TSFR( originalPitch )
            TSFR( pitchCorrection )
            TSFR( sampleLink )
            TSFR( sampleType )
    }
};
#undef TSFR

struct Region
{
    int16_t        loop_mode = 0;
    uint32_t       sample_rate = 0;
    uint8_t        lokey = 0, hikey = 127, lovel = 0, hivel = 127;
    uint32_t       group = 0, offset = 0, end = 0, loop_start = 0, loop_end = 0;
    int16_t        transpose = 0, tune = 0, pitch_keycenter = 60, pitch_keytrack = 0;
    float          attenuation = 0.0f, pan = 0.0f;
    envelopeParams ampenv, modenv;
    int16_t        initialFilterQ = 0, initialFilterFc = 0;
    int16_t        modEnvToPitch = 0, modEnvToFilterFc = 0, modLfoToFilterFc = 0, modLfoToVolume = 0;
    float          delayModLFO = 0.0f;
    int16_t        freqModLFO = 0, modLfoToPitch = 0;
    float          delayVibLFO = 0.0f;
    int16_t        freqVibLFO = 0, vibLfoToPitch = 0;
    void           clear( bool for_relative )
    {
        loop_mode = 0;
        sample_rate = 0;
        lokey = 0;
        hikey = 127;
        lovel = 0;
        hivel = 127;
        group = 0;
        offset = 0;
        end = 0;
        loop_start = 0;
        loop_end = 0;
        transpose = 0;
        tune = 0;
        pitch_keycenter = 60;
        pitch_keytrack = 0;
        attenuation = 0.0f;
        pan = 0.0f;
        ampenv.clear();
        modenv.clear();
        initialFilterQ = 0;
        initialFilterFc = 0;
        modEnvToPitch = 0;
        modEnvToFilterFc = 0;
        modLfoToFilterFc = 0;
        modLfoToVolume = 0;
        delayModLFO = 0;
        freqModLFO = 0;
        modLfoToPitch = 0;
        delayVibLFO = 0;
        freqVibLFO = 0, vibLfoToPitch = 0;
        if( for_relative )
            return;
        pitch_keytrack = 100;
        pitch_keycenter = -1;
        // SF2 defaults in timecents.
        ampenv.delay = ampenv.attack = ampenv.hold = ampenv.decay = ampenv.release = -12000.0f;
        modenv.delay = modenv.attack = modenv.hold = modenv.decay = modenv.release = -12000.0f;
        initialFilterFc = 13500;
        delayModLFO = -12000.0f;
        delayVibLFO = -12000.0f;
    }
};

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
static const float EPSILON = 0.0001f; // 1e-4
// scaled epsilon float epsilon = std::max(std::fabs(a), std::fabs(b)) * 1e-4;

static inline bool isEqual( double a, double b, double epsilon = EPSILON )
{
    return ((a - b) < epsilon) && ((b - a) < epsilon);
}

static inline bool isNotEqual( double a, double b, double epsilon = (double)EPSILON )
{
    return !isEqual( a, b, epsilon );
}

static inline bool isEqualf( float a, float b, float epsilon = EPSILON )
{
    return ((a - b) < epsilon) && ((b - a) < epsilon);
}

static inline bool isNotEqualf( float a, float b, float epsilon = EPSILON )
{
    return !isEqualf( a, b, epsilon );
}

static float remap( uint8_t in, float min, float max )
{
    double slope = (max - min) / 128;
    return min + slope * in;
}

static float clamp( float in, float min, float max )
{
    float retval = in > max ? max : in;
    retval = retval < min ? min : retval;
    return retval;
}

struct filterEnvelope
{
    float attack = 0.f;
    float targetVal = 0.f;
    float currentVal = 0.1f;
    float delta = 0.f;
    uint8_t phase = 0; // attack = 0; release  = 1

    void noteOn( float target, float attack, float sampleRate )
    {
        phase = 0;
        targetVal = target;
        if( attack > 0.f )
        {
            delta = (targetVal) / (sampleRate * attack);
            currentVal = 0.1f;
        } else
        {
            currentVal = targetVal;
            delta = 0.f;
        }
    }

    void noteOff( float release, float sampleRate )
    {
        phase = 1;
        targetVal = 0.1f;
        if( release > 0.f )
        {
            delta = (currentVal) / (sampleRate * release);
        } else
        {
            currentVal = targetVal;
            delta = 0.f;
        }
    }

    float process( uint32_t samples )
    {
        if( phase )
        {
            if( currentVal > targetVal )
            {
                currentVal -= delta * samples;
            }
        } else
        {
            if( currentVal < targetVal )
            {
                currentVal += delta * samples;
            }
        }
        return currentVal;
    }
};

struct VoiceFilter
{
    float cutoff_{ 0.9f }, resonance_{ 0.f }, feedbackAmount_{ 0.f };
    float buf0_{ 0.f }, buf1_{ 0.f }, buf2_{ 0.f }, buf3_{ 0.f };
    void init( float cutoff, float resonance )
    {
        buf0_ = 0.f;
        buf1_ = 0.f;
        buf2_ = 0.f;
        buf3_ = 0.f;
        feedbackAmount_ = 0.f;
        cutoff_ = cutoff;
        resonance_ = resonance;
        recalculate();
    }

    void setCutoff( float newCutoff )
    {
        if( isNotEqualf( newCutoff, cutoff_ ) )
        {
            cutoff_ = newCutoff;
            recalculate();
        }
    };

    void setResonance( float newResonance )
    {
        resonance_ = newResonance;
        recalculate();
    };

    void recalculate()
    {
        buf0_ = 0.f;
        buf1_ = 0.f;
        feedbackAmount_ = resonance_ + resonance_ / (1.f - cutoff_);
    }

    const float process( float inputValue )
    {
        buf0_ += cutoff_ * ((double)inputValue - buf0_ + feedbackAmount_ * (buf0_ - buf1_));
        buf1_ += cutoff_ * (buf0_ - buf1_);
        buf2_ += cutoff_ * (buf1_ - buf2_);
        buf3_ += cutoff_ * (buf2_ - buf3_);
        return buf3_;
    }
};

struct Voice
{
    Voice()
    {
        mutex = TSF_MUTEX_INIT;
    }

    ~Voice()
    {
        TSF_MUTEX_DEINIT( mutex );
    }
    int32_t           playingPreset = -1, playingKey, playingChannel;
    uint16_t          velocity = 0;
    Region* region = nullptr;
    float             pitchInputTimecents, pitchOutputFactor;
    double            sourceSamplePosition;
    float             noteGainDB, panFactorLeft, panFactorRight;
    uint32_t          playIndex, loopStart, loopEnd;
    uint8_t           locked;
    TSF_MUTEX_TYPEDEF mutex;
    VoiceEnvelope     ampenv, modenv;
    VoiceLfo          modlfo, viblfo;
    VoiceFilter       filter;
    filterEnvelope    filterEnv;

    void setup( TSFChannel& ch, uint32_t chindex, float outSampleRate )
    {
        float newpan = region->pan + ch.panOffset;
        playingChannel = chindex;
        noteGainDB += ch.gainDB;
        calcpitchratio( (ch.pitchWheel == 8192 ? ch.tuning : ((ch.pitchWheel / 16383.0f * ch.pitchRange * 2.0f) - ch.pitchRange + ch.tuning)), outSampleRate );
        if( newpan <= -0.5f )
        {
            panFactorLeft = 1.0f;
            panFactorRight = 0.0f;
        } else if( newpan >= 0.5f )
        {
            panFactorLeft = 0.0f;
            panFactorRight = 1.0f;
        } else
        {
            panFactorLeft = sqrtf( 0.5f - newpan );
            panFactorRight = sqrtf( 0.5f + newpan );
        }
    }
    inline bool isLooping() { return (loopStart < loopEnd); }
    void        calcpitchratio( float pitchShift, float outSampleRate )
    {
        float note = playingKey + region->transpose + region->tune / 100.0f;
        float adjustedPitch = region->pitch_keycenter + (note - region->pitch_keycenter) * (region->pitch_keytrack / 100.0f);
        if( pitchShift )
            adjustedPitch += pitchShift;
        pitchInputTimecents = adjustedPitch * 100.0f;
        pitchOutputFactor = region->sample_rate / (timecents2Secsf( region->pitch_keycenter * 100.0f ) * outSampleRate);
    }

    void kill()
    {
        playingPreset = -1;
    }

    void end()
    {
        ampenv.GateOff();
        modenv.GateOff();
        if( region->loop_mode == LoopMode::sustain )
        {
            // Continue playing, but stop looping.
            loopEnd = loopStart;
        }
    }

    void endquick()
    {
        ampenv.parameters.release = 0.0f;
        ampenv.GateOff();
        modenv.parameters.release = 0.0f;
        modenv.GateOff();
        kill();
    }

    void setAmpEnvelope( float sampleRate )
    {
        ampenv.initialize( &region->ampenv, playingKey, velocity, true, sampleRate );
    }

    void setModEnvelope( float sampleRate )
    {
        modenv.initialize( &region->modenv, playingKey, velocity, false, sampleRate );
    }

    void setModLFO( float sampleRate )
    {
        modlfo.setup( region->delayModLFO, region->freqModLFO, sampleRate );
    }

    void setVibLFO( float sampleRate )
    {
        viblfo.setup( region->delayVibLFO, region->freqVibLFO, sampleRate );
    }
};

static void tsf_region_operator( Region* region, tsf_u16 genOper, HydraGenAmount* amount, Region* merge_region )
{
    enum
    {
        _GEN_TYPE_MASK = 0x0F,
        GEN_FLOAT = 0x01,
        GEN_INT = 0x02,
        GEN_UINT_ADD = 0x03,
        GEN_UINT_ADD15 = 0x04,
        GEN_KEYRANGE = 0x05,
        GEN_VELRANGE = 0x06,
        GEN_LOOPMODE = 0x07,
        GEN_GROUP = 0x08,
        GEN_KEYCENTER = 0x09,

        _GEN_LIMIT_MASK = 0xF0,
        GEN_INT_LIMIT12K = 0x10, // min -12000, max 12000
        GEN_INT_LIMITFC = 0x20, // min 1500, max 13500
        GEN_INT_LIMITQ = 0x30, // min 0, max 960
        GEN_INT_LIMIT960 = 0x40, // min -960, max 960
        GEN_INT_LIMIT16K4500 = 0x50, // min -16000, max 4500
        GEN_FLOAT_LIMIT12K5K = 0x60, // min -12000, max 5000
        GEN_FLOAT_LIMIT12K8K = 0x70, // min -12000, max 8000
        GEN_FLOAT_LIMIT1200 = 0x80, // min -1200, max 1200
        GEN_FLOAT_LIMITPAN = 0x90, //* .001f, min -.5f, max .5f,
        GEN_FLOAT_LIMITATTN = 0xA0, //* .1f, min 0, max 144.0
        GEN_FLOAT_MAX1000 = 0xB0, // min 0, max 1000
        GEN_FLOAT_MAX1440 = 0xC0, // min 0, max 1440

        _GEN_MAX = 59,
    };
#define _TSFREGIONOFFSET( TYPE, FIELD )         ( uint8_t )( ( (TYPE*)&( (Region*)0 )->FIELD ) - (TYPE*)0 )
#define _TSFREGIONENVOFFSET( TYPE, ENV, FIELD ) ( uint8_t )( ( (TYPE*)&( ( &( ( (Region*)0 )->ENV ) )->FIELD ) ) - (TYPE*)0 )
    static const struct
    {
        uint8_t mode, offset;
    } genMetas[_GEN_MAX] = {
        { GEN_UINT_ADD, _TSFREGIONOFFSET( uint32_t, offset ) }, // 0 StartAddrsOffset
        { GEN_UINT_ADD, _TSFREGIONOFFSET( uint32_t, end ) }, // 1 EndAddrsOffset
        { GEN_UINT_ADD, _TSFREGIONOFFSET( uint32_t, loop_start ) }, // 2 StartloopAddrsOffset
        { GEN_UINT_ADD, _TSFREGIONOFFSET( uint32_t, loop_end ) }, // 3 EndloopAddrsOffset
        { GEN_UINT_ADD15, _TSFREGIONOFFSET( uint32_t, offset ) }, // 4 StartAddrsCoarseOffset
        { GEN_INT | GEN_INT_LIMIT12K, _TSFREGIONOFFSET( int16_t, modLfoToPitch ) }, // 5 ModLfoToPitch
        { GEN_INT | GEN_INT_LIMIT12K, _TSFREGIONOFFSET( int16_t, vibLfoToPitch ) }, // 6 VibLfoToPitch
        { GEN_INT | GEN_INT_LIMIT12K, _TSFREGIONOFFSET( int16_t, modEnvToPitch ) }, // 7 ModEnvToPitch
        { GEN_INT | GEN_INT_LIMITFC, _TSFREGIONOFFSET( int16_t, initialFilterFc ) }, // 8 InitialFilterFc
        { GEN_INT | GEN_INT_LIMITQ, _TSFREGIONOFFSET( int16_t, initialFilterQ ) }, // 9 InitialFilterQ
        { GEN_INT | GEN_INT_LIMIT12K, _TSFREGIONOFFSET( int16_t, modLfoToFilterFc ) }, // 10 ModLfoToFilterFc
        { GEN_INT | GEN_INT_LIMIT12K, _TSFREGIONOFFSET( int16_t, modEnvToFilterFc ) }, // 11 ModEnvToFilterFc
        { GEN_UINT_ADD15, _TSFREGIONOFFSET( uint32_t, end ) }, // 12 EndAddrsCoarseOffset
        { GEN_INT | GEN_INT_LIMIT960, _TSFREGIONOFFSET( int16_t, modLfoToVolume ) }, // 13 ModLfoToVolume
        { 0, (0) }, //   Unused
        { 0, (0) }, // 15 ChorusEffectsSend (unsupported)
        { 0, (0) }, // 16 ReverbEffectsSend (unsupported)
        { GEN_FLOAT | GEN_FLOAT_LIMITPAN, _TSFREGIONOFFSET( float, pan ) }, // 17 Pan
        { 0, (0) }, //   Unused
        { 0, (0) }, //   Unused
        { 0, (0) }, //   Unused
        { GEN_FLOAT | GEN_FLOAT_LIMIT12K5K, _TSFREGIONOFFSET( float, delayModLFO ) }, // 21 DelayModLFO
        { GEN_INT | GEN_INT_LIMIT16K4500, _TSFREGIONOFFSET( int16_t, freqModLFO ) }, // 22 FreqModLFO
        { GEN_FLOAT | GEN_FLOAT_LIMIT12K5K, _TSFREGIONOFFSET( float, delayVibLFO ) }, // 23 DelayVibLFO
        { GEN_INT | GEN_INT_LIMIT16K4500, _TSFREGIONOFFSET( int16_t, freqVibLFO ) }, // 24 FreqVibLFO
        { GEN_FLOAT | GEN_FLOAT_LIMIT12K5K, _TSFREGIONENVOFFSET( float, modenv, delay ) }, // 25 DelayModEnv
        { GEN_FLOAT | GEN_FLOAT_LIMIT12K8K, _TSFREGIONENVOFFSET( float, modenv, attack ) }, // 26 AttackModEnv
        { GEN_FLOAT | GEN_FLOAT_LIMIT12K5K, _TSFREGIONENVOFFSET( float, modenv, hold ) }, // 27 HoldModEnv
        { GEN_FLOAT | GEN_FLOAT_LIMIT12K8K, _TSFREGIONENVOFFSET( float, modenv, decay ) }, // 28 DecayModEnv
        { GEN_FLOAT | GEN_FLOAT_MAX1000, _TSFREGIONENVOFFSET( float, modenv, sustain ) }, // 29 SustainModEnv
        { GEN_FLOAT | GEN_FLOAT_LIMIT12K8K, _TSFREGIONENVOFFSET( float, modenv, release ) }, // 30 ReleaseModEnv
        { GEN_FLOAT | GEN_FLOAT_LIMIT1200, _TSFREGIONENVOFFSET( float, modenv, keynumToHold ) }, // 31 KeynumToModEnvHold
        { GEN_FLOAT | GEN_FLOAT_LIMIT1200, _TSFREGIONENVOFFSET( float, modenv, keynumToDecay ) }, // 32 KeynumToModEnvDecay
        { GEN_FLOAT | GEN_FLOAT_LIMIT12K5K, _TSFREGIONENVOFFSET( float, ampenv, delay ) }, // 33 DelayVolEnv
        { GEN_FLOAT | GEN_FLOAT_LIMIT12K8K, _TSFREGIONENVOFFSET( float, ampenv, attack ) }, // 34 AttackVolEnv
        { GEN_FLOAT | GEN_FLOAT_LIMIT12K5K, _TSFREGIONENVOFFSET( float, ampenv, hold ) }, // 35 HoldVolEnv
        { GEN_FLOAT | GEN_FLOAT_LIMIT12K8K, _TSFREGIONENVOFFSET( float, ampenv, decay ) }, // 36 DecayVolEnv
        { GEN_FLOAT | GEN_FLOAT_MAX1440, _TSFREGIONENVOFFSET( float, ampenv, sustain ) }, // 37 SustainVolEnv
        { GEN_FLOAT | GEN_FLOAT_LIMIT12K8K, _TSFREGIONENVOFFSET( float, ampenv, release ) }, // 38 ReleaseVolEnv
        { GEN_FLOAT | GEN_FLOAT_LIMIT1200, _TSFREGIONENVOFFSET( float, ampenv, keynumToHold ) }, // 39 KeynumToVolEnvHold
        { GEN_FLOAT | GEN_FLOAT_LIMIT1200, _TSFREGIONENVOFFSET( float, ampenv, keynumToDecay ) }, // 40 KeynumToVolEnvDecay
        { 0, (0) }, //   Instrument (special)
        { 0, (0) }, //   Reserved
        { GEN_KEYRANGE, (0) }, // 43 KeyRange
        { GEN_VELRANGE, (0) }, // 44 VelRange
        { GEN_UINT_ADD15, _TSFREGIONOFFSET( uint32_t, loop_start ) }, // 45 StartloopAddrsCoarseOffset
        { 0, (0) }, // 46 Keynum (special)
        { 0, (0) }, // 47 Velocity (special)
        { GEN_FLOAT | GEN_FLOAT_LIMITATTN, _TSFREGIONOFFSET( float, attenuation ) }, // 48 InitialAttenuation
        { 0, (0) }, //   Reserved
        { GEN_UINT_ADD15, _TSFREGIONOFFSET( uint32_t, loop_end ) }, // 50 EndloopAddrsCoarseOffset
        { GEN_INT, _TSFREGIONOFFSET( int16_t, transpose ) }, // 51 CoarseTune
        { GEN_INT, _TSFREGIONOFFSET( int16_t, tune ) }, // 52 FineTune
        { 0, (0) }, //   SampleID (special)
        { GEN_LOOPMODE, _TSFREGIONOFFSET( int16_t, loop_mode ) }, // 54 SampleModes
        { 0, (0) }, //   Reserved
        { GEN_INT, _TSFREGIONOFFSET( int16_t, pitch_keytrack ) }, // 56 ScaleTuning
        { GEN_GROUP, _TSFREGIONOFFSET( uint32_t, group ) }, // 57 ExclusiveClass
        { GEN_KEYCENTER, _TSFREGIONOFFSET( int16_t, pitch_keycenter ) }, // 58 OverridingRootKey
    };
#undef _TSFREGIONOFFSET
#undef _TSFREGIONENVOFFSET
    if( amount )
    {
        int32_t offset;
        if( genOper >= _GEN_MAX )
            return;
        offset = genMetas[genOper].offset;
        switch( genMetas[genOper].mode & _GEN_TYPE_MASK )
        {
            case GEN_FLOAT:
            ((float*)region)[offset] = amount->shortAmount;
            return;
            case GEN_INT:
            ((int16_t*)region)[offset] = amount->shortAmount;
            return;
            case GEN_UINT_ADD:
            ((uint32_t*)region)[offset] += amount->shortAmount;
            return;
            case GEN_UINT_ADD15:
            ((uint32_t*)region)[offset] += amount->shortAmount << 15;
            return;
            case GEN_KEYRANGE:
            region->lokey = amount->range.lo;
            region->hikey = amount->range.hi;
            return;
            case GEN_VELRANGE:
            region->lovel = amount->range.lo;
            region->hivel = amount->range.hi;
            return;
            case GEN_LOOPMODE:
            region->loop_mode = ((amount->wordAmount & 3) == 3 ? (int16_t)LoopMode::sustain : ((amount->wordAmount & 3) == 1 ? (int16_t)LoopMode::continuous : (int16_t)LoopMode::none));
            return;
            case GEN_GROUP:
            region->group = amount->wordAmount;
            return;
            case GEN_KEYCENTER:
            region->pitch_keycenter = amount->shortAmount;
            return;
        }
    } else // merge regions and clamp values
    {
        for( genOper = 0; genOper != _GEN_MAX; genOper++ )
        {
            int32_t offset = genMetas[genOper].offset;
            switch( genMetas[genOper].mode & _GEN_TYPE_MASK )
            {
                case GEN_FLOAT:
                {
                    float* val = &((float*)region)[offset], vfactor, vmin, vmax;
                    *val += ((float*)merge_region)[offset];
                    switch( genMetas[genOper].mode & _GEN_LIMIT_MASK )
                    {
                        case GEN_FLOAT_LIMIT12K5K:
                        vfactor = 1.0f;
                        vmin = -12000.0f;
                        vmax = 5000.0f;
                        break;
                        case GEN_FLOAT_LIMIT12K8K:
                        vfactor = 1.0f;
                        vmin = -12000.0f;
                        vmax = 8000.0f;
                        break;
                        case GEN_FLOAT_LIMIT1200:
                        vfactor = 1.0f;
                        vmin = -1200.0f;
                        vmax = 1200.0f;
                        break;
                        case GEN_FLOAT_LIMITPAN:
                        vfactor = 0.001f;
                        vmin = -0.5f;
                        vmax = 0.5f;
                        break;
                        case GEN_FLOAT_LIMITATTN:
                        vfactor = 0.1f;
                        vmin = 0.0f;
                        vmax = 144.0f;
                        break;
                        case GEN_FLOAT_MAX1000:
                        vfactor = 1.0f;
                        vmin = 0.0f;
                        vmax = 1000.0f;
                        break;
                        case GEN_FLOAT_MAX1440:
                        vfactor = 1.0f;
                        vmin = 0.0f;
                        vmax = 1440.0f;
                        break;
                        default:
                        continue;
                    }
                    *val *= vfactor;
                    if( *val < vmin )
                        *val = vmin;
                    else if( *val > vmax )
                        *val = vmax;
                    continue;
                }
                case GEN_INT:
                {
                    short* val = &((int16_t*)region)[offset], vmin, vmax;
                    *val += ((int16_t*)merge_region)[offset];
                    switch( genMetas[genOper].mode & _GEN_LIMIT_MASK )
                    {
                        case GEN_INT_LIMIT12K:
                        vmin = -12000;
                        vmax = 12000;
                        break;
                        case GEN_INT_LIMITFC:
                        vmin = 1500;
                        vmax = 13500;
                        break;
                        case GEN_INT_LIMITQ:
                        vmin = 0;
                        vmax = 960;
                        break;
                        case GEN_INT_LIMIT960:
                        vmin = -960;
                        vmax = 960;
                        break;
                        case GEN_INT_LIMIT16K4500:
                        vmin = -16000;
                        vmax = 4500;
                        break;
                        default:
                        continue;
                    }
                    if( *val < vmin )
                        *val = vmin;
                    else if( *val > vmax )
                        *val = vmax;
                    continue;
                }
                case GEN_UINT_ADD:
                {
                    ((uint32_t*)region)[offset] += ((uint32_t*)merge_region)[offset];
                    continue;
                }
            }
        }
    }
}

struct Preset
{
    ~Preset()
    {
        unload();
        refCount = 0;
    }
    void init( HydraPhdr& phdr, int32_t index )
    {
#ifndef TSF_NO_PRESET_NAME
        memcpy( presetName, phdr.presetName, 20 );
        presetName[sizeof( presetName ) - 1] = '\0'; // should be zero terminated in source file but make sure
#endif
        bank = phdr.bank;
        preset = phdr.preset;
        regionNum = 0;
        regions.clear();
        pphdrIdx = index;
        loaded = false;
        //printf("PRESET name:%s bank:%d preset:%d\n", presetName, bank, preset);
    }
    bool isLoaded()
    {
        return loaded;
    }

    void load( IStream* stream, Hydra* hydra, uint32_t sampleCount )
    {
        if( loaded )
        {
            return;
        }

        enum
        {
            GenInstrument = 41,
            GenKeyRange = 43,
            GenVelRange = 44,
            GenSampleID = 53
        };

        HydraPhdr pphdr;
        HydraPhdr nextpphdr;
        Region    globalRegion;
        int32_t   region_index = 0;

        stream->seek( hydra->phdrPos + pphdrIdx * phdrSizeInFile );
        pphdr.read( stream );

#ifndef TSF_NO_PRESET_NAME
        memcpy( presetName, pphdr.presetName, 20 );
        presetName[sizeof( presetName ) - 1] = '\0'; // should be zero terminated in source file but make sure
#endif
        bank = pphdr.bank;
        preset = pphdr.preset;
        pphdrIdx = pphdrIdx;

        //printf("Load PRESET name:%s bank:%d preset:%d\n", presetName, bank, preset);

        stream->seek( hydra->phdrPos + (pphdrIdx + 1) * phdrSizeInFile );
        nextpphdr.read( stream );

        // regions.reserve(regionNum);
        globalRegion.clear( true );

        // Zones.
        int32_t ppbagIdx;
        for( ppbagIdx = pphdr.presetBagNdx; ppbagIdx < nextpphdr.presetBagNdx; ppbagIdx++ )
        {
            HydraPbag ppbag, nextppbag;

            stream->seek( hydra->pbagPos + ppbagIdx * pbagSizeInFile );
            ppbag.read( stream );
            stream->seek( hydra->pbagPos + (ppbagIdx + 1) * pbagSizeInFile );
            nextppbag.read( stream );

            Region  presetRegion = globalRegion;
            int32_t hadGenInstrument = 0;

            // Generators.
            int32_t ppgenIdx;
            for( ppgenIdx = ppbag.genNdx; ppgenIdx < nextppbag.genNdx; ppgenIdx++ )
            {
                HydraPgen ppgen;

                stream->seek( hydra->pgenPos + ppgenIdx * pgenSizeInFile );
                ppgen.read( stream );

                // Instrument.
                if( ppgen.genOper == GenInstrument )
                {
                    HydraInst pinst, nextpinst;

                    Region  instRegion;
                    tsf_u16 whichInst = ppgen.genAmount.wordAmount;
                    if( whichInst >= hydra->instNum )
                        continue;

                    instRegion.clear( false );

                    stream->seek( hydra->instPos + whichInst * instSizeInFile );
                    pinst.read( stream );

                    stream->seek( hydra->instPos + (whichInst + 1) * instSizeInFile );
                    nextpinst.read( stream );

                    int32_t pibagIdx;
                    for( pibagIdx = pinst.instBagNdx; pibagIdx < nextpinst.instBagNdx; pibagIdx++ )
                    {
                        HydraIBag pibag, nextpibag;

                        stream->seek( hydra->ibagPos + pibagIdx * ibagSizeInFile );
                        pibag.read( stream );

                        stream->seek( hydra->ibagPos + (pibagIdx + 1) * ibagSizeInFile );
                        nextpibag.read( stream );

                        // Generators.
                        Region  zoneRegion = instRegion;
                        int32_t hadSampleID = 0;
                        int32_t pigenIdx;
                        for( pigenIdx = pibag.instGenNdx; pigenIdx < nextpibag.instGenNdx; pigenIdx++ )
                        {
                            HydraIGen pigen;

                            stream->seek( hydra->igenPos + pigenIdx * igenSizeInFile );
                            pigen.read( stream );

                            if( pigen.genOper == GenSampleID )
                            {
                                HydraSHdr pshdr;

                                // preset region key and vel ranges are a filter for the zone regions
                                if( zoneRegion.hikey < presetRegion.lokey || zoneRegion.lokey > presetRegion.hikey )
                                    continue;
                                if( zoneRegion.hivel < presetRegion.lovel || zoneRegion.lovel > presetRegion.hivel )
                                    continue;
                                if( presetRegion.lokey > zoneRegion.lokey )
                                    zoneRegion.lokey = presetRegion.lokey;
                                if( presetRegion.hikey < zoneRegion.hikey )
                                    zoneRegion.hikey = presetRegion.hikey;
                                if( presetRegion.lovel > zoneRegion.lovel )
                                    zoneRegion.lovel = presetRegion.lovel;
                                if( presetRegion.hivel < zoneRegion.hivel )
                                    zoneRegion.hivel = presetRegion.hivel;

                                // sum regions
                                tsf_region_operator( &zoneRegion, 0, nullptr, &presetRegion );

                                // EG times need to be converted from timecents to seconds.
                                zoneRegion.ampenv.tosecs( true );
                                zoneRegion.modenv.tosecs( false );

                                // LFO times need to be converted from timecents to seconds.
                                zoneRegion.delayModLFO = (zoneRegion.delayModLFO < -11950.0f ? 0.0f : timecents2Secsf( zoneRegion.delayModLFO ));
                                zoneRegion.delayVibLFO = (zoneRegion.delayVibLFO < -11950.0f ? 0.0f : timecents2Secsf( zoneRegion.delayVibLFO ));

                                // Fixup sample positions
                                // pshdr = &hydra->shdrs[pigen->genAmount.wordAmount];

                                stream->seek( hydra->shdrPos + pigen.genAmount.wordAmount * shdrSizeInFile );
                                pshdr.read( stream );

                                zoneRegion.offset += pshdr.start;
                                zoneRegion.end += pshdr.end;
                                zoneRegion.loop_start += pshdr.startLoop;
                                zoneRegion.loop_end += pshdr.endLoop;
                                if( pshdr.endLoop > 0 )
                                    zoneRegion.loop_end -= 1;
                                if( zoneRegion.pitch_keycenter == -1 )
                                    zoneRegion.pitch_keycenter = pshdr.originalPitch;
                                zoneRegion.tune += pshdr.pitchCorrection;
                                zoneRegion.sample_rate = pshdr.sampleRate;
                                if( zoneRegion.end && zoneRegion.end < sampleCount )
                                    zoneRegion.end++;
                                else
                                    zoneRegion.end = sampleCount;
                                regions.emplace_back( zoneRegion );
                                region_index++;
                                hadSampleID = 1;
                            } else
                                tsf_region_operator( &zoneRegion, pigen.genOper, &pigen.genAmount, nullptr );
                        }

                        // Handle instrument's global zone.
                        if( pibagIdx == pinst.instBagNdx && !hadSampleID )
                            instRegion = zoneRegion;

                        // Modulators (TODO)
                        // if (ibag->instModNdx < ibag[1].instModNdx) addUnsupportedOpcode("any modulator");
                    }
                    hadGenInstrument = 1;
                } else
                    tsf_region_operator( &presetRegion, ppgen.genOper, &ppgen.genAmount, nullptr );
            }

            // Modulators (TODO)
            // if (pbag->modNdx < pbag[1].modNdx) addUnsupportedOpcode("any modulator");

            // Handle preset's global zone.
            if( ppbagIdx == pphdr.presetBagNdx && !hadGenInstrument )
                globalRegion = presetRegion;
        }

        loaded = true;
    }
    void unload()
    {
        if( !loaded )
            return;
        //printf("unload %s", presetName);
        regions.clear();
        loaded = false;
    }

    int32_t              getPhdrIdx() { return pphdrIdx; }
    const char* getName() { return presetName; }
    uint16_t             getPresetNumber() { return preset; }
    uint16_t             getBankIndex() { return bank; }
    std::vector<Region>& getRegions() { return regions; }
    uint32_t             refCount = 0;
    int32_t              regionNum = 0;

private:
#ifndef TSF_NO_PRESET_NAME
    tsf_char20 presetName;
#endif
    tsf_u16             preset = 0, bank = 0;
    int32_t             pphdrIdx = 0;
    bool                loaded = false;
    std::vector<Region> regions;
};

// 2nd Order IIR Filter
struct peakingFilter
{
    float sampleTimesec;
    float x[3]; // Input samples x[0] = current Sample, x[1] = x[0] - 1, ...
    float y[3]; // Output samples y[0] = current Sample, y[1] = y[0] - 1, ...
    float a[3]; // x[n] coefficients
    float b[3]; // y[n] coefficients
    float wcT;
    float Q;
    float _gain; // < 0 : notch, 1.f = Allpass, >1.f boost

    void init( float sampleRateHz, float centerFreqHz, float bandwidth )
    {
        sampleTimesec = 1.f / sampleRateHz;
        wcT = 2.f * tanf( M_PI * centerFreqHz * sampleTimesec );
        Q = bandwidth / centerFreqHz;
        for( uint8_t n = 0; n < 3; n++ )
        {
            x[n] = 0.f;
            y[n] = 0.f;
        }
        _gain = 1.f;
        calculateCoefficients();
    }

    void setGain( float gain )
    {
        if( _gain != gain )
        {
            _gain = gain;
            calculateCoefficients();
        }
    }

    float process( float in )
    {
        x[2] = x[1];
        x[1] = x[0];
        x[0] = in;

        y[2] = y[1];
        y[1] = y[0];

        y[0] = b[0] * ((a[0] * x[0] + a[1] * x[1] + a[2] * x[2]) + (b[1] * y[1] + b[2] * y[2]));
        return y[0];
    }

    void calculateCoefficients()
    {
        float wcTsq = wcT * wcT;
        a[0] = 4.f + 2.f * (_gain / Q) * wcT + wcTsq;
        a[1] = 2.f * wcTsq - 8.f;
        a[2] = 4.f - 2.f * (_gain / Q) * wcT + wcTsq;

        b[0] = 1.f / (4.f + (2.f / Q) * wcT + wcTsq);
        b[1] = -(2.f * wcTsq - 8.f);
        b[2] = -(4.f - (2.f / Q) * wcT + wcTsq);
    }
};

struct EQ2Band
{
    peakingFilter lowband;
    peakingFilter highband;

    void init( float sampleRate, float fclow, float fchigh )
    {
        lowband.init( sampleRate, fclow, 400.f );
        highband.init( sampleRate, fchigh, 3000.f );
    }

    void setLowGain( uint8_t gain )
    {
        lowband.setGain( remap( gain, 0.f, 1.f ) );
    }

    void setHighGain( uint8_t gain )
    {
        highband.setGain( remap( gain, 0.f, 1.f ) );
    }

    float process( float in )
    {
        return highband.process( lowband.process( in ) );
    }
};

#define MAX_DELAY_LINES 4
#define MAX_DELAY_LENGTH 8000
#define LFO_FREQUENCY 0.2f
#define MIXING_RATIO 0.7f

struct Chorus
{
    float delay_lines[MAX_DELAY_LINES][MAX_DELAY_LENGTH] = { 0.0f };
    float delay_lengths[MAX_DELAY_LINES] = { 0.0f };
    int read_index = 0;
    int write_index = 0;
    float lfo_phase = 0.0f;
    float chorus_amount = 0.0f;
    uint16_t sampleRate = 48000;

    void init( uint16_t sr )
    {
        for( int i = 0; i < MAX_DELAY_LINES; i++ )
        {
            for( int j = 0; j < MAX_DELAY_LENGTH; j++ )
            {
                delay_lines[i][j] = 0.0f;
            }
            delay_lengths[i] = (i + 1) * (MAX_DELAY_LENGTH / (MAX_DELAY_LINES + 1));
        }
        read_index = 0;
        write_index = 0;
        lfo_phase = 0.0f;
        chorus_amount = 0.0f;
        sampleRate = sr;
    }

    void set_chorus_amount( uint8_t chorus_param )
    {
        clamp( remap( chorus_param, 0.f, 0.6f ), 0.f, 0.6f );
    }

    float process_sample( float input_sample )
    {
        float output_sample = input_sample;
        for( int i = 0; i < MAX_DELAY_LINES; ++i )
        {
            // Calculate delay time modulation
            float modulation = sin( 2.0f * M_PI * lfo_phase );
            float delay_time = delay_lengths[i] + modulation * (MAX_DELAY_LENGTH / (MAX_DELAY_LINES + 1));

            // Ensure delay time remains within the valid range
            if( delay_time < 0.0f )
            {
                delay_time = 0.0f;
            } else if( delay_time >= MAX_DELAY_LENGTH )
            {
                delay_time = MAX_DELAY_LENGTH - 1;
            }

            // Get fractional and integer parts of the delay time
            int index = static_cast<int>(delay_time);
            float fraction = delay_time - index;

            // Get delayed samples from the delay line and perform cubic interpolation
            int prev_index = (index - 1 + MAX_DELAY_LENGTH) % MAX_DELAY_LENGTH;
            int next_index = (index + 1) % MAX_DELAY_LENGTH;
            int next2_index = (index + 2) % MAX_DELAY_LENGTH;

            // Get delayed samples from the delay line and perform linear interpolation
            float delayed_sample = (1.0f - fraction) * delay_lines[i][prev_index] + fraction * delay_lines[i][next_index];

            // Apply low-pass filtering (you can change the filter parameters)
            float alpha = 0.2f;
            delayed_sample = alpha * delayed_sample + (1.0f - alpha) * delay_lines[i][prev_index];

            // Mix the delayed sample with the input sample
            output_sample += chorus_amount * MIXING_RATIO * delayed_sample;

            // Store the mixed sample in the delay line
            delay_lines[i][write_index] = input_sample + delayed_sample * 0.5f;
        }

        write_index = (write_index + 1) % MAX_DELAY_LENGTH;

        // Update LFO phase
        lfo_phase += LFO_FREQUENCY / sampleRate;
        if( lfo_phase > 1.0f )
        {
            lfo_phase -= 1.0f;
        }

        return output_sample;
    }
};

#define BUFFER_SIZE 1024

// Comb filter struct
struct CombFilter
{
    float delayTime;
    float feedbackGain;
    float* delayLine;
    uint32_t delayLength;
    uint32_t delayIndex;
    void initializeCombFilter( float delayTime, float feedbackGain, uint32_t sampleRate )
    {
        delayTime = delayTime;
        feedbackGain = feedbackGain;
        delayLength = (uint32_t)(delayTime * sampleRate);
        delayLine = (float*)malloc( delayLength * sizeof( float ) );
        delayIndex = 0;

        memset( delayLine, 0, delayLength * sizeof( float ) );
    }

    float processCombFilter( float input )
    {
        float output = delayLine[delayIndex];
        delayLine[delayIndex] = input + feedbackGain * output;

        delayIndex = (delayIndex + 1) % delayLength;
        return output;
    }
};

// All-pass filter struct
struct AllPassFilter
{
    float delayTime;
    float gain;
    float* delayLine;
    uint32_t delayLength;
    uint32_t delayIndex;

    void initializeAllPassFilter( float delayTime, float gain, uint32_t sampleRate )
    {
        delayTime = delayTime;
        gain = gain;
        delayLength = (uint32_t)(delayTime * sampleRate);
        delayLine = (float*)malloc( delayLength * sizeof( float ) );
        delayIndex = 0;

        memset( delayLine, 0, delayLength * sizeof( float ) );
    }

    float processAllPassFilter( float input )
    {
        float output = input - gain * delayLine[delayIndex];
        delayLine[delayIndex] = input + gain * output;

        delayIndex++;
        if( delayIndex >= delayLength )
        {
            delayIndex = 0;
        }

        return output;
    }
};

// Reverb struct
struct Reverb
{
    CombFilter combFilters[4];
    AllPassFilter allPassFilters[2];
    float effectLevel = 0.f;

    void init( uint32_t sampleRate )
    {
        combFilters[0].initializeCombFilter( 0.0131f, -0.5f, sampleRate );
        combFilters[1].initializeCombFilter( 0.0173f, -0.5f, sampleRate );
        combFilters[2].initializeCombFilter( 0.0209f, -0.5f, sampleRate );
        combFilters[3].initializeCombFilter( 0.0287f, -0.5f, sampleRate );

        allPassFilters[0].initializeAllPassFilter( 0.0191f, 0.7f, sampleRate );
        allPassFilters[1].initializeAllPassFilter( 0.0233f, 0.7f, sampleRate );
        effectLevel = 0.0f;
    }

    float process( float in )
    {
        float combOutput = 0.0f;
        for( uint8_t j = 0; j < 4; j++ )
        {
            combOutput += combFilters[j].processCombFilter( in );
        }

        float allPassOutput = combOutput;
        for( uint8_t j = 0; j < 2; j++ )
        {
            allPassOutput = allPassFilters[j].processAllPassFilter( allPassOutput );
        }

        float output = (effectLevel * allPassOutput) + ((1.0f - effectLevel) * in);
        return allPassOutput;
    }

    void setReverb( uint8_t level )
    {
        float newLevel = remap( level, 0.f, 1.f );
        if( !isEqual( newLevel, effectLevel ) )
        {
            effectLevel = newLevel;
        }
    }
};

struct tsf
{
    std::vector<Preset>        presets;
    IStream* stream = nullptr;
    Hydra                      hydra;
    int16_t* fontSamples = nullptr;
    uint32_t                   fontSamplesOffset = 0;
    uint32_t                   fontSampleCount = 0;
    std::vector<Voice>         voices;
    std::array<TSFChannel, 16> channels;
    uint32_t                   activeChannel = 0;
    int32_t                    outputSampleSize = 0;
    uint32_t                   voicePlayIndex = 0;


    enum OutputMode outputmode = OutputMode::StereoInterleaved;
    float           outSampleRate = 0.0f;
    float           globalGainDB = 0.0f;

    uint16_t gc = 0;

    float* renderBuffer[2] = { nullptr, nullptr };

    float lowpassFc = 0.90f;
    float lowpassQ = 0.0f;
    float filterAttack = 0.0f;
    float filterRelease = 0.0f;
    struct Chorus _chorus;
    struct EQ2Band _eq;
    struct Reverb _reverb;

    tsf()
        : voices( 32, Voice{} )
    {}
    ~tsf()
    {
    }
    void setLowPassCutoff( uint8_t val )
    {
        if( val <= 0x7f )
        {
            auto newVal = 1 - clamp( remap( val, 0.1f, 0.90f ), 0.1f, 0.90f );
            lowpassFc = newVal;
        }
    }

    void setLowPassResonance( uint8_t val )
    {
        if( val <= 0x7f )
        {
            auto newVal = clamp( remap( val, 0.1f, 0.9f ), 0.1f, 0.9f );
            lowpassQ = newVal;
        }
    }

    void setLowPassAttack( uint8_t val )
    {
        if( val <= 0x7f )
        {
            auto newVal = clamp( remap( val, 00.f, 1.f ), 0.f, 1.f );
            filterAttack = newVal;
        }
    }

    void setLowPassRelease( uint8_t val )
    {
        if( val <= 0x7f )
        {
            auto newVal = clamp( remap( val, 0.f, 1.f ), 0.f, 1.f );
            filterRelease = newVal;
        }
    }

    void setReverb( uint8_t val )
    {
        if( val <= 0x7f )
        {
            _reverb.setReverb( val );
        }
    }

    void setChorus( uint8_t val )
    {
        if( val <= 0x7f )
        {
            _chorus.set_chorus_amount( val );
        }
    }

    void setEqLow( uint8_t val )
    {
        if( val <= 0x7f )
        {
            _eq.setLowGain( val );
        }
    }

    void setEqHigh( uint8_t val )
    {
        if( val <= 0x7f )
        {
            _eq.setHighGain( val );
        }
    }

    Voice* getReusableVoice( float cap )
    {
        Voice* reuseVoice = nullptr;
        float minLevel = 2.0f;
        for( auto& v : voices )
        {
            TSF_MUTEX_LOCK( v.mutex );
            if( v.locked == 0 && v.ampenv.isPostAttack() )
            {
                minLevel = v.ampenv.level;
                reuseVoice = &v;
            }
            TSF_MUTEX_UNLOCK( v.mutex );
        }

        if( minLevel < cap )
            return reuseVoice;
        else
            return nullptr;
    }

    void presetNoteOn( int32_t preset_index, int32_t key, float vel )
    {
        int16_t midiVelocity = (int16_t)(vel * 127);
        int32_t voiceIndex;

        if( preset_index < 0 && preset_index >= presets.size() )
            return;
        if( vel <= 0.0f )
        {
            presetNoteOff( preset_index, key );
            return;
        }
        auto& preset = presets[preset_index];

        if( !preset.isLoaded() )
        {
            preset.load( stream, &hydra, fontSampleCount );
        }

        // Play all matching regions.
        voiceIndex = voicePlayIndex++;
        for( auto& region : preset.getRegions() )
        {
            Voice* voice = nullptr;
            if( key < region.lokey || key > region.hikey || midiVelocity < region.lovel || midiVelocity > region.hivel )
                continue;
            for( auto& v : voices )
            {
                if( region.group )
                {
                    if( v.playingPreset == preset_index && v.region->group == region.group )
                        v.endquick();
                    else if( v.playingPreset == -1 && !voice )
                        voice = &v;
                } else
                {
                    if( v.playingPreset == -1 )
                    {
                        voice = &v;
                        break;
                    }
                }
            }

            if( !voice )
            {
                voice = getReusableVoice( TSF_REUSE_LEVEL );
            }

            if( voice )
            {
                TSF_MUTEX_LOCK( voice->mutex );
                voice->locked = 1;
                voice->region = &region;
                voice->playingPreset = preset_index;
                voice->playingKey = key;
                voice->velocity = midiVelocity;
                voice->playIndex = voiceIndex;
                voice->noteGainDB = globalGainDB - region.attenuation - gainToDecibels( 1.0f / vel );

                if( channels.size() )
                {
                    auto ch = getChannel( activeChannel );
                    voice->setup( *ch, activeChannel, outSampleRate );
                } else
                {
                    voice->calcpitchratio( 0, outSampleRate );
                    // The SFZ spec is silent about the pan curve, but a 3dB pan law seems common. This sqrt() curve matches what Dimension LE does; Alchemy Free seems closer to sin(adjustedPan * pi/2).
                    voice->panFactorLeft = sqrtf( 0.5f - region.pan );
                    voice->panFactorRight = sqrtf( 0.5f + region.pan );
                }

                // Offset/end.
                voice->sourceSamplePosition = region.offset;

                // Loop.
                bool doLoop = (region.loop_mode != LoopMode::none && region.loop_start < region.loop_end);
                voice->loopStart = (doLoop ? region.loop_start : 0);
                voice->loopEnd = (doLoop ? region.loop_end : 0);

                // Setup envelopes.
                voice->setAmpEnvelope( outSampleRate );
                voice->setModEnvelope( outSampleRate );
                // Setup lowpass filter.
                voice->filter.init( lowpassFc, lowpassQ );
                voice->filterEnv.noteOn( lowpassFc, filterAttack, outSampleRate );

                // Setup LFO filters.
                voice->setModLFO( outSampleRate );
                voice->setVibLFO( outSampleRate );
                voice->locked = 0;
                TSF_MUTEX_UNLOCK( voice->mutex );
            }
        }
    }

    void presetNoteOff( int32_t preset_index, int32_t key )
    {
        Voice* vMatchFirst = nullptr, * vMatchLast = nullptr;
        for( auto& v : voices )
        {
            // Find the first and last entry in the voices list with matching preset, key and look up the smallest play index
            if( v.playingPreset != preset_index || v.playingKey != key || v.ampenv.isReleased() )
                continue;
            else if( !vMatchFirst || v.playIndex < vMatchFirst->playIndex )
                vMatchFirst = vMatchLast = &v;
            else if( v.playIndex == vMatchFirst->playIndex )
                vMatchLast = &v;
        }
        if( !vMatchFirst )
            return;
        for( auto v = vMatchFirst; v <= vMatchLast; v++ )
        {
            // Stop all voices with matching preset, key and the smallest play index which was enumerated above
            if( v != vMatchFirst && v != vMatchLast && (v->playIndex != vMatchFirst->playIndex || v->playingPreset != preset_index || v->playingKey != key || v->ampenv.isReleased()) )
                continue;
            v->filterEnv.noteOff( filterRelease, outSampleRate );
            v->end();
        }
    }

    void noteOffAll( bool quick = false )
    {
        for( auto& v : voices )
        {
            if( v.playingPreset != -1 && v.ampenv.notReleased() )
                if( quick )
                    v.endquick();
                else
                    v.end();
        }
    }

    TSFChannel* getChannel( int32_t channel )
    {
        if( channel < channels.size() )
            return &channels[channel];
        return nullptr;
    }

    void channelApplyPitch( int32_t channel, TSFChannel* c )
    {
        float pitchShift = (c->pitchWheel == 8192 ? c->tuning : ((c->pitchWheel / 16383.0f * c->pitchRange * 2.0f) - c->pitchRange + c->tuning));
        for( auto& v : voices )
        {
            if( v.playingChannel == channel && v.playingPreset != -1 )
                v.calcpitchratio( pitchShift, outSampleRate );
        }
    }

    int32_t setChannelPresetNumber( int32_t channel, int32_t preset_number, int32_t flag_mididrums )
    {
        auto    c = getChannel( channel );
        int32_t preset_index;

        if( flag_mididrums )
        {
            preset_index = getBankPresetIndex( 128 | (c->bank & 0x7FFF), preset_number );
            if( preset_index == -1 )
                preset_index = getBankPresetIndex( 128, preset_number );
            if( preset_index == -1 )
                preset_index = getBankPresetIndex( 128, 0 );
            if( preset_index == -1 )
                preset_index = getBankPresetIndex( (c->bank & 0x7FFF), preset_number );
        } else
            preset_index = getBankPresetIndex( (c->bank & 0x7FFF), preset_number );
        if( preset_index == -1 )
            preset_index = getBankPresetIndex( 0, preset_number );
        if( preset_index != -1 )
        {
            //printf("channel_set_presetnumber preset:%d channel:%d\n", preset_number, channel);
            c->setPresetIndex( (uint16_t)preset_index );
            return 1;
        }
        return 0;
    }

    int32_t setChannelBankPreset( int32_t channel, int32_t bank, int32_t preset_number )
    {
        auto    c = getChannel( channel );
        int32_t preset_index = getBankPresetIndex( bank, preset_number );
        if( preset_index == -1 )
            return 0;
        c->presetIndex = (uint16_t)preset_index;
        c->bank = (uint16_t)bank;
        return 1;
    }

    void setChannelPan( int32_t channel, float pan )
    {
        for( auto& v : voices )
        {
            if( v.playingChannel == channel && v.playingPreset != -1 )
            {
                float newpan = v.region->pan + pan - 0.5f;
                if( newpan <= -0.5f )
                {
                    v.panFactorLeft = 1.0f;
                    v.panFactorRight = 0.0f;
                } else if( newpan >= 0.5f )
                {
                    v.panFactorLeft = 0.0f;
                    v.panFactorRight = 1.0f;
                } else
                {
                    v.panFactorLeft = sqrtf( 0.5f - newpan );
                    v.panFactorRight = sqrtf( 0.5f + newpan );
                }
            }
        }
        getChannel( channel )->panOffset = pan - 0.5f;
    }

    void setChannelVolume( int32_t channel, float volume )
    {
        auto  c = getChannel( channel );
        float gainDB = gainToDecibels( volume ), gainDBChange = gainDB - c->gainDB;
        if( gainDBChange == 0 )
            return;
        for( auto& v : voices )
        {
            if( v.playingChannel == channel && v.playingPreset != -1 )
                v.noteGainDB += gainDBChange;
        }
        c->gainDB = gainDB;
    }

    void setPitchWheel( int32_t channel, int32_t pitch_wheel )
    {
        auto c = getChannel( channel );
        if( c->pitchWheel == pitch_wheel )
            return;
        c->pitchWheel = (uint16_t)pitch_wheel;
        channelApplyPitch( channel, c );
    }

    void setPitchRange( int32_t channel, float pitch_range )
    {
        auto c = getChannel( channel );
        if( c->pitchRange == pitch_range )
            return;
        c->pitchRange = pitch_range;
        if( c->pitchWheel != 8192 )
            channelApplyPitch( channel, c );
    }

    void setTuning( int32_t channel, float tuning )
    {
        auto c = getChannel( channel );
        if( c->tuning == tuning )
            return;
        c->tuning = tuning;
        channelApplyPitch( channel, c );
    }

    void channelNoteOn( int32_t channel, int32_t key, float vel )
    {
        auto ch = getChannel( channel );
        if( ch )
        {
            activeChannel = channel;
            presetNoteOn( ch->getPresetIndex(), key, vel );
        }
    }

    void channelNoteOff( int32_t channel, int32_t key )
    {
        Voice* vMatchFirst = nullptr, * vMatchLast = nullptr;
        for( auto& v : voices )
        {
            // Find the first and last entry in the voices list with matching channel, key and look up the smallest play index
            if( v.playingPreset == -1 || v.playingChannel != channel || v.playingKey != key || v.ampenv.isReleased() )
                continue;
            else if( !vMatchFirst || v.playIndex < vMatchFirst->playIndex )
                vMatchFirst = vMatchLast = &v;
            else if( v.playIndex == vMatchFirst->playIndex )
                vMatchLast = &v;
        }
        if( !vMatchFirst )
            return;
        for( auto v = vMatchFirst; v <= vMatchLast; v++ )
        {
            // Stop all voices with matching channel, key and the smallest play index which was enumerated above
            if( v != vMatchFirst && v != vMatchLast && (v->playIndex != vMatchFirst->playIndex || v->playingPreset == -1 || v->playingChannel != channel || v->playingKey != key || v->ampenv.isReleased()) )
                continue;
            v->end();
        }
    }

    void channelNoteOffAll( int32_t channel )
    {
        for( auto& v : voices )
        {
            if( v.playingPreset != -1 && v.playingChannel == channel && v.ampenv.notReleased() )
                v.end();
        }
    }

    void channelSoundOffAll( int32_t channel )
    {
        for( auto& v : voices )
        {
            if( v.playingPreset != -1 && v.playingChannel == channel && (v.ampenv.notReleased() || v.ampenv.parameters.release) )
                v.endquick();
        }
    }

    void reset()
    {
        for( auto& v : voices )
        {
            if( v.playingPreset != -1 && (v.ampenv.notReleased() || v.ampenv.parameters.release) )
                v.endquick();
        }
        channels.fill( TSFChannel() );
    }

    int32_t getBankPresetIndex( int32_t bank, int32_t preset_number )
    {
        int32_t i = 0;
        for( auto& preset : presets )
        {
            if( preset.getPresetNumber() == preset_number && preset.getBankIndex() == bank )
            {
                return i;
            }
            i++;
        }
        return -1;
    }

    uint32_t getPresetCount()
    {
        return (uint32_t)presets.size();
    }

    const char* getPresetName( int32_t preset )
    {
#ifndef TSF_NO_PRESET_NAME
        return (preset >= presets.size() ? nullptr : presets[preset].getName());
#else
        return nullptr;
#endif
    }

    const char* getBankPresetName( int32_t bank, int32_t preset_number )
    {
        return getPresetName( getBankPresetIndex( bank, preset_number ) );
    }

    void setOutputSpecs( enum OutputMode mode, size_t blocksize, int32_t samplerate, float global_gain_db )
    {
        if( renderBuffer[0] )
            delete[] renderBuffer[0];
        if( renderBuffer[1] )
            delete[] renderBuffer[1];
        ;
        outputmode = mode;
        renderBuffer[0] = new float[blocksize];
        renderBuffer[1] = outputmode != Mono ? new float[blocksize] : nullptr;
        outSampleRate = (float)(samplerate >= 1 ? samplerate : 44100.0f);
        globalGainDB = global_gain_db;
    }

    void setVolume( float global_volume )
    {
        globalGainDB = (global_volume == 1.0f ? 0 : -gainToDecibels( 1.0f / global_volume ));
    }

    int32_t getActiveVoiceCount()
    {
        int32_t count = 0;
        for( auto& v : voices )
        {
            if( v.playingPreset != -1 )
                count++;
        }
        return count;
    }

    void collectGarbage()
    {
        if( gc == TSF_GC_F )
        {
            gc = 0;
            for( auto& v : voices )
            {
                if( v.playingPreset != -1 )
                {
                    presets[v.playingPreset].refCount++;
                }
            }

            for( auto& preset : presets )
            {
                if( preset.refCount == 0 && preset.isLoaded() )
                {
                    preset.unload();
                }
                preset.refCount = 0;
            }
        }
        gc++;
    }

    int32_t getChannelPresetIndex( int32_t channel )
    {
        auto ch = getChannel( channel );
        return ch ? ch->getPresetIndex() : 0;
    }

    int32_t getPresetBank( int32_t channel )
    {
        auto ch = getChannel( channel );
        return ch ? ch->getPresetBank() : 0;
    }

    int32_t getChannelPresetNumber( int32_t channel )
    {
        auto ch = getChannel( channel );
        return ch ? presets[ch->getPresetIndex()].getPresetNumber() : 0;
    }

    float getPan( int32_t channel )
    {
        auto ch = getChannel( channel );
        return ch ? ch->getPan() : 0.5f;
    }

    float getVolume( int32_t channel )
    {
        auto ch = getChannel( channel );
        return ch ? ch->getVolume() : 1.0f;
    }

    int32_t getPitchWheel( int32_t channel )
    {
        auto ch = getChannel( channel );
        return ch ? ch->getPitchWheel() : 8192;
    }

    float getPitchRange( int32_t channel )
    {
        auto ch = getChannel( channel );
        return ch ? ch->getPitchRange() : 2.0f;
    }

    float getTuning( int32_t channel )
    {
        auto ch = getChannel( channel );
        return ch ? ch->getTuning() : 0.0f;
    }

    void render( short* outbuffer, size_t blocksize )
    {
        render( renderBuffer, blocksize );
        for( auto ch = 0; ch < (outputmode == Mono ? 1 : 2); ch++ )
        {
            for( auto sample = 0; sample < blocksize; sample++ )
            {
                auto  v = renderBuffer[ch][sample];
                short val = (v < -1.00004566f ? (short)-32768 : (v > 1.00001514f ? (short)32767 : (short)(v * 32767.5f)));

                switch( outputmode )
                {
                    case StereoInterleaved:
                    outbuffer[(2 * sample) + ch] = val;
                    break;
                    case StereoUnweaved:
                    case Mono:
                    outbuffer[(ch * blocksize) + sample] = val;
                    break;
                    default:
                    break;
                }
            }
        }
    }

    void render( long* outbuffer, size_t blocksize )
    {
        render( renderBuffer, blocksize );
        for( auto ch = 0; ch < (outputmode == Mono ? 1 : 2); ch++ )
        {
            for( auto sample = 0; sample < blocksize; sample++ )
            {
                auto v = renderBuffer[ch][sample];
                long val = (v < -1.00004566f ? (short)-32768 : (v > 1.00001514f ? (short)32767 : (short)(v * 32767.5f)));

                switch( outputmode )
                {
                    case StereoInterleaved:
                    outbuffer[(2 * sample) + ch] = val;
                    break;
                    case StereoUnweaved:
                    case Mono:
                    outbuffer[(ch * blocksize) + sample] = val;
                    break;
                    default:
                    break;
                }
            }
        }
    }

    void render( float* outbuffer, size_t blocksize )
    {
        render( renderBuffer, blocksize );
        for( auto ch = 0; ch < (outputmode == Mono ? 1 : 2); ch++ )
        {
            for( auto sample = 0; sample < blocksize; sample++ )
            {
                float val = renderBuffer[ch][sample];

                switch( outputmode )
                {
                    case StereoInterleaved:
                    outbuffer[(2 * sample) + ch] = val;
                    break;
                    case StereoUnweaved:
                    case Mono:
                    outbuffer[(ch * blocksize) + sample] = val;
                    break;
                    default:
                    break;
                }
            }
        }
    }

    void render( float** buffer, size_t blocksize )
    {
        // collectGarbage();
        memset( buffer[0], 0, sizeof( float ) * blocksize );
        if( outputmode != Mono )
        {
            memset( buffer[1], 0, sizeof( float ) * blocksize );
        }
        for( auto& v : voices )
        {
            //if( v.playingPreset != -1 )
            //    renderVoice( v, buffer, (int)blocksize );
        }
        float* output = buffer[0];
        for( uint16_t sample = 0; sample < blocksize; sample++ )
        {
            output[sample] = _reverb.process( output[sample] );
        }
        for( uint16_t sample = 0; sample < blocksize; sample++ )
        {
            output[sample] = _chorus.process_sample( output[sample] );
        }
        for( uint16_t sample = 0; sample < blocksize; sample++ )
        {
            output[sample] = _eq.process( output[sample] );
        }
    }

    void renderMono( float* buffer, size_t blocksize )
    {
        // collectGarbage();
        memset( buffer, 0, sizeof( float ) * blocksize );

        for( auto& v : voices )
        {
            if( v.playingPreset != -1 )
                renderVoice( v, buffer, (int)blocksize );
        }
        for( uint16_t sample = 0; sample < blocksize; sample++ )
        {
            buffer[sample] = _reverb.process( buffer[sample] );
        }
        for( uint16_t sample = 0; sample < blocksize; sample++ )
        {
            buffer[sample] = _chorus.process_sample( buffer[sample] );
        }
        for( uint16_t sample = 0; sample < blocksize; sample++ )
        {
            buffer[sample] = _eq.process( buffer[sample] );
        }
    }

    void renderVoice( Voice& v, float* outputBuffer, int blockSize )
    {
        auto region = v.region;
        if( !region )
            return;
        int16_t* input = fontSamplesOffset + fontSamples;
        if( !outputBuffer )
            return;

        // Cache some values, to give them at least some chance of ending up in registers.
        bool         updateModEnv = (region->modEnvToPitch || region->modEnvToFilterFc);
        bool         updateModLFO = (v.modlfo.delta && (region->modLfoToPitch || region->modLfoToFilterFc || region->modLfoToVolume));
        bool         updateVibLFO = (v.viblfo.delta && (region->vibLfoToPitch));
        bool         isLooping = v.isLooping();
        unsigned int tmpLoopStart = v.loopStart, tmpLoopEnd = v.loopEnd;
        double       tmpSampleEndDbl = (double)region->end, tmpLoopEndDbl = (double)tmpLoopEnd + 1.0;
        double       tmpSourceSamplePosition = v.sourceSamplePosition;
        //VoiceLowPass tmpLowpass = v.lowpass;

        bool  dynamicLowpass = (region->modLfoToFilterFc || region->modEnvToFilterFc);
        float tmpSampleRate = outSampleRate, tmpInitialFilterFc, tmpModLfoToFilterFc, tmpModEnvToFilterFc;

        bool   dynamicPitchRatio = (region->modLfoToPitch || region->modEnvToPitch || region->vibLfoToPitch);
        double pitchRatio;
        float  tmpModLfoToPitch, tmpVibLfoToPitch, tmpModEnvToPitch;

        bool  dynamicGain = (region->modLfoToVolume != 0);
        float noteGain = 0, tmpModLfoToVolume;

        //if( dynamicLowpass )
        //    tmpInitialFilterFc = (float)region->initialFilterFc, tmpModLfoToFilterFc = (float)region->modLfoToFilterFc, tmpModEnvToFilterFc = (float)region->modEnvToFilterFc;
        //else
        //    tmpInitialFilterFc = 0, tmpModLfoToFilterFc = 0, tmpModEnvToFilterFc = 0;

        if( dynamicPitchRatio )
            pitchRatio = 0, tmpModLfoToPitch = (float)region->modLfoToPitch, tmpVibLfoToPitch = (float)region->vibLfoToPitch, tmpModEnvToPitch = (float)region->modEnvToPitch;
        else
            pitchRatio = timecents2Secsf( v.pitchInputTimecents ) * v.pitchOutputFactor, tmpModLfoToPitch = 0, tmpVibLfoToPitch = 0, tmpModEnvToPitch = 0;

        if( dynamicGain )
            tmpModLfoToVolume = (float)region->modLfoToVolume * 0.1f;
        else
            noteGain = decibelsToGain( v.noteGainDB ), tmpModLfoToVolume = 0;

        float gainMono, gainLeft, gainRight;

        //if( dynamicLowpass )
        //{
        //    float fres = tmpInitialFilterFc + v.modlfo.level * tmpModLfoToFilterFc + v.modenv.level * tmpModEnvToFilterFc;
        //    float lowpassFc = (fres <= 13500 ? cents2Hertz( fres ) / tmpSampleRate : 1.0f);
        //    tmpLowpass.setCutOff( lowpassFc );
        //}

        if( dynamicPitchRatio )
            pitchRatio = timecents2Secsf( v.pitchInputTimecents + (v.modlfo.level * tmpModLfoToPitch + v.viblfo.level * tmpVibLfoToPitch + v.modenv.level * tmpModEnvToPitch) ) * v.pitchOutputFactor;

        if( dynamicGain )
            noteGain = decibelsToGain( v.noteGainDB + (v.modlfo.level * tmpModLfoToVolume) );

        gainMono = noteGain * v.ampenv.level;

        // Update EG.
        v.ampenv.process( blockSize );

        v.filter.setCutoff( v.filterEnv.process( blockSize ) );

        if( updateModEnv )
            v.modenv.process( blockSize );

        // Update LFOs.
        if( updateModLFO )
            v.modlfo.process( blockSize );
        if( updateVibLFO )
            v.viblfo.process( blockSize );

        while( blockSize-- && tmpSourceSamplePosition < tmpSampleEndDbl )
        {
            unsigned int pos = (unsigned int)tmpSourceSamplePosition, nextPos = (pos >= tmpLoopEnd && isLooping ? tmpLoopStart : pos + 1);
            // Simple linear interpolation.
            float alpha = (float)(tmpSourceSamplePosition - pos), val = (fixed_to_float( input[pos] ) * (1.0f - alpha) + fixed_to_float( input[nextPos] ) * alpha);
            // Low-pass filter.
            val = v.filter.process( val );
            *outputBuffer++ += val * gainMono;
            
            // Next sample.
            tmpSourceSamplePosition += pitchRatio;
            if( tmpSourceSamplePosition >= tmpLoopEndDbl && isLooping )
                tmpSourceSamplePosition -= (tmpLoopEnd - tmpLoopStart + 1.0);
        }
        if( tmpSourceSamplePosition >= tmpSampleEndDbl || v.ampenv.isIdle() )
        {
            v.kill();
            return;
        }

        v.sourceSamplePosition = tmpSourceSamplePosition;
        //if( tmpLowpass.isActive() || dynamicLowpass )
        //    v.lowpass = tmpLowpass;
    }

    void channelMidiControl( int32_t channel, int32_t controller, int32_t control_value )
    {
        auto c = getChannel( channel );
        switch( controller )
        {
            case 7 /*VOLUME_MSB*/:
            c->midiVolume = (uint16_t)((c->midiVolume & 0x7F) | (control_value << 7));
            goto TCMC_SET_VOLUME;
            case 39 /*VOLUME_LSB*/:
            c->midiVolume = (uint16_t)((c->midiVolume & 0x3F80) | control_value);
            goto TCMC_SET_VOLUME;
            case 11 /*EXPRESSION_MSB*/:
            c->midiExpression = (uint16_t)((c->midiExpression & 0x7F) | (control_value << 7));
            goto TCMC_SET_VOLUME;
            case 43 /*EXPRESSION_LSB*/:
            c->midiExpression = (uint16_t)((c->midiExpression & 0x3F80) | control_value);
            goto TCMC_SET_VOLUME;
            case 10 /*PAN_MSB*/:
            c->midiPan = (uint16_t)((c->midiPan & 0x7F) | (control_value << 7));
            goto TCMC_SET_PAN;
            case 42 /*PAN_LSB*/:
            c->midiPan = (uint16_t)((c->midiPan & 0x3F80) | control_value);
            goto TCMC_SET_PAN;
            case 6 /*DATA_ENTRY_MSB*/:
            c->midiData = (uint16_t)((c->midiData & 0x7F) | (control_value << 7));
            goto TCMC_SET_DATA;
            case 38 /*DATA_ENTRY_LSB*/:
            c->midiData = (uint16_t)((c->midiData & 0x3F80) | control_value);
            goto TCMC_SET_DATA;
            case 0 /*BANK_SELECT_MSB*/:
            c->bank = (uint16_t)(0x8000 | control_value);
            return; // bank select MSB alone acts like LSB
            case 32 /*BANK_SELECT_LSB*/:
            c->bank = (uint16_t)((c->bank & 0x8000 ? ((c->bank & 0x7F) << 7) : 0) | control_value);
            return;
            case 101 /*RPN_MSB*/:
            c->midiRPN = (uint16_t)(((c->midiRPN == 0xFFFF ? 0 : c->midiRPN) & 0x7F) | (control_value << 7));
            return;
            case 100 /*RPN_LSB*/:
            c->midiRPN = (uint16_t)(((c->midiRPN == 0xFFFF ? 0 : c->midiRPN) & 0x3F80) | control_value);
            return;
            case 98 /*NRPN_LSB*/:
            c->midiRPN = 0xFFFF;
            return;
            case 99 /*NRPN_MSB*/:
            c->midiRPN = 0xFFFF;
            return;
            case 120 /*ALL_SOUND_OFF*/:
            channelSoundOffAll( channel );
            return;
            case 123 /*ALL_NOTES_OFF*/:
            channelNoteOffAll( channel );
            return;
            case 121 /*ALL_CTRL_OFF*/:
            c->midiVolume = c->midiExpression = 16383;
            c->midiPan = 8192;
            c->bank = 0;
            setChannelVolume( channel, 1.0f );
            setChannelPan( channel, 0.5f );
            setPitchRange( channel, 2.0f );
            return;
            case 91 /* reverb */:
            c->reverb = (float)control_value / 127.0f; /* printf("TSF send reverb chan:%d val:%f\n", channel, c->reverb); */
            return;
            case 93 /* chorus */:
            c->chorus = (float)control_value / 127.0f; /* printf("TSF send chorus chan:%d val:%f\n", channel, c->chorus); */
            return;
            // default: printf("TSF UNSUPPORTED CC %d(%x) : %d %d\n",controller,controller,channel,control_value); return;
        }
        return;
TCMC_SET_VOLUME:
        // Raising to the power of 3 seems to result in a decent sounding volume curve for MIDI
        setChannelVolume( channel, powf( (c->midiVolume / 16383.0f) * (c->midiExpression / 16383.0f), 3.0f ) );
        return;
TCMC_SET_PAN:
        setChannelPan( channel, c->midiPan / 16383.0f );
        return;
TCMC_SET_DATA:
        if( c->midiRPN == 0 )
            setPitchRange( channel, (c->midiData >> 7) + 0.01f * (c->midiData & 0x7F) );
        else if( c->midiRPN == 1 )
            setTuning( channel, (int32_t)c->tuning + ((float)c->midiData - 8192.0f) / 8192.0f ); // fine tune
        else if( c->midiRPN == 2 && controller == 6 )
            setTuning( channel, ((float)control_value - 64.0f) + (c->tuning - (int32_t)c->tuning) ); // coarse tune
        return;
    }

    void setMaxVoices( int max )
    {
        voices.resize( max );
    }
};

class TSFLoader
{
public:
    ~TSFLoader()
    {
        unload();
    }

    tsf* load( IStream* stream )
    {
        if( stream && !_tsf )
        {
            _tsf = tsf_load( stream );
        }
        return _tsf;
    }
    void unload()
    {
        if( _tsf )
        {
            _tsf->presets.clear();
            _tsf->voices.clear();
            delete _tsf;
        }
    }

private:
    bool readRiffchunk( Riffchunk* parent, Riffchunk* chunk, IStream* stream )
    {
        bool IsRiff, IsList;
        if( parent && sizeof( tsf_fourcc ) + sizeof( tsf_u32 ) > parent->size )
            return false;
        if( !stream->read( &chunk->id, sizeof( tsf_fourcc ) ) || *chunk->id <= ' ' || *chunk->id >= 'z' )
            return false;
        if( !stream->read( &chunk->size, sizeof( tsf_u32 ) ) )
            return false;
        if( parent && sizeof( tsf_fourcc ) + sizeof( tsf_u32 ) + chunk->size > parent->size )
            return false;
        if( parent )
            parent->size -= sizeof( tsf_fourcc ) + sizeof( tsf_u32 ) + chunk->size;
        IsRiff = TSF_FourCCEquals( chunk->id, "RIFF" ), IsList = TSF_FourCCEquals( chunk->id, "LIST" );
        if( IsRiff && parent )
            return false; // not allowed
        if( !IsRiff && !IsList )
            return true; // custom type without sub type
        if( !stream->read( &chunk->id, sizeof( tsf_fourcc ) ) || *chunk->id <= ' ' || *chunk->id >= 'z' )
            return false;
        chunk->size -= sizeof( tsf_fourcc );
        return true;
    }

    int32_t tsf_preset_idx( Hydra* hydra, IStream* stream, HydraPhdr* pphdr, int32_t pphdrIdx )
    {
        int32_t idx = 0;
        int32_t otherpphdrIdx;
        for( otherpphdrIdx = 0; otherpphdrIdx < hydra->phdrNum - 1; otherpphdrIdx++ )
        {
            HydraPhdr otherphdr;
            stream->seek( hydra->phdrPos + otherpphdrIdx * phdrSizeInFile );
            otherphdr.read( stream );

            if( otherphdr.bank > pphdr->bank )
                continue;
            else if( otherphdr.bank < pphdr->bank )
                idx++;
            else if( otherphdr.preset > pphdr->preset )
                continue;
            else if( otherphdr.preset < pphdr->preset )
                idx++;
            else if( otherpphdrIdx < pphdrIdx )
                idx++;
        }
        return idx;
    }

    void tsf_preload_presets( tsf* res )
    {
        IStream* stream = res->stream;
        Hydra* hydra = &res->hydra;
        // Read each preset.
        for( int32_t pphdrIdx = 0; pphdrIdx < hydra->phdrNum - 1; pphdrIdx++ )
        {
            HydraPhdr pphdr;
            stream->seek( hydra->phdrPos + pphdrIdx * phdrSizeInFile );
            pphdr.read( stream );
            auto  sortedIndex = tsf_preset_idx( hydra, stream, &pphdr, pphdrIdx );
            auto& preset = res->presets[sortedIndex];
            preset.init( pphdr, pphdrIdx );
            // preset.load(stream, hydra, res->fontSampleCount);
        }
    }

    void tsf_load_samples( int16_t** fontSamples, uint32_t* fontSampleCount, Riffchunk* chunkSmpl, IStream* stream )
    {
        // Read sample data into float format buffer.
        uint32_t samplesLeft, samplesToRead;
        samplesLeft = *fontSampleCount = chunkSmpl->size / sizeof( int16_t );
        uint32_t offset = stream->tell();
        offset;
#ifndef TSF_MMAP
        int16_t* sampleData = new int16_t[samplesLeft];
        *fontSamples = sampleData;
        for( ; samplesLeft; samplesLeft -= samplesToRead )
        {
            samplesToRead = (samplesLeft > 1024 ? 1024 : samplesLeft);
            stream->read( sampleData, samplesToRead * sizeof( short ) );
            sampleData += samplesToRead;
        }
#else
        * fontSamples = (int16_t*)TSF_MMAP( 0, chunkSmpl->size + offset, ((TSF_FILE*)stream->data) );
        for( ; samplesLeft; samplesLeft -= samplesToRead )
        {
            samplesToRead = (samplesLeft > 1024 ? 1024 : samplesLeft);
            stream->skip( samplesToRead * sizeof( int16_t ) );
        }
#endif
    }

    tsf* tsf_load( IStream* stream )
    {
        tsf* res = nullptr;
        Riffchunk chunkHead;
        Riffchunk chunkList;
        Hydra     hydra;
        int16_t* fontSamples = nullptr;
        uint32_t  offset = 0;
        uint32_t  fontSampleCount = 0;

        if( !readRiffchunk( nullptr, &chunkHead, stream ) || !TSF_FourCCEquals( chunkHead.id, "sfbk" ) )
        {
            // if (e) *e = TSF_INVALID_NOSF2HEADER;
            return res;
        }

        // Read hydra and locate sample data.
        memset( &hydra, 0, sizeof( hydra ) );
        while( readRiffchunk( &chunkHead, &chunkList, stream ) )
        {
            Riffchunk chunk;
            if( TSF_FourCCEquals( chunkList.id, "pdta" ) )
            {
                while( readRiffchunk( &chunkList, &chunk, stream ) )
                {
#define HandleChunk( chunkName )                                                            \
    ( TSF_FourCCEquals( chunk.id, #chunkName ) && !( chunk.size % chunkName##SizeInFile ) ) \
    {                                                                                       \
        int32_t num          = chunk.size / chunkName##SizeInFile;                          \
        hydra.chunkName##Num = num;                                                         \
        hydra.chunkName##Pos = stream->tell();                                              \
        stream->skip( chunk.size );                                                         \
    }

                    if HandleChunk( phdr )
                    else if HandleChunk( pbag ) else if HandleChunk( pmod ) else if HandleChunk( pgen ) else if HandleChunk( inst ) else if HandleChunk( ibag ) else if HandleChunk( imod ) else if HandleChunk( igen ) else if HandleChunk( shdr ) else stream->skip( chunk.size );
#undef HandleChunk
                }
            } else if( TSF_FourCCEquals( chunkList.id, "sdta" ) )
            {
                while( readRiffchunk( &chunkList, &chunk, stream ) )
                {
                    if( TSF_FourCCEquals( chunk.id, "smpl" ) )
                    {
                        offset = stream->tell();
                        tsf_load_samples( &fontSamples, &fontSampleCount, &chunk, stream );
                    } else
                        stream->skip( chunk.size );
                }
            } else
                stream->skip( chunkList.size );
        }
        if( !hydra.phdrNum )
        {
            // if (e) *e = TSF_INVALID_INCOMPLETE;
        } else
        {
            res = new tsf;
            memset( res, 0, sizeof( tsf ) );
            res->presets.resize( hydra.phdrNum - 1 );
            res->fontSamples = fontSamples;
            res->fontSamplesOffset = offset / sizeof( int16_t );
            res->fontSampleCount = fontSampleCount;
            res->outSampleRate = 44100.0f;
            //res->setMaxVoices( 64 );

            res->stream = stream;
            res->hydra = hydra;
            res->gc = 0;

            tsf_preload_presets( res );
        }
        return res;
    }
    tsf* _tsf = nullptr;
};
