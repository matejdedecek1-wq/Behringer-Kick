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

// module include files...

namespace mt
{
    /// @brief
    KickSampler::KickSampler()
        : AudIO::Object( 0, 2, kParamMax )
    {
        setName( "KickSampler" );
    }

    KickSampler::~KickSampler()
    {
    }

    void KickSampler::setAudIOSampler( ISampler* Prog )
    {
        prog.store( Prog );
        if( prog )
            prog.load()->prepareToPlay( _sampleRate, _blockSize );
    }

    void KickSampler::prepareToPlay( uint32_t samplerate, uint32_t blocksize )
    {
        AudIO::Object::prepareToPlay( samplerate, blocksize );
        if( prog )
            prog.load()->prepareToPlay( samplerate, blocksize );
    }

    void KickSampler::process( AudIO::Buffer<float>& buffer )
    {
        buffer.clearOutputs();

        if( prog )
            prog.load()->process( buffer );
    }

    ///
    ///
    ///
    ///

    KickVoiceSampler::KickVoiceSampler( int Polyphony )
        : numPolyphony( Polyphony )
    {
        // Limits check...
        numPolyphony = juce::jlimit( 0, MAX_POLYPHONY_PER_OBJECT, numPolyphony );

        for( auto i = 0; i < numPolyphony; ++i )
        {
            voices[i]       = std::make_unique<SamplerVoice>();
            playingNotes[i] = -1;
        }

        // Pre-reserve memory
        _inserts.resize( MAX_INSERTS_PER_OBJECT );
    }

    KickVoiceSampler ::~KickVoiceSampler()
    {
        stopAllNotes();
    }

    void KickVoiceSampler::muteNote( int Note, bool Mute )
    {
        auto idx = isVoicePlaying( Note );
        if( juce::isPositiveAndBelow( idx, numPolyphony ) )
        {
            voices[idx]->setMute( Mute );
        }
    }

    void KickVoiceSampler::restartNote( int Note )
    {
        auto idx = isVoicePlaying( Note );
        if( juce::isPositiveAndBelow( idx, numPolyphony ) )
        {
            voices[idx]->refresh();
        }
    }

    void KickVoiceSampler::stopNote( int Note )
    {
        auto idx = isVoicePlaying( Note );
        if( juce::isPositiveAndBelow( idx, numPolyphony ) )
        {
            voices[idx]->stop();
        }
    }

    void KickVoiceSampler::stopNote( std::vector<int>& Notes )
    {
        for( auto it : Notes )
            stopNote( it );
    }

    int KickVoiceSampler::isVoicePlaying( int Note )
    {
        if( CommonUtils::isValidMidiNote( Note ) )
        {
            for( auto i = 0; i < numPolyphony; ++i )
            {
                if( playingNotes[i] == Note )
                    return i;
            }
        }
        return -1;
    }

    int KickVoiceSampler::findFreeVoice( int Note )
    {
        for( auto i = 0; i < numPolyphony; ++i )
        {
            if( voices[i]->isFinished )
                return i;
        }
        return -1;
    }

    const std::vector<std::string> KickVoiceSampler::getInserts()
    {
        std::vector<std::string> _n;
        for( auto i = 0; i < MAX_INSERTS_PER_OBJECT; ++i )
            _n.push_back( _inserts[i] ? _inserts[i]->getName() : "" );
        return _n;
    }

    const std::vector<AudIO::ParamInfo>& KickVoiceSampler::getInsertParameters( int Position )
    {
        if( Position < MAX_INSERTS_PER_OBJECT )
        {
            if( _inserts[Position] )
                return _inserts[Position]->getParameters();
        }
        return {};
    }

    bool KickVoiceSampler::addInsert( std::string& Insert, int Position )
    {
        if( Position < MAX_INSERTS_PER_OBJECT )
        {
            removeInsert( Position );
            _inserts[Position] = AudIO::ObjectFactory::Get().Create( Insert );
            return _inserts[Position] ? true : false;
        }
        return false;
    }

    bool KickVoiceSampler::removeInsert( int Position )
    {
        if( Position < MAX_INSERTS_PER_OBJECT )
        {
            _inserts[Position].reset();
            return true;
        }
        return false;
    }

    float KickVoiceSampler::getInsertParameter( int Position, int ParamIndex )
    {
        if( Position < MAX_INSERTS_PER_OBJECT )
        {
            if( _inserts[Position] )
                return _inserts[Position]->getParameter( ParamIndex );
        }
        return 0.0;
    }

    void KickVoiceSampler::setInsertParameter( int Position, int ParamIndex, float ParamValue )
    {
        if( Position < MAX_INSERTS_PER_OBJECT )
        {
            if( _inserts[Position] )
                _inserts[Position]->setParameter( ParamIndex, ParamValue );
        }
    }

    void KickVoiceSampler::process( AudIO::Buffer<float>& buffer )
    {
        buffer.clearOutputs();

        if( bypass_.load() )
            return;

        // Stop all notes...
        if( stopAll_.load() )
        {
            for( auto i = 0; i < numPolyphony; ++i )
                voices[i]->stop();
            stopAll_ = false;
        }
        else
        {
            // Do pre-processing...
            preProcess( buffer );

            for( auto idx = 0; idx < numPolyphony; ++idx )
            {
                if( !voices[idx]->isFinished.load() )
                {
                    voices[idx]->process( buffer, _blockSize );
                    if( voices[idx]->isFinished.load() )
                    {
                        freeVoiceFromPlayback( idx );
                    }
                }
            }

            // Do post-processing...
            postProcess( buffer );

            // Perform inserts...
            for( auto& it : _inserts )
                if( it ) it->process( buffer );
        }
    }

};
