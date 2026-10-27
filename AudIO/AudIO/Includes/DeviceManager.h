#pragma once

#include "IAudioDevice.h"

#include <list>
#include <map>

namespace AudIO
{
    class DeviceManager : public IProcessable
    {
    public:
        DeviceManager() = default;

        void start( uint32_t samplerate, uint32_t blocksize ) const
        {
            for( const auto& [name, device] : _audiodevices )
                device->start( samplerate, blocksize );

            resume();
        }

        void pause() const
        {
            for( const auto& [name, device] : _audiodevices )
                device->pause();
        }

        void resume() const
        {
            for( const auto& [name, device] : _audiodevices )
                device->resume();
        }

        void stop() const
        {
            for( const auto& [name, device] : _audiodevices )
                device->stop();
        }

        bool addAudioDevice( const char* name, IAudioDevice* device, bool master )
        {
            if( name && device && ( _audiodevices.find( std::string( name ) ) == _audiodevices.end() ) )
            {
                if( master )
                {
                    if( _master )
                        _master->setCallback( nullptr );

                    _master = device;
                    _master->setCallback( this );
                }
                _audiodevices[std::string( name )] = device;
                return true;
            }
            return false;
        }

        IAudioDevice* getAudioDevice( const char* name )
        {
            IAudioDevice* dev = nullptr;
            if( name )
            {
                auto it = _audiodevices.find( std::string( name ) );
                if( it != _audiodevices.end() )
                {
                    dev = it->second;
                }
            }
            return dev;
        }

        void setAudioCallback( IProcessable* listener )
        {
            _activeListener.store( listener );
        }

        void prepareToPlay( uint32_t samplerate, uint32_t blocksize ) override
        {
            if( auto ptr = _activeListener.load() )
            {
                ptr->prepareToPlay( samplerate, blocksize );
            }
        }

        void process( Buffer<float>& Buffer ) override
        {
            if( auto ptr = _activeListener.load() )
            {
                ptr->process( Buffer );
            }
        }

        void teardown() override
        {
            if( auto ptr = _activeListener.load() )
            {
                ptr->teardown();
            }
        }

        std::map<std::string, IAudioDevice*> _audiodevices;
        std::atomic<IProcessable*>           _activeListener{ nullptr };
        IAudioDevice*                        _master{ nullptr };
    };
}
