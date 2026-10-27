
#include "audioeffectx.h"
#include "../Includes/ObjectFactory.h"

#include <set>

#define __audioeffect__
#if 1

namespace LeadAmp
{
    #include "src/LeadAmp/LeadAmp.h"
    REG_AIRWIN( LeadAmp, Amp_Sims )
    #include "src/LeadAmp/LeadAmp.cpp"
    #include "src/LeadAmp/LeadAmpProc.cpp"
}

namespace LeftoMono
{
    #include "src/LeftoMono/LeftoMono.h"
    REG_AIRWIN( LeftoMono, Utility )
    #include "src/LeftoMono/LeftoMono.cpp"
    #include "src/LeftoMono/LeftoMonoProc.cpp"
}

namespace LilAmp
{
    #include "src/LilAmp/LilAmp.h"
    REG_AIRWIN( LilAmp, Amp_Sims )
    #include "src/LilAmp/LilAmp.cpp"
    #include "src/LilAmp/LilAmpProc.cpp"
}

namespace Logical4
{
    #include "src/Logical4/Logical4.h"
    REG_AIRWIN( Logical4, Dynamics )
    #include "src/Logical4/Logical4.cpp"
    #include "src/Logical4/Logical4Proc.cpp"
}

namespace Loud
{
    #include "src/Loud/Loud.h"
    REG_AIRWIN( Loud, Distortion )
    #include "src/Loud/Loud.cpp"
    #include "src/Loud/LoudProc.cpp"
}

namespace Lowpass
{
    #include "src/Lowpass/Lowpass.h"
    REG_AIRWIN( Lowpass, Filter )
    #include "src/Lowpass/Lowpass.cpp"
    #include "src/Lowpass/LowpassProc.cpp"
}

namespace Lowpass2
{
    #include "src/Lowpass2/Lowpass2.h"
    REG_AIRWIN( Lowpass2, Filter )
    #include "src/Lowpass2/Lowpass2.cpp"
    #include "src/Lowpass2/Lowpass2Proc.cpp"
}

namespace LRFlipTimer
{
    #include "src/LRFlipTimer/LRFlipTimer.h"
    REG_AIRWIN( LRFlipTimer, Stereo )
    #include "src/LRFlipTimer/LRFlipTimer.cpp"
    #include "src/LRFlipTimer/LRFlipTimerProc.cpp"
}

namespace Luxor
{
    #include "src/Luxor/Luxor.h"
    REG_AIRWIN( Luxor, Tone_Color )
    #include "src/Luxor/Luxor.cpp"
    #include "src/Luxor/LuxorProc.cpp"
}

namespace MackEQ
{
    #include "src/MackEQ/MackEQ.h"
    REG_AIRWIN( MackEQ, Filter )
    #include "src/MackEQ/MackEQ.cpp"
    #include "src/MackEQ/MackEQProc.cpp"
}

namespace Mackity
{
    #include "src/Mackity/Mackity.h"
    REG_AIRWIN( Mackity, Distortion )
    #include "src/Mackity/Mackity.cpp"
    #include "src/Mackity/MackityProc.cpp"
}

namespace MatrixVerb
{
    #include "src/MatrixVerb/MatrixVerb.h"
    REG_AIRWIN( MatrixVerb, Reverb )
    #include "src/MatrixVerb/MatrixVerb.cpp"
    #include "src/MatrixVerb/MatrixVerbProc.cpp"
}

namespace Melt
{
    #include "src/Melt/Melt.h"
    REG_AIRWIN( Melt, Ambience )
    #include "src/Melt/Melt.cpp"
    #include "src/Melt/MeltProc.cpp"
}

namespace MidAmp
{
    #include "src/MidAmp/MidAmp.h"
    REG_AIRWIN( MidAmp, Amp_Sims )
    #include "src/MidAmp/MidAmp.cpp"
    #include "src/MidAmp/MidAmpProc.cpp"
}

namespace MidSide
{
    #include "src/MidSide/MidSide.h"
    REG_AIRWIN( MidSide, Utility )
    #include "src/MidSide/MidSide.cpp"
    #include "src/MidSide/MidSideProc.cpp"
}

namespace Mojo
{
    #include "src/Mojo/Mojo.h"
    REG_AIRWIN( Mojo, Saturation )
    #include "src/Mojo/Mojo.cpp"
    #include "src/Mojo/MojoProc.cpp"
}

namespace Monitoring
{
    #include "src/Monitoring/Monitoring.h"
    REG_AIRWIN( Monitoring, Utility )
    #include "src/Monitoring/Monitoring.cpp"
    #include "src/Monitoring/MonitoringProc.cpp"
}

namespace Monitoring2
{
    #include "src/Monitoring2/Monitoring2.h"
    REG_AIRWIN( Monitoring2, Utility )
    #include "src/Monitoring2/Monitoring2.cpp"
    #include "src/Monitoring2/Monitoring2Proc.cpp"
}

namespace Monitoring3
{
    #include "src/Monitoring3/Monitoring3.h"
    REG_AIRWIN( Monitoring3, Utility )
    #include "src/Monitoring3/Monitoring3.cpp"
    #include "src/Monitoring3/Monitoring3Proc.cpp"
}

namespace MoNoam
{
    #include "src/MoNoam/MoNoam.h"
    REG_AIRWIN( MoNoam, Utility )
    #include "src/MoNoam/MoNoam.cpp"
    #include "src/MoNoam/MoNoamProc.cpp"
}

