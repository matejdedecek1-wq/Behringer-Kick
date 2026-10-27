#pragma once

#include "airwindows_base.h"

#include "../Includes/AudioObject.h"

#include <cstdio>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <string.h>
#include <vector>

#define REG_AIRWIN( Name, Cat ) REGISTER_OBJECT_WITH_ARGS( #Name, Name, 1, "AirWindows", "Plugin", #Cat );

class AudioEffectX : public AudIO::Object
{
public:
    AudioEffectX( int Callback, int NumProgs, int NumParams )
        : AudIO::Object( 2, 2, NumParams )
        , callback( Callback )
        , numPrograms( NumProgs )
    {
    }

    virtual bool            getEffectName( char* name )    = 0; // The plug-in name
    virtual VstPlugCategory getPlugCategory()              = 0; // The general category for the plug-in
    virtual bool            getProductString( char* text ) = 0; // This is a unique plug-in string provided by Steinberg
    virtual bool            getVendorString( char* text )  = 0; // Vendor info
    virtual VstInt32        getVendorVersion()             = 0; // Version number
    virtual void            getProgramName( char* name )   = 0; // read the name from the host
    virtual void            setProgramName( char* name )   = 0; // changes the name of the preset displayed in the host

    int getNumPrograms() { return numPrograms; }

    virtual VstInt32 getChunk( void** data, bool isPreset ) { return 0; };
    virtual VstInt32 setChunk( void* data, VstInt32 byteSize, bool isPreset ) { return 0; };

    virtual std::vector<AudIO::ParamInfo>& getParameters() override
    {
        initializeParams();

        char displayValue[kVstMaxParamStrLen];
        for( int i = 0; i < getNumParameters(); ++i )
        {
            memset( displayValue, '\0', kVstMaxParamStrLen );
            getParameterDisplay( i, displayValue );
            _parameters[i].displayValue = displayValue;
            _parameters[i].value        = getParameter( i );
        }
        return _parameters;
    }

    virtual void setNumInputs( int i )
    { /*AudIO::Object::resize(i, AudIO::Object::outputCount);*/
    }
    virtual void setNumOutputs( int i ) {}
    virtual void setUniqueID( unsigned long l ) { uniqueID = l; }
    virtual void programsAreChunks( bool b ) {}
    virtual void setPlugCategory( VstPlugCategory C ) { kPlugCategEffect = C; }

    inline char* vst_strncpy( char* destination, const char* source, size_t num )
    {
        return strncpy( destination, source, num );
    }

    //-------------------------------------------------------------------------------------------------------
    // Strings Conversion
    //-------------------------------------------------------------------------------------------------------
    /*!
        \param value Value to convert
        \param text	String up to length char
        \param maxLen Maximal length of the string
    */
    void dB2string( float value, char* text, VstInt32 maxLen )
    {
        if( value <= 0.00001 ) // -100 dB, show -inf from that point onwards
            vst_strncpy( text, "-oo", maxLen );
        //vst_strncpy( text, "-inf", maxLen );
        else
            float2string( (float)( 20.0 * log10( value ) ), text, maxLen );
    }

    //-------------------------------------------------------------------------------------------------------
    /*!
        \param samples Number of samples
        \param text	String up to length char
        \param maxLen Maximal length of the string
    */
    void Hz2string( float samples, char* text, VstInt32 maxLen )
    {
        float sampleRate = getSampleRate();
        if( !samples )
            float2string( 0, text, maxLen );
        else
            float2string( sampleRate / samples, text, maxLen );
    }

    //-------------------------------------------------------------------------------------------------------
    /*!
        \param samples Number of samples
        \param text	String up to length char
        \param maxLen Maximal length of the string
    */
    void ms2string( float samples, char* text, VstInt32 maxLen )
    {
        float2string( (float)( samples * 1000. / getSampleRate() ), text, maxLen );
    }

    //-------------------------------------------------------------------------------------------------------
    /*!
        \param value Value to convert
        \param text	String up to length char
        \param maxLen Maximal length of the string
    */
    inline void float2string( float f, char* t, size_t maxLen )
    {
        //auto dp = airwindowsSurgeDisplayPrecision();
        auto dp = 2;
        snprintf( t, maxLen, "%.*f", dp, f );
    }

    //-------------------------------------------------------------------------------------------------------
    /*!
        \param value Value to convert
        \param text	String up to length char
        \param maxLen Maximal length of the string
    */
    inline void int2string( int value, char* t, size_t maxLen )
    {
        snprintf( t, maxLen, "%d", value );
    }

    inline float string2dB( const char* t, float value )
    {
        if( ( strcmp( t, "-inf" ) == 0 ) || ( strcmp( t, "-oo" ) == 0 ) )
        {
            return 0.0;
        }
        else
        {
            return pow( 10.0, value / 20.0 );
        }
    }

protected:
    VstPlugCategory kPlugCategEffect{ 0 };

    virtual bool canProcessReplacing() { return true; }
    virtual void processReplacing( float** inputs, float** outputs, VstInt32 sampleFrames ) = 0;
    virtual bool canDoubleReplacing() { return false; }
    virtual void processDoubleReplacing( double** inputs, double** outputs, VstInt32 sampleFrames ) = 0;

private:
    bool          _paramsInitialized{ false };
    unsigned long uniqueID{ 0 };
    int           callback{ 0 }, numPrograms{ 0 };

    void prepareToPlay( uint32_t samplerate, uint32_t blocksize ) override
    {
        AudIO::Object::prepareToPlay( samplerate, blocksize );
    };

    void process( AudIO::Buffer<float>& Buffer ) override
    {
        processReplacing( (float**)Buffer.getOutputs(), Buffer.getOutputs(), (VstInt32)blockSize() );
    };

    void initializeParams()
    {
        if( !_paramsInitialized )
        {
            _paramsInitialized = true;

            char name[kVstMaxParamStrLen], unit[kVstMaxParamStrLen];
            memset( name, '\0', kVstMaxParamStrLen );
            getEffectName( name );
            setName( std::string( name ) );

            _parameters.resize( AudIO::Object::_numParams );

            for( int i = 0; i < (int)AudIO::Object::_numParams; ++i )
            {
                memset( name, '\0', kVstMaxParamStrLen );
                memset( unit, '\0', kVstMaxParamStrLen );
                getParameterName( i, name );
                getParameterLabel( i, unit );
                _parameters[i].name = name;
                _parameters[i].unit = unit;
            }
        }
    }
};
