#pragma once

#include <unordered_map>
#include <vector>

#include "Chain.h"

namespace AudIO
{

    class Bus : public IChain
        , public Object
    {
        friend class designMeta;

    public:
        Bus();
        virtual ~Bus() = default;

        void initializeBus( objectMeta* BusMeta );

        void setName( const std::string& Name ) override;

        std::pair<bool, uint32_t> addInput( objectMeta& Src, float Gain );
        std::pair<bool, uint32_t> removeInput( objectMeta& Src );

        bool isInputConnected( objectMeta& Src );
        int  getInputConnectedIndex( objectMeta& Src );

        bool  setInputGain( objectMeta& Src, float Gain );
        float getInputGain( objectMeta& Src );

        bool changeCapacity( uint32_t NewCapacity ) override;

        uint32_t getCapacity() const;
        uint32_t getNumConnections() const;

        BusInputMeta getConnectedInput( uint32_t position ) const;
        void         getConnectedInputs( std::vector<BusInputMeta>& Inputs ) const;

        void enableInputMetering( bool Status ) override { _chain.enableInputMetering( Status ); }
        void enableOutputMetering( bool Status ) override { _chain.enableOutputMetering( Status ); }

        MeterData& readInMeter() override { return _chain.readInMeter(); };
        MeterData& readOutMeter() override { return _chain.readOutMeter(); };

        // Processing Chain within BUS...
        uint32_t createInsert( const char* Type ) override;
        uint32_t createInsert( std::unique_ptr<Object> Obj ) override;
        Object*  getObject( uint32_t Id ) override;
        bool     insertObject( uint32_t Id, uint32_t Pos ) override;
        bool     removeInsertById( uint32_t Id ) override;
        bool     removeInsertByPos( uint32_t Pos ) override;
        bool     deleteInsert( uint32_t Id ) override;

        // Refer to Chain object for parameter Index...
        void  setParameter( int Index, float Value ) override;
        float getParameter( int Index ) override;

        std::pair<float, float> getOutputLevelAndPan();
        void                    setOutputLevelAndPan( float Gain, float Pan );

        void prepareToPlay( uint32_t SampleRate, uint32_t BlockSize ) override;
        void process( Buffer<float>& BufferRef ) override;

    private:
        bool findFreeIndex( uint32_t& Index );

        Chain                _chain{ true };
        objectMeta           _chainMeta{ _chain };
        objectMeta*          _busmeta{ nullptr };
        AudIO::Buffer<float> _mixOutBuffer{ 0, false, 2, true };

        std::vector<float>                   _gains;
        std::unordered_map<objectMeta*, int> _busInputMap;
    };
} // Namespace AudIO