namespace MultiBandDistortion
{
    #include "src/MultiBandDistortion/MultiBandDistortion.h"
    REG_AIRWIN( MultiBandDistortion, Distortion )
    #include "src/MultiBandDistortion/MultiBandDistortion.cpp"
    #include "src/MultiBandDistortion/MultiBandDistortionProc.cpp"
}

namespace MV
{
    #include "src/MV/MV.h"
    REG_AIRWIN( MV, Reverb )
    #include "src/MV/MV.cpp"
    #include "src/MV/MVProc.cpp"
}

namespace NaturalizeDither
{
    #include "src/NaturalizeDither/NaturalizeDither.h"
    REG_AIRWIN( NaturalizeDither, Dithers )
    #include "src/NaturalizeDither/NaturalizeDither.cpp"
    #include "src/NaturalizeDither/NaturalizeDitherProc.cpp"
}

namespace NCSeventeen
{
    #include "src/NCSeventeen/NCSeventeen.h"
    REG_AIRWIN( NCSeventeen, Saturation )
    #include "src/NCSeventeen/NCSeventeen.cpp"
    #include "src/NCSeventeen/NCSeventeenProc.cpp"
}

namespace Neverland
{
    #include "src/Neverland/Neverland.h"
    REG_AIRWIN( Neverland, Tone_Color )
    #include "src/Neverland/Neverland.cpp"
    #include "src/Neverland/NeverlandProc.cpp"
}

namespace Nikola
{
    #include "src/Nikola/Nikola.h"
    REG_AIRWIN( Nikola, Effects )
    #include "src/Nikola/Nikola.cpp"
    #include "src/Nikola/NikolaProc.cpp"
}

namespace NodeDither
{
    #include "src/NodeDither/NodeDither.h"
    REG_AIRWIN( NodeDither, Dithers )
    #include "src/NodeDither/NodeDither.cpp"
    #include "src/NodeDither/NodeDitherProc.cpp"
}

namespace Noise
{
    #include "src/Noise/Noise.h"
    REG_AIRWIN( Noise, Noise )
    #include "src/Noise/Noise.cpp"
    #include "src/Noise/NoiseProc.cpp"
}

namespace NonlinearSpace
{
    #include "src/NonlinearSpace/NonlinearSpace.h"
    REG_AIRWIN( NonlinearSpace, Reverb )
    #include "src/NonlinearSpace/NonlinearSpace.cpp"
    #include "src/NonlinearSpace/NonlinearSpaceProc.cpp"
}

namespace NotJustAnotherCD
{
    #include "src/NotJustAnotherCD/NotJustAnotherCD.h"
    REG_AIRWIN( NotJustAnotherCD, Dithers )
    #include "src/NotJustAnotherCD/NotJustAnotherCD.cpp"
    #include "src/NotJustAnotherCD/NotJustAnotherCDProc.cpp"
}

namespace NotJustAnotherDither
{
    #include "src/NotJustAnotherDither/NotJustAnotherDither.h"
    REG_AIRWIN( NotJustAnotherDither, Dithers )
    #include "src/NotJustAnotherDither/NotJustAnotherDither.cpp"
    #include "src/NotJustAnotherDither/NotJustAnotherDitherProc.cpp"
}

namespace OneCornerClip
{
    #include "src/OneCornerClip/OneCornerClip.h"
    REG_AIRWIN( OneCornerClip, Clipping )
    #include "src/OneCornerClip/OneCornerClip.cpp"
    #include "src/OneCornerClip/OneCornerClipProc.cpp"
}

namespace Pafnuty
{
    #include "src/Pafnuty/Pafnuty.h"
    REG_AIRWIN( Pafnuty, Filter )
    #include "src/Pafnuty/Pafnuty.cpp"
    #include "src/Pafnuty/PafnutyProc.cpp"
}

namespace PaulDither
{
    #include "src/PaulDither/PaulDither.h"
    REG_AIRWIN( PaulDither, Dithers )
    #include "src/PaulDither/PaulDither.cpp"
    #include "src/PaulDither/PaulDitherProc.cpp"
}

namespace PaulWide
{
    #include "src/PaulWide/PaulWide.h"
    REG_AIRWIN( PaulWide, Dithers )
    #include "src/PaulWide/PaulWide.cpp"
    #include "src/PaulWide/PaulWideProc.cpp"
}

namespace PDBuss
{
    #include "src/PDBuss/PDBuss.h"
    REG_AIRWIN( PDBuss, Consoles )
    #include "src/PDBuss/PDBuss.cpp"
    #include "src/PDBuss/PDBussProc.cpp"
}

namespace PDChannel
{
    #include "src/PDChannel/PDChannel.h"
    REG_AIRWIN( PDChannel, Consoles )
    #include "src/PDChannel/PDChannel.cpp"
    #include "src/PDChannel/PDChannelProc.cpp"
}

namespace PeaksOnly
{
    #include "src/PeaksOnly/PeaksOnly.h"
    REG_AIRWIN( PeaksOnly, Utility )
    #include "src/PeaksOnly/PeaksOnly.cpp"
    #include "src/PeaksOnly/PeaksOnlyProc.cpp"
}

namespace PhaseNudge
{
    #include "src/PhaseNudge/PhaseNudge.h"
    REG_AIRWIN( PhaseNudge, Subtlety )
    #include "src/PhaseNudge/PhaseNudge.cpp"
    #include "src/PhaseNudge/PhaseNudgeProc.cpp"
}

