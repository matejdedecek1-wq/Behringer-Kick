#pragma once

#include "../Includes/AudIOBuffer.h"
#include "SPSCQueue.h"

namespace AudIO
{

    enum class PatchType
    {
        input,
        output
    };

    template<typename T>
    struct PatchData
    {
        Buffer<T>* dest   = nullptr;
        T*         src    = nullptr;
        uint32_t   destch = 0;
        PatchType  type   = PatchType::input;
    };

    template<typename T>
    struct PatchQueue
    {
        static struct PatchQueue& get()
        {
            static struct PatchQueue _s;
            return _s;
        }

        void start()
        {
            process();
            _running.store( true );
        }

        void stop()
        {
            _running.store( false );
            process();
        }

        void process()
        {
            PatchData<T> data;
            while( _queue.pop( data ) )
            {
                process( data );
            }
        }

        void patchInput( Buffer<T>* destination, T* src, uint32_t destCh )
        {
            if( destination )
            {
                destination->acquireInputForPatch( destCh );
                PatchData<T> data;
                data.dest   = destination;
                data.src    = src;
                data.destch = destCh;
                data.type   = PatchType::input;
                push( data );
            }
        }

        void patchOutput( Buffer<T>* destination, T* src, uint32_t destCh )
        {
            if( destination )
            {
                PatchData<T> data;
                data.dest   = destination;
                data.src    = src;
                data.destch = destCh;
                data.type   = PatchType::output;
                push( data );
            }
        }

    private:
        void push( PatchData<T>& data )
        {
            if( _running )
            {
                _queue.push( data );
            }
            else
            {
                process( data );
            }
        }

        void process( PatchData<T>& data )
        {
            if( data.type == PatchType::input )
            {
                data.dest->patchInput( data.src, data.destch );
            }
            else
            {
                data.dest->patchOutput( data.src, data.destch );
            }
        }
        std::atomic<bool>           _running{ false };
        SPSCQUeue<PatchData<T>, 64> _queue;
    };
}
