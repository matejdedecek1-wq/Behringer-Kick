#pragma once

#include "Bus.h"

namespace AudIO
{
    Bus::Bus()
        : Object( 0, 2, 0 )
    {
        // pre-reserve memory to hold 4 stereo channels.
        _busInputMap.reserve( 8 );
        _gains.reserve( 4 );

        _chain.initializeChain( &_chainMeta );
        // patch mix out to chain in
        PatchQueue<float>::get().patchInput( &_chainMeta.getBuffer(), _mixOutBuffer.getOutputBuffer( 0 ), 0 );
        PatchQueue<float>::get().patchInput( &_chainMeta.getBuffer(), _mixOutBuffer.getOutputBuffer( 1 ), 1 );
    }

    void Bus::initializeBus( objectMeta* BusMeta )
    {
        _busmeta = BusMeta;
        if( !_busmeta )
            return;
        PatchQueue<float>::get().patchOutput( &_chainMeta.getBuffer(), _busmeta->getBuffer().getOutputBuffer( 0 ), 0 );
        PatchQueue<float>::get().patchOutput( &_chainMeta.getBuffer(), _busmeta->getBuffer().getOutputBuffer( 1 ), 1 );
    }

    void Bus::setName( const std::string& Name )
    {
        Object::setName( Name );
        _chain.setName( Name );
    }

    std::pair<bool, uint32_t> Bus::addInput( objectMeta& Src, float Gain )
    {
        if( _busmeta && Src.getBuffer().isOutputStereo() && ( 0 > getInputConnectedIndex( Src ) ) )
        {
            uint32_t freeFirstMonoChannel = 0;
            auto&    buffer               = _busmeta->getBuffer();
            if( !findFreeIndex( freeFirstMonoChannel ) )
            {
                buffer.resizeInputs( buffer.getNumInputChannels() + 2 );
                if( !findFreeIndex( freeFirstMonoChannel ) )
                {
                    // Roll back allocated memory...
                    if( buffer.getNumInputChannels() >= 2 )
                        buffer.resizeInputs( buffer.getNumInputChannels() - 2 );
                    return { false, 0 };
                }
                // Resize gains to hold additional input.
                _gains.push_back( 0.f );
            }
            assert( _gains.size() == ( buffer.getNumInputChannels() / 2 ) );
            _busInputMap[&Src]               = freeFirstMonoChannel;
            _gains[freeFirstMonoChannel / 2] = Gain;
            _numInputs                       = buffer.getNumInputChannels();
            // Actual patching will be done by the design Meta later
            return { true, freeFirstMonoChannel };
        }
        return { false, 0 };
    }

    std::pair<bool, uint32_t> Bus::removeInput( objectMeta& Src )
    {
        auto index = getInputConnectedIndex( Src );
        if( _busmeta && utils::isPositive( index ) )
        {
            // Remove this input source and clear meta properties...
            _gains[index / 2] = 0.f;
            _busInputMap.erase( &Src );
            // Actual unpatching will be done by the design Meta
            return { true, index };
        }
        return { false, 0 };
    }

    bool Bus::isInputConnected( objectMeta& Src )
    {
        return ( -1 < getInputConnectedIndex( Src ) );
    }

    int Bus::getInputConnectedIndex( objectMeta& Src )
    {
        auto it = _busInputMap.find( &Src );
        return ( it != _busInputMap.end() ) ? it->second : -1;
    }

    bool Bus::setInputGain( objectMeta& Src, float Gain )
    {
        auto index = getInputConnectedIndex( Src );
        if( utils::isPositive( index ) )
        {
            _gains[index / 2] = Gain;
            return true;
        }
        return false;
    }

    float Bus::getInputGain( objectMeta& Src )
    {
        auto index = getInputConnectedIndex( Src );
        if( utils::isPositive( index ) )
        {
            return _gains[index / 2];
        }
        return 0.f;
    }

    bool Bus::changeCapacity( uint32_t newcapacity )
    {
        newcapacity;
        return false;
    }

    uint32_t Bus::getCapacity() const
    {
        return _busmeta ? _busmeta->getBuffer().getNumInputChannels() / 2 : 0;
    }

    uint32_t Bus::getNumConnections() const
    {
        return static_cast<uint32_t>( _busInputMap.size() );
    }

    BusInputMeta Bus::getConnectedInput( uint32_t position ) const
    {
        if( position < getCapacity() )
        {
            for( auto& it : _busInputMap )
            {
                if( it.second == (int)position )
                    return { it.first->getObject().getId(), _gains[position] };
            }
        }
        return {};
    }

    void Bus::getConnectedInputs( std::vector<BusInputMeta>& Inputs ) const
    {
        auto capacity = getCapacity();
        Inputs.resize( capacity );

        for( int i = 0; i < _busInputMap.size(); i++ )
        {
            Inputs[i] = getConnectedInput( i );
        }
    }

    // Processing Chain within BUS...
    uint32_t Bus::createInsert( const char* type )
    {
        return _chain.createInsert( type );
    }

    uint32_t Bus::createInsert( std::unique_ptr<Object> obj )
    {
        return _chain.createInsert( std::move( obj ) );
    }

    Object* Bus::getObject( uint32_t Id )
    {
        return _chain.getObject( Id );
    }

    bool Bus::insertObject( uint32_t Id, uint32_t pos )
    {
        return _chain.insertObject( Id, pos );
    }

    bool Bus::removeInsertById( uint32_t Id )
    {
        return _chain.removeInsertById( Id );
    }

    bool Bus::removeInsertByPos( uint32_t pos )
    {
        return _chain.removeInsertByPos( pos );
    }

    bool Bus::deleteInsert( uint32_t Id )
    {
        return _chain.deleteInsert( Id );
    }

    void Bus::setParameter( int Index, float Value )
    {
        _chain.setParameter( Index, Value );
    }

    float Bus::getParameter( int Index )
    {
        return _chain.getParameter( Index );
    }

    std::pair<float, float> Bus::getOutputLevelAndPan()
    {
        return _chain.getLevelAndPan();
    }

    void Bus::setOutputLevelAndPan( float Gain, float Pan )
    {
        _chain.setLevelAndPan( Gain, Pan );
    }

    void Bus::prepareToPlay( uint32_t SampleRate, uint32_t BlockSize )
    {
        Object::prepareToPlay( SampleRate, BlockSize );
        _chain.prepareToPlay( SampleRate, BlockSize );
    }

    void Bus::process( Buffer<float>& Buffer )
    {
        if( _busInputMap.size() )
        {
            _mixOutBuffer.stereoMixer( Buffer, _gains );
            _chain.process( _chainMeta.getBuffer() );
        }
        else
        {
            _chainMeta.getBuffer().clearOutputs();
        }
    }

    bool Bus::findFreeIndex( uint32_t& Index )
    {
        auto& buffer = _busmeta->getBuffer();
        if( uint32_t inputsize = buffer.getNumInputChannels() )
        {
            for( uint32_t ch = 0; ch < inputsize - 1; ch += 2 )
            {
                if( buffer.isInputFree( ch ) && buffer.isInputFree( ch + 1 ) )
                {
                    Index = ch;
                    return true;
                }
            }
        }
        return false;
    }
} // Namespace AudIO
