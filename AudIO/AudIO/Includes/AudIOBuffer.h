#pragma once

#include <vector>
#include <memory>

#include "AudioDataTypes.h"
#include "MemoryPool.h"
#include "Utils.h"
#include "../Dsp/pan.h"

namespace AudIO
{
    template<typename T>
    using ChBufferPtr = std::shared_ptr<T*>;
    template<typename T>
    using PortBuffer = std::vector<ChBufferPtr<T>>;

    template<typename T>
    class Buffer
    {
    public:
        Buffer( uint32_t nInputs = 0, bool allocateIns = false, uint32_t nOutputs = 0, bool allocateOuts = false )
        {
            resizeInputs( nInputs, allocateIns );
            resizeOutputs( nOutputs, allocateOuts );
        }

        ~Buffer()
        {
            for( auto& it : inputs )
            {
                MemoryPool<T>::get().release( *it );
            }
            for( auto& it : outputs )
            {
                MemoryPool<T>::get().release( *it );
            }
        }

        // make this class non-copyable...
        //Buffer( Buffer const& )            = delete;
        Buffer& operator=( Buffer const& ) = delete;

        inline bool hasInputs() const { return 0 < outputs.size(); }
        inline bool hasOutputs() const { return 0 < outputs.size(); }

        inline bool     isOutputMono() const { return 1 == (uint32_t)outputs.size(); }
        inline bool     isOutputStereo() const { return 2 <= (uint32_t)outputs.size(); }
        inline uint32_t getNumInputChannels() const { return (uint32_t)inputs.size(); }
        inline uint32_t getNumOutputChannels() const { return (uint32_t)outputs.size(); }

        inline uint32_t getBlockSize() const { return blockSize; }

        const T** getInputs() { return ins.data(); }
        T**       getOutputs() { return outs.data(); }

        // Clear Buffer...
        void clearInput( uint32_t channel )
        {
            if( auto in = getInputBuffer( channel ) )
                utils::clear<T>( in, getBlockSize() );
        }

        void clearInputs()
        {
            for( auto ch = 0; ch < getNumInputChannels(); ++ch )
                clearInput( ch );
        }

        void clearOutput( uint32_t channel )
        {
            if( auto out = getOutputBuffer( channel ) )
                utils::clear<T>( out, getBlockSize() );
        }

        void clearOutputs()
        {
            for( auto ch = 0; ch < (int)getNumOutputChannels(); ++ch )
                clearOutput( ch );
        }

        // COPY API's...
        void copyInputToOutput( uint32_t InCh, uint32_t OutCh, float Gain = 1.f )
        {
            if( auto out = getOutputBuffer( OutCh ) )
                if( auto in = getInputBuffer( InCh ) )
                    utils::copyWithGain<T, float>( out, in, getBlockSize(), Gain );
        }

        void copyInputToOutput( uint32_t ch, float Gain = 1.f )
        {
            copyInputToOutput( ch, ch, Gain );
        }

        void copyInputsToOutputs( float Gain = 1.f )
        {
            auto count = std::min( inputs.size(), outputs.size() );
            for( uint32_t ch = 0; ch < count; ch++ )
                copyInputToOutput( ch, ch, Gain );
        }

        void passthrough( float Gain = 1.f )
        {
            copyInputsToOutputs( Gain );
        }

        // Gain API's
        void applyGain( uint32_t channel, T gain )
        {
            auto len = getBlockSize();
            if( auto out = getOutputBuffer( channel ) )
                utils::multiply<T>( out, len, gain );
        };

        void applyGain( T gain )
        {
            auto numChannels = (int)getNumOutputChannels();
            for( auto ch = 0; ch < numChannels; ++ch )
                applyGain( ch, gain );
        }

        void applyGainAndPan( T gain, T pan )
        {
            if( utils::isEqual( 1.f, gain ) && utils::isEqual( 0.f, pan ) )
                return;

            if( 2 == (int)getNumOutputChannels() )
            {
                Panner<T> panner( pan );
                auto      l = gain * panner.getLeft();
                auto      r = gain * panner.getRight();

                applyGain( 0, l );
                applyGain( 1, r );
            }
            else
            {
                applyGain( gain );
            }
        }

        // Downmix to Mono
        void stereoToMono( T gain, bool toLeft ) // true : Left Channel; false : Right Channel.
        {
            if( 2 <= (int)getNumOutputChannels() )
            {
                auto l   = getOutputBuffer( 0 );
                auto r   = getOutputBuffer( 1 );
                auto dst = toLeft ? l : r;
                auto clr = toLeft ? r : l;

                for( uint32_t i = 0; i < getBlockSize(); ++i )
                {
                    dst[i] = ( 0.5f * ( l[i] + r[i] ) ) * gain;
                    clr[i] = 0;
                }
                //utils::compressor<T>( dst, getBlockSize() );
            }
            else
            {
                applyGain( gain );
            }
        }

        // Addition API's
        void addInputToOutput( uint32_t input, uint32_t output )
        {
            if( auto out = *outputs[output] )
                if( auto in = *inputs[input] )
                    utils::add<T>( out, in, getBlockSize() );
        }

        void addInputToOutputWithGain( uint32_t input, uint32_t output, float gain )
        {
            if( auto out = *outputs[output] )
                if( auto in = *inputs[input] )
                    utils::addWithGain<T, float>( out, in, getBlockSize(), gain );
        }

        // Compress API;s
        void compressOutput( uint32_t channel )
        {
            if( auto out = getOutputBuffer( channel ) )
                utils::compressor<T>( out, getBlockSize() );
        }

        void compressOutputs()
        {
            for( auto ch = 0; ch < getNumOutputChannels(); ++ch )
                compressOutput( ch );
        }

        // Mixer API...
        void stereoMixer( Buffer<float>& InBuffer, std::vector<float>& Gains )
        {
            if( getNumOutputChannels() == 2 )
            {
                if( auto numMixStereoChannels = utils::min( (int)Gains.size(), (int)( InBuffer.getNumInputChannels() / 2 ) ) )
                {
                    clearOutputs();

                    const auto size     = getBlockSize();
                    auto       leftOut  = getOutputBuffer( 0 );
                    auto       rightOut = getOutputBuffer( 1 );

                    for( auto ch = 0; ch < numMixStereoChannels; ++ch )
                    {
                        auto gain = Gains[ch];
                        if( utils::isNotEqual( 0.f, gain ) )
                        {
                            if( auto leftIn = InBuffer.getInputBuffer( ( 2 * ch ) ) )
                            {
                                utils::addWithGain<float>( leftOut, leftIn, size, gain );
                            }
                            if( auto rightIn = InBuffer.getInputBuffer( ( 1 + ( 2 * ch ) ) ) )
                            {
                                utils::addWithGain<float>( rightOut, rightIn, size, gain );
                            }
                        }
                    }
                    /*
                    uint32_t ch            = 0;
                    auto     nMonoChannels = (uint32_t)( 2 * numMixStereoChannels );
                    auto     size          = getBlockSize();
                    auto     gain          = Gains[ch / 2];

                    if( auto leftIn = *InBuffer.inputs[0] )
                    {
                        utils::copyWithGain<float>( leftOut, leftIn, size, gain );
                    }
                    if( auto rightIn = *InBuffer.inputs[1] )
                    {
                        utils::copyWithGain<float>( rightOut, rightIn, size, gain );
                    }
                    for( ch = 2; ch < nMonoChannels; ch += 2 )
                    {
                        gain = Gains[ch / 2];
                        if( utils::isNotEqual( 0.f, gain ) )
                        {
                            if( auto leftIn = *InBuffer.inputs[ch] )
                            {
                                utils::addWithGain<float>( leftOut, leftIn, size, gain );
                            }
                            if( auto rightIn = *InBuffer.inputs[ch + 1] )
                            {
                                utils::addWithGain<float>( rightOut, rightIn, size, gain );
                            }
                        }
                    }
                    */
                    //compressOutputs();
                    return;
                }
            }
            InBuffer.clearOutputs();
        }

        // RMS Measurement API's...
        T getInputRMSValue( uint32_t channel ) const
        {
            if( auto in = getInputBuffer( channel ) )
                return utils::rms<T>( in, getBlockSize() );
            return T( 0 );
        }

        T getOutputRMSValue( uint32_t channel ) const
        {
            if( auto out = getOutputBuffer( channel ) )
                return utils::rms<T>( out, getBlockSize() );
            return T( 0 );
        }

        inline T* getInputBuffer( uint32_t channel ) const
        {
            return utils::isBelow( channel, (uint32_t)inputs.size() ) ? *inputs[channel] : nullptr;
        }

        inline T* getOutputBuffer( uint32_t channel ) const
        {
            return utils::isBelow( channel, (uint32_t)outputs.size() ) ? *outputs[channel] : nullptr;
        }

