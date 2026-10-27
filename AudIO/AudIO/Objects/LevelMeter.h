#pragma once

#include "../Includes/AudioDataTypes.h"
#include "../Includes/AudIOBuffer.h"

namespace AudIO
{
    struct MeterStats
    {
        float rms{ 0.f };
        float min{ 1.f };
        float max{ -1.f };
    };
    using MeterStatsContainer = std::vector<MeterStats>;

    class LevelMeter
    {
    public:
        explicit LevelMeter();
        ~LevelMeter() = default;

        const char* getType() const;

        void process( const AudIO::Buffer<float>& buffer );
        void processIn( const AudIO::Buffer<float>& buffer );
        void processOut( const AudIO::Buffer<float>& buffer );
        void processInAndOut( const AudIO::Buffer<float>& buffer );

        MeterData&        getMeter( MeterData& data );
        MeterData&        getMeterIn( MeterData& data );
        inline MeterData& getMeterOut( MeterData& data ) { return getMeter( data ); };

        const MeterStatsContainer& getInStatistics() const { return _sIns; }
        const MeterStatsContainer& getOutStatistics() const { return _sOuts; }

        float getInRMS( int channel = -1 );
        float getOutRMS( int channel = -1 );

    private:
        MeterStatsContainer _sIns;
        MeterStatsContainer _sOuts;
    };
}
