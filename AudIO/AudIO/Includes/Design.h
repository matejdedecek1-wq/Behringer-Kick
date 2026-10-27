#pragma once

#include "AudioObject.h"
#include "AudioDataTypes.h"

#include <stdint.h>

namespace AudIO
{
    class DesignImpl;

    class Design : public Object
    {
    public:
        Design( uint32_t ninputs, uint32_t noutputs );
        virtual ~Design();

        void prepareToPlay( uint32_t samplerate, uint32_t blocksize ) override;
        void process( Buffer<float>& Buffer ) override;
        void teardown() override;

        void               setName( const std::string& name ) override;
        void               setName( const uint32_t id, const std::string& name );
        const std::string& getName() const override;
        const std::string& getName( const uint32_t id );

        uint32_t createObject( const char* type );
        uint32_t createObject( std::unique_ptr<Object> obj );
        bool     removeObject( uint32_t id );
        Object*  getObject( uint32_t id );

        // Chain API's
        uint32_t createChain( bool stereo = true );

        // BUS API's ...
        uint32_t createBus();
        bool     sendToBus( uint32_t busid, uint32_t objid, float amount );
        bool     removeFromBus( uint32_t busid, uint32_t objid );
        bool     isConnectedToBus( uint32_t busid, uint32_t objid );

        uint32_t getBusCapacity( uint32_t busid );
        uint32_t getBusConnections( uint32_t busid );

        bool  setBusInputGain( uint32_t busid, uint32_t objid, float gain );
        float getBusInputGain( uint32_t busid, uint32_t objid );

        BusInputMeta getBusConnectedInput( uint32_t busid, uint32_t position );
        void         getBusConnectedInputs( uint32_t busid, std::vector<BusInputMeta>& Inputs );

        // Child Design API's ...
        uint32_t                insertDesign( std::shared_ptr<Design> obj );
        std::shared_ptr<Design> getDesign( uint32_t objid );

        // Meter API's...
        bool             addMeter( uint32_t objid );
        bool             removeMeter( uint32_t objid );
        void             removeAllMeters();
        const MeterData& readMeter( uint32_t meterid ) const;
        void             readAllMeters( const std::function<void( const std::string& name, MeterData& data )>& MeterCallback );
        std::string      dumpMeter( uint32_t meterid );

        void       enableInputMetering( uint32_t chainorbusid, bool Enable );
        void       enableOutputMetering( uint32_t chainorbusid, bool Enable );
        MeterData& readInMeter( uint32_t chainorbusid );
        MeterData& readOutMeter( uint32_t chainorbusid );

        uint32_t createInsert( uint32_t chainorbusid, const char* type );
        uint32_t createInsert( uint32_t chainorbusid, std::unique_ptr<Object> obj );
        bool     deleteInsert( uint32_t chainorbusid, uint32_t id );
        Object*  getInsert( uint32_t chainorbusid, uint32_t id );
        bool     addInsert( uint32_t chainorbusid, uint32_t id, uint32_t pos = POS_END );
        bool     removeInsert( uint32_t chainorbusid, uint32_t id );
        bool     removeInsertByPos( uint32_t chainorbusid, uint32_t pos );

        bool exists( uint32_t id ) const;

        bool patch( uint32_t srcid, uint32_t dstid );
        bool unpatch( uint32_t srcid, uint32_t dstid );

        bool patch( uint32_t srcid, uint32_t srcch, uint32_t dstid, uint32_t dstch );
        bool unpatch( uint32_t srcid, uint32_t srcch, uint32_t dstid, uint32_t dstch );

        bool setDesignInput( uint32_t objid, bool force = false );
        bool setDesignOutput( uint32_t objid, bool force = false );
        void clearDesignInput( bool force = false );
        void clearDesignOutput( bool force = false );

        bool isBypassed() const;
        void setBypass( const bool bypass );

        // Common API's to Bus or Chain...
        bool isBypassed( const uint32_t chainorbusid );
        void setBypass( const uint32_t chainorbusid, const bool bypass );

        bool isMuted( const uint32_t chainorbusid );
        void setMute( const uint32_t chainorbusid, const bool mute );

        void  setGain( const uint32_t chainorbusid, const float gain );
        float getGain( const uint32_t chainorbusid );

        void  setPan( const uint32_t chainorbusid, const float pan );
        float getPan( const uint32_t chainorbusid );

        void  setParameter( int chainorbusid, uint32_t parameterId, float amount );
        float getParameter( int chainorbusid, uint32_t parameterId );

        std::pair<float, float> getLevelAndPan( const uint32_t chainorbusid );
        void                    setLevelAndPan( const uint32_t chainorbusid, float Gain, float Pan );

        void clear();

    private:
        std::unique_ptr<DesignImpl> pImpl;
        Design*                     pParent{ nullptr };
        recursive_mutex             lockerMutex;
    };

} // Namespace AudIO