namespace PitchDelay
{
    #include "src/PitchDelay/PitchDelay.h"
    REG_AIRWIN( PitchDelay, Ambience )
    #include "src/PitchDelay/PitchDelay.cpp"
    #include "src/PitchDelay/PitchDelayProc.cpp"
}

namespace PitchNasty
{
    #include "src/PitchNasty/PitchNasty.h"
    REG_AIRWIN( PitchNasty, Effects )
    #include "src/PitchNasty/PitchNasty.cpp"
    #include "src/PitchNasty/PitchNastyProc.cpp"
}

namespace PocketVerbs
{
    #include "src/PocketVerbs/PocketVerbs.h"
    REG_AIRWIN( PocketVerbs, Reverb )
    #include "src/PocketVerbs/PocketVerbs.cpp"
    #include "src/PocketVerbs/PocketVerbsProc.cpp"
}

namespace Pockey
{
    #include "src/Pockey/Pockey.h"
    REG_AIRWIN( Pockey, Lo_Fi )
    #include "src/Pockey/Pockey.cpp"
    #include "src/Pockey/PockeyProc.cpp"
}

namespace Pockey2
{
    #include "src/Pockey2/Pockey2.h"
    REG_AIRWIN( Pockey2, Lo_Fi )
    #include "src/Pockey2/Pockey2.cpp"
    #include "src/Pockey2/Pockey2Proc.cpp"
}

namespace Podcast
{
    #include "src/Podcast/Podcast.h"
    REG_AIRWIN( Podcast, Dynamics )
    #include "src/Podcast/Podcast.cpp"
    #include "src/Podcast/PodcastProc.cpp"
}

namespace PodcastDeluxe
{
    #include "src/PodcastDeluxe/PodcastDeluxe.h"
    REG_AIRWIN( PodcastDeluxe, Dynamics )
    #include "src/PodcastDeluxe/PodcastDeluxe.cpp"
    #include "src/PodcastDeluxe/PodcastDeluxeProc.cpp"
}

namespace Point
{
    #include "src/Point/Point.h"
    REG_AIRWIN( Point, Dynamics )
    #include "src/Point/Point.cpp"
    #include "src/Point/PointProc.cpp"
}

namespace Pop
{
    #include "src/Pop/Pop.h"
    REG_AIRWIN( Pop, Dynamics )
    #include "src/Pop/Pop.cpp"
    #include "src/Pop/PopProc.cpp"
}

namespace Pop2
{
    #include "src/Pop2/Pop2.h"
    REG_AIRWIN( Pop2, Dynamics )
    #include "src/Pop2/Pop2.cpp"
    #include "src/Pop2/Pop2Proc.cpp"
}

namespace PowerSag
{
    #include "src/PowerSag/PowerSag.h"
    REG_AIRWIN( PowerSag, Effects )
    #include "src/PowerSag/PowerSag.cpp"
    #include "src/PowerSag/PowerSagProc.cpp"
}

namespace PowerSag2
{
    #include "src/PowerSag2/PowerSag2.h"
    REG_AIRWIN( PowerSag2, Effects )
    #include "src/PowerSag2/PowerSag2.cpp"
    #include "src/PowerSag2/PowerSag2Proc.cpp"
}

namespace Precious
{
    #include "src/Precious/Precious.h"
    REG_AIRWIN( Precious, Tone_Color )
    #include "src/Precious/Precious.cpp"
    #include "src/Precious/PreciousProc.cpp"
}

namespace Preponderant
{
    #include "src/Preponderant/Preponderant.h"
    REG_AIRWIN( Preponderant, Effects )
    #include "src/Preponderant/Preponderant.cpp"
    #include "src/Preponderant/PreponderantProc.cpp"
}

namespace Pressure4
{
    #include "src/Pressure4/Pressure4.h"
    REG_AIRWIN( Pressure4, Dynamics )
    #include "src/Pressure4/Pressure4.cpp"
    #include "src/Pressure4/Pressure4Proc.cpp"
}

namespace Pressure5
{
    #include "src/Pressure5/Pressure5.h"
    REG_AIRWIN( Pressure5, Dynamics )
    #include "src/Pressure5/Pressure5.cpp"
    #include "src/Pressure5/Pressure5Proc.cpp"
}

namespace PurestAir
{
    #include "src/PurestAir/PurestAir.h"
    REG_AIRWIN( PurestAir, Brightness )
    #include "src/PurestAir/PurestAir.cpp"
    #include "src/PurestAir/PurestAirProc.cpp"
}

namespace PurestConsole2Buss
{
    #include "src/PurestConsole2Buss/PurestConsole2Buss.h"
    REG_AIRWIN( PurestConsole2Buss, Consoles )
    #include "src/PurestConsole2Buss/PurestConsole2Buss.cpp"
    #include "src/PurestConsole2Buss/PurestConsole2BussProc.cpp"
}

namespace PurestConsole2Channel
{
    #include "src/PurestConsole2Channel/PurestConsole2Channel.h"
    REG_AIRWIN( PurestConsole2Channel, Consoles )
    #include "src/PurestConsole2Channel/PurestConsole2Channel.cpp"
    #include "src/PurestConsole2Channel/PurestConsole2ChannelProc.cpp"
}

namespace PurestConsoleBuss
{
    #include "src/PurestConsoleBuss/PurestConsoleBuss.h"
    REG_AIRWIN( PurestConsoleBuss, Consoles )
    #include "src/PurestConsoleBuss/PurestConsoleBuss.cpp"
    #include "src/PurestConsoleBuss/PurestConsoleBussProc.cpp"
}

