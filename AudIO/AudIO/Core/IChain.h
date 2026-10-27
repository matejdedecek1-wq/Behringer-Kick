#pragma once

#include <memory>
#include <stdint.h>

#include "../Includes/AudioDataTypes.h"

namespace AudIO
{
    class Object;
    // Pure virtual interface class...
    struct IChain
    {
        virtual bool     changeCapacity( uint32_t newcapacity )      = 0;
        virtual uint32_t createInsert( const char* type )            = 0;
        virtual uint32_t createInsert( std::unique_ptr<Object> obj ) = 0;
        virtual Object*  getObject( uint32_t id )                    = 0;
        virtual bool     insertObject( uint32_t id, uint32_t pos )   = 0;
        virtual bool     removeInsertById( uint32_t id )             = 0;
        virtual bool     removeInsertByPos( uint32_t pos )           = 0;
        virtual bool     deleteInsert( uint32_t id )                 = 0;

        virtual float getParameter( int index )              = 0;
        virtual void  setParameter( int index, float value ) = 0;

        virtual void       enableInputMetering( bool Enable )  = 0;
        virtual void       enableOutputMetering( bool Enable ) = 0;
        virtual MeterData& readInMeter()                       = 0;
        virtual MeterData& readOutMeter()                      = 0;
    };
} // Namespace AudIO
