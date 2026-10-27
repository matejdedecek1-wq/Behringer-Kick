
#include "audioeffectx.h"
#include "../Includes/ObjectFactory.h"

#include <set>

#define __audioeffect__
#if 1

namespace Acceleration
{
    #include "src/Acceleration/Acceleration.h"
    REG_AIRWIN( Acceleration, Brightness )
    #include "src/Acceleration/Acceleration.cpp"
    #include "src/Acceleration/AccelerationProc.cpp"
}

namespace Acceleration2
{
    #include "src/Acceleration2/Acceleration2.h"
    REG_AIRWIN( Acceleration2, Brightness )
    #include "src/Acceleration2/Acceleration2.cpp"
    #include "src/Acceleration2/Acceleration2Proc.cpp"
}

namespace ADClip7
{
    #include "src/ADClip7/ADClip7.h"
    REG_AIRWIN( ADClip7, Clipping )
    #include "src/ADClip7/ADClip7.cpp"
    #include "src/ADClip7/ADClip7Proc.cpp"
}

namespace ADT
{
    #include "src/ADT/ADT.h"
    REG_AIRWIN( ADT, Ambience )
    #include "src/ADT/ADT.cpp"
    #include "src/ADT/ADTProc.cpp"
}
namespace Air
{
    #include "src/Air/Air.h"
    REG_AIRWIN( Air, Brightness )
    #include "src/Air/Air.cpp"
    #include "src/Air/AirProc.cpp"
}

namespace Air2
{
    #include "src/Air2/Air2.h"
    REG_AIRWIN( Air2, Brightness )
    #include "src/Air2/Air2.cpp"
    #include "src/Air2/Air2Proc.cpp"
}

namespace Apicolypse
{
    #include "src/Apicolypse/Apicolypse.h"
    REG_AIRWIN( Apicolypse, Tone_Color )
    #include "src/Apicolypse/Apicolypse.cpp"
    #include "src/Apicolypse/ApicolypseProc.cpp"
}

namespace AQuickVoiceClip
{
    #include "src/AQuickVoiceClip/AQuickVoiceClip.h"
    REG_AIRWIN( AQuickVoiceClip, Clipping )
    #include "src/AQuickVoiceClip/AQuickVoiceClip.cpp"
    #include "src/AQuickVoiceClip/AQuickVoiceClipProc.cpp"
}

namespace AtmosphereBuss
{
    #include "src/AtmosphereBuss/AtmosphereBuss.h"
    REG_AIRWIN( AtmosphereBuss, Consoles )
    #include "src/AtmosphereBuss/AtmosphereBuss.cpp"
    #include "src/AtmosphereBuss/AtmosphereBussProc.cpp"
}

namespace AtmosphereChannel
{
    #include "src/AtmosphereChannel/AtmosphereChannel.h"
    REG_AIRWIN( AtmosphereChannel, Consoles )
    #include "src/AtmosphereChannel/AtmosphereChannel.cpp"
    #include "src/AtmosphereChannel/AtmosphereChannelProc.cpp"
}

namespace Aura
{
    #include "src/Aura/Aura.h"
    REG_AIRWIN( Aura, Effects )
    #include "src/Aura/Aura.cpp"
    #include "src/Aura/AuraProc.cpp"
}

namespace AutoPan
{
    #include "src/AutoPan/AutoPan.h"
    REG_AIRWIN( AutoPan, Stereo )
    #include "src/AutoPan/AutoPan.cpp"
    #include "src/AutoPan/AutoPanProc.cpp"
}

namespace Average
{
    #include "src/Average/Average.h"
    REG_AIRWIN( Average, Filter )
    #include "src/Average/Average.cpp"
    #include "src/Average/AverageProc.cpp"
}

namespace AverMatrix
{
    #include "src/AverMatrix/AverMatrix.h"
    REG_AIRWIN( AverMatrix, Filter )
    #include "src/AverMatrix/AverMatrix.cpp"
    #include "src/AverMatrix/AverMatrixProc.cpp"
}

namespace Balanced
{
    #include "src/Balanced/Balanced.h"
    REG_AIRWIN( Balanced, Utility )
    #include "src/Balanced/Balanced.cpp"
    #include "src/Balanced/BalancedProc.cpp"
}

namespace BassAmp
{
    #include "src/BassAmp/BassAmp.h"
    REG_AIRWIN( BassAmp, Amp_Sims )
    #include "src/BassAmp/BassAmp.cpp"
    #include "src/BassAmp/BassAmpProc.cpp"
}

namespace BassDrive
{
    #include "src/BassDrive/BassDrive.h"
    REG_AIRWIN( BassDrive, Saturation )
    #include "src/BassDrive/BassDrive.cpp"
    #include "src/BassDrive/BassDriveProc.cpp"
}

namespace BassKit
{
    #include "src/BassKit/BassKit.h"
    REG_AIRWIN( BassKit, Bass )
    #include "src/BassKit/BassKit.cpp"
    #include "src/BassKit/BassKitProc.cpp"
}

namespace Baxandall
{
    #include "src/Baxandall/Baxandall.h"
    REG_AIRWIN( Baxandall, Filter )
    #include "src/Baxandall/Baxandall.cpp"
    #include "src/Baxandall/BaxandallProc.cpp"
}

namespace Baxandall2
{
    #include "src/Baxandall2/Baxandall2.h"
    REG_AIRWIN( Baxandall2, Filter )
    #include "src/Baxandall2/Baxandall2.cpp"
    #include "src/Baxandall2/Baxandall2Proc.cpp"
}

namespace Beam
{
    #include "src/Beam/Beam.h"
    REG_AIRWIN( Beam, Dithers )
    #include "src/Beam/Beam.cpp"
    #include "src/Beam/BeamProc.cpp"
}

