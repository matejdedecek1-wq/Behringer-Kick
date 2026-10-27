
#include "../Includes/Platform.h"
#include "../Includes/Design.h"
#include "DesignImpl.h"

#include <iostream>

namespace AudIO
{
    static MeterData _emptyMeter;

#define GET_OBJECT( ID )        pImpl->getObject( ID )
#define GET_OBJECT_META( ID )   pImpl->getObjectMeta( ID )
#define GET_OBJECT_BUFFER( ID ) pImpl->getObjectBuffer( ID )

#define GET_CHAIN( ID )         pImpl->getChain( ID )
#define GET_BUS( ID )           pImpl->getBus( ID )
#define GET_CHAIN_OR_BUS( ID )  pImpl->getChainOrBus( ID )

    // Design class...
    Design::Design( uint32_t ninputs, uint32_t noutputs )
        : Object( ninputs, noutputs, 0 ), pImpl( std::make_unique<DesignImpl>( ninputs, noutputs ) )
    {}

    Design::~Design()
    {
        removeAllMeters();
        clear();
    }

    void Design::prepareToPlay( uint32_t samplerate, uint32_t blocksize )
    {
        Object::prepareToPlay( samplerate, blocksize );
        pImpl->prepareToPlay( samplerate, blocksize );
        PatchQueue<float>::get().start();
    }

    void Design::process( Buffer<float>& Buffer )
    {
        // Apply ramp to all parameters...
        if( nullptr == pParent )
        {
            PatchQueue<float>::get().process();
        }

        pImpl->process( Buffer );
    }

    void Design::teardown()
    {
        PatchQueue<float>::get().stop();
    }

    inline void Design::setName( const std::string& name )
    {
        Object::setName( name );
    }

    void Design::setName( const uint32_t id, const std::string& name )
    {
        if( auto obj = getObject( id ) )
        {
            obj->setName( name );
        }
    }

    inline const std::string& Design::getName() const
    {
        return Object::getName();
    }

    const std::string& Design::getName( const uint32_t id )
    {
        static std::string emptyName{ "" };
        if( auto obj = getObject( id ) )
        {
            return obj->getName();
        }
        return emptyName;
    }

    uint32_t Design::createObject( const char* type )
    {
        auto obj = CREATE_OBJECT( type );
        return createObject( std::move( obj ) );
    }

    uint32_t Design::createObject( std::unique_ptr<Object> obj )
    {
        auto id = 0;
        if( auto pObj = obj.get() )
        {
            autolock guard{ lockerMutex };
            if( pImpl->insertObject( std::move( obj ), sampleRate(), blockSize() ) )
            {
#if defined( DEBUG_PRINTS ) && ( DEBUG_PRINTS > 0 )
                std::cout << pObj->getType() << " (" << pObj->getId() << ")" << std::endl;
#endif
                return pObj->getId();
            }
        }
        return id;
    }

    bool Design::removeObject( uint32_t id )
    {
        autolock guard{ lockerMutex };
        return pImpl->removeObject( GET_OBJECT_META( id ) );
    }

    Object* Design::getObject( uint32_t id )
    {
        autolock guard{ lockerMutex };
        return GET_OBJECT( id );
    }

    uint32_t Design::createChain( bool stereo )
    {
        autolock guard{ lockerMutex };
        if( auto chain = pImpl->createChain( stereo ) )
            return chain->getId();

        return 0;
    }

    uint32_t Design::createBus()
    {
        autolock guard{ lockerMutex };
        if( auto bus = pImpl->createBus() )
            return bus->getId();

        return 0;
    }

    bool Design::sendToBus( uint32_t busid, uint32_t objid, float amount )
    {
        autolock guard{ lockerMutex };
        if( auto srcmeta = GET_OBJECT_META( objid ) )
        {
            if( auto bus = GET_BUS( busid ) )
                if( auto busmeta = GET_OBJECT_META( busid ) )
                {
                    auto [result, chn] = bus->addInput( *srcmeta, amount );
                    // TODO: Do this from Audio Thread
                    if( result )
                    {
                        if( pImpl->patch( *srcmeta, (uint32_t)0, *busmeta, chn ) )
                        {
                            if( pImpl->patch( *srcmeta, (uint32_t)1, *busmeta, chn + 1 ) )
                            {
                                pImpl->markDirty();
                                return true;
                            }
                            bus->removeInput( *srcmeta );
                            pImpl->markDirty();
                        }
                    }
                }
        }
        return false;
    }

