
#include "../Includes/Design.h"
#include "Console.h"
#include "DeviceManager.h"
#include "SFSynth.h"
#include "SineWave.h"
#include "StereoOut.h"
#include <iomanip>
#include <iostream>
#include <stdlib.h>
#include <thread>

std::shared_ptr<AudIO::Design> pMainDesign{ nullptr };
std::shared_ptr<AudIO::Design> pSubDesign1{ nullptr };
std::shared_ptr<AudIO::Design> pSubDesign2{ nullptr };

#define MEM_POOL_SIZE 30
#define SAMPLE_RATE   48000
#define BLOCK_SIZE    960

// NOTE::
// This buffer will be used by AudIO to create audio graph.
// Ensure that this has sufficient storage to accomodate worst case design requirement.
float _audioPoolMemBuffer[( MEM_POOL_SIZE * BLOCK_SIZE )];

int main()
{
    AudIO::MemoryPool<float>::get().initialize( _audioPoolMemBuffer, MEM_POOL_SIZE, BLOCK_SIZE );
    AudIO::DeviceManager devmgr;
    devmgr.addAudioDevice( "StereoOut", new AudIO::StereoOut(), true );

    pMainDesign = std::make_shared<AudIO::Design>( 0, 2 );
    pMainDesign->setName( "Main Design" );

    pSubDesign1 = std::make_shared<AudIO::Design>( 0, 2 );
    pSubDesign1->setName( "Sub Design 1" );
    auto subdesign1 = pMainDesign->insertDesign( pSubDesign1 );

    auto sine1 = pSubDesign1->createObject( "SineWave" );
    pSubDesign1->setDesignOutput( sine1 );

    pSubDesign2 = std::make_shared<AudIO::Design>( 0, 2 );

    pSubDesign2->setName( "Sub Design 2" );

    auto subdesign2 = pMainDesign->insertDesign( pSubDesign2 );

    auto synth1 = pSubDesign2->createObject( "SFSynth" );
    pSubDesign2->setDesignOutput( synth1 );

    auto pSynth1 = dynamic_cast<AudIO::SFSynth*>( pSubDesign2->getObject( synth1 ) );
    pSynth1->setName( "Synth1" );

    auto sine2  = pMainDesign->createObject( "SineWave" );
    auto pSine2 = pMainDesign->getObject( sine2 );
    pSine2->setName( "Sine2" );
    pSine2->setParameter( AudIO::SineWave::kParamFreq, 400 );
    pSine2->setParameter( AudIO::SineWave::kParamAmplitude, 1.0f );

    pMainDesign->addMeter( subdesign1 );
    pMainDesign->addMeter( subdesign2 );

    auto bus1 = pMainDesign->createBus();
    auto pBus = pMainDesign->getObject( bus1 );
    if( pBus )
    {
        pBus->setName( "Bus1" );
    }
    pMainDesign->sendToBus( bus1, subdesign1, 1.0f );
    pMainDesign->sendToBus( bus1, sine2, 1.0f );

    pMainDesign->addMeter( bus1 );
    pMainDesign->setDesignOutput( bus1 );

    auto stereoOut = devmgr.getAudioDevice( "StereoOut" );
    pMainDesign->setBypass( false );
    pSubDesign1->setBypass( false );
    pSubDesign2->setBypass( false );
    devmgr.setAudioCallback( pMainDesign.get() );

    bool b{ false };
    auto start = std::chrono::system_clock::now();
    auto end   = std::chrono::system_clock::now();
    AudIO::Console::get().reInit();
    AudIO::MeterData _data;
    std::cout << "USED MEM: " << AudIO::MemoryPool<float>::get().getUsed() << std::endl;

    devmgr.start( SAMPLE_RATE, BLOCK_SIZE );
    pSynth1->programChange( 1, 1 );
    pSynth1->programChange( 3, 2 );
    pSynth1->programChange( 4, 3 );
    pSynth1->programChange( 5, 4 );
    uint8_t count = 0;
    while( true )
    {
        if( pSubDesign2 )
        {
            pSynth1->channelNoteOn( 48, 80, 1 );
            std::this_thread::sleep_for( std::chrono::milliseconds( 50 ) );

            pSynth1->channelNoteOn( 48, 80, 2 );
            std::this_thread::sleep_for( std::chrono::milliseconds( 50 ) );

            pSynth1->channelNoteOn( 48, 80, 3 );
            std::this_thread::sleep_for( std::chrono::milliseconds( 50 ) );

            pSynth1->channelNoteOn( 48, 80, 4 );
            std::this_thread::sleep_for( std::chrono::milliseconds( 50 ) );

            pSynth1->noteOffAll();
        }
        if( b )
        {
            std::cout << pMainDesign->dumpMeter( subdesign2 );
        }
        else
        {
            std::cout << pMainDesign->dumpMeter( subdesign1 );
        }
        std::cout << pMainDesign->dumpMeter( bus1 );

        std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
        end                                = std::chrono::system_clock::now();
        std::chrono::duration<double> diff = end - start;
        if( diff.count() > 2.0 )
        {
            b = !b;
            count++;
            if( count == 4 )
            {
                pMainDesign->removeObject( subdesign2 );
                std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
                pSubDesign2.reset();
            }
            start = std::chrono::system_clock::now();

            if( count < 4 )
            {
                if( b )
                {
                    pMainDesign->removeFromBus( bus1, subdesign1 );
                    pMainDesign->sendToBus( bus1, subdesign2, 1.0f );
                }
                else
                {
                    pMainDesign->removeFromBus( bus1, subdesign2 );
                    pMainDesign->sendToBus( bus1, subdesign1, 1.0f );
                }
            }
        }
    }
}