        // API's related to connections...
        bool acquireInputForPatch( uint32_t channel )
        {
            if( utils::isBelow( channel, (unsigned int)inputs.size() ) && !inputPatched[channel] )
            {
                inputPatched[channel] = true;
                return true;
            }
            return false;
        }

        bool patchInput( T* block, uint32_t channel )
        {
            if( utils::isBelow( channel, (unsigned int)inputs.size() ) )
            {
                auto& container = inputs[channel];
                // Release earlier patch
                MemoryPool<T>::get().release( *container );
                // Create new patch
                *container            = MemoryPool<T>::get().acquire( block );
                inputPatched[channel] = ( nullptr != block );
                refreshInputs();
                return true;
            }
            return false;
        }

        bool patchOutput( T* block, uint32_t channel )
        {
            if( utils::isBelow( channel, (unsigned int)outputs.size() ) )
            {
                auto& container = outputs[channel];
                if( *container != block )
                {
                    // Release earlier patch
                    MemoryPool<T>::get().release( *container );
                    *container = nullptr;
                    // Create new patch
                    *container = MemoryPool<T>::get().acquire( block, true );
                    refreshOutputs();
                    return true;
                }
            }
            return false;
        }

        /// Operator overloads
        Buffer<T>& operator=( Buffer<T>& In )
        {
            auto minChannels = std::min( getNumOutputChannels(), In.getNumOutputChannels() );
            auto len         = getBlockSize();
            for( auto i = 0; i < minChannels; ++i )
            {
                auto in  = In.getOutputBuffer( i );
                auto out = getOutputBuffer( i );
                if( in && out )
                    utils::copy<T>( out, in, len );
            }
            return *this;
        }

        Buffer<T>& operator=( T Factor )
        {
            auto minChannels = getNumOutputChannels();
            auto len         = getBlockSize();
            for( uint32_t i = 0; i < minChannels; ++i )
                if( auto out = getOutputBuffer( i ) )
                    utils::copy<T>( out, len, Factor );
            return *this;
        }

        Buffer<T>& operator+=( Buffer<T>& In )
        {
            auto minChannels = std::min( getNumOutputChannels(), In.getNumOutputChannels() );
            auto len         = getBlockSize();
            for( auto i = 0; i < minChannels; ++i )
            {
                auto in  = In.getOutputBuffer( i );
                auto out = getOutputBuffer( i );
                if( in && out )
                    utils::add<T>( out, in, len );
            }
            return *this;
        }

        Buffer<T>& operator+=( T Factor )
        {
            auto minChannels = getNumOutputChannels();
            auto len         = getBlockSize();
            for( auto i = 0; i < minChannels; ++i )
                if( auto out = getOutputBuffer( i ) )
                    utils::add<T>( out, len, Factor );
            return *this;
        }

        Buffer<T>& operator-=( Buffer<T>& In )
        {
            auto minChannels = std::min( getNumOutputChannels(), In.getNumOutputChannels() );
            auto len         = getBlockSize();
            for( auto i = 0; i < minChannels; ++i )
            {
                auto in  = In.getOutputBuffer( i );
                auto out = getOutputBuffer( i );
                if( in && out )
                    utils::substract<T>( out, in, len );
            }
            return *this;
        }

        Buffer<T>& operator-=( T Factor )
        {
            auto minChannels = getNumOutputChannels();
            auto len         = getBlockSize();
            for( auto i = 0; i < minChannels; ++i )
                if( auto out = getOutputBuffer( i ) )
                    utils::substract<T>( out, len, Factor );
            return *this;
        }

        Buffer<T>& operator*=( Buffer<T>& In )
        {
            auto minChannels = std::min( getNumOutputChannels(), In.getNumOutputChannels() );
            auto len         = getBlockSize();
            for( auto i = 0; i < minChannels; ++i )
            {
                auto in  = In.getOutputBuffer( i );
                auto out = getOutputBuffer( i );
                if( in && out )
                    utils::multiply<T>( out, in, len );
            }
            return *this;
        }

        Buffer<T>& operator*=( T Gain )
        {
            auto minChannels = getNumOutputChannels();
            auto len         = getBlockSize();
            for( auto i = 0; i < minChannels; ++i )
                if( auto out = getOutputBuffer( i ) )
                    utils::multiply<T>( out, len, Gain );
            return *this;
        }