    bool Design::removeFromBus( uint32_t busid, uint32_t objid )
    {
        autolock guard{ lockerMutex };
        if( auto srcmeta = GET_OBJECT_META( objid ) )
        {
            if( auto bus = GET_BUS( busid ) )
            {
                auto [result, chn] = bus->removeInput( *srcmeta );
                auto busmeta       = GET_OBJECT_META( busid );
                if( result )
                {
                    srcmeta->unpatch( *busmeta, 0, chn );
                    srcmeta->unpatch( *busmeta, 1, chn + 1 );
                    pImpl->markDirty();
                    return true;
                }
            }
        }
        return false;
    }

    bool Design::isConnectedToBus( uint32_t busid, uint32_t objid )
    {
        autolock guard{ lockerMutex };
        if( auto pBus = GET_BUS( busid ) )
            if( auto pMeta = GET_OBJECT_META( objid ) )
                return pBus->isInputConnected( *pMeta );
        return false;
    }

    uint32_t Design::getBusCapacity( uint32_t busid )
    {
        autolock guard{ lockerMutex };
        if( auto pBus = GET_BUS( busid ) )
            return pBus->getCapacity();
        return 0;
    }

    uint32_t Design::getBusConnections( uint32_t busid )
    {
        autolock guard{ lockerMutex };
        if( auto pBus = GET_BUS( busid ) )
            return pBus->getNumConnections();
        return 0;
    }

    bool Design::setBusInputGain( uint32_t busid, uint32_t objid, float gain )
    {
        autolock guard{ lockerMutex };
        if( auto pBus = GET_BUS( busid ) )
            if( auto pMeta = GET_OBJECT_META( objid ) )
                return pBus->setInputGain( *pMeta, gain );
        return false;
    }

    float Design::getBusInputGain( uint32_t busid, uint32_t objid )
    {
        autolock guard{ lockerMutex };
        if( auto pBus = GET_BUS( busid ) )
            if( auto pMeta = GET_OBJECT_META( objid ) )
                return pBus->getInputGain( *pMeta );
        return false;
    }

    BusInputMeta Design::getBusConnectedInput( uint32_t busid, uint32_t position )
    {
        autolock guard{ lockerMutex };
        if( auto pBus = GET_BUS( busid ) )
            return pBus->getConnectedInput( position );
        return {};
    }

    void Design::getBusConnectedInputs( uint32_t busid, std::vector<BusInputMeta>& Inputs )
    {
        autolock guard{ lockerMutex };
        if( auto pBus = GET_BUS( busid ) )
            pBus->getConnectedInputs( Inputs );
    }

    uint32_t Design::insertDesign( std::shared_ptr<Design> obj )
    {
        autolock guard{ lockerMutex };

        auto retVal = pImpl->insertDesign( obj );
        if( 0 != retVal )
            obj->pParent = this;

        return retVal;
    }

    std::shared_ptr<Design> Design::getDesign( uint32_t id )
    {
        return pImpl->getDesign( id );
    }

    bool Design::addMeter( uint32_t id )
    {
        autolock guard{ lockerMutex };
        if( auto srcmeta = GET_OBJECT_META( id ) )
        {
            return pImpl->_meters.addMeter( *srcmeta );
        }
        return false;
    }

    bool Design::removeMeter( uint32_t id )
    {
        autolock guard{ lockerMutex };
        return pImpl->_meters.removeMeter( id );
    }

    void Design::removeAllMeters()
    {
        autolock guard{ lockerMutex };
        return pImpl->_meters.clear();
    }

    const MeterData& Design::readMeter( uint32_t meterid ) const
    {
        return pImpl->_meters.read( meterid );
    }

    std::string Design::dumpMeter( uint32_t meterid )
    {
        std::string str;
        str            = getName( meterid );
        const auto& m  = readMeter( meterid );
        int         ch = 0;
        for( const auto& it : m )
            str.append( ": " + std::to_string( ch++ ) + "]: rms: " + std::to_string( it.rms ) + ", peak: " + std::to_string( it.peak ) );

        return str;
    }

    void Design::readAllMeters( const std::function<void( const std::string& name, MeterData& data )>& MeterCallback )
    {
        pImpl->_meters.readAll( [this, MeterCallback]( uint32_t id, AudIO::MeterData& data ) {
            MeterCallback( getName( id ), data );
        } );
    }

    void Design::enableInputMetering( uint32_t chainorbusid, bool Enable )
    {
        if( auto pObj = GET_CHAIN_OR_BUS( chainorbusid ) )
            pObj->enableInputMetering( Enable );
    }