namespace PurestConsoleChannel
{
    #include "src/PurestConsoleChannel/PurestConsoleChannel.h"
    REG_AIRWIN( PurestConsoleChannel, Consoles )
    #include "src/PurestConsoleChannel/PurestConsoleChannel.cpp"
    #include "src/PurestConsoleChannel/PurestConsoleChannelProc.cpp"
}

namespace PurestDrive
{
    #include "src/PurestDrive/PurestDrive.h"
    REG_AIRWIN( PurestDrive, Saturation )
    #include "src/PurestDrive/PurestDrive.cpp"
    #include "src/PurestDrive/PurestDriveProc.cpp"
}

namespace PurestEcho
{
    #include "src/PurestEcho/PurestEcho.h"
    REG_AIRWIN( PurestEcho, Ambience )
    #include "src/PurestEcho/PurestEcho.cpp"
    #include "src/PurestEcho/PurestEchoProc.cpp"
}

namespace PurestFade
{
    #include "src/PurestFade/PurestFade.h"
    REG_AIRWIN( PurestFade, Utility )
    #include "src/PurestFade/PurestFade.cpp"
    #include "src/PurestFade/PurestFadeProc.cpp"
}

namespace PurestGain
{
    #include "src/PurestGain/PurestGain.h"
    REG_AIRWIN( PurestGain, Utility )
    #include "src/PurestGain/PurestGain.cpp"
    #include "src/PurestGain/PurestGainProc.cpp"
}

namespace PurestSquish
{
    #include "src/PurestSquish/PurestSquish.h"
    REG_AIRWIN( PurestSquish, Dynamics )
    #include "src/PurestSquish/PurestSquish.cpp"
    #include "src/PurestSquish/PurestSquishProc.cpp"
}

namespace PurestWarm
{
    #include "src/PurestWarm/PurestWarm.h"
    REG_AIRWIN( PurestWarm, Subtlety )
    #include "src/PurestWarm/PurestWarm.cpp"
    #include "src/PurestWarm/PurestWarmProc.cpp"
}

namespace PurestWarm2
{
    #include "src/PurestWarm2/PurestWarm2.h"
    REG_AIRWIN( PurestWarm2, Subtlety )
    #include "src/PurestWarm2/PurestWarm2.cpp"
    #include "src/PurestWarm2/PurestWarm2Proc.cpp"
}

namespace Pyewacket
{
    #include "src/Pyewacket/Pyewacket.h"
    REG_AIRWIN( Pyewacket, Dynamics )
    #include "src/Pyewacket/Pyewacket.cpp"
    #include "src/Pyewacket/PyewacketProc.cpp"
}

namespace RawGlitters
{
    #include "src/RawGlitters/RawGlitters.h"
    REG_AIRWIN( RawGlitters, Dithers )
    #include "src/RawGlitters/RawGlitters.cpp"
    #include "src/RawGlitters/RawGlittersProc.cpp"
}

namespace RawTimbers
{
    #include "src/RawTimbers/RawTimbers.h"
    REG_AIRWIN( RawTimbers, Dithers )
    #include "src/RawTimbers/RawTimbers.cpp"
    #include "src/RawTimbers/RawTimbersProc.cpp"
}

namespace Recurve
{
    #include "src/Recurve/Recurve.h"
    REG_AIRWIN( Recurve, Dynamics )
    #include "src/Recurve/Recurve.cpp"
    #include "src/Recurve/RecurveProc.cpp"
}

namespace Remap
{
    #include "src/Remap/Remap.h"
    REG_AIRWIN( Remap, Subtlety )
    #include "src/Remap/Remap.cpp"
    #include "src/Remap/RemapProc.cpp"
}

namespace ResEQ
{
    #include "src/ResEQ/ResEQ.h"
    REG_AIRWIN( ResEQ, Filter )
    #include "src/ResEQ/ResEQ.cpp"
    #include "src/ResEQ/ResEQProc.cpp"
}

namespace Reverb
{
    #include "src/Reverb/Reverb.h"
    REG_AIRWIN( Reverb, Reverb )
    #include "src/Reverb/Reverb.cpp"
    #include "src/Reverb/ReverbProc.cpp"
}

namespace Righteous4
{
    #include "src/Righteous4/Righteous4.h"
    REG_AIRWIN( Righteous4, Saturation )
    #include "src/Righteous4/Righteous4.cpp"
    #include "src/Righteous4/Righteous4Proc.cpp"
}

namespace RightoMono
{
    #include "src/RightoMono/RightoMono.h"
    REG_AIRWIN( RightoMono, Utility )
    #include "src/RightoMono/RightoMono.cpp"
    #include "src/RightoMono/RightoMonoProc.cpp"
}

namespace Shape
{
    #include "src/Shape/Shape.h"
    REG_AIRWIN( Shape, Subtlety )
    #include "src/Shape/Shape.cpp"
    #include "src/Shape/ShapeProc.cpp"
}

namespace SideDull
{
    #include "src/SideDull/SideDull.h"
    REG_AIRWIN( SideDull, Stereo )
    #include "src/SideDull/SideDull.cpp"
    #include "src/SideDull/SideDullProc.cpp"
}

namespace Sidepass
{
    #include "src/Sidepass/Sidepass.h"
    REG_AIRWIN( Sidepass, Stereo )
    #include "src/Sidepass/Sidepass.cpp"
    #include "src/Sidepass/SidepassProc.cpp"
}

