
#include "Console.h"
#include "DeviceManager.h"
#include "Design.h"
#include "SFSynth.h"
#include "SineWave.h"
#include "StereoOut.h"
#include <iomanip>
#include <iostream>
#include <stdlib.h>
#include <thread>

AudIO::Design*                 pDesign{ nullptr };
std::unique_ptr<AudIO::Design> pDesign1{ nullptr };
std::unique_ptr<AudIO::Design> pDesign2{ nullptr };

//// overloaded operator new,
//// allocates memory of a given size
// void* operator new(size_t size)
//{
//     void* ptr;
//
//     std::cout << "Allocate memory (" << size << ")!\n";
//     ptr = malloc(size); // Attempt to allocate memory
//
//     if (!ptr) // if no memory is allocated, then generate an exception
//     {
//         std::bad_alloc ba;
//         std::cout << "Memory allocation error.\n";
//         throw ba;
//     } else {
//         return ptr;
//     }
// }
//
//// It frees the memory allocated to the overloaded
//// operator function operator new()
// void operator delete(void* ptr)
//{
//     std::cout << "Free memory\n";
//     free(ptr);
// }

#define MEM_POOL_SIZE 20
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

    // Audio Device...
    devmgr.addAudioDevice( "StereoOut", new AudIO::StereoOut(), true );
    auto stereoOut = devmgr.getAudioDevice( "StereoOut" );

    // Design - 1
    pDesign1.reset( new AudIO::Design( 0, 2 ) );
    auto sine1  = pDesign1->createObject( "SineWave" );
    auto bus1   = pDesign1->createBus();
    auto chain1 = pDesign1->createChain();
    auto synth1 = pDesign1->createInsert( chain1, std::make_unique<AudIO::SFSynth>() );

    pDesign1->sendToBus( bus1, chain1, 0.5f );
    pDesign1->sendToBus( bus1, sine1, 0.5f );
    pDesign1->setDesignOutput( bus1 );

    if( auto pSine1 = pDesign1->getInsert( bus1, sine1 ) )
        pSine1->setParameter( AudIO::SineWave::kParamFreq, 400 );

    //  To trigger the cycle detection add fake connection to itself
    pDesign1->patch( chain1, 0, chain1, 0 );
    pDesign1->addMeter( chain1 );
    pDesign1->addMeter( sine1 );
    pDesign1->addMeter( bus1 );

    // Design - 2
    pDesign2.reset( new AudIO::Design( 0, 2 ) );
    auto synth2 = pDesign2->createObject( std::make_unique<AudIO::SFSynth>() );
    auto bus2   = pDesign2->createBus();
    auto chain2 = pDesign2->createChain();
    auto sine2  = pDesign2->createInsert( chain2, "SineWave" );
    pDesign2->addInsert( chain2, sine2, 0 );

    pDesign2->sendToBus( bus2, chain2, 0.5f );
    pDesign2->sendToBus( bus2, synth2, 0.5f );
    pDesign2->setDesignOutput( bus2 );

    pDesign2->addMeter( chain2 );
    pDesign2->addMeter( sine2 );
    pDesign2->addMeter( bus2 );
    auto pSine2 = pDesign2->getInsert( chain2, sine2 );
    if( pSine2 )
        pSine2->setParameter( AudIO::SineWave::kParamFreq, 2000 );

    // Design...
    pDesign = pDesign1.get();
    pDesign->setBypass( false );

    // Start callback to process audio design.
    devmgr.setAudioCallback( pDesign );

    bool b{ false };
    auto start = std::chrono::system_clock::now();
    auto end   = std::chrono::system_clock::now();
    AudIO::Console::get().reInit();
    std::cout << "USED MEM: " << AudIO::MemoryPool<float>::get().getUsed() << std::endl;

    // Start Device Manager...
    devmgr.start( SAMPLE_RATE, BLOCK_SIZE );
    auto pSynth1     = dynamic_cast<AudIO::SFSynth*>( pDesign1->getInsert( chain1, synth1 ) );
    auto pSynth2     = dynamic_cast<AudIO::SFSynth*>( pDesign2->getObject( synth2 ) );
    bool leftOrRight = true;
    while( true )
    {
        pSynth1->programChange( 1, 1 );
        pSynth1->programChange( 3, 2 );
        pSynth1->programChange( 4, 3 );
        pSynth1->programChange( 5, 4 );
        pSynth1->channelNoteOn( 48, 80, 1 );
        pSynth2->channelNoteOn( 48, 80, 10 );
        std::this_thread::sleep_for( std::chrono::milliseconds( 50 ) );

        pSynth1->channelNoteOn( 48, 80, 2 );
        pSynth2->channelNoteOn( 80, 80, 10 );
        std::this_thread::sleep_for( std::chrono::milliseconds( 50 ) );

        pSynth1->channelNoteOn( 48, 80, 3 );
        pSynth2->channelNoteOn( 78, 80, 10 );
        std::this_thread::sleep_for( std::chrono::milliseconds( 50 ) );

        pSynth1->channelNoteOn( 48, 80, 4 );
        pSynth2->channelNoteOn( 54, 80, 10 );
        std::this_thread::sleep_for( std::chrono::milliseconds( 50 ) );

        pSynth1->noteOffAll();
        pSynth2->noteOffAll();
        std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
        end                                = std::chrono::system_clock::now();
        std::chrono::duration<double> diff = end - start;
        if( diff.count() > 10.0 )
        {
            start = std::chrono::system_clock::now();
            b     = !b;

            // chain.setParameter(AudIO::kParamMute, (b ? 1.0f : 0.0f));
            devmgr.stop();
            pDesign->setBypass( true );
            pDesign = b ? pDesign2.get() : pDesign1.get();
            devmgr.setAudioCallback( pDesign );
            pDesign->setBypass( false );
            devmgr.start( SAMPLE_RATE, BLOCK_SIZE );
        }

        int iDiff = (int)diff.count();
        if( iDiff & 4 )
        {
            leftOrRight = !leftOrRight;

            pDesign1->setBusInputGain( bus1, sine1, leftOrRight ? 0.25f : 0.65f );
            pDesign2->setBusInputGain( bus2, sine2, leftOrRight ? 0.25f : 0.65f );

            if( auto p = pDesign1->getObject( bus1 ) )
                p->setParameter( ( leftOrRight ? AudIO::BusOrChain::kParamDownMixToLeft : AudIO::BusOrChain::kParamDownMixToRight ), 1.f );
            if( auto p = pDesign2->getObject( bus2 ) )
                p->setParameter( ( leftOrRight ? AudIO::BusOrChain::kParamDownMixToLeft : AudIO::BusOrChain::kParamDownMixToRight ), 1.f );
        }

        if( b )
        {
            std::cout << pDesign->dumpMeter( chain2 );
            std::cout << pDesign->dumpMeter( sine2 );
            std::cout << pDesign->dumpMeter( bus2 );
        }
        else
        {
            std::cout << pDesign->dumpMeter( chain1 );
            std::cout << pDesign->dumpMeter( sine1 );
            std::cout << pDesign->dumpMeter( bus1 );
        }
    }
}
