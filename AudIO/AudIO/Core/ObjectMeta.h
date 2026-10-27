#pragma once

#include "../Includes/AudioObject.h"
#include "PatchQueue.h"

#include <functional>
#include <unordered_map>

namespace AudIO
{
    class objectMeta
    {
    public:
        friend class Design;
        friend class Bus;
        objectMeta( Object& object, bool allocateOuts = true )
            : _object( object )
            , _buffer( object.inputCount(), false, object.outputCount(), allocateOuts )
            , _outputs( object.outputCount() )
        {}

        virtual ~objectMeta() = default;

        void removeConnectionsFrom()
        {
            for( const auto& [objMeta, patchMap] : _outputs )
            {
                if( objMeta )
                {
                    for( const auto& [first, second] : patchMap )
                    {
                        unpatch( *objMeta, second, first );
                    }
                }
            }
        }

        void prepareToPlay( uint32_t samplerate, uint32_t blocksize )
        {
            _object.prepareToPlay( samplerate, blocksize );
        }

        void process()
        {
            _object.process( _buffer );
        }

        inline Object& getObject()
        {
            return _object;
        }

        inline Buffer<float>& getBuffer()
        {
            return _buffer;
        }

        bool patch( objectMeta& dstmeta, uint32_t srcch, uint32_t dstch )
        {
            if( utils::isBelow( dstch, dstmeta.getObject().inputCount() ) && utils::isBelow( srcch, _object.outputCount() ) )
            {
                // Design Assumption: If Buffer is not null, That means some connection exists already
                // if( dstmeta.getBuffer().getInputBuffer( dstch ) )
                //     return false;
                // Already Patched
                if( connectionExists( dstmeta, dstch ) )
                    return false;

                if( auto block = _buffer.getOutputBuffer( srcch ) )
                {
                    _outputs[&dstmeta][dstch] = srcch;
                    PatchQueue<float>::get().patchInput( &dstmeta.getBuffer(), block, dstch );
                    return true;
                }
            }
            return false;
        }

        bool unpatch( objectMeta& dstmeta, uint32_t srcch, uint32_t dstch )
        {
            if( utils::isBelow( dstch, dstmeta.getObject().inputCount() ) && utils::isBelow( srcch, _object.outputCount() ) )
            {
                // Design Assumption: If Buffer is null, That means no connection exists
                // if( !dstmeta.getBuffer().getInputBuffer( dstch ) )
                //     return false;
                // not patched to anything
                if( !connectionExists( dstmeta, dstch ) )
                    return false;

                auto& m = _outputs[&dstmeta];
                if( m.erase( dstch ) )
                {
                    if( m.size() == 0 )
                    {
                        _outputs.erase( &dstmeta );
                    }
                    // TBD::: dstmeta.getBuffer().markinputforUnpatch( dstch );
                    PatchQueue<float>::get().patchInput( &dstmeta.getBuffer(), nullptr, dstch );
                    return true;
                }
            }
            return false;
        }

        bool connectionExists( objectMeta& dstmeta, uint32_t dstch )
        {
            if( auto it = _outputs.find( &dstmeta ); it != _outputs.end() )
            {
                auto& m = it->second;
                if( m.find( dstch ) != m.end() )
                    return true;
            }
            return false;
        }

        void removeConnectionsTo( objectMeta* meta )
        {
            _outputs.erase( meta );
        }

        void visitAll( const std::function<void( objectMeta& )>& func, bool& stop )
        {
            for( const auto& [objMeta, map] : _outputs )
            {
                if( stop )
                    return;
                if( objMeta )
                    func( *objMeta );
            }
        }
        void markDfs()
        {
            _dfs++;
        }
        void clearDfs()
        {
            _dfs = 0;
        }
        uint32_t dfs() const
        {
            return _dfs;
        }

        void markVisited()
        {
            _visited++;
        }
        void clearVisited()
        {
            _visited = 0;
        }
        uint32_t visited() const
        {
            return _visited;
        }

        Object&                                                                 _object;
        Buffer<float>                                                           _buffer;
        std::unordered_map<objectMeta*, std::unordered_map<uint32_t, uint32_t>> _outputs;
        uint32_t                                                                _visited{ 0 };
        uint32_t                                                                _dfs{ 0 };
    };
} // Namespace AudIO