namespace BigAmp
{
    #include "src/BigAmp/BigAmp.h"
    REG_AIRWIN( BigAmp, Amp_Sims )
    #include "src/BigAmp/BigAmp.cpp"
    #include "src/BigAmp/BigAmpProc.cpp"
}

namespace Biquad
{
    #include "src/Biquad/Biquad.h"
    REG_AIRWIN( Biquad, Biquads )
    #include "src/Biquad/Biquad.cpp"
    #include "src/Biquad/BiquadProc.cpp"
}

namespace Biquad2
{
    #include "src/Biquad2/Biquad2.h"
    REG_AIRWIN( Biquad2, Biquads )
    #include "src/Biquad2/Biquad2.cpp"
    #include "src/Biquad2/Biquad2Proc.cpp"
}

namespace BiquadDouble
{
    #include "src/BiquadDouble/BiquadDouble.h"
    REG_AIRWIN( BiquadDouble, Biquads )
    #include "src/BiquadDouble/BiquadDouble.cpp"
    #include "src/BiquadDouble/BiquadDoubleProc.cpp"
}

namespace BiquadOneHalf
{
    #include "src/BiquadOneHalf/BiquadOneHalf.h"
    REG_AIRWIN( BiquadOneHalf, Biquads )
    #include "src/BiquadOneHalf/BiquadOneHalf.cpp"
    #include "src/BiquadOneHalf/BiquadOneHalfProc.cpp"
}

namespace BiquadPlus
{
    #include "src/BiquadPlus/BiquadPlus.h"
    REG_AIRWIN( BiquadPlus, Biquads )
    #include "src/BiquadPlus/BiquadPlus.cpp"
    #include "src/BiquadPlus/BiquadPlusProc.cpp"
}

namespace BiquadTriple
{
    #include "src/BiquadTriple/BiquadTriple.h"
    REG_AIRWIN( BiquadTriple, Biquads )
    #include "src/BiquadTriple/BiquadTriple.cpp"
    #include "src/BiquadTriple/BiquadTripleProc.cpp"
}

namespace Bite
{
    #include "src/Bite/Bite.h"
    REG_AIRWIN( Bite, Lo_Fi )
    #include "src/Bite/Bite.cpp"
    #include "src/Bite/BiteProc.cpp"
}

namespace BitGlitter
{
    #include "src/BitGlitter/BitGlitter.h"
    REG_AIRWIN( BitGlitter, Lo_Fi )
    #include "src/BitGlitter/BitGlitter.cpp"
    #include "src/BitGlitter/BitGlitterProc.cpp"
}

namespace BitShiftGain
{
    #include "src/BitShiftGain/BitShiftGain.h"
    REG_AIRWIN( BitShiftGain, Utility )
    #include "src/BitShiftGain/BitShiftGain.cpp"
    #include "src/BitShiftGain/BitShiftGainProc.cpp"
}

namespace BlockParty
{
    #include "src/BlockParty/BlockParty.h"
    REG_AIRWIN( BlockParty, Dynamics )
    #include "src/BlockParty/BlockParty.cpp"
    #include "src/BlockParty/BlockPartyProc.cpp"
}

namespace BrassRider
{
    #include "src/BrassRider/BrassRider.h"
    REG_AIRWIN( BrassRider, Dynamics )
    #include "src/BrassRider/BrassRider.cpp"
    #include "src/BrassRider/BrassRiderProc.cpp"
}

namespace BrightAmbience
{
    #include "src/BrightAmbience/BrightAmbience.h"
    REG_AIRWIN( BrightAmbience, Ambience )
    #include "src/BrightAmbience/BrightAmbience.cpp"
    #include "src/BrightAmbience/BrightAmbienceProc.cpp"
}

namespace BrightAmbience2
{
    #include "src/BrightAmbience2/BrightAmbience2.h"
    REG_AIRWIN( BrightAmbience2, Ambience )
    #include "src/BrightAmbience2/BrightAmbience2.cpp"
    #include "src/BrightAmbience2/BrightAmbience2Proc.cpp"
}

namespace BrightAmbience3
{
    #include "src/BrightAmbience3/BrightAmbience3.h"
    REG_AIRWIN( BrightAmbience3, Ambience )
    #include "src/BrightAmbience3/BrightAmbience3.cpp"
    #include "src/BrightAmbience3/BrightAmbience3Proc.cpp"
}

namespace BuildATPDF
{
    #include "src/BuildATPDF/BuildATPDF.h"
    REG_AIRWIN( BuildATPDF, Dithers )
    #include "src/BuildATPDF/BuildATPDF.cpp"
    #include "src/BuildATPDF/BuildATPDFProc.cpp"
}

namespace BussColors4
{
    #include "src/BussColors4/BussColors4.h"
    REG_AIRWIN( BussColors4, Tone_Color )
    #include "src/BussColors4/BussColors4.cpp"
    #include "src/BussColors4/BussColors4Proc.cpp"
}

namespace ButterComp
{
    #include "src/ButterComp/ButterComp.h"
    REG_AIRWIN( ButterComp, Dynamics )
    #include "src/ButterComp/ButterComp.cpp"
    #include "src/ButterComp/ButterCompProc.cpp"
}

namespace ButterComp2
{
    #include "src/ButterComp2/ButterComp2.h"
    REG_AIRWIN( ButterComp2, Dynamics )
    #include "src/ButterComp2/ButterComp2.cpp"
    #include "src/ButterComp2/ButterComp2Proc.cpp"
}

namespace C5RawBuss
{
    #include "src/C5RawBuss/C5RawBuss.h"
    REG_AIRWIN( C5RawBuss, Consoles )
    #include "src/C5RawBuss/C5RawBuss.cpp"
    #include "src/C5RawBuss/C5RawBussProc.cpp"
}