namespace Silhouette
{
    #include "src/Silhouette/Silhouette.h"
    REG_AIRWIN( Silhouette, Noise )
    #include "src/Silhouette/Silhouette.cpp"
    #include "src/Silhouette/SilhouetteProc.cpp"
}

namespace SingleEndedTriode
{
    #include "src/SingleEndedTriode/SingleEndedTriode.h"
    REG_AIRWIN( SingleEndedTriode, Subtlety )
    #include "src/SingleEndedTriode/SingleEndedTriode.cpp"
    #include "src/SingleEndedTriode/SingleEndedTriodeProc.cpp"
}

namespace Slew
{
    #include "src/Slew/Slew.h"
    REG_AIRWIN( Slew, Brightness )
    #include "src/Slew/Slew.cpp"
    #include "src/Slew/SlewProc.cpp"
}

namespace Slew2
{
    #include "src/Slew2/Slew2.h"
    REG_AIRWIN( Slew2, Brightness )
    #include "src/Slew2/Slew2.cpp"
    #include "src/Slew2/Slew2Proc.cpp"
}

namespace Slew3
{
    #include "src/Slew3/Slew3.h"
    REG_AIRWIN( Slew3, Brightness )
    #include "src/Slew3/Slew3.cpp"
    #include "src/Slew3/Slew3Proc.cpp"
}

namespace SlewOnly
{
    #include "src/SlewOnly/SlewOnly.h"
    REG_AIRWIN( SlewOnly, Utility )
    #include "src/SlewOnly/SlewOnly.cpp"
    #include "src/SlewOnly/SlewOnlyProc.cpp"
}

namespace Smooth
{
    #include "src/Smooth/Smooth.h"
    REG_AIRWIN( Smooth, Brightness )
    #include "src/Smooth/Smooth.cpp"
    #include "src/Smooth/SmoothProc.cpp"
}

namespace SoftGate
{
    #include "src/SoftGate/SoftGate.h"
    REG_AIRWIN( SoftGate, Dynamics )
    #include "src/SoftGate/SoftGate.cpp"
    #include "src/SoftGate/SoftGateProc.cpp"
}

namespace SpatializeDither
{
    #include "src/SpatializeDither/SpatializeDither.h"
    REG_AIRWIN( SpatializeDither, Dithers )
    #include "src/SpatializeDither/SpatializeDither.cpp"
    #include "src/SpatializeDither/SpatializeDitherProc.cpp"
}

namespace Spiral
{
    #include "src/Spiral/Spiral.h"
    REG_AIRWIN( Spiral, Saturation )
    #include "src/Spiral/Spiral.cpp"
    #include "src/Spiral/SpiralProc.cpp"
}

namespace Spiral2
{
    #include "src/Spiral2/Spiral2.h"
    REG_AIRWIN( Spiral2, Saturation )
    #include "src/Spiral2/Spiral2.cpp"
    #include "src/Spiral2/Spiral2Proc.cpp"
}

namespace Srsly
{
    #include "src/Srsly/Srsly.h"
    REG_AIRWIN( Srsly, Stereo )
    #include "src/Srsly/Srsly.cpp"
    #include "src/Srsly/SrslyProc.cpp"
}

namespace Srsly2
{
    #include "src/Srsly2/Srsly2.h"
    REG_AIRWIN( Srsly2, Stereo )
    #include "src/Srsly2/Srsly2.cpp"
    #include "src/Srsly2/Srsly2Proc.cpp"
}

namespace StarChild
{
    #include "src/StarChild/StarChild.h"
    REG_AIRWIN( StarChild, Ambience )
    #include "src/StarChild/StarChild.cpp"
    #include "src/StarChild/StarChildProc.cpp"
}

namespace StereoChorus
{
    #include "src/StereoChorus/StereoChorus.h"
    REG_AIRWIN( StereoChorus, Ambience )
    #include "src/StereoChorus/StereoChorus.cpp"
    #include "src/StereoChorus/StereoChorusProc.cpp"
}

namespace StereoDoubler
{
    #include "src/StereoDoubler/StereoDoubler.h"
    REG_AIRWIN( StereoDoubler, Ambience )
    #include "src/StereoDoubler/StereoDoubler.cpp"
    #include "src/StereoDoubler/StereoDoublerProc.cpp"
}

namespace StereoEnsemble
{
    #include "src/StereoEnsemble/StereoEnsemble.h"
    REG_AIRWIN( StereoEnsemble, Ambience )
    #include "src/StereoEnsemble/StereoEnsemble.cpp"
    #include "src/StereoEnsemble/StereoEnsembleProc.cpp"
}

namespace StereoFX
{
    #include "src/StereoFX/StereoFX.h"
    REG_AIRWIN( StereoFX, Stereo )
    #include "src/StereoFX/StereoFX.cpp"
    #include "src/StereoFX/StereoFXProc.cpp"
}

namespace StudioTan
{
    #include "src/StudioTan/StudioTan.h"
    REG_AIRWIN( StudioTan, Dithers )
    #include "src/StudioTan/StudioTan.cpp"
    #include "src/StudioTan/StudioTanProc.cpp"
}

namespace SubsOnly
{
    #include "src/SubsOnly/SubsOnly.h"
    REG_AIRWIN( SubsOnly, Utility )
    #include "src/SubsOnly/SubsOnly.cpp"
    #include "src/SubsOnly/SubsOnlyProc.cpp"
}

