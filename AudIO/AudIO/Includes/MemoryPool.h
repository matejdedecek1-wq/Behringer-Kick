#pragma once

#include <memory>
#include <vector>

#include "Utils.h"

namespace AudIO
{
    template<typename T>
    class MemoryPool
    {
    public:
        static MemoryPool<T>& get()
        {
            static MemoryPool<T> _s;
            return _s;
        }

        // Memory pointed by `poolMemPtr` should be atleast `(poolsize * blocksize)` bytes.
        void initialize( T* poolMemPtr, uint16_t poolsize, uint32_t blocksize )
        {
            partitionPoolMemory( poolMemPtr, poolsize, blocksize );
        }

        void release( T* Ptr )
        {
            int index = getPoolIndex( Ptr );
            if( utils::isBetween( index, -1, refCounts_.size() ) && ( 0 < refCounts_[index] ) )
            {
                --refCounts_[index];
                if( refCounts_[index] <= 0 )
                {
                    ++free_;
                }
            }
        }

        T* acquire( T* Ptr, bool Allocate = false )
        {
            if( nullptr == Ptr && Allocate )
            {
                return allocate_();
            }
            else
            {
                if( const int index = getPoolIndex( Ptr ); 0 <= index )
                {
                    ++refCounts_[index];
                    return getPoolMemory( index );
                }
            }
            return nullptr;
        }

        inline unsigned int getCapacity() const { return capacity_; }
        inline unsigned int getPeakUsed() const { return peak_; }
        inline unsigned int getBlockSize() const { return blockSize_; }

        unsigned int getUsed() const { return ( capacity_ - free_ ); }
        unsigned int getFree() const { return free_; }

        float getUsedMemoryPercentage() const { return (float)( invCapacity_ * getUsed() ); }
        float getFreeMemoryPercentage() const { return (float)( invCapacity_ * getFree() ); }

    private:
        MemoryPool<T>() = default;

        void partitionPoolMemory( T* poolMemPtr, unsigned int numblocks, unsigned int blocksize )
        {
            assert( numblocks != 0 );
            assert( blocksize != 0 );

            blockSize_ = blocksize;

            auto poolSize = numblocks * blocksize;
            auto memPtr   = poolMemPtr;
            // Clear allocated memory space...
            utils::clear( memPtr, poolSize );
            // Compute meta info to manage pools.
            refCounts_.resize( numblocks, 0 );
            // Update boundaries..
            poolStart = memPtr;
            poolEnd   = ( memPtr + poolSize );
            // Update statiscs.
            capacity_ += numblocks;
            free_ += numblocks;
            invCapacity_ = 100.f / (float)capacity_;
        }

        int getPoolIndex( T* Ptr )
        {
            if( ( poolStart <= Ptr ) && ( Ptr < poolEnd ) )
            {
                if( 0 == ( ( Ptr - poolStart ) % blockSize_ ) )
                {
                    int index = ( Ptr - poolStart ) / blockSize_;
                    return index;
                }
            }
            return -1;
        }

        int findFreeIndex()
        {
            if( 0 < free_ )
            {
                for( int i = 0; i < refCounts_.size(); ++i )
                    if( 0 == refCounts_[i] )
                        return i;
            }
            return -1;
        }

        // Ensure this functions is always called from bounds check.
        T* getPoolMemory( int Index )
        {
            return ( poolStart + ( Index * blockSize_ ) );
        }

        T* allocate_( void )
        {
            auto index = findFreeIndex();
            if( 0 <= index )
            {
                // obtain memory and as mark aquired...
                ++refCounts_[index];
                auto blockAddress = getPoolMemory( index );

                // Update statistics...
                --free_;
                peak_ = utils::max( peak_, ( capacity_ - free_ ) );

                // Clear allocated memory...
                utils::clear( blockAddress, blockSize_ );
                return blockAddress;
            }
            assert( 0 <= index );
            return nullptr;
        }

    private:
        T* poolStart{ nullptr };
        T* poolEnd{ nullptr };

        std::vector<int> refCounts_;
        unsigned int     blockSize_{ 0 };
        unsigned int     capacity_{ 0 };
        unsigned int     peak_{ 0 };
        unsigned int     free_{ 0 };
        float            invCapacity_;
    };

    using MemoryPoolFloat = MemoryPool<float>;
} // Namespace AudIO