namespace C5RawChannel
{
    #include "src/C5RawChannel/C5RawChannel.h"
    REG_AIRWIN( C5RawChannel, Consoles )
    #include "src/C5RawChannel/C5RawChannel.cpp"
    #include "src/C5RawChannel/C5RawChannelProc.cpp"
}

namespace Cabs
{
    #include "src/Cabs/Cabs.h"
    REG_AIRWIN( Cabs, Amp_Sims )
    #include "src/Cabs/Cabs.cpp"
    #include "src/Cabs/CabsProc.cpp"
}

namespace Calibre
{
    #include "src/Calibre/Calibre.h"
    REG_AIRWIN( Calibre, Tone_Color )
    #include "src/Calibre/Calibre.cpp"
    #include "src/Calibre/CalibreProc.cpp"
}

namespace Capacitor
{
    #include "src/Capacitor/Capacitor.h"
    REG_AIRWIN( Capacitor, Filter )
    #include "src/Capacitor/Capacitor.cpp"
    #include "src/Capacitor/CapacitorProc.cpp"
}

namespace Capacitor2
{
    #include "src/Capacitor2/Capacitor2.h"
    REG_AIRWIN( Capacitor2, Filter )
    #include "src/Capacitor2/Capacitor2.cpp"
    #include "src/Capacitor2/Capacitor2Proc.cpp"
}

namespace Chamber
{
    #include "src/Chamber/Chamber.h"
    REG_AIRWIN( Chamber, Reverb )
    #include "src/Chamber/Chamber.cpp"
    #include "src/Chamber/ChamberProc.cpp"
}

namespace Chamber2
{
    #include "src/Chamber2/Chamber2.h"
    REG_AIRWIN( Chamber2, Reverb )
    #include "src/Chamber2/Chamber2.cpp"
    #include "src/Chamber2/Chamber2Proc.cpp"
}

namespace Channel4
{
    #include "src/Channel4/Channel4.h"
    REG_AIRWIN( Channel4, Tone_Color )
    #include "src/Channel4/Channel4.cpp"
    #include "src/Channel4/Channel4Proc.cpp"
}

namespace Channel5
{
    #include "src/Channel5/Channel5.h"
    REG_AIRWIN( Channel5, Tone_Color )
    #include "src/Channel5/Channel5.cpp"
    #include "src/Channel5/Channel5Proc.cpp"
}

namespace Channel6
{
    #include "src/Channel6/Channel6.h"
    REG_AIRWIN( Channel6, Tone_Color )
    #include "src/Channel6/Channel6.cpp"
    #include "src/Channel6/Channel6Proc.cpp"
}

namespace Channel7
{
    #include "src/Channel7/Channel7.h"
    REG_AIRWIN( Channel7, Tone_Color )
    #include "src/Channel7/Channel7.cpp"
    #include "src/Channel7/Channel7Proc.cpp"
}

namespace Channel8
{
    #include "src/Channel8/Channel8.h"
    REG_AIRWIN( Channel8, Tone_Color )
    #include "src/Channel8/Channel8.cpp"
    #include "src/Channel8/Channel8Proc.cpp"
}

namespace Channel9
{
    #include "src/Channel9/Channel9.h"
    REG_AIRWIN( Channel9, Tone_Color )
    #include "src/Channel9/Channel9.cpp"
    #include "src/Channel9/Channel9Proc.cpp"
}

namespace Chorus
{
    #include "src/Chorus/Chorus.h"
    REG_AIRWIN( Chorus, Ambience )
    #include "src/Chorus/Chorus.cpp"
    #include "src/Chorus/ChorusProc.cpp"
}

namespace ChorusEnsemble
{
    #include "src/ChorusEnsemble/ChorusEnsemble.h"
    REG_AIRWIN( ChorusEnsemble, Ambience )
    #include "src/ChorusEnsemble/ChorusEnsemble.cpp"
    #include "src/ChorusEnsemble/ChorusEnsembleProc.cpp"
}

namespace ChromeOxide
{
    #include "src/ChromeOxide/ChromeOxide.h"
    REG_AIRWIN( ChromeOxide, Lo_Fi )
    #include "src/ChromeOxide/ChromeOxide.cpp"
    #include "src/ChromeOxide/ChromeOxideProc.cpp"
}

namespace Cider
{
    #include "src/Cider/Cider.h"
    REG_AIRWIN( Cider, Tone_Color )
    #include "src/Cider/Cider.cpp"
    #include "src/Cider/CiderProc.cpp"
}

namespace ClipOnly
{
    #include "src/ClipOnly/ClipOnly.h"
    REG_AIRWIN( ClipOnly, Clipping )
    #include "src/ClipOnly/ClipOnly.cpp"
    #include "src/ClipOnly/ClipOnlyProc.cpp"
}

namespace ClipOnly2
{
    #include "src/ClipOnly2/ClipOnly2.h"
    REG_AIRWIN( ClipOnly2, Clipping )
    #include "src/ClipOnly2/ClipOnly2.cpp"
    #include "src/ClipOnly2/ClipOnly2Proc.cpp"
}

namespace ClipSoftly
{
    #include "src/ClipSoftly/ClipSoftly.h"
    REG_AIRWIN( ClipSoftly, Clipping )
    #include "src/ClipSoftly/ClipSoftly.cpp"
    #include "src/ClipSoftly/ClipSoftlyProc.cpp"
}

namespace Coils
{
    #include "src/Coils/Coils.h"
    REG_AIRWIN( Coils, Subtlety )
    #include "src/Coils/Coils.cpp"
    #include "src/Coils/CoilsProc.cpp"
}

namespace Coils2
{
    #include "src/Coils2/Coils2.h"
    REG_AIRWIN( Coils2, Subtlety )
    #include "src/Coils2/Coils2.cpp"
    #include "src/Coils2/Coils2Proc.cpp"
}