namespace Surge
{
    #include "src/Surge/Surge.h"
    REG_AIRWIN( Surge, Dynamics )
    #include "src/Surge/Surge.cpp"
    #include "src/Surge/SurgeProc.cpp"
}

namespace SurgeTide
{
    #include "src/SurgeTide/SurgeTide.h"
    REG_AIRWIN( SurgeTide, Dynamics )
    #include "src/SurgeTide/SurgeTide.cpp"
    #include "src/SurgeTide/SurgeTideProc.cpp"
}

namespace Swell
{
    #include "src/Swell/Swell.h"
    REG_AIRWIN( Swell, Dynamics )
    #include "src/Swell/Swell.cpp"
    #include "src/Swell/SwellProc.cpp"
}

namespace Tape
{
    #include "src/Tape/Tape.h"
    REG_AIRWIN( Tape, Tape )
    #include "src/Tape/Tape.cpp"
    #include "src/Tape/TapeProc.cpp"
}

namespace TapeDelay
{
    #include "src/TapeDelay/TapeDelay.h"
    REG_AIRWIN( TapeDelay, Ambience )
    #include "src/TapeDelay/TapeDelay.cpp"
    #include "src/TapeDelay/TapeDelayProc.cpp"
}

namespace TapeDelay2
{
    #include "src/TapeDelay2/TapeDelay2.h"
    REG_AIRWIN( TapeDelay2, Ambience )
    #include "src/TapeDelay2/TapeDelay2.cpp"
    #include "src/TapeDelay2/TapeDelay2Proc.cpp"
}

namespace TapeDither
{
    #include "src/TapeDither/TapeDither.h"
    REG_AIRWIN( TapeDither, Dithers )
    #include "src/TapeDither/TapeDither.cpp"
    #include "src/TapeDither/TapeDitherProc.cpp"
}

namespace TapeDust
{
    #include "src/TapeDust/TapeDust.h"
    REG_AIRWIN( TapeDust, Noise )
    #include "src/TapeDust/TapeDust.cpp"
    #include "src/TapeDust/TapeDustProc.cpp"
}

namespace TapeFat
{
    #include "src/TapeFat/TapeFat.h"
    REG_AIRWIN( TapeFat, Filter )
    #include "src/TapeFat/TapeFat.cpp"
    #include "src/TapeFat/TapeFatProc.cpp"
}

namespace Texturize
{
    #include "src/Texturize/Texturize.h"
    REG_AIRWIN( Texturize, Noise )
    #include "src/Texturize/Texturize.cpp"
    #include "src/Texturize/TexturizeProc.cpp"
}

namespace TexturizeMS
{
    #include "src/TexturizeMS/TexturizeMS.h"
    REG_AIRWIN( TexturizeMS, Noise )
    #include "src/TexturizeMS/TexturizeMS.cpp"
    #include "src/TexturizeMS/TexturizeMSProc.cpp"
}

namespace Thunder
{
    #include "src/Thunder/Thunder.h"
    REG_AIRWIN( Thunder, Dynamics )
    #include "src/Thunder/Thunder.cpp"
    #include "src/Thunder/ThunderProc.cpp"
}

namespace ToneSlant
{
    #include "src/ToneSlant/ToneSlant.h"
    REG_AIRWIN( ToneSlant, Filter )
    #include "src/ToneSlant/ToneSlant.cpp"
    #include "src/ToneSlant/ToneSlantProc.cpp"
}

namespace ToTape5
{
    #include "src/ToTape5/ToTape5.h"
    REG_AIRWIN( ToTape5, Tape )
    #include "src/ToTape5/ToTape5.cpp"
    #include "src/ToTape5/ToTape5Proc.cpp"
}

namespace ToTape6
{
    #include "src/ToTape6/ToTape6.h"
    REG_AIRWIN( ToTape6, Tape )
    #include "src/ToTape6/ToTape6.cpp"
    #include "src/ToTape6/ToTape6Proc.cpp"
}

namespace ToVinyl4
{
    #include "src/ToVinyl4/ToVinyl4.h"
    REG_AIRWIN( ToVinyl4, Stereo )
    #include "src/ToVinyl4/ToVinyl4.cpp"
    #include "src/ToVinyl4/ToVinyl4Proc.cpp"
}

namespace TPDFDither
{
    #include "src/TPDFDither/TPDFDither.h"
    REG_AIRWIN( TPDFDither, Dithers )
    #include "src/TPDFDither/TPDFDither.cpp"
    #include "src/TPDFDither/TPDFDitherProc.cpp"
}

namespace TPDFWide
{
    #include "src/TPDFWide/TPDFWide.h"
    REG_AIRWIN( TPDFWide, Dithers )
    #include "src/TPDFWide/TPDFWide.cpp"
    #include "src/TPDFWide/TPDFWideProc.cpp"
}

namespace TransDesk
{
    #include "src/TransDesk/TransDesk.h"
    REG_AIRWIN( TransDesk, Subtlety )
    #include "src/TransDesk/TransDesk.cpp"
    #include "src/TransDesk/TransDeskProc.cpp"
}

namespace Tremolo
{
    #include "src/Tremolo/Tremolo.h"
    REG_AIRWIN( Tremolo, Effects )
    #include "src/Tremolo/Tremolo.cpp"
    #include "src/Tremolo/TremoloProc.cpp"
}

