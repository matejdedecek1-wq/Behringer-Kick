
#pragma once

#include "../../Objects/Recorder.h"
#include "../../Objects/LevelMeter.h"
#include "../DawRouting.h"
#include "../DawInserts.h"

namespace AudIO
{
    class DRecorder
        : public AudioRecorder
        , public DawRouting
        , public DawInserts
    {
    public:
        explicit DRecorder( Design& D )
            : AudioRecorder()
            , DawRouting( D, BusType::Object, BusWidth::Stereo )
        {
            AudioRecorder::setName( "D-AudioRecorder" );
            DawRouting::setID( getId() );
        }

        inline MeterData& readInputMeter()
        {
            return metering.getMeterIn( inMeter );
        }

        inline MeterData& readOutputMeter()
        {
            return metering.getMeter( outMeter );
        }

        void reset() override
        {
            AudioRecorder::reset();
            DawInserts::removeAllEffects();
            DawRouting::resetRouting();
        }

    private:
        void prepareToPlay( uint32_t samplerate, uint32_t blocksize ) override
        {
            DawInserts::prepareToPlay( samplerate, blocksize );
            AudioRecorder::prepareToPlay( samplerate, blocksize );
        }

        void process( Buffer<float>& buffer ) override
        {
            metering.processIn( buffer );
            if( buffer.getInputBuffer( 0 ) && buffer.getInputBuffer( 1 ) )
            {
                DawInserts::process( buffer );
                AudioRecorder::process( buffer );
            }
            metering.process( buffer );
        }

    private:
        LevelMeter metering;
        MeterData  inMeter;
        MeterData  outMeter;
    };

}