namespace Cojones
{
    #include "src/Cojones/Cojones.h"
    REG_AIRWIN( Cojones, Lo_Fi )
    #include "src/Cojones/Cojones.cpp"
    #include "src/Cojones/CojonesProc.cpp"
}

namespace Compresaturator
{
    #include "src/Compresaturator/Compresaturator.h"
    REG_AIRWIN( Compresaturator, Dynamics )
    #include "src/Compresaturator/Compresaturator.cpp"
    #include "src/Compresaturator/CompresaturatorProc.cpp"
}

namespace Console4Buss
{
    #include "src/Console4Buss/Console4Buss.h"
    REG_AIRWIN( Console4Buss, Consoles )
    #include "src/Console4Buss/Console4Buss.cpp"
    #include "src/Console4Buss/Console4BussProc.cpp"
}

namespace Console4Channel
{
    #include "src/Console4Channel/Console4Channel.h"
    REG_AIRWIN( Console4Channel, Consoles )
    #include "src/Console4Channel/Console4Channel.cpp"
    #include "src/Console4Channel/Console4ChannelProc.cpp"
}

namespace Console5Buss
{
    #include "src/Console5Buss/Console5Buss.h"
    REG_AIRWIN( Console5Buss, Consoles )
    #include "src/Console5Buss/Console5Buss.cpp"
    #include "src/Console5Buss/Console5BussProc.cpp"
}

namespace Console5Channel
{
    #include "src/Console5Channel/Console5Channel.h"
    REG_AIRWIN( Console5Channel, Consoles )
    #include "src/Console5Channel/Console5Channel.cpp"
    #include "src/Console5Channel/Console5ChannelProc.cpp"
}

namespace Console5DarkCh
{
    #include "src/Console5DarkCh/Console5DarkCh.h"
    REG_AIRWIN( Console5DarkCh, Consoles )
    #include "src/Console5DarkCh/Console5DarkCh.cpp"
    #include "src/Console5DarkCh/Console5DarkChProc.cpp"
}

namespace Console6Buss
{
    #include "src/Console6Buss/Console6Buss.h"
    REG_AIRWIN( Console6Buss, Consoles )
    #include "src/Console6Buss/Console6Buss.cpp"
    #include "src/Console6Buss/Console6BussProc.cpp"
}

namespace Console6Channel
{
    #include "src/Console6Channel/Console6Channel.h"
    REG_AIRWIN( Console6Channel, Consoles )
    #include "src/Console6Channel/Console6Channel.cpp"
    #include "src/Console6Channel/Console6ChannelProc.cpp"
}

namespace Console7Buss
{
    #include "src/Console7Buss/Console7Buss.h"
    REG_AIRWIN( Console7Buss, Consoles )
    #include "src/Console7Buss/Console7Buss.cpp"
    #include "src/Console7Buss/Console7BussProc.cpp"
}

namespace Console7Cascade
{
    #include "src/Console7Cascade/Console7Cascade.h"
    REG_AIRWIN( Console7Cascade, Consoles )
    #include "src/Console7Cascade/Console7Cascade.cpp"
    #include "src/Console7Cascade/Console7CascadeProc.cpp"
}

namespace Console7Channel
{
    #include "src/Console7Channel/Console7Channel.h"
    REG_AIRWIN( Console7Channel, Consoles )
    #include "src/Console7Channel/Console7Channel.cpp"
    #include "src/Console7Channel/Console7ChannelProc.cpp"
}

namespace Console7Crunch
{
    #include "src/Console7Crunch/Console7Crunch.h"
    REG_AIRWIN( Console7Crunch, Consoles )
    #include "src/Console7Crunch/Console7Crunch.cpp"
    #include "src/Console7Crunch/Console7CrunchProc.cpp"
}

namespace Console8BussOut
{
    #include "src/Console8BusOut/Console8BussOut.h"
    REG_AIRWIN( Console8BussOut, Consoles )
    #include "src/Console8BusOut/Console8BussOut.cpp"
    #include "src/Console8BusOut/Console8BussOutProc.cpp"
}

namespace Console8BussHype
{
    #include "src/Console8BussHype/Console8BussHype.h"
    REG_AIRWIN( Console8BussHype, Consoles )
    #include "src/Console8BussHype/Console8BussHype.cpp"
    #include "src/Console8BussHype/Console8BussHypeProc.cpp"
}

namespace Console8BussIn
{
    #include "src/Console8BussIn/Console8BussIn.h"
    REG_AIRWIN( Console8BussIn, Consoles )
    #include "src/Console8BussIn/Console8BussIn.cpp"
    #include "src/Console8BussIn/Console8BussInProc.cpp"
}

namespace Console8ChannelHype
{
    #include "src/Console8ChannelHype/Console8ChannelHype.h"
    REG_AIRWIN( Console8ChannelHype, Consoles )
    #include "src/Console8ChannelHype/Console8ChannelHype.cpp"
    #include "src/Console8ChannelHype/Console8ChannelHypeProc.cpp"
}

namespace Console8ChannelIn
{
    #include "src/Console8ChannelIn/Console8ChannelIn.h"
    REG_AIRWIN( Console8ChannelIn, Consoles )
    #include "src/Console8ChannelIn/Console8ChannelIn.cpp"
    #include "src/Console8ChannelIn/Console8ChannelInProc.cpp"
}

namespace Console8ChannelOut
{
    #include "src/Console8ChannelOut/Console8ChannelOut.h"
    REG_AIRWIN( Console8ChannelOut, Consoles )
    #include "src/Console8ChannelOut/Console8ChannelOut.cpp"
    #include "src/Console8ChannelOut/Console8ChannelOutProc.cpp"
}

namespace Console8SubHype
{
    #include "src/Console8SubHype/Console8SubHype.h"
    REG_AIRWIN( Console8SubHype, Consoles )
    #include "src/Console8SubHype/Console8SubHype.cpp"
    #include "src/Console8SubHype/Console8SubHypeProc.cpp"
}

