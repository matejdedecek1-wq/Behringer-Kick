#include "LevelMeter.h"

namespace AudIO
{
    ///
    /// Static methods.
    ///

    static void _meteringProcess( const float* buffer, unsigned int size, MeterStats& stats )
    {
        if( buffer )
            utils::rmsMinAndMax( buffer, size, stats.rms, stats.min, stats.max );
    }

    static void _processInMeter( const AudIO::Buffer<float>& buffer, MeterStatsContainer& stats )
    {
        auto size  = buffer.getBlockSize();
        auto chMax = utils::min<int>( (int)buffer.getNumInputChannels(), (int)stats.size() );
        for( auto ch = 0; ch < chMax; ++ch )
            _meteringProcess( buffer.getInputBuffer( ch ), size, stats[ch] );
    }

    static void _processOutMeter( const AudIO::Buffer<float>& buffer, MeterStatsContainer& stats )
    {
        auto size  = buffer.getBlockSize();
        auto chMax = utils::min<int>( (int)buffer.getNumOutputChannels(), (int)stats.size() );
        for( auto ch = 0; ch < chMax; ++ch )
            _meteringProcess( buffer.getOutputBuffer( ch ), size, stats[ch] );
    }

    static float _getRMS( MeterStatsContainer& stats, int channel = -1 )
    {
        float rms = 0.f;
        if( auto sz = (int)stats.size(); 0 < sz )
        {
            rms = stats[0].rms;
            if( 1 < sz )
            {
                rms += stats[1].rms;
                rms *= 0.5f;
            }
        }
        return rms;
    }

    static MeterData& _getMeter( MeterData& data, MeterStatsContainer& container )
    {
        auto chMax = (int)container.size();
        data.resize( chMax, {} );

        for( auto ch = 0; ch < chMax; ++ch )
        {
            auto&       d = data[ch];
            const auto& s = container[ch];

            d.rms  = utils::linearTolog( s.rms );
            d.peak = utils::linearTolog( utils::max( fabs( s.min ), s.max ) );
        }
        return data;
    }

    ///
    /// LevelMeter Class definitions.
    ///
    ///
    LevelMeter::LevelMeter()
    {
        _sIns.resize( 2 );
        _sOuts.resize( 2 );
    }

    const char* LevelMeter::getType() const
    {
        return "LevelMeter";
    }

    void LevelMeter::process( const AudIO::Buffer<float>& buffer )
    {
        _processOutMeter( buffer, _sOuts );
    }

    void LevelMeter::processIn( const AudIO::Buffer<float>& buffer )
    {
        _processInMeter( buffer, _sIns );
    }

    void LevelMeter::processOut( const AudIO::Buffer<float>& buffer )
    {
        _processOutMeter( buffer, _sOuts );
    }

    void LevelMeter::processInAndOut( const AudIO::Buffer<float>& buffer )
    {
        _processInMeter( buffer, _sIns );
        _processOutMeter( buffer, _sOuts );
    }

    MeterData& LevelMeter::getMeter( MeterData& data )
    {
        return _getMeter( data, _sOuts );
    };

    MeterData& LevelMeter::getMeterIn( MeterData& data )
    {
        return _getMeter( data, _sIns );
    };

    float LevelMeter::getInRMS( int channel )
    {
        return _getRMS( _sIns, channel );
    }

    float LevelMeter::getOutRMS( int channel )
    {
        return _getRMS( _sOuts, channel );
    }

}
