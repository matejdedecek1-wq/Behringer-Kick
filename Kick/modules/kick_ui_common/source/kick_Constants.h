/*
 * Copyright (c) 2021 MUSIC Tribe
 *
 * This file is part of Kick.
 *
 * Kick is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Affero General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * Kick is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with Kick. If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */
#pragma once

namespace kick_ui
{
    //==============================================================================
    enum class RadioGroupId
    {
        AppGridHeader = 2000,
        AppMainMode,
        AppSelectProgram,
        AppTopMenu,
        ParamViewControl,
        SampleActions,
        SampleEditor,
        SampleEditorOptions,
        SampleEditorXFade,
        StepSequence,
        FileBrowserFilter,
        ColourEdit,
        ControlEdit,
        Padperform,
        Knobedit,
        QlinkTab,
        PreferenceView,
        ListBox,
        ProgramSelect,
        ClipProgram,
        DrumProgram,
        KeyGroupProgram,
        MidiProgram,
        SettingsScreen,
        midiControl,
        InsertList,
        InsertOption,
        NoteSelectorOctaves,
        NoteSelectorRootNotes,
        DefaultOctaves,
        NoteSelector,
        AudioHeader
    };

    enum class MidiProgramCCControls
    {
        Modulation,
        Breath,
        CC3,
        Foot,
        Portamento,
        DataEntry,
        Volume,
        Balance,
        CC9,
        Pan,
        Expression,
        Effect1,
        Effect2,
        CC14,
        CC15,
        GenPurpose1,
        GenPurpose2,
        genpurpose3,
        CC20,
        CC21,
        CC23,
        CC24,
        CC25,
        CC26,
        CC27,
        CC28,
        CC29,
        CC30,
        CC31,
        BankSelLSB,
        ModulationLSB,
        BreathLSB,
        CC35_Undefined,
        FootLSB,
        PortamentoLSB,
        DataEntryLSB,
        VolumeLSB,
        BalanceLSB,
        CC41_Undefined,
        PanLSB,
        ExpressionLSB,
        Effect1LSB,
        Effect2LSB,
        CC46,
        CC47,
        GenPur1LSB,
        GenPur2LSB,
        GenPur3LSB,
        GenPur4LSB,
        CC52,
        CC53,
        CC54,
        CC55,
        CC56,
        CC57,
        CC58,
        CC59,
        CC60,
        CC61,
        CC62,
        CC63,
        SustainOnOff,
        PmentoOnOff,
        SostOnOff,
        SoftPedOnOff,
        LegatoAwitch,
        Hold2,
        SndControl1,
        SndControl2,
        SndControl3,
        SndControl4,
        SndControl5,
        SndControl6,
        SndControl7,
        SndControl8,
        SndControl9,
        SndControl10,
        GenPurpose5,
        GenPurpose6,
        GenPurpose7,
        GenPurpose8,
        PmentoControl,
        CC85,
        CC86,
        CC87,
        HiResVelocity,
        CC89,
        CC90,
        Send1,
        Send2,
        Send3,
        Send4,
        DataIncrement,
        DataDecrement,
        NRPNLSB,
        NRPNMSB,
        RPNLSB,
        RPNMSB,
        CC102,
        CC103,
        CC104,
        CC105,
        CC106,
        CC107,
        CC108,
        CC109,
        CC110,
        CC111,
        CC112,
        CC113,
        CC114,
        CC115,
        CC116,
        CC117,
        CC118,
        CC119,
        AllSoundOff,
        ResetAll,
        Localonoff,
        AllnoteOff,
        OmniModeOff,
        OmniModeOn,
        MonoMode,
        PolyMode
    };

    enum class MidiLeranDrumProgram
    {
        Polyphony,
        GlideAll,
        BendRange,
        Doubling,
        DoublingDetune,
        GlideTime,
        Osc1Octave,
        Osc1Fine,
        Osc1Shape,
        Osc1F_Envtoshape,
        Osc1Quard,
        Quard_Detune,
        SubOscShape,
        Osc2Octave,
        Osc2MicroDetune,
        Osc2Shape,
        Osc2F_EnvtoShape,
        Osc2Sync,
        Osc2EQGain,
        Osc2EQFreq,
        Osc2EQKeytrack,
        Osc2EQDrive,
        Osc1Level,
        Osc2Level,
        SubOscLevel,
        RingModLevel,
        MixerDrive,
        LPFCutoff,
        LPFReso,
        LPFSlop,
        LPFSaturation,
        LPFEnv,
        LPFKeytrack,
        FilterAttack,
        FilterDecay,
        FilterSustain,
        FilterRelease,
        AmpAttack,
        AmpDecay,
        AmpSustain,
        AmpRelease,
        Env3StartLevel,
        Env3StartTime,
        Env3SlopHold,
        Env3SlopRelease,
        Env3Destination,
        LFO1Dest,
        LFO1Depth,
        LFO1DepthFade,
        LFO1rate
    };

    enum class DrumProgramWithoutPad
    {
        GlobalSemiTune,
        GlobalFineTune
    };

    enum class MidiLeranKeyProgram
    {
        ProgramPolyphony,
        GlobalSemiTune,
        GlobalFineTune,
        PortamentoTime,
        PortLegato,
        PortQuantize
    };

    enum class MidiLearnClip
    {
        SemiTune,
        FineTune,
        Reverse,
        BPM,
        Fade
    };

    enum class MidiLearnCV
    {
        CVOut1,
        CVOut2,
        CVOut3,
        CVOut4,
        CVOut5,
        CVOut6,
        CVOut7,
        CVOut8
    };

}

namespace kick_Constants
{
    // AppGrid
    constexpr float noteLineWidth{ 16.0f };
    constexpr int   totalPianoKeys{ 127 };

    constexpr int kiBlueStripHeight{ 15 };
    constexpr int minimumBarOrBeatWidthToDraw{ 19 };

    // Parameter View
    constexpr int minimumCellWidthToDraw{ 10 };

    // Step Sequencer
    constexpr int stepButtonViewHeight{ 35 };
    constexpr int stepControlHeight{ 30 };
    constexpr int stepButtonHeight{ 30 };
    constexpr int presetWidth{ 45 };
}

// Grid
constexpr int playHeadWidth   = 10;
constexpr int playHeadWidth_2 = ( playHeadWidth / 2 );
constexpr int playHeadHeight  = 16;

// Font sizes...
namespace FONT
{
    static constexpr auto XS  = 14.f;
    static constexpr auto S   = 16.f;
    static constexpr auto M   = 18.f;
    static constexpr auto L   = 21.f;
    static constexpr auto XL  = 28.f;
    static constexpr auto XXL = 37.f;
}

namespace UI
{
    enum BUTTONS
    {
        CANCEL = -80,
        NO     = CANCEL,
        CLOSE,
        APPLY    = 80,
        YES      = APPLY,
        OK       = YES,
        DELETE   = 90,
        SHUTDOWN = 200,
        RESTART,
    };
} // namespace ui