namespace Console8SubIn
{
    #include "src/Console8SubIn/Console8SubIn.h"
    REG_AIRWIN( Console8SubIn, Consoles )
    #include "src/Console8SubIn/Console8SubIn.cpp"
    #include "src/Console8SubIn/Console8SubInProc.cpp"
}

namespace Console8SubOut
{
    #include "src/Console8SubOut/Console8SubOut.h"
    REG_AIRWIN( Console8SubOut, Consoles )
    #include "src/Console8SubOut/Console8SubOut.cpp"
    #include "src/Console8SubOut/Console8SubOutProc.cpp"
}

namespace CrunchyGrooveWear
{
    #include "src/CrunchyGrooveWear/CrunchyGrooveWear.h"
    REG_AIRWIN( CrunchyGrooveWear, Lo_Fi )
    #include "src/CrunchyGrooveWear/CrunchyGrooveWear.cpp"
    #include "src/CrunchyGrooveWear/CrunchyGrooveWearProc.cpp"
}

namespace Crystal
{
    #include "src/Crystal/Crystal.h"
    REG_AIRWIN( Crystal, Tone_Color )
    #include "src/Crystal/Crystal.cpp"
    #include "src/Crystal/CrystalProc.cpp"
}

namespace CStrip
{
    #include "src/CStrip/CStrip.h"
    REG_AIRWIN( CStrip, Dynamics )
    #include "src/CStrip/CStrip.cpp"
    #include "src/CStrip/CStripProc.cpp"
}

namespace curve
{
    #include "src/curve/curve.h"
    REG_AIRWIN( curve, Dynamics )
    #include "src/curve/curve.cpp"
    #include "src/curve/curveProc.cpp"
}

namespace Dark
{
    #include "src/Dark/Dark.h"
    REG_AIRWIN( Dark, Dithers )
    #include "src/Dark/Dark.cpp"
    #include "src/Dark/DarkProc.cpp"
}

namespace DarkNoise
{
    #include "src/DarkNoise/DarkNoise.h"
    REG_AIRWIN( DarkNoise, Noise )
    #include "src/DarkNoise/DarkNoise.cpp"
    #include "src/DarkNoise/DarkNoiseProc.cpp"
}

namespace DCVoltage
{
    #include "src/DCVoltage/DCVoltage.h"
    REG_AIRWIN( DCVoltage, Utility )
    #include "src/DCVoltage/DCVoltage.cpp"
    #include "src/DCVoltage/DCVoltageProc.cpp"
}

namespace DeBess
{
    #include "src/DeBess/DeBess.h"
    REG_AIRWIN( DeBess, Brightness )
    #include "src/DeBess/DeBess.cpp"
    #include "src/DeBess/DeBessProc.cpp"
}

namespace Deckwrecka
{
    #include "src/Deckwrecka/Deckwrecka.h"
    REG_AIRWIN( Deckwrecka, Lo_Fi )
    #include "src/Deckwrecka/Deckwrecka.cpp"
    #include "src/Deckwrecka/DeckwreckaProc.cpp"
}

namespace DeEss
{
    #include "src/DeEss/DeEss.h"
    REG_AIRWIN( DeEss, Brightness )
    #include "src/DeEss/DeEss.cpp"
    #include "src/DeEss/DeEssProc.cpp"
}

namespace DeHiss
{
    #include "src/DeHiss/DeHiss.h"
    REG_AIRWIN( DeHiss, Brightness )
    #include "src/DeHiss/DeHiss.cpp"
    #include "src/DeHiss/DeHissProc.cpp"
}

namespace Density
{
    #include "src/Density/Density.h"
    REG_AIRWIN( Density, Distortion )
    #include "src/Density/Density.cpp"
    #include "src/Density/DensityProc.cpp"
}

namespace Density2
{
    #include "src/Density2/Density2.h"
    REG_AIRWIN( Density2, Distortion )
    #include "src/Density2/Density2.cpp"
    #include "src/Density2/Density2Proc.cpp"
}

namespace DeRez
{
    #include "src/DeRez/DeRez.h"
    REG_AIRWIN( DeRez, Lo_Fi )
    #include "src/DeRez/DeRez.cpp"
    #include "src/DeRez/DeRezProc.cpp"
}

namespace DeRez2
{
    #include "src/DeRez2/DeRez2.h"
    REG_AIRWIN( DeRez2, Lo_Fi )
    #include "src/DeRez2/DeRez2.cpp"
    #include "src/DeRez2/DeRez2Proc.cpp"
}

namespace Desk
{
    #include "src/Desk/Desk.h"
    REG_AIRWIN( Desk, Subtlety )
    #include "src/Desk/Desk.cpp"
    #include "src/Desk/DeskProc.cpp"
}

namespace Desk4
{
    #include "src/Desk4/Desk4.h"
    REG_AIRWIN( Desk4, Saturation )
    #include "src/Desk4/Desk4.cpp"
    #include "src/Desk4/Desk4Proc.cpp"
}

namespace DigitalBlack
{
    #include "src/DigitalBlack/DigitalBlack.h"
    REG_AIRWIN( DigitalBlack, Dynamics )
    #include "src/DigitalBlack/DigitalBlack.cpp"
    #include "src/DigitalBlack/DigitalBlackProc.cpp"
}

namespace Dirt
{
    #include "src/Dirt/Dirt.h"
    REG_AIRWIN( Dirt, Distortion )
    #include "src/Dirt/Dirt.cpp"
    #include "src/Dirt/DirtProc.cpp"
}

namespace Distance
{
    #include "src/Distance/Distance.h"
    REG_AIRWIN( Distance, Filter )
    #include "src/Distance/Distance.cpp"
    #include "src/Distance/DistanceProc.cpp"
}

