
#include "Mixer.h"

namespace AudIO
{
    Mixer::Mixer( uint32_t inputs )
        : AudIO::Object( ( inputs >> 2 ) << 2, 2, Mixer::kParamMax )
    {
        iGain.resize( ( inputCount() / 2 ), RampedParameter{ 1.f } );
    }

    void Mixer::setParameter( int index, float value )
    {
        switch( index )
        {
            break;
            case Mixer::kParamOutputGain:
                oGain = value;
                break;
            case Mixer::kParamPan:
                pan = value;
                break;
            case Mixer::kParamInputGain:
                for( auto& it : iGain )
                    it = value;
        }
    }

    float Mixer::getParameter( int index )
    {
        switch( index )
        {
            break;
            case Mixer::kParamOutputGain:
                return oGain.value();
                break;
            case Mixer::kParamPan:
                return pan;
                break;
            case Mixer::kParamInputGain:
                return iGain[0].value();
        }
        return .0f;
    }

    void Mixer::process( AudIO::Buffer<float>& Buffer )
    {
        oGain.getNext( Object::blockSize() );
        for( auto& it : iGain )
            it.getNext( Object::blockSize() );

        AudIO::Panner<float> _pan( pan );

        const auto numIpChans = inputCount();

        uint32_t ch        = 0;
        auto     leftgain  = iGain[ch / 2].getCurrent() * _pan.getLeft();
        auto     rightgain = iGain[ch / 2].getCurrent() * _pan.getRight();

        Buffer.copyInputToOutput( ch++, 0, leftgain );
        Buffer.copyInputToOutput( ch++, 1, rightgain );

        for( ; ch < numIpChans; ch += 2 )
        {
            leftgain  = iGain[ch / 2].getCurrent() * _pan.getLeft();
            rightgain = iGain[ch / 2].getCurrent() * _pan.getRight();

            Buffer.addInputToOutputWithGain( ch, 0, leftgain );
            Buffer.addInputToOutputWithGain( ch + 1, 1, rightgain );
        }
    }
}
