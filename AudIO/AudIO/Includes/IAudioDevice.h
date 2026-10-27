#pragma once
#include "AudIOBuffer.h"
#include "IProcessable.h"
#include <atomic>

namespace AudIO
{
    enum class AudioDeviceState
    {
        uninitialized,
        playing,
        stopped,
        paused,
    };

    class IAudioDevice
    {
    public:
        IAudioDevice( uint32_t nInputs, uint32_t nOutputs )
            : _buffer( nInputs, true, nOutputs, true ) {}

        inline bool isPaused() const noexcept { return AudioDeviceState::paused == _state; }
        inline bool isPlaying() const noexcept { return AudioDeviceState::playing == _state; }
        inline bool isInitialized() const noexcept { return AudioDeviceState::uninitialized != _state; }
        inline bool isInputDevice() const { return _buffer.hasInputs(); }
        inline bool isOutputDevice() const { return _buffer.hasOutputs(); }

        inline Buffer<float>&   getBuffer() noexcept { return _buffer; }
        inline AudioDeviceState getState() const noexcept { return _state; }

        inline void setState( AudioDeviceState State ) noexcept { _state.store( State ); }
        inline void setCallback( IProcessable* cb ) noexcept { _callback.store( cb ); }

        ///
        /// Virtual functions
        ///
        virtual void start( uint32_t samplerate, uint32_t blocksize ) = 0;
        virtual void resume()                                         = 0;
        virtual void pause()                                          = 0;
        virtual void stop()                                           = 0;

    protected:
        virtual void prepareToPlay( uint32_t samplerate, uint32_t blocksize )
        {
            if( auto ptr = _callback.load() )
                ptr->prepareToPlay( samplerate, blocksize );
        }

        virtual void teardown()
        {
            if( auto ptr = _callback.load() )
                ptr->teardown();
        }

        virtual void process( Buffer<float>& buf )
        {
            if( auto ptr = _callback.load() )
                ptr->process( buf );
        }

        Buffer<float>                 _buffer;
        std::atomic<IProcessable*>    _callback = nullptr;
        std::atomic<AudioDeviceState> _state{ AudioDeviceState::uninitialized };
    };
}