namespace Distance2
{
    #include "src/Distance2/Distance2.h"
    REG_AIRWIN( Distance2, Filter )
    #include "src/Distance2/Distance2.cpp"
    #include "src/Distance2/Distance2Proc.cpp"
}

namespace Distortion
{
    #include "src/Distortion/Distortion.h"
    REG_AIRWIN( Distortion, Distortion )
    #include "src/Distortion/Distortion.cpp"
    #include "src/Distortion/DistortionProc.cpp"
}

namespace Ditherbox
{
    #include "src/Ditherbox/Ditherbox.h"
    REG_AIRWIN( Ditherbox, Dithers )
    #include "src/Ditherbox/Ditherbox.cpp"
    #include "src/Ditherbox/DitherboxProc.cpp"
}

namespace DitherFloat
{
    #include "src/DitherFloat/DitherFloat.h"
    REG_AIRWIN( DitherFloat, Dithers )
    #include "src/DitherFloat/DitherFloat.cpp"
    #include "src/DitherFloat/DitherFloatProc.cpp"
}

namespace DitherMeDiskers
{
    #include "src/DitherMeDiskers/DitherMeDiskers.h"
    REG_AIRWIN( DitherMeDiskers, Dithers )
    #include "src/DitherMeDiskers/DitherMeDiskers.cpp"
    #include "src/DitherMeDiskers/DitherMeDiskersProc.cpp"
}

namespace DitherMeTimbers
{
    #include "src/DitherMeTimbers/DitherMeTimbers.h"
    REG_AIRWIN( DitherMeTimbers, Dithers )
    #include "src/DitherMeTimbers/DitherMeTimbers.cpp"
    #include "src/DitherMeTimbers/DitherMeTimbersProc.cpp"
}

namespace Doublelay
{
    #include "src/Doublelay/Doublelay.h"
    REG_AIRWIN( Doublelay, Ambience )
    #include "src/Doublelay/Doublelay.cpp"
    #include "src/Doublelay/DoublelayProc.cpp"
}

namespace DoublePaul
{
    #include "src/DoublePaul/DoublePaul.h"
    REG_AIRWIN( DoublePaul, Dithers )
    #include "src/DoublePaul/DoublePaul.cpp"
    #include "src/DoublePaul/DoublePaulProc.cpp"
}

namespace Drive
{
    #include "src/Drive/Drive.h"
    REG_AIRWIN( Drive, Distortion )
    #include "src/Drive/Drive.cpp"
    #include "src/Drive/DriveProc.cpp"
}

namespace DrumSlam
{
    #include "src/DrumSlam/DrumSlam.h"
    REG_AIRWIN( DrumSlam, Dynamics )
    #include "src/DrumSlam/DrumSlam.cpp"
    #include "src/DrumSlam/DrumSlamProc.cpp"
}

namespace DubCenter
{
    #include "src/DubCenter/DubCenter.h"
    REG_AIRWIN( DubCenter, Bass )
    #include "src/DubCenter/DubCenter.cpp"
    #include "src/DubCenter/DubCenterProc.cpp"
}

namespace Dubly
{
    #include "src/Dubly/Dubly.h"
    REG_AIRWIN( Dubly, Effects )
    #include "src/Dubly/Dubly.cpp"
    #include "src/Dubly/DublyProc.cpp"
}

namespace DubSub
{
    #include "src/DubSub/DubSub.h"
    REG_AIRWIN( DubSub, Bass )
    #include "src/DubSub/DubSub.cpp"
    #include "src/DubSub/DubSubProc.cpp"
}

namespace DustBunny
{
    #include "src/DustBunny/DustBunny.h"
    REG_AIRWIN( DustBunny, Lo_Fi )
    #include "src/DustBunny/DustBunny.cpp"
    #include "src/DustBunny/DustBunnyProc.cpp"
}

namespace Dynamics
{
    #include "src/Dynamics/Dynamics.h"
    REG_AIRWIN( Dynamics, Dynamics )
    #include "src/Dynamics/Dynamics.cpp"
    #include "src/Dynamics/DynamicsProc.cpp"
}

namespace Dyno
{
    #include "src/Dyno/Dyno.h"
    REG_AIRWIN( Dyno, Saturation )
    #include "src/Dyno/Dyno.cpp"
    #include "src/Dyno/DynoProc.cpp"
}

namespace Edge
{
    #include "src/Edge/Edge.h"
    REG_AIRWIN( Edge, Distortion )
    #include "src/Edge/Edge.cpp"
    #include "src/Edge/EdgeProc.cpp"
}

namespace EdIsDim
{
    #include "src/EdIsDim/EdIsDim.h"
    REG_AIRWIN( EdIsDim, Utility )
    #include "src/EdIsDim/EdIsDim.cpp"
    #include "src/EdIsDim/EdIsDimProc.cpp"
}

namespace Elation
{
    #include "src/Elation/Elation.h"
    REG_AIRWIN( Elation, Tone_Color )
    #include "src/Elation/Elation.cpp"
    #include "src/Elation/ElationProc.cpp"
}

namespace ElectroHat
{
    #include "src/ElectroHat/ElectroHat.h"
    REG_AIRWIN( ElectroHat, Noise )
    #include "src/ElectroHat/ElectroHat.cpp"
    #include "src/ElectroHat/ElectroHatProc.cpp"
}

namespace Energy
{
    #include "src/Energy/Energy.h"
    REG_AIRWIN( Energy, Effects )
    #include "src/Energy/Energy.cpp"
    #include "src/Energy/EnergyProc.cpp"
}

namespace Energy2
{
    #include "src/Energy2/Energy2.h"
    REG_AIRWIN( Energy2, Effects )
    #include "src/Energy2/Energy2.cpp"
    #include "src/Energy2/Energy2Proc.cpp"
}

