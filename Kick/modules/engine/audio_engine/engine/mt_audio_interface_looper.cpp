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

namespace mt
{
    ///
    /// Looper interface API's
    ///

    /// Setters...
    void MTLooperInterface::setLooperEnable( bool Enable ) noexcept { manager->getLooper()->setEnable( Enable ); }
    void MTLooperInterface::setLooperLength( double timeInSeconds ) noexcept { manager->getLooper()->setLength( timeInSeconds ); }
    void MTLooperInterface::setLooperMonitorOn( bool Monitor ) noexcept { manager->getLooper()->setMonitorOn( Monitor ); }

    void MTLooperInterface::setLooperOverdub( bool Overdub ) noexcept { manager->getLooper()->setOverdub( Overdub ); }
    void MTLooperInterface::setLooperRecord( bool Record ) noexcept { manager->getLooper()->setRecord( Record ); }
    void MTLooperInterface::setLooperPlay() noexcept { manager->getLooper()->setPlay(); }

    void MTLooperInterface::setLooperLooping( bool Looping ) noexcept { manager->getLooper()->setLooping( Looping ); }
    void MTLooperInterface::setLooperMonitorGain( float Gain ) noexcept { manager->getLooper()->setMonitorGain( Gain ); }
    void MTLooperInterface::setLooperPan( float Pan ) noexcept { manager->getLooper()->setPan( Pan ); }
    void MTLooperInterface::setLooperGain( float Gain ) noexcept { manager->getLooper()->setGain( Gain ); }
    void MTLooperInterface::setLooperGainDB( float GainDB ) noexcept { manager->getLooper()->setGainDB( GainDB ); }
    void MTLooperInterface::setLooperThreshold( float Gain ) noexcept { manager->getLooper()->setThreshold( Gain ); }
    void MTLooperInterface::setLooperThresholdDB( float GainDB ) noexcept { manager->getLooper()->setThresholdDB( GainDB ); }
    void MTLooperInterface::setLooperDirection( Recorder::Direction Direction ) noexcept { manager->getLooper()->setDirection( Direction ); }
    void MTLooperInterface::setLooperRecordingMode( Recorder::Mode Mode ) noexcept { manager->getLooper()->setRecordingMode( Mode ); }

    /// Getters...
    float          MTLooperInterface::getLooperSampleRate() const noexcept { return manager->getLooper()->getSampleRate(); }
    float          MTLooperInterface::getLooperLength() const noexcept { return manager->getLooper()->getLength(); }
    float          MTLooperInterface::getLooperMonitorGain() const noexcept { return manager->getLooper()->getMonitorGain(); }
    float          MTLooperInterface::getLooperPan() const noexcept { return manager->getLooper()->getPan(); }
    float          MTLooperInterface::getLooperGain() const noexcept { return manager->getLooper()->getGain(); }
    float          MTLooperInterface::getLooperThreshold() const noexcept { return manager->getLooper()->getThreshold(); }
    float          MTLooperInterface::getLooperThresholdDB() const noexcept { return manager->getLooper()->getThresholdDB(); }
    Recorder::Mode MTLooperInterface::getLooperRecordingMode() const noexcept { return manager->getLooper()->getRecordingMode(); }

    bool MTLooperInterface::isLooperEnabled() const noexcept { return manager->getLooper()->isEnabled(); }
    bool MTLooperInterface::isLooperEmpty() const noexcept { return manager->getLooper()->isEmpty(); }
    bool MTLooperInterface::isLooperMonitorOn() const noexcept { return manager->getLooper()->isMonitorOn(); }
    bool MTLooperInterface::isLooperLooping() const noexcept { return manager->getLooper()->isLooping(); }
    bool MTLooperInterface::isLooperOverdubbing() const noexcept { return manager->getLooper()->isOverdubbing(); }
    bool MTLooperInterface::isLooperRecording() const noexcept { return manager->getLooper()->isRecording(); }
    bool MTLooperInterface::isLooperPlay() const noexcept { return manager->getLooper()->isPreviewing(); }
    bool MTLooperInterface::isLooperForward() const noexcept { return manager->getLooper()->isForward(); }
    bool MTLooperInterface::isLooperReverse() const noexcept { return manager->getLooper()->isReverse(); }
    bool MTLooperInterface::isLooperRunning() const noexcept { return manager->getLooper()->isRunning(); }

    MTMeterData MTLooperInterface::getLooperInputMeter() noexcept { return manager->getLooper()->readInputMeter(); }
    MTMeterData MTLooperInterface::getLooperOutputMeter() noexcept { return manager->getLooper()->readOutputMeter(); }

    juce::AudioSampleBuffer& MTLooperInterface::getLooperSampleBuffer() const noexcept { return manager->getLooper()->getAudioBuffer(); }
    MTIInsertsInterface*     MTLooperInterface::getLooperInsertsInterface() { return manager->getLooper()->getInsertsInterface(); }

    /// Playback controls...
    void    MTLooperInterface::looperStart( bool FromBeginning, bool SkipThresholdDetection ) noexcept { manager->getLooper()->start( FromBeginning, SkipThresholdDetection ); }
    void    MTLooperInterface::looperPlayback( double timeInSeconds ) noexcept { manager->getLooper()->play( timeInSeconds ); }
    void    MTLooperInterface::looperStop() noexcept { manager->getLooper()->stop(); }
    void    MTLooperInterface::looperClear() noexcept { manager->getLooper()->clear(); }
    void    MTLooperInterface::looperUndo() noexcept { manager->getLooper()->undo(); }
    void    MTLooperInterface::looperRedo() noexcept { manager->getLooper()->redo(); }
    void    MTLooperInterface::looperSave( const juce::String& FilePath ) noexcept { manager->getLooper()->save( FilePath ); }
    double& MTLooperInterface::getLooperProgress() noexcept { return manager->getLooper()->getProgress(); }

}
