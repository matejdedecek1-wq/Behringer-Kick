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
#define MAX_POLYPHONY_PER_OBJECT ( 16 )
#define MAX_INSERTS_PER_OBJECT   ( 4 )

    class MTIProgram;
    struct SamplerVoice;

    class ISampler
    {
    public:
        virtual void setProgram( MTIProgram* Prog ) = 0;

        virtual void addNote( int Note, int Velocity )   = 0;
        virtual void muteNote( int Note, bool Mute )     = 0;
        virtual bool isPlaying( int Note )               = 0;
        virtual void stopNote( int Note )                = 0;
        virtual void stopNote( std::vector<int>& Notes ) = 0;
        virtual void stopAllNotes()                      = 0;

        virtual void process( AudIO::Buffer<float>& buffer ) = 0;
        virtual void prepareToPlay( uint32_t samplerate, uint32_t blocksize )
        {
            _sampleRate = samplerate;
            _blockSize  = blocksize;
        }

    protected:
        uint32_t _sampleRate, _blockSize;
    };

    class KickSampler : public AudIO::Object
    {
    public:
        enum
        {
            kParamMax,
        };

        KickSampler();
        virtual ~KickSampler();

        void setAudIOSampler( ISampler* Prog );

    protected:
        void prepareToPlay( uint32_t samplerate, uint32_t blocksize ) override;
        void process( AudIO::Buffer<float>& buffer ) override;

        std::atomic<ISampler*> prog{ nullptr };
    };

    class KickVoiceSampler : public ISampler
    {
    public:
        KickVoiceSampler( int Polyphony );
        ~KickVoiceSampler();

        inline void setBypass( bool Bypass ) { bypass_ = Bypass; }

        void muteNote( int Note, bool Mute );

        void restartNote( int Note );

        void stopNote( int Note );
        void stopNote( std::vector<int>& Notes );

        inline void stopAllNotes() { stopAll_ = true; }
        inline bool isPlaying( int Note ) { return 0 <= isVoicePlaying( Note ); };

        virtual void preProcess( AudIO::Buffer<float>& buffer ){};
        virtual void postProcess( AudIO::Buffer<float>& buffer ){};

        const std::vector<std::string>       getInserts();
        const std::vector<AudIO::ParamInfo>& getInsertParameters( int Position );

        bool addInsert( std::string& Insert, int Position );
        bool removeInsert( int Position );

        float getInsertParameter( int Position, int ParamIndex );
        void  setInsertParameter( int Position, int ParamIndex, float ParamValue );

    protected:
        int isVoicePlaying( int Note );
        int findFreeVoice( int Note );

        inline void addVoiceForPlayback( int Idx, int Note )
        {
            if( Idx < numPolyphony )
            {
                voices[Idx]->refresh();
                playingNotes[Idx] = Note;
            }
        }

        inline void freeVoiceFromPlayback( int Idx )
        {
            if( Idx < numPolyphony )
            {
                voices[Idx]->isFinished = true;
                playingNotes[Idx]       = -1;
            }
        }

        inline SamplerVoice* getVoiceAtIndex( int Idx )
        {
            if( Idx < numPolyphony )
                return voices[Idx].get();
            return nullptr;
        }

    private:
        void process( AudIO::Buffer<float>& buffer ) override;

        std::unique_ptr<SamplerVoice> voices[MAX_POLYPHONY_PER_OBJECT];

        int               playingNotes[MAX_POLYPHONY_PER_OBJECT], numPolyphony;
        std::atomic<bool> stopAll_{ false }, bypass_{ true };

        std::vector<std::unique_ptr<AudIO::Object>> _inserts;
    };

    REGISTER_OBJECT( "AudIOKickSampler", KickSampler );
};