namespace TremoSquare
{
    #include "src/TremoSquare/TremoSquare.h"
    REG_AIRWIN( TremoSquare, Effects )
    #include "src/TremoSquare/TremoSquare.cpp"
    #include "src/TremoSquare/TremoSquareProc.cpp"
}

namespace TripleSpread
{
    //#include "src/TripleSpread/TripleSpread.h"
    //REG_AIRWIN( TripleSpread )
    //#include "src/TripleSpread/TripleSpread.cpp"
    //#include "src/TripleSpread/TripleSpreadProc.cpp"
}

namespace Tube
{
    #include "src/Tube/Tube.h"
    REG_AIRWIN( Tube, Saturation )
    #include "src/Tube/Tube.cpp"
    #include "src/Tube/TubeProc.cpp"
}

namespace Tube2
{
    #include "src/Tube2/Tube2.h"
    REG_AIRWIN( Tube2, Saturation )
    #include "src/Tube2/Tube2.cpp"
    #include "src/Tube2/Tube2Proc.cpp"
}

namespace TubeDesk
{
    #include "src/TubeDesk/TubeDesk.h"
    REG_AIRWIN( TubeDesk, Subtlety )
    #include "src/TubeDesk/TubeDesk.cpp"
    #include "src/TubeDesk/TubeDeskProc.cpp"
}

namespace uLawDecode
{
    #include "src/uLawDecode/uLawDecode.h"
    REG_AIRWIN( uLawDecode, Utility )
    #include "src/uLawDecode/uLawDecode.cpp"
    #include "src/uLawDecode/uLawDecodeProc.cpp"
}

namespace uLawEncode
{
    #include "src/uLawEncode/uLawEncode.h"
    REG_AIRWIN( uLawEncode, Utility )
    #include "src/uLawEncode/uLawEncode.cpp"
    #include "src/uLawEncode/uLawEncodeProc.cpp"
}

namespace Ultrasonic
{
    #include "src/Ultrasonic/Ultrasonic.h"
    REG_AIRWIN( Ultrasonic, Brightness )
    #include "src/Ultrasonic/Ultrasonic.cpp"
    #include "src/Ultrasonic/UltrasonicProc.cpp"
}

namespace UltrasonicLite
{
    #include "src/UltrasonicLite/UltrasonicLite.h"
    REG_AIRWIN( UltrasonicLite, Brightness )
    #include "src/UltrasonicLite/UltrasonicLite.cpp"
    #include "src/UltrasonicLite/UltrasonicLiteProc.cpp"
}

namespace UltrasonicMed
{
    #include "src/UltrasonicMed/UltrasonicMed.h"
    REG_AIRWIN( UltrasonicMed, Brightness )
    #include "src/UltrasonicMed/UltrasonicMed.cpp"
    #include "src/UltrasonicMed/UltrasonicMedProc.cpp"
}

namespace UltrasonX
{
    #include "src/UltrasonX/UltrasonX.h"
    REG_AIRWIN( UltrasonX, Brightness )
    #include "src/UltrasonX/UltrasonX.cpp"
    #include "src/UltrasonX/UltrasonXProc.cpp"
}

namespace UnBox
{
    #include "src/UnBox/UnBox.h"
    REG_AIRWIN( UnBox, Saturation )
    #include "src/UnBox/UnBox.cpp"
    #include "src/UnBox/UnBoxProc.cpp"
}

namespace VariMu
{
    #include "src/VariMu/VariMu.h"
    REG_AIRWIN( VariMu, Dynamics )
    #include "src/VariMu/VariMu.cpp"
    #include "src/VariMu/VariMuProc.cpp"
}

namespace Verbity
{
    #include "src/Verbity/Verbity.h"
    REG_AIRWIN( Verbity, Reverb )
    #include "src/Verbity/Verbity.cpp"
    #include "src/Verbity/VerbityProc.cpp"
}

namespace Verbity2
{
    #include "src/Verbity2/Verbity2.h"
    REG_AIRWIN( Verbity2, Reverb )
    #include "src/Verbity2/Verbity2.cpp"
    #include "src/Verbity2/Verbity2Proc.cpp"
}

namespace Vibrato
{
    #include "src/Vibrato/Vibrato.h"
    REG_AIRWIN( Vibrato, Lo_Fi )
    #include "src/Vibrato/Vibrato.cpp"
    #include "src/Vibrato/VibratoProc.cpp"
}

namespace VinylDither
{
    #include "src/VinylDither/VinylDither.h"
    REG_AIRWIN( VinylDither, Dithers )
    #include "src/VinylDither/VinylDither.cpp"
    #include "src/VinylDither/VinylDitherProc.cpp"
}

namespace VoiceOfTheStarship
{
    #include "src/VoiceOfTheStarship/VoiceOfTheStarship.h"
    REG_AIRWIN( VoiceOfTheStarship, Noise )
    #include "src/VoiceOfTheStarship/VoiceOfTheStarship.cpp"
    #include "src/VoiceOfTheStarship/VoiceOfTheStarshipProc.cpp"
}

namespace VoiceTrick
{
    #include "src/VoiceTrick/VoiceTrick.h"
    REG_AIRWIN( VoiceTrick, Utility )
    #include "src/VoiceTrick/VoiceTrick.cpp"
    #include "src/VoiceTrick/VoiceTrickProc.cpp"
}

namespace Wider
{
    #include "src/Wider/Wider.h"
    REG_AIRWIN( Wider, Stereo )
    #include "src/Wider/Wider.cpp"
    #include "src/Wider/WiderProc.cpp"
}

