#pragma once

#include "MeterEngine.h"

namespace AudIO
{
    MeterEngine::MeterEngine( uint32_t count )
        : Object( 0, 0, 0 )
    {
        _meters.reserve( count );
        setRateHz( 20 );
    }

    bool MeterEngine::addMeter( objectMeta& srcobj )
    {
        if( srcobj.getObject().outputCount() < 2 )
        {
            return false;
        }
        auto objid = srcobj.getObject().getId();
        if( auto it = _meters.find( objid ); it == _meters.end() )
        {
            autolock guard{ _lock };
            _meters.try_emplace( objid, std::make_pair( LevelMeter{}, &srcobj ) );
        }
#if defined( METERING_LOAD_BALANCE ) && ( METERING_LOAD_BALANCE > 0 )
        recalculate();
#endif
        return true;
    }

    bool MeterEngine::removeMeter( uint32_t id )
    {
        autolock guard{ _lock };
        _meters.erase( id );
        return true;
    }

    void MeterEngine::clear()
    {
        autolock guard{ _lock };
        _meters.clear();
    }

    void MeterEngine::prepareToPlay( uint32_t samplerate, uint32_t blocksize )
    {
        Object::prepareToPlay( samplerate, blocksize );
#if defined( METERING_LOAD_BALANCE ) && ( METERING_LOAD_BALANCE > 0 )
        recalculate();
#endif
    }

#if defined( METERING_LOAD_BALANCE ) && ( METERING_LOAD_BALANCE > 0 )
    void MeterEngine::setRateHz( float RateHz )
    {
        _rateMs = ( 1000.f / RateHz );
        recalculate();
    }

    inline void MeterEngine::recalculate()
    {
        _loopCntr       = (int)( _rateMs / blockRate() );
        _metersPerBlock = (int)( 1 + ( (float)_meters.size() / (float)_loopCntr ) );
    }
#endif

    void MeterEngine::process( Buffer<float>& /*buffer*/ )
    {
#if defined( METERING_LOAD_BALANCE ) && ( METERING_LOAD_BALANCE > 0 )
        auto numMeters = utils::min( _metersPerBlock, ( (int)_meters.size() - _meterIdx ) );
        if( 0 < numMeters )
        {
            auto it = _meters.begin();
            std::advance( it, _meterIdx );
            for( auto i = 0; i < numMeters; ++i )
            {
                if( it != _meters.end() )
                {
                    auto& [meter, objMeta] = it->second;
                    if( objMeta )
                        meter.process( objMeta->getBuffer() );
                    ++_meterIdx;
                    ++it;
                }
                else
                {
                    _meterIdx = 0;
                    break;
                }
            }
        }
#else
        for( auto& [id, meter] : _meters )
        {
            meter.process();
        }
#endif
    }

    MeterData& MeterEngine::read( uint32_t id )
    {
        if( auto it = _meters.find( id ); it != _meters.end() )
        {
            auto& [meter, objMeta] = it->second;
            meter.getMeter( _data );
        }
        else
        {
            for( auto& itr : _data )
                itr.reset();
        }
        return _data;
    }

    void MeterEngine::readAll( const std::function<void( uint32_t meterid, MeterData& data )>& MeterCallback )
    {
        if( MeterCallback )
            for( auto& [id, meteringInfo] : _meters )
            {
                auto& [meter, objMeta] = meteringInfo;
                meter.getMeter( _data );
                MeterCallback( id, _data );
            }
    }

} // Namespace AudIO