namespace Ensemble
{
    #include "src/Ensemble/Ensemble.h"
    REG_AIRWIN( Ensemble, Ambience )
    #include "src/Ensemble/Ensemble.cpp"
    #include "src/Ensemble/EnsembleProc.cpp"
}

namespace EQ
{
    #include "src/EQ/EQ.h"
    REG_AIRWIN( EQ, Filter )
    #include "src/EQ/EQ.cpp"
    #include "src/EQ/EQProc.cpp"
}

namespace EveryTrim
{
    #include "src/EveryTrim/EveryTrim.h"
    REG_AIRWIN( EveryTrim, Utility )
    #include "src/EveryTrim/EveryTrim.cpp"
    #include "src/EveryTrim/EveryTrimProc.cpp"
}

namespace Exciter
{
    #include "src/Exciter/Exciter.h"
    REG_AIRWIN( Exciter, Effects )
    #include "src/Exciter/Exciter.cpp"
    #include "src/Exciter/ExciterProc.cpp"
}

namespace Facet
{
    #include "src/Facet/Facet.h"
    REG_AIRWIN( Facet, Effects )
    #include "src/Facet/Facet.cpp"
    #include "src/Facet/FacetProc.cpp"
}

namespace FathomFive
{
    #include "src/FathomFive/FathomFive.h"
    REG_AIRWIN( FathomFive, Bass )
    #include "src/FathomFive/FathomFive.cpp"
    #include "src/FathomFive/FathomFiveProc.cpp"
}

namespace FireAmp
{
    #include "src/FireAmp/FireAmp.h"
    REG_AIRWIN( FireAmp, Amp_Sims )
    #include "src/FireAmp/FireAmp.cpp"
    #include "src/FireAmp/FireAmpProc.cpp"
}

namespace Flipity
{
    #include "src/Flipity/Flipity.h"
    REG_AIRWIN( Flipity, Utility )
    #include "src/Flipity/Flipity.cpp"
    #include "src/Flipity/FlipityProc.cpp"
}

namespace Floor
{
    #include "src/Floor/Floor.h"
    REG_AIRWIN( Floor, Bass )
    #include "src/Floor/Floor.cpp"
    #include "src/Floor/FloorProc.cpp"
}

namespace Flutter
{
    #include "src/Flutter/Flutter.h"
    REG_AIRWIN( Flutter, Lo_Fi )
    #include "src/Flutter/Flutter.cpp"
    #include "src/Flutter/FlutterProc.cpp"
}

namespace Focus
{
    #include "src/Focus/Focus.h"
    REG_AIRWIN( Focus, Saturation )
    #include "src/Focus/Focus.cpp"
    #include "src/Focus/FocusProc.cpp"
}

namespace Fracture
{
    #include "src/Fracture/Fracture.h"
    REG_AIRWIN( Fracture, Effects )
    #include "src/Fracture/Fracture.cpp"
    #include "src/Fracture/FractureProc.cpp"
}

namespace FromTape
{
    #include "src/FromTape/FromTape.h"
    REG_AIRWIN( FromTape, Effects )
    #include "src/FromTape/FromTape.cpp"
    #include "src/FromTape/FromTapeProc.cpp"
}

namespace Galactic
{
    #include "src/Galactic/Galactic.h"
    REG_AIRWIN( Galactic, Reverb )
    #include "src/Galactic/Galactic.cpp"
    #include "src/Galactic/GalacticProc.cpp"
}

namespace Gatelope
{
    #include "src/Gatelope/Gatelope.h"
    REG_AIRWIN( Gatelope, Dynamics )
    #include "src/Gatelope/Gatelope.cpp"
    #include "src/Gatelope/GatelopeProc.cpp"
}

namespace GlitchShifter
{
    //#include "src/GlitchShifter/GlitchShifter.h"
    //REG_AIRWIN( GlitchShifter, Effects )
    //#include "src/GlitchShifter/GlitchShifter.cpp"
    //#include "src/GlitchShifter/GlitchShifterProc.cpp"
}

namespace Golem
{
    #include "src/Golem/Golem.h"
    REG_AIRWIN( Golem, Utility )
    #include "src/Golem/Golem.cpp"
    #include "src/Golem/GolemProc.cpp"
}

namespace GrindAmp
{
    #include "src/GrindAmp/GrindAmp.h"
    REG_AIRWIN( GrindAmp, Amp_Sims )
    #include "src/GrindAmp/GrindAmp.cpp"
    #include "src/GrindAmp/GrindAmpProc.cpp"
}

namespace Gringer
{
    #include "src/Gringer/Gringer.h"
    REG_AIRWIN( Gringer, Effects )
    #include "src/Gringer/Gringer.cpp"
    #include "src/Gringer/GringerProc.cpp"
}

namespace GrooveWear
{
    #include "src/GrooveWear/GrooveWear.h"
    REG_AIRWIN( GrooveWear, Lo_Fi )
    #include "src/GrooveWear/GrooveWear.cpp"
    #include "src/GrooveWear/GrooveWearProc.cpp"
}

namespace GuitarConditioner
{
    #include "src/GuitarConditioner/GuitarConditioner.h"
    REG_AIRWIN( GuitarConditioner, Effects )
    #include "src/GuitarConditioner/GuitarConditioner.cpp"
    #include "src/GuitarConditioner/GuitarConditionerProc.cpp"
}

namespace HardVacuum
{
    #include "src/HardVacuum/HardVacuum.h"
    REG_AIRWIN( HardVacuum, Distortion )
    #include "src/HardVacuum/HardVacuum.cpp"
    #include "src/HardVacuum/HardVacuumProc.cpp"
}

namespace Hermepass
{
    #include "src/Hermepass/Hermepass.h"
    REG_AIRWIN( Hermepass, Bass )
    #include "src/Hermepass/Hermepass.cpp"
    #include "src/Hermepass/HermepassProc.cpp"
}