    void Design::enableOutputMetering( uint32_t chainorbusid, bool Enable )
    {
        if( auto pObj = GET_CHAIN_OR_BUS( chainorbusid ) )
            pObj->enableOutputMetering( Enable );
    }

    MeterData& Design::readInMeter( uint32_t chainorbusid )
    {
        if( auto pObj = GET_CHAIN_OR_BUS( chainorbusid ) )
            return pObj->readInMeter();
        return _emptyMeter;
    }

    MeterData& Design::readOutMeter( uint32_t chainorbusid )
    {
        if( auto pObj = GET_CHAIN_OR_BUS( chainorbusid ) )
            return pObj->readOutMeter();
        return _emptyMeter;
    }

    uint32_t Design::createInsert( uint32_t chainorbusid, const char* type )
    {
        autolock guard{ lockerMutex };
        if( auto pObj = GET_CHAIN_OR_BUS( chainorbusid ) )
            return pObj->createInsert( type );
        return 0;
    }

    uint32_t Design::createInsert( uint32_t chainorbusid, std::unique_ptr<Object> obj )
    {
        autolock guard{ lockerMutex };
        if( auto pObj = GET_CHAIN_OR_BUS( chainorbusid ) )
            return pObj->createInsert( std::move( obj ) );
        return 0;
    }

    bool Design::deleteInsert( uint32_t chainorbusid, uint32_t id )
    {
        autolock guard{ lockerMutex };
        if( auto pObj = GET_CHAIN_OR_BUS( chainorbusid ) )
            return pObj->deleteInsert( id );
        return false;
    }

    Object* Design::getInsert( uint32_t chainorbusid, uint32_t id )
    {
        autolock guard{ lockerMutex };
        if( auto pObj = GET_CHAIN_OR_BUS( chainorbusid ) )
            return pObj->getObject( id );
        return nullptr;
    }

    bool Design::addInsert( uint32_t chainorbusid, uint32_t id, uint32_t pos )
    {
        autolock guard{ lockerMutex };
        if( auto pObj = GET_CHAIN_OR_BUS( chainorbusid ) )
            return pObj->insertObject( id, pos );
        return false;
    }

    bool Design::removeInsert( uint32_t chainorbusid, uint32_t id )
    {
        autolock guard{ lockerMutex };
        if( auto pObj = GET_CHAIN_OR_BUS( chainorbusid ) )
            return pObj->removeInsertById( id );
        return false;
    }

    bool Design::removeInsertByPos( uint32_t chainorbusid, uint32_t pos )
    {
        autolock guard{ lockerMutex };
        if( auto pObj = GET_CHAIN_OR_BUS( chainorbusid ) )
            return pObj->removeInsertByPos( pos );
        return false;
    }

    bool Design::exists( uint32_t id ) const
    {
        return ( nullptr != GET_OBJECT( id ) );
    }

    bool Design::patch( uint32_t srcid, uint32_t dstid )
    {
        autolock guard{ lockerMutex };
        bool     retVal = false;
        auto     srcobj = GET_OBJECT( srcid );
        auto     dstobj = GET_OBJECT( dstid );
        if( !srcobj || !dstobj )
            return retVal;
        auto count = std::min( srcobj->outputCount(), dstobj->inputCount() );
        for( uint32_t ch = 0; ch < count; ++ch )
        {
            retVal = patch( srcid, ch, dstid, ch );
            if( !retVal )
            {
                for( uint32_t i = 0; i < ch; ++i )
                {
                    unpatch( srcid, i, dstid, i );
                }
                break;
            }
        }
        return retVal;
    }

    bool Design::unpatch( uint32_t srcid, uint32_t dstid )
    {
        autolock guard{ lockerMutex };
        bool     retVal = false;
        auto     srcobj = getObject( srcid );
        auto     dstobj = getObject( dstid );
        if( !srcobj || !dstobj )
            return retVal;
        auto count = std::min( srcobj->outputCount(), dstobj->inputCount() );
        for( uint32_t ch = 0; ch < count; ++ch )
        {
            retVal = unpatch( srcid, ch, dstid, ch );
            if( !retVal )
                break;
        }
        return retVal;
    }

    bool Design::patch( uint32_t srcid, uint32_t srcch, uint32_t dstid, uint32_t dstch )
    {
        autolock guard{ lockerMutex };
        return pImpl->patch( srcid, srcch, dstid, dstch );
    }

    bool Design::unpatch( uint32_t srcid, uint32_t srcch, uint32_t dstid, uint32_t dstch )
    {
        autolock guard{ lockerMutex };
        return pImpl->unpatch( srcid, srcch, dstid, dstch );
    }

