#pragma once

#include "../Includes/AudioObject.h"

namespace AudIO
{
    class WhiteNoise : public AudIO::Object
    {
    public:
        enum
        {
            kParamMax = 0
        };
        WhiteNoise()
            : AudIO::Object( 0, 2, WhiteNoise::kParamMax ) {}

    private:
        void process( AudIO::Buffer<float>& buffer ) override
        {
            if( const auto numOuts = (int)buffer.getNumOutputChannels() )
            {
                auto _blkSize = buffer.getBlockSize();
                for( uint32_t sample = 0; sample < _blkSize; ++sample )
                {
                    float value = ( ( ( ( (float)std::rand() ) / RAND_MAX ) * 2.0f ) - 1.0f ) * 0.20f;

                    for( auto ch = 0; ch < numOuts; ++ch )
                        if( auto ptr = buffer.getOutputBuffer( ch ) )
                            ptr[sample] = value;
                }
            }
        }
    };

    REGISTER_OBJECT( "WhiteNoise", WhiteNoise );
}