namespace HermeTrim
{
    #include "src/HermeTrim/HermeTrim.h"
    REG_AIRWIN( HermeTrim, Utility )
    #include "src/HermeTrim/HermeTrim.cpp"
    #include "src/HermeTrim/HermeTrimProc.cpp"
}

namespace HighGlossDither
{
    #include "src/HighGlossDither/HighGlossDither.h"
    REG_AIRWIN( HighGlossDither, Dithers )
    #include "src/HighGlossDither/HighGlossDither.cpp"
    #include "src/HighGlossDither/HighGlossDitherProc.cpp"
}

namespace HighImpact
{
    #include "src/HighImpact/HighImpact.h"
    REG_AIRWIN( HighImpact, Distortion )
    #include "src/HighImpact/HighImpact.cpp"
    #include "src/HighImpact/HighImpactProc.cpp"
}

namespace Highpass
{
    #include "src/Highpass/Highpass.h"
    REG_AIRWIN( Highpass, Filter )
    #include "src/Highpass/Highpass.cpp"
    #include "src/Highpass/HighpassProc.cpp"
}

namespace Highpass2
{
    #include "src/Highpass2/Highpass2.h"
    REG_AIRWIN( Highpass2, Filter )
    #include "src/Highpass2/Highpass2.cpp"
    #include "src/Highpass2/Highpass2Proc.cpp"
}

namespace Holt
{
    #include "src/Holt/Holt.h"
    REG_AIRWIN( Holt, Filter )
    #include "src/Holt/Holt.cpp"
    #include "src/Holt/HoltProc.cpp"
}

namespace Holt2
{
    #include "src/Holt2/Holt2.h"
    REG_AIRWIN( Holt2, Filter )
    #include "src/Holt2/Holt2.cpp"
    #include "src/Holt2/Holt2Proc.cpp"
}

namespace Hombre
{
    #include "src/Hombre/Hombre.h"
    REG_AIRWIN( Hombre, Ambience )
    #include "src/Hombre/Hombre.cpp"
    #include "src/Hombre/HombreProc.cpp"
}

namespace Hull
{
    #include "src/Hull/Hull.h"
    REG_AIRWIN( Hull, Filter )
    #include "src/Hull/Hull.cpp"
    #include "src/Hull/HullProc.cpp"
}

namespace Hype
{
    #include "src/Hype/Hype.h"
    REG_AIRWIN( Hype, Subtlety )
    #include "src/Hype/Hype.cpp"
    #include "src/Hype/HypeProc.cpp"
}

namespace Hypersonic
{
    #include "src/Hypersonic/Hypersonic.h"
    REG_AIRWIN( Hypersonic, Brightness )
    #include "src/Hypersonic/Hypersonic.cpp"
    #include "src/Hypersonic/HypersonicProc.cpp"
}

namespace HypersonX
{
    #include "src/HypersonX/HypersonX.h"
    REG_AIRWIN( HypersonX, Brightness )
    #include "src/HypersonX/HypersonX.cpp"
    #include "src/HypersonX/HypersonXProc.cpp"
}

namespace Infinity
{
    #include "src/Infinity/Infinity.h"
    REG_AIRWIN( Infinity, Reverb )
    #include "src/Infinity/Infinity.cpp"
    #include "src/Infinity/InfinityProc.cpp"
}

namespace Infinity2
{
    #include "src/Infinity2/Infinity2.h"
    REG_AIRWIN( Infinity2, Reverb )
    #include "src/Infinity2/Infinity2.cpp"
    #include "src/Infinity2/Infinity2Proc.cpp"
}

namespace Infrasonic
{
    #include "src/Infrasonic/Infrasonic.h"
    REG_AIRWIN( Infrasonic, Bass )
    #include "src/Infrasonic/Infrasonic.cpp"
    #include "src/Infrasonic/InfrasonicProc.cpp"
}

namespace Interstage
{
    #include "src/Interstage/Interstage.h"
    REG_AIRWIN( Interstage, Subtlety )
    #include "src/Interstage/Interstage.cpp"
    #include "src/Interstage/InterstageProc.cpp"
}

namespace IronOxide5
{
    #include "src/IronOxide5/IronOxide5.h"
    REG_AIRWIN( IronOxide5, Tape )
    #include "src/IronOxide5/IronOxide5.cpp"
    #include "src/IronOxide5/IronOxide5Proc.cpp"
}

namespace IronOxideClassic
{
    #include "src/IronOxideClassic/IronOxideClassic.h"
    REG_AIRWIN( IronOxideClassic, Tape )
    #include "src/IronOxideClassic/IronOxideClassic.cpp"
    #include "src/IronOxideClassic/IronOxideClassicProc.cpp"
}

namespace IronOxideClassic2
{
    #include "src/IronOxideClassic2/IronOxideClassic2.h"
    REG_AIRWIN( IronOxideClassic2, Tape )
    #include "src/IronOxideClassic2/IronOxideClassic2.cpp"
    #include "src/IronOxideClassic2/IronOxideClassic2Proc.cpp"
}

namespace Isolator
{
    #include "src/Isolator/Isolator.h"
    REG_AIRWIN( Isolator, Filter )
    #include "src/Isolator/Isolator.cpp"
    #include "src/Isolator/IsolatorProc.cpp"
}

namespace Isolator2
{
    #include "src/Isolator2/Isolator2.h"
    REG_AIRWIN( Isolator2, Filter )
    #include "src/Isolator2/Isolator2.cpp"
    #include "src/Isolator2/Isolator2Proc.cpp"
}

#endif
#undef __audioeffect__

extern void initAirWindowsEffects2();
void        initAirWindowsEffects()
{
    initAirWindowsEffects2();
}