namespace XBandpass
{
    #include "src/XBandpass/XBandpass.h"
    REG_AIRWIN( XBandpass, XYZ_Filter )
    #include "src/XBandpass/XBandpass.cpp"
    #include "src/XBandpass/XBandpassProc.cpp"
}

namespace XHighpass
{
    #include "src/XHighpass/XHighpass.h"
    REG_AIRWIN( XHighpass, XYZ_Filter )
    #include "src/XHighpass/XHighpass.cpp"
    #include "src/XHighpass/XHighpassProc.cpp"
}

namespace XLowpass
{
    #include "src/XLowpass/XLowpass.h"
    REG_AIRWIN( XLowpass, XYZ_Filter )
    #include "src/XLowpass/XLowpass.cpp"
    #include "src/XLowpass/XLowpassProc.cpp"
}

namespace XNotch
{
    #include "src/XNotch/XNotch.h"
    REG_AIRWIN( XNotch, XYZ_Filter )
    #include "src/XNotch/XNotch.cpp"
    #include "src/XNotch/XNotchProc.cpp"
}

namespace XRegion
{
    #include "src/XRegion/XRegion.h"
    REG_AIRWIN( XRegion, XYZ_Filter )
    #include "src/XRegion/XRegion.cpp"
    #include "src/XRegion/XRegionProc.cpp"
}

namespace YBandpass
{
    #include "src/YBandpass/YBandpass.h"
    REG_AIRWIN( YBandpass, XYZ_Filter )
    #include "src/YBandpass/YBandpass.cpp"
    #include "src/YBandpass/YBandpassProc.cpp"
}

namespace YHighpass
{
    #include "src/YHighpass/YHighpass.h"
    REG_AIRWIN( YHighpass, XYZ_Filter )
    #include "src/YHighpass/YHighpass.cpp"
    #include "src/YHighpass/YHighpassProc.cpp"
}

namespace YLowpass
{
    #include "src/YLowpass/YLowpass.h"
    REG_AIRWIN( YLowpass, XYZ_Filter )
    #include "src/YLowpass/YLowpass.cpp"
    #include "src/YLowpass/YLowpassProc.cpp"
}

namespace YNotch
{
    #include "src/YNotch/YNotch.h"
    REG_AIRWIN( YNotch, XYZ_Filter )
    #include "src/YNotch/YNotch.cpp"
    #include "src/YNotch/YNotchProc.cpp"
}

namespace ZBandpass
{
    #include "src/ZBandpass/ZBandpass.h"
    REG_AIRWIN( ZBandpass, XYZ_Filter )
    #include "src/ZBandpass/ZBandpass.cpp"
    #include "src/ZBandpass/ZBandpassProc.cpp"
}

namespace ZBandpass2
{
    #include "src/ZBandpass2/ZBandpass2.h"
    REG_AIRWIN( ZBandpass2, XYZ_Filter )
    #include "src/ZBandpass2/ZBandpass2.cpp"
    #include "src/ZBandpass2/ZBandpass2Proc.cpp"
}

namespace ZHighpass
{
    #include "src/ZHighpass/ZHighpass.h"
    REG_AIRWIN( ZHighpass, XYZ_Filter )
    #include "src/ZHighpass/ZHighpass.cpp"
    #include "src/ZHighpass/ZHighpassProc.cpp"
}

namespace ZHighpass2
{
    #include "src/ZHighpass2/ZHighpass2.h"
    REG_AIRWIN( ZHighpass2, XYZ_Filter )
    #include "src/ZHighpass2/ZHighpass2.cpp"
    #include "src/ZHighpass2/ZHighpass2Proc.cpp"
}

namespace ZLowpass
{
    #include "src/ZLowpass/ZLowpass.h"
    REG_AIRWIN( ZLowpass, XYZ_Filter )
    #include "src/ZLowpass/ZLowpass.cpp"
    #include "src/ZLowpass/ZLowpassProc.cpp"
}

namespace ZLowpass2
{
    #include "src/ZLowpass2/ZLowpass2.h"
    REG_AIRWIN( ZLowpass2, XYZ_Filter )
    #include "src/ZLowpass2/ZLowpass2.cpp"
    #include "src/ZLowpass2/ZLowpass2Proc.cpp"
}

namespace ZNotch
{
    #include "src/ZNotch/ZNotch.h"
    REG_AIRWIN( ZNotch, XYZ_Filter )
    #include "src/ZNotch/ZNotch.cpp"
    #include "src/ZNotch/ZNotchProc.cpp"
}

namespace ZNotch2
{
    #include "src/ZNotch2/ZNotch2.h"
    REG_AIRWIN( ZNotch2, XYZ_Filter )
    #include "src/ZNotch2/ZNotch2.cpp"
    #include "src/ZNotch2/ZNotch2Proc.cpp"
}

namespace ZRegion
{
    #include "src/ZRegion/ZRegion.h"
    REG_AIRWIN( ZRegion, XYZ_Filter )
    #include "src/ZRegion/ZRegion.cpp"
    #include "src/ZRegion/ZRegionProc.cpp"
}

namespace ZRegion2
{
    #include "src/ZRegion2/ZRegion2.h"
    REG_AIRWIN( ZRegion2, XYZ_Filter )
    #include "src/ZRegion2/ZRegion2.cpp"
    #include "src/ZRegion2/ZRegion2Proc.cpp"
}

#endif
#undef __audioeffect__

void initAirWindowsEffects2()
{
}
