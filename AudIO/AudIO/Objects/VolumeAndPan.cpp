#include "VolumeAndPan.h"

namespace AudIO
{
    VolumeAndPan::VolumeAndPan( uint32_t inputs )
        : AudIO::Object( 2, 2, VolumeAndPan::kParamMax )
    {
        inputs;
    }

    void VolumeAndPan::setParameter( int index, float value )
    {
        switch( index )
        {
            default: break;
            case VolumeAndPan::kParamGain: gain = value; break;
            case VolumeAndPan::kParamPan: pan = value; break;
        }
    }

    float VolumeAndPan::getParameter( int index )
    {
        switch( index )
        {
            default: break;
            case VolumeAndPan::kParamGain: return gain.getTarget(); break;
            case VolumeAndPan::kParamPan: return pan.getTarget(); break;
        }
        return .0f;
    }

    float VolumeAndPan::getGain( uint32_t channel )
    {
        AudIO::Panner<float> _pan( pan.getCurrent(), gain.getCurrent() );
        return channel ? _pan.getLeft() : _pan.getRight();
    }

    void VolumeAndPan::process( AudIO::Buffer<float>& buffer )
    {
        auto pIns  = buffer.getInputs();
        auto pOuts = buffer.getOutputs();

        float lGain = 1;
        float rGain = 1;

        for( uint32_t sample = 0; sample < blockSize(); ++sample )
        {
            pan.getNext();
            gain.getNext();

            lGain = getGain( 0 );
            rGain = getGain( 1 );

            for( uint32_t channel = 0; channel < buffer.getNumInputChannels(); channel++ )
            {
                pOuts[channel][sample] = pIns[channel][sample] * ( ( channel & 0x01 ) ? rGain : lGain );
            }
        }

        /*
        pan.getNext( Object::blockSize() );
        gain.getNext( Object::blockSize() );

        auto lGain = getGain( 0 );
        auto rGain = getGain( 1 );

        for( uint32_t ch = 0; ch < inputCount(); ++ch )
        {
            buffer.copyInputToOutput( ch, ( ch & 0x01 ) ? rGain : lGain );
        }
        */
    }
}