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

#include "mt_audio_iprogram.h"

namespace mt
{
    class MTAudioProgram : public MTIProgram
        , public MTIAudioProgramData
    {
    public:
        explicit MTAudioProgram( AudIO::DawEngine& Engine, juce::ValueTree& VT, juce::UndoManager* UM = nullptr );
        virtual ~MTAudioProgram();

        void forAllAudio( const std::function<void( const juce::ValueTree& audioVT )>& func );

        void setSelectedAudioSampleLevel( int value );
        void setSelectedAudioSampleMute( bool isMute );
        void setSelectedAudioSampleReverse( bool isReverse );
        void setSelectedAudioSampleWarp( bool isWarp );
        void setSelectedAudioSampleBPM( int value );

        juce::ValueTree&              getAudioSampleList() { return sampleTree; }
        juce::Array<juce::ValueTree>& getSelectedAudioSampleList() { return selectedList; }

        void setLoopEnd( int max, int Start );
        void setLoopStart( int min, int Start );

        void            selectAllSamples( bool Select ) { selectedList.clearQuick(); }
        void            setValueForParam( const juce::String&, int Start, double Value );
        juce::ValueTree getParamTree( const juce::String& Param );
        void            eraseSelectedSample();

        void addSample( const juce::String& Source, int Start, int Length, int Slice );
        void removeSample( int Start );
        void forAllAudio( const std::function<void( MTIAudio& audio )>& func );

        void selectAudioSample( int Start );
        void setSelectAudioSample( int Start, bool IsSelect );
        void setStartForSample( int, int );
        void setLoopPointsForSample( int, double, double );
        void setSampleLength( int, int );

        juce::ValueTree getClipTree( int Start );
        MTIAudio*       getAudioClip( int Start );
        ASoundSlice*    getAudioSlice( int Start );
        MTIAudio*       getAudioSample( int Start );

        juce::Array<std::unique_ptr<MTIAudio>> getAudioClipsData();

        //Parameter API's

        int editSelectedSampleStart( int, int, bool );
        int editSelectedSampleEnd( int, int, bool );

    protected:
    private:
        std::unique_ptr<MTIAudio> getOrCreateAudioSample( int Start );

    private:
        void                                   refresh();
        MTAudioInterface&                      ae{ getAudioInterface() };
        juce::Array<juce::ValueTree>           selectedList;
        std::vector<std::unique_ptr<MTIAudio>> selectedLists;
        juce::ValueTree                        sampleTree;
        bool                                   editStart{ false };

    private:
        AudIO::DawEngine& engine;
        AudioProgram*     prog;
    };
} // namespace MT
