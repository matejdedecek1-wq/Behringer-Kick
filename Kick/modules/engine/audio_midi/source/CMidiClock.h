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

namespace mt
{
    constexpr int MAX_EXT_INTERVALS   = 24;
    constexpr int TEMPO_SYNC_INTERVAL = 24;

    constexpr float SECONDS_IN_MINUTE = 60.f;
    constexpr float MILLS_IN_MINUTE   = ( SECONDS_IN_MINUTE * 1000.f );
    constexpr float MICROS_IN_MINUTE  = ( MILLS_IN_MINUTE * 1000.f );

    class ICMidiClockListener
    {
    public:
        virtual ~ICMidiClockListener() = default;
        virtual void onTick( unsigned int ) {} // Current Tick
        virtual void onStart( unsigned int ) {} // Tick count at Play/Resume
        virtual void onResume( unsigned int ) {} // Tick count at Play/Resume
        virtual void onStop( unsigned int ) {} // Tick count at Stop
        virtual void onPause( unsigned int ) {} // Tick count at Pause
    };

    class CMidiClock : private juce::HighResolutionTimer
    {
    public:
        enum class SyncMode
        {
            INTERNAL_CLOCK = 0,
            EXTERNAL_CLOCK
        };

        enum class ClockState
        {
            STOPPED = 0,
            STARTING,
            STARTED
        };

        enum PPQNResolution
        {
            PPQN_24  = 24,
            PPQN_48  = 48,
            PPQN_96  = 96,
            PPQN_384 = 384,
            PPQN_480 = 480,
            PPQN_960 = 960
        };

        std::function<void( uint32_t tick )> onTick       = []( uint32_t ) {};
        std::function<void()>                onClockStart = []() {};
        std::function<void()>                onClockStop  = []() {};

        std::function<void( uint32_t tick, uint32_t microsPerQN )> onSync24Callback = []( uint32_t, uint32_t ) {};

        explicit CMidiClock( PPQNResolution PPQN = PPQN_96 );
        ~CMidiClock();

        // Clock control API's
        void init() noexcept;
        void start( bool ClearCounters ) noexcept;
        void stop() noexcept;

        // Clock state API's...
        inline bool isRunning() const noexcept { return state != ClockState::STOPPED; }
        inline bool isStopped() const noexcept { return state == ClockState::STOPPED; }

        // PPQN factors...
        void           setInternalPPQN( PPQNResolution resolution );
        void           setExternalPPQN( PPQNResolution resolution );
        PPQNResolution getInternalPPQN() const noexcept { return intPPQN; }
        PPQNResolution getExternalPPQN() const noexcept { return extPPQN; }

        // Tempo API's
        float getTempo();
        void  setTempo( float Tempo, double RampLengthInSeconds = 0.001 );
        void  setTempoByTime( uint32_t MicrosPerQuarterNote );

        // Sync API's
        void tickExternal();
        void tickInternal();

        // Clock Synchronization mode.
        void        setClockSyncMode( SyncMode tempo_mode ) noexcept { syncMode = tempo_mode; }
        SyncMode    getClockSyncMode() const noexcept { return syncMode; };
        inline bool isInternalClockSync() const noexcept { return syncMode == SyncMode::INTERNAL_CLOCK; };
        inline bool isExternalClockSync() const noexcept { return syncMode == SyncMode::EXTERNAL_CLOCK; };

        // Calculations...
        static float tempoToMicros( float Tempo ) noexcept { return ( MICROS_IN_MINUTE / Tempo ); }
        static float microsToTempo( float Micros ) noexcept { return ( MICROS_IN_MINUTE / Micros ); }
        static float tempoToPPQNMicros( float Tempo, int PPQN ) noexcept { return ( tempoToMicros( Tempo ) / (float)PPQN ); }
        static float PPQNMicrosToTempo( float Micros, int PPQN ) noexcept { return microsToTempo( Micros * (float)PPQN ); }

    private:
        CMidiClock( const CMidiClock& )            = delete;
        CMidiClock& operator=( const CMidiClock& ) = delete;

        static void initPlatformTimer();
        static void stopPlatformTimer();
        static void startPlatformTimer( [[maybe_unused]] uint32_t microSeconds );
        static void setPlatformTimerInterval( [[maybe_unused]] float microSeconds );
        static void handlePlatformTimer();

        void     resetCounters();
        void     resetExternalClockCounters();
        uint32_t averageOfExternalIntervals() const noexcept;

        inline void hiResTimerCallback() final { handlePlatformTimer(); }
        inline void recomputePPQNFactor() noexcept { intClockPulsesPerExtClockPulse = intPPQN / extPPQN; };

    private:
        static double sTickCounter;
        static double sTickFactor;
        static bool   isTimerRunning;

        // Defaults...
        PPQNResolution intPPQN = PPQN_96;
        PPQNResolution extPPQN = PPQN_24;

        // External clock control
        bool extClockFirstInterval{ false };
        bool syncTempo{ false };

        ClockState state{ ClockState::STOPPED };
        SyncMode   syncMode{ SyncMode::INTERNAL_CLOCK };

        uint32_t microsPerQuarterNote{ 0 };
        uint32_t ticks{ 0 };
        uint32_t intClockSyncPulses{ 0 };
        uint32_t extClockSyncPulses{ 0 };
        uint32_t intClockPulsesPerExtClockPulse{ 0 };
        uint32_t clockCounter{ 0 };
        uint32_t extCounter{ 0 };
        uint32_t syncCounter{ 0 };

        double ts;
        double te;
        double prevTime;

        juce::SmoothedValue<float> tempo{ (float)1.0 };

        uint32_t                                extIntervalIdx{ 0 };
        std::array<uint32_t, MAX_EXT_INTERVALS> extIntervals;
    };

    // Global MIDI Clock Instance
    CMidiClock& getMidiClock() noexcept;
} // namespace mt
