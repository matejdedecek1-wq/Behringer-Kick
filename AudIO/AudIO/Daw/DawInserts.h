#pragma once

#include "../Includes/AudioDataTypes.h"
#include "../Includes/AudioObject.h"

#include <string>
#include <array>

namespace AudIO
{
    class Design;
    class DawInserts
    {
    public:
        const static int MAX_INSERTS{ 4 };

        explicit DawInserts();
        ~DawInserts() = default;

        // Add or Remove inserts...
        int  addEffect( const std::string& Effect, unsigned int Position );
        int  removeEffect( const std::string& Effect );
        int  removeEffect( unsigned int Position );
        void removeAllEffects();

        // Effects helpers...
        inline static const std::map<const std::string, ObjectInfo>& getEffectsMap() { return ObjectFactory::Get().GetObjectsInfo(); };

        inline int               getNumEffects() const { return (int)_inserts.size(); }
        std::vector<std::string> getEffectsNames() const;
        std::vector<bool>        getEffectsBypass() const;

        const std::string& getEffectName( unsigned int Position );

        // Effect Parameters...
        int                           getNumEffectParameters( unsigned int Position );
        const std::vector<ParamInfo>& getEffectParameters( unsigned int Position );

        float getEffectParameter( unsigned int Position, unsigned int Parameter );
        void  setEffectParameter( unsigned int Position, unsigned int Parameter, float Value );

        // Bypass...
        inline bool isAllEffectsBypassed() const { return _bypassAll.load(); };
        inline void setBypassAllEffects( bool Value ) { _bypassAll.store( Value ); };

        inline bool isEffectBypassed( unsigned int Position ) const { return ( Position < MAX_INSERTS ) ? _inserts[Position].bypassed.load() : false; };
        void        setEffectBypass( unsigned int Position, bool Value );

        // Serialize and Deserialize...
        void serializeEffect( unsigned int Position, std::vector<float>& Values );
        void deserializeEffect( unsigned int Position, const std::vector<float>& Values );

        AUD_IO_MAKE_NON_COPYABLE( DawInserts );

    protected:
        void prepareToPlay( uint32_t Samplerate, uint32_t Blocksize );
        void process( Buffer<float>& Buffer );

    private:
        struct InsertEffects
        {
            std::atomic<bool>       bypassed{ true };
            std::unique_ptr<Object> pObj;

            int  reset();
            int  create( const std::string& Effect );
            bool isSame( const std::string& Effect ) const;
        };

        std::atomic<bool>                      _bypassAll{ false };
        std::array<InsertEffects, MAX_INSERTS> _inserts;

        InsertEffects* getEffect( const std::string& Effect );
        InsertEffects* getEffect( unsigned int Position );
    };
};
