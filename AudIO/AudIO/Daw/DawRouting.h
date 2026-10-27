#pragma once

#include "../Includes/Design.h"
#include "DawTypes.h"

namespace AudIO
{

    class DawRouting
    {
    public:
        explicit DawRouting( Design& Design_, BusType Type = BusType::Out, BusWidth Width = BusWidth::Stereo );
        ~DawRouting();

        void initAudioRouting( bool RouteToSend = true );

        // Getters...
        inline BusType  getType() const { return type_; }
        inline BusWidth getWidth() const { return width_; }

        static int getNumIns();
        static int getNumMonoIns();
        static int getNumStereoIns();
        static int getNumOuts();
        static int getNumMonoOuts();
        static int getNumStereoOuts();
        static int getNumSends();
        static int getNumReturns();
        static int getNumSubmixes();
        static int getNumAuxes();
        static int getNumObjects();
        static int getNumOthers();

        // Default Bus...
        static uint32_t getDefaultStereoInID();
        static uint32_t getDefaultMonoInID();
        static uint32_t getDefaultStereoOutID();
        static uint32_t getDefaultMonoOutID();

        // Get Bus API's
        static std::vector<uint32_t>& getBusIDs( BusType Type );
        static uint32_t               getBusID( BusType Type, uint32_t Position );

        static uint32_t getStereoInID( uint32_t Position );
        static uint32_t getMonoInID( uint32_t Position );
        static uint32_t getStereoOutID( uint32_t Position );
        static uint32_t getMonoOutID( uint32_t Position );
        static uint32_t getSendReturnID( uint32_t Position );
        static uint32_t getSubmixID( uint32_t Position );
        static uint32_t getAuxID( uint32_t Position );
        static uint32_t getInID( uint32_t Position );
        static uint32_t getOutID( uint32_t Position );

        // Types API's...
        inline bool isSend() const { return BusType::SendReturn == type_; };
        inline bool isReturn() const { return BusType::SendReturn == type_; };
        inline bool isSubmix() const { return BusType::Submix == type_; };
        inline bool isIn() const { return BusType::In == type_; };
        inline bool isOut() const { return BusType::Out == type_; };
        inline bool isInOrOut() const { return ( ( BusType::In == type_ ) || ( BusType::Out == type_ ) ); };
        inline bool isAux() const { return BusType::Aux == type_; };
        inline bool isObject() const { return BusType::Object == type_; };

        // Width API's...
        inline bool isMonoL() const { return ( BusWidth::Mono_L == width_ ); };
        inline bool isMonoR() const { return ( BusWidth::Mono_R == width_ ); };
        inline bool isMono() const { return ( ( BusWidth::Mono_L == width_ ) || ( BusWidth::Mono_R == width_ ) ); };
        inline bool isStereo() const { return ( BusWidth::Stereo == width_ ); };

        // Connection API's
        bool connectInputTo( uint32_t ToId );
        bool connectOutputTo( uint32_t ToId, int position = 0, float Gain = 1.f );

        bool                         isConnectedTo( uint32_t BusId );
        const uint32_t               getConnectedTo( uint32_t position ) const;
        const std::vector<uint32_t>& getConnectedTo() const { return connectedTo_; };

        //
        // Helper functions...
        //
        bool isInBus( const uint32_t BusId ) const;
        bool isSendBus( const uint32_t BusId ) const;
        bool isReturnsBus( const uint32_t BusId ) const;
        bool isOutBus( const uint32_t BusId ) const;
        bool isAuxBus( const uint32_t BusId ) const;
        bool isSubmixBus( const uint32_t BusId ) const;
        bool isRoutingToSendAllowed( const uint32_t BusId );

        // Send Gain API's
        bool setGainForSend( int SendPosition, float Gain );
        void setGainsForSend( const std::vector<float>& Gains );

        float              getGainForSends( int SendPosition ) const;
        std::vector<float> getGainForSends() const;

    protected:
        uint32_t    id_;
        inline void setID( uint32_t ID ) { id_ = ID; }

        void routeToSends();
        void removeFromSends();
        void resetRouting();

    private:
        AUD_IO_MAKE_NON_COPYABLE( DawRouting );

    protected:
        Design&        design_;
        const BusType  type_;
        const BusWidth width_;
        bool           initialized_{ false };

    private:
        std::vector<uint32_t> connectedTo_;
        std::vector<float>    sendGains_;
    };

};