        Buffer<T>& operator*=( Gains<T>& g )
        {
            auto len = getBlockSize();
            if( auto out = getOutputBuffer( 0 ) )
                utils::multiply<T>( out, len, g.l );
            if( auto out = getOutputBuffer( 1 ) )
                utils::multiply<T>( out, len, g.r );
            return *this;
        }

        Buffer<T>& operator/=( Buffer<T>& In )
        {
            auto minChannels = std::min( getNumOutputChannels(), In.getNumOutputChannels() );
            auto len         = getBlockSize();
            for( auto i = 0; i < minChannels; ++i )
            {
                auto in  = In.getOutputBuffer( i );
                auto out = getOutputBuffer( i );
                if( in && out )
                    utils::divide<T>( out, in, len );
            }
            return *this;
        }

        Buffer<T>& operator/=( T Dividor )
        {
            auto minChannels = getNumOutputChannels();
            auto len         = getBlockSize();
            for( auto i = 0; i < minChannels; ++i )
                if( auto out = getOutputBuffer( i ) )
                    utils::divide<T>( out, len, Dividor );
            return *this;
        }

    private:
        friend class objectMeta;
        friend class insertMeta;
        friend class MeterEngine;
        friend class Chain;
        friend class DesignImpl;
        friend class Bus;

        void removeAllInputs()
        {
            for( auto& container : inputs )
            {
                MemoryPool<T>::get().release( container );
                *container = nullptr;
            }
            refreshInputs();
        }

        void removeAllOutputs()
        {
            for( auto& container : outputs )
            {
                MemoryPool<T>::get().release( container );
                *container = nullptr;
            }
            refreshOutputs();
        }

        void resizeInputs( uint32_t newsize, bool allocateMemory = false )
        {
            auto oldsize = inputs.size();

            if( newsize == oldsize )
                return;

            if( newsize < oldsize )
            {
                for( uint32_t i = oldsize; i > newsize; --i )
                {
                    auto idx = i - 1;
                    // unpatch input.
                    patchInput( nullptr, idx );
                    // Remove input.
                    inputs.pop_back();
                    inputPatched.pop_back();
                }
            }
            else
            {
                for( uint32_t i = oldsize; i < newsize; ++i )
                {
                    auto& ref = inputs.emplace_back( std::make_shared<T*>() );
                    inputPatched.emplace_back( false );
                    if( allocateMemory )
                    {
                        *ref            = MemoryPool<T>::get().acquire( nullptr, true );
                        inputPatched[i] = true;
                    }
                }
            }
            blockSize = MemoryPool<T>::get().getBlockSize();
            refreshInputs();
        }

        void resizeOutputs( uint32_t newsize, bool allocateMemory = false )
        {
            auto oldsize = outputs.size();

            if( newsize == oldsize )
                return;

            if( newsize < oldsize )
            {
                for( uint32_t i = oldsize; i > newsize; --i )
                {
                    auto idx = i - 1;
                    // Release earlier patch
                    auto& ref = outputs[idx];
                    MemoryPool<T>::get().release( *ref );
                    // Remove output.
                    outputs.pop_back();
                }
            }
            else
            {
                for( uint32_t i = oldsize; i < newsize; ++i )
                {
                    auto& ref = outputs.emplace_back( std::make_shared<T*>() );
                    if( allocateMemory )
                    {
                        *ref = MemoryPool<T>::get().acquire( nullptr, true );
                    }
                }
            }
            blockSize = MemoryPool<T>::get().getBlockSize();
            refreshOutputs();
        }

        bool isInputFree( uint32_t channel )
        {
            if( utils::isBelow( channel, (uint32_t)inputs.size() ) )
            {
                return ( nullptr == *inputs[channel] ) && ( !inputPatched[channel] );
            }
            return false;
        }

        void refreshInputs()
        {
            if( ins.size() != inputs.size() )
                ins.resize( inputs.size() );

            for( int i = 0; i < (int)inputs.size(); ++i )
                ins[i] = getInputBuffer( i );
        }

        void refreshOutputs()
        {
            if( outs.size() != outputs.size() )
                outs.resize( outputs.size() );

            for( int i = 0; i < (int)outputs.size(); ++i )
                outs[i] = getOutputBuffer( i );
        }

    private:
        PortBuffer<T>         inputs;
        PortBuffer<T>         outputs;
        std::vector<bool>     inputPatched;
        std::vector<const T*> ins;
        std::vector<T*>       outs;
        uint32_t              blockSize;
        bool                  isClear{ false };
    };

    using FloatBuffer = Buffer<float>;
} // Namespace AudIO
