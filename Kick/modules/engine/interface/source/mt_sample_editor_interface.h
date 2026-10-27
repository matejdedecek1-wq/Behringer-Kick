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
#include "mt_audio_interface_types.h"
namespace mt
{
    // Module include files...
    namespace mt
    {
#define sampleEditorBufferSize 16384
    }

    class MTSamplePlayerListener
    {
    public:
        /** Destructor. */
        virtual ~MTSamplePlayerListener() = default;
        // called when playhead position changes
        virtual void positionChanged( juce::int64 pos ) = 0;
        virtual void stopped()                          = 0;
        virtual void sampleModified()                   = 0;
    };

    class MTSampleModifiedListener
    {
    public:
        /** Destructor. */
        virtual ~MTSampleModifiedListener() = default;

        // called when playhead position changes
        virtual void sampleModified() = 0;
    };

    enum MTSampleEditorAction
    {
        seActionDiscard,
        seActionDelete,
        seActionSilence,
        seActionExtract,
        seActionNormalize,
        seActionReverse,
        seActionFadeIn,
        seActionFadeOut,
        seActionPitchShift,
        seActionTimeStretch,
        seActionGainChange,
        seActionCopy,
        seActionBitReduce,
        seActionStereoToMono
    };

    class MTSampleEditorInterface
    {
    public:
        MTSampleEditorInterface();
        ~MTSampleEditorInterface();

        void                   setSource( juce::File source );
        double                 detectBpm();
        double                 getNumberOfBeats();
        bool                   performAction( const MTSampleRegion& region, MTSampleEditorAction action, const std::list<juce::var>& params );
        std::list<juce::int64> splitByThreshold( int thresholdDb, int minTime, juce::int64 startSample, juce::int64 endSample );
        juce::int64            getLengthInSamples();
        double                 getLengthInSec();
        void                   undo();
        void                   redo();
        void                   addListener( MTSamplePlayerListener* listener );
        void                   removeListener( MTSamplePlayerListener* listener );
        //void addListener(MTSampleModifiedListener* listener);
        //void removeListener(MTSampleModifiedListener* listener);
        juce::int64 getNextZeroSnapPos( juce::int64 pos, bool forward );

        //transport
        void play();
        void play( const MTSampleRegion& region, MTSamplePlayMode mode );
        void stop();

        juce::AudioThumbnailBase* getThumbnail();

    private:
        class MTSampleEditorData;
        MTSampleEditorData* pimpl;
    };
}
