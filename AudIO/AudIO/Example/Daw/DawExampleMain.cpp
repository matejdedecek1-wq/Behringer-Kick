
#include "DawEngine.h"
#include "DawSequence.h"
#include "SineWave.h"
#include "StereoInOut.h"

#include <iostream>

int main()
{
    AudIO::DawEngine _eng( 2, 2 );
    _eng.addAudioDevice( "StereoInOut", new AudIO::StereoInOut(), true );

    auto s1 = _eng.createSequence( "Seq-1" );
    auto s2 = _eng.createSequence( "Seq-2" );
    auto s3 = _eng.createSequence( "Seq-3" );

    auto t11 = s1->createTrack( "Trk-1" );
    auto t12 = s1->createTrack( "Trk-2" );
    auto t13 = s1->createTrack( "Trk-3" );

    auto t21 = s2->createTrack( "Trk-1" );
    auto t22 = s2->createTrack( "Trk-2" );

    auto t31 = s3->createTrack( "Trk-1" );
    auto t32 = s3->createTrack( "Trk-2" );
    auto t33 = s3->createTrack( "Trk-3" );
    auto t34 = s3->createTrack( "Trk-4" );

    auto sine = std::make_unique<AudIO::SineWave>();
    //t11->addSource( std::move( sine ) );

    _eng.startAudioDevice( 48000, 480 );
    _eng.playSequence( "Seq-1" );
    _eng.setBypass( false );

    /*_eng.play( "Seq-2" );
    _eng.play( "Seq-3" );*/

    //_enq.play( { {"Seq-1", 5000}, {"Seq-3", 1000}, {"Seq-2", 4000} );
    //s3->setSolo( "Trk-2", true );

    while( true )
    {
        std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );

        std::cout << "\n Available Memory: " << _eng.getFeeMemory();

        _eng.dumpAllMeters();

        /* _eng.readAllMeters( [&]( const std::string& Name, AudIO::MeterData* Meter ) {
            std::cout << "\n Name: " << Name
                      << " Rms[L,R]: " << Meter->rms[0] << "," << Meter->rms[1]
                      << " Peak[L,R]: " << Meter->peak[0] << "," << Meter->peak[1];
        } );
        */
    }
}
