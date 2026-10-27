
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

std::unique_ptr<AudIO::Design> pDesign{ nullptr };

#define MEM_POOL_SIZE 200
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
    auto stereoOut = devmgr.getAudioDevice( "StereoOut" );
    devmgr.addAudioDevice( "StereoOut", new AudIO::StereoOut(), true );

    // Design - 1
    pDesign.reset( new AudIO::Design( 0, 2 ) );

    // Main out
    auto busMainOut = pDesign->createBus();
    auto busLR      = pDesign->createBus();
    auto busL       = pDesign->createBus();
    auto busR       = pDesign->createBus();

    pDesign->setName( busMainOut, "Master OUT" );
    pDesign->setName( busLR, "Bus L+R" );
    pDesign->setName( busL, "Bus LL" );
    pDesign->setName( busR, "Bus RR" );

    // make L and R bus as downmixed mono outs.
    pDesign->setParameter( busL, AudIO::BusOrChain::kParamDownMixToLeft, 1.f );
    pDesign->setParameter( busR, AudIO::BusOrChain::kParamDownMixToRight, 1.f );

    pDesign->setDesignOutput( busMainOut );

    // Make design live...
    devmgr.setAudioCallback( pDesign.get() );
    devmgr.start( SAMPLE_RATE, BLOCK_SIZE );

    // LR Bus ..
    std::vector<uint32_t> busIDs;
    busIDs.push_back( busLR );
    busIDs.push_back( busL );
    busIDs.push_back( busR );

    // Add L, R, LR bus to Main out
    pDesign->sendToBus( busMainOut, busLR, 1.f );
    pDesign->sendToBus( busMainOut, busL, 1.f );
    pDesign->sendToBus( busMainOut, busR, 1.f );

    // Synth objects...
    auto synth1  = std::make_unique<AudIO::SFSynth>();
    auto synth2  = std::make_unique<AudIO::SFSynth>();
    auto pSynth1 = synth1.get();
    auto pSynth2 = synth2.get();
    pSynth1->setName( "Synth-111" );
    pSynth2->setName( "Synth-222" );
    pSynth1->programChange( 2, 1 );
    pSynth2->programChange( 4, 1 );

    // Add to design...
    auto synth1Id = pDesign->createObject( std::move( synth1 ) );
    auto synth2Id = pDesign->createObject( std::move( synth2 ) );

    // Add meters...
    pDesign->addMeter( busMainOut );
    pDesign->addMeter( busLR );
    pDesign->addMeter( busL );
    pDesign->addMeter( busR );
    pDesign->addMeter( synth1Id );
    pDesign->addMeter( synth2Id );

    pDesign->setBypass( false );

    AudIO::Console::get().reInit();
    std::cout << "USED MEM: " << AudIO::MemoryPool<float>::get().getUsed() << std::endl;

    // Create default routing...
    uint32_t sOut1 = busLR, sOut2 = busLR;
    pDesign->sendToBus( sOut1, synth1Id, 1.f );
    pDesign->sendToBus( sOut2, synth2Id, 1.f );

    bool    leftOrRight = true;
    auto    start       = std::chrono::high_resolution_clock::now();
    auto    end         = std::chrono::high_resolution_clock::now();
    uint8_t pitch[2]    = { 45, 64 };
    while( true )
    {
        std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
        end           = std::chrono::high_resolution_clock::now();
        auto _diffCnt = ( end - start ).count();

        if( 2.0 >= std::fmod( _diffCnt, 20 ) )
        {
            // Read all meters every 20 mSec...
            pDesign->readAllMeters( [&]( const std::string& name, AudIO::MeterData& data ) {
                int ch = 0;
                std::cout << "\n Meter : " << name;
                for( const auto& it : data )
                    std::cout << "\t [" << ch++ << "] RMS:" << it.rms << "\t PEAK: " << it.peak;
            } );
        }

        if( 2.0 >= std::fmod( _diffCnt, 100 ) )
        {
            pSynth1->channelNoteOff( pitch[0], 1 );
            pSynth2->channelNoteOff( pitch[1], 1 );
            pitch[0] = AudIO::utils::random<int>( 1, 127 );
            pitch[1] = AudIO::utils::random<int>( 1, 127 );
            pSynth1->channelNoteOn( pitch[0], AudIO::utils::random<int>( 90, 127 ), 1 );
            pSynth2->channelNoteOn( pitch[1], AudIO::utils::random<int>( 90, 127 ), 1 );
            std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
        }

        if( 2.0 >= std::fmod( _diffCnt, 5000 ) ) // Every 5 seconds...
        {
            start = std::chrono::high_resolution_clock::now();
            pDesign->removeFromBus( sOut1, synth1Id );
            pDesign->removeFromBus( sOut2, synth2Id );
            sOut1 = busIDs[AudIO::utils::random<int>( 0, 2 )];
            sOut2 = busIDs[AudIO::utils::random<int>( 0, 2 )];
            pDesign->sendToBus( sOut1, synth1Id, 1.f );
            pDesign->sendToBus( sOut2, synth2Id, 1.f );
        }
    }
}