    bool Design::setDesignInput( uint32_t objid, bool force )
    {
        autolock guard{ lockerMutex };
        return pImpl->setDesignInput( objid, force );
    }

    bool Design::setDesignOutput( uint32_t objid, bool force )
    {
        autolock guard{ lockerMutex };
        return pImpl->setDesignOutput( objid, force );
    }

    void Design::clearDesignInput( bool force )
    {
        autolock guard{ lockerMutex };
        pImpl->clearDesignInput( force );
    }

    void Design::clearDesignOutput( bool force )
    {
        autolock guard{ lockerMutex };
        pImpl->clearDesignOutput( force );
    }

    bool Design::isBypassed() const { return pImpl->isBypassed(); }

    void Design::setBypass( const bool bypass )
    {
        autolock guard{ lockerMutex };
        pImpl->setBypass( bypass );
    }

    bool Design::isBypassed( const uint32_t chainorbusid )
    {
        autolock guard{ lockerMutex };
        if( auto pObj = GET_CHAIN_OR_BUS( chainorbusid ) )
            return pObj->getParameter( BusOrChain::kParamBypass );
        return false;
    }

    bool Design::isMuted( const uint32_t chainorbusid )
    {
        autolock guard{ lockerMutex };
        if( auto pObj = GET_CHAIN_OR_BUS( chainorbusid ) )
            return pObj->getParameter( BusOrChain::kParamMute );
        return false;
    }

    void Design::setBypass( const uint32_t chainorbusid, const bool bypass )
    {
        autolock guard{ lockerMutex };
        if( auto pObj = GET_CHAIN_OR_BUS( chainorbusid ) )
            pObj->setParameter( BusOrChain::kParamBypass, (float)bypass );
    }

    void Design::setMute( const uint32_t chainorbusid, const bool mute )
    {
        autolock guard{ lockerMutex };
        if( auto pObj = GET_CHAIN_OR_BUS( chainorbusid ) )
            pObj->setParameter( BusOrChain::kParamMute, (float)mute );
    }

    void Design::setGain( const uint32_t chainorbusid, const float gain )
    {
        autolock guard{ lockerMutex };
        if( auto pObj = GET_CHAIN_OR_BUS( chainorbusid ) )
            pObj->setParameter( BusOrChain::kParamGain, gain );
    }

    float Design::getGain( const uint32_t chainorbusid )
    {
        autolock guard{ lockerMutex };
        if( auto pObj = GET_CHAIN_OR_BUS( chainorbusid ) )
            return pObj->getParameter( BusOrChain::kParamGain );
        return 0.f;
    }

    void Design::setPan( const uint32_t chainorbusid, const float pan )
    {
        autolock guard{ lockerMutex };
        if( auto pObj = GET_CHAIN_OR_BUS( chainorbusid ) )
            pObj->setParameter( BusOrChain::kParamPan, pan );
    }

    float Design::getPan( const uint32_t chainorbusid )
    {
        autolock guard{ lockerMutex };
        if( auto pObj = GET_CHAIN_OR_BUS( chainorbusid ) )
            return pObj->getParameter( BusOrChain::kParamPan );
        return 0.f;
    }

    void Design::setParameter( int chainorbusid, uint32_t parameterId, float amount )
    {
        if( auto pObj = GET_CHAIN_OR_BUS( chainorbusid ) )
            pObj->setParameter( parameterId, amount );
    }

    float Design::getParameter( int chainorbusid, uint32_t parameterId )
    {
        if( auto pObj = GET_CHAIN_OR_BUS( chainorbusid ) )
            return pObj->getParameter( parameterId );
        return 0.f;
    }

    std::pair<float, float> Design::getLevelAndPan( const uint32_t chainorbusid )
    {
        autolock guard{ lockerMutex };
        if( auto pObj = GET_CHAIN_OR_BUS( chainorbusid ) )
            return std::make_pair<float, float>( pObj->getParameter( BusOrChain::kParamGain ), pObj->getParameter( BusOrChain::kParamPan ) );

        return std::make_pair<float, float>( 0.f, 0.f );
    }

    void Design::setLevelAndPan( const uint32_t chainorbusid, float Gain, float Pan )
    {
        autolock guard{ lockerMutex };
        if( auto pObj = GET_CHAIN_OR_BUS( chainorbusid ) )
        {
            pObj->setParameter( BusOrChain::kParamGain, Gain );
            pObj->setParameter( BusOrChain::kParamPan, Pan );
        }
    }

    void Design::clear()
    {
        pImpl->clear();
    }

} // Namespace AudIO
