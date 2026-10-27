/*
 * Copyright (c) 2024 MUSIC Tribe
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
    class MTTempoSettings
    {
    public:
        explicit MTTempoSettings( const juce::ValueTree& VT, juce::UndoManager* UM = nullptr );

        // State...
        inline juce::ValueTree& getTempoState() noexcept { return vTree; }

        CachedValue<BPMConstrainer> tempo;
        CachedValue<int>            length;
        CachedValue<int>            num;

        // Getters...
        inline float getBPM() const { return tempo.get(); }
        inline int   getLength() const { return length.get(); }
        inline int   getNumerator() const { return num.get(); };
        int          getDenominator() const;
        int          getQuantization() const;

        MTTimeBase   getTimeBase() const;
        MTTimeBase   getQuantizationLevel() const;
        MTBarNBeats  getLengthBnB() const;
        unsigned int toTicks( double TimeInSeconds ) const;
        double       toSeconds( unsigned int TimeInTicks ) const;
        MTBarNBeats  toBnB( unsigned int TimeInTicks ) const;
        MTBarNBeats  toBnB( double TimeInSeconds ) const;

        // Setters...
        void setBPM( const float BPM, bool internalSource = false );
        void setLength( const int Bar, bool internalSource = false );
        void setNumerator( const int Numerator, bool internalSource = false );
        void setTimebase( MTTimeBase Timebase, bool internalSource = false );
        void setQuantization( MTTimeBase Quantization, bool internalSource = false );
        void setLengthBnB( const MTBarNBeats& BnB, bool internalSource = false );

        // Length API's
        // In terms of ticks...
        int getTicksPerBeat() const;
        int getTicksPerBar() const;
        int getTicksPerTimebase( MTTimeBase TimeBase ) const;
        int getTicksPerQuantization() const;
        int getLengthInTicks() const;
        // In terms of seconds...
        double getSecondsPerBeat() const;
        double getSecondsPerBar() const;
        double getSecondsPerTick() const;
        double getLengthInSeconds() const;

    private:
        juce::ValueTree    vTree;
        juce::UndoManager* um{ nullptr };
    };

    class MTTempoMgr
    {
    public:
        MTTempoMgr();

        void setup( const juce::ValueTree& VT, juce::UndoManager* UM );

        juce::ValueTree& getTempoMgrState() noexcept;
        juce::ValueTree  getTempoState( int Index ) noexcept;

        // Tempo Settings...
        int              getNumTempoSettings() const;
        MTTempoSettings& getTempoSettings( int Index = 0 );
        MTTempoSettings& getTempoSettingsAtBar( int Bar );
        MTTempoSettings& getTempoSettingsAtTick( unsigned int Ticks );

        void addTransport( MTTempoSettings& TempoSetting, int Index = -1 );
        void removeTransport( MTTempoSettings& TempoSetting );
        void removeTransport( int Index = -1 );

        //
        void setLength( int Length, bool internalSource = false, int Index = 0 );
        void setBPM( float Tempo, bool internalSource = false, int Index = 0 );
        void setNumerator( int Numerator, bool internalSource = false, int Index = 0 );
        void setQuantization( MTTimeBase Quantization, bool internalSource = false, int Index = 0 );
        void setTimeBase( MTTimeBase TimeBase, bool internalSource = false, int Index = 0 );

        int        getLength( int Bar = 0 );
        int        getBPM( int Bar = 0 );
        int        getNumerator( int Bar = 0 );
        int        getDenominator( int Bar = 0 );
        int        getQuantization( int Bar = 0 );
        MTTimeBase getTimeBase( int Index = 0 );

        int getBeatsPerBar( int Bar = 0 );
        int getQuantizationPerBar( int Bar = 0 );

        int getTicksPerBeat( int Bar = 0 );
        int getTicksPerBar( int Bar = 0 );
        int getTicksPerQuantization( int Bar = 0 );
        int getTicksPerTimebase( int Bar = 0, MTTimeBase Timebase = defaultTimeBase );

        int getBarAtTick( int Ticks );
        int getBaseTicksAtTempoSettingOfBar( int Bar = 0 );
        int getBaseBarAtTempoSetting( int Ticks );
        int getBaseTicksAtTempoSetting( int Ticks );

        double getSecondsPerBeat( int Bar = 0 );
        double getSecondsPerBar( int Bar = 0 );

        // Utility iterator.
        // Iterate through each MTTempoSettings until std::function returns true. Stops iterating if std::function returns false.
        void forEachTempo( std::function<bool( MTTempoSettings& )> Func );

        unsigned int getTicksUpto( unsigned int Bar, unsigned int Beat = 0, unsigned int Ticks = 0 );
        unsigned int getTicksUptoSeconds( double seconds );
        double       getSecondsUpto( unsigned int Bar, unsigned int Beat = 0, unsigned int Ticks = 0 );
        double       getSecondsUptoTicks( unsigned int Ticks );

        MTBarNBeats toBnB( unsigned int TimeInTicks );
        MTBarNBeats toBnB( double TimeInMSeconds );
        MTBarNBeats toBnBFromNormalized( double Normalized, double MaxLengthInSeconds );

        unsigned int toTicks( MTTimeBase Time, unsigned int TempoIndex = 0 );
        unsigned int toTicks( const MTBarNBeats& BnB );
        unsigned int toTicksFromNormalized( double Normalized, double MaxLengthInSeconds );

        double toSeconds( const MTBarNBeats& BnB );
        double toSeconds( unsigned int Ticks );

    private:
        void updateTempos();

    private:
        juce::ValueTree    vTree;
        juce::UndoManager* um{ nullptr };

        MTTempoSettings                               defTempo;
        std::vector<std::unique_ptr<MTTempoSettings>> tempos;
    };

} // namespace MT
