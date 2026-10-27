#pragma once

//#include "AudioObject.h"
#include <stdint.h>
#include "PatchQueue.h"

namespace AudIO
{
    class insertMeta
    {
    public:
        explicit insertMeta( Object& obj )
            : _object( obj )
            , _buffer( obj.inputCount(), false, obj.outputCount(), true ) {}

        virtual ~insertMeta() = default;

        inline Object&        getObject() { return _object; }
        inline Buffer<float>& getBuffer() { return _buffer; }

        inline void prepareToPlay( uint32_t samplerate, uint32_t blocksize ) { _object.prepareToPlay( samplerate, blocksize ); }
        inline void process() { _object.process( _buffer ); }

        bool patch( Buffer<float>& dstbuffer )
        {
            bool retVal = true;
            auto count  = std::min( _buffer.getNumOutputChannels(), dstbuffer.getNumInputChannels() );
            for( uint32_t ch = 0; ch < count; ++ch )
            {
                PatchQueue<float>::get().patchInput( &dstbuffer, _buffer.getOutputBuffer( ch ), ch );
            }
            return retVal;
        }

    private:
        Object&       _object;
        Buffer<float> _buffer;
    };
} // Namespace AudIO
