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
    DrumSampler::DrumSampler( MTIProgram* PDrum )
        : KickVoiceSampler( MAX_POLYPHONY_PER_OBJECT )
        , pDrum( dynamic_cast<MTDrumProgram*>( PDrum ) )
    {
        setBypass( ( pDrum == nullptr ) );
    }

    DrumSampler::~DrumSampler()
    {
        setBypass( true );
    }

    void DrumSampler::setProgram( MTIProgram* PDrum )
    {
        pDrum = dynamic_cast<MTDrumProgram*>( PDrum );

        stopAllNotes();
        setBypass( ( pDrum == nullptr ) );
    }

    void DrumSampler::addNote( int Note, int Velocity )
    {
        if( !pDrum ) return;
        if( !CommonUtils::isValidMidiNote( Note ) ) return;
        if( !pDrum->isSourceAssigned( Note ) ) return;

        //DBG( "AA" << Note << " " << Velocity );
        auto vt = pDrum->getSample( Note );
        if( vt.isValid() )
        {
            // No polyphony on the same note number...
            auto idx   = isVoicePlaying( Note );
            bool reset = ( 0 <= idx );

            // If, `Note` is not playing already, find a free voice.
            if( !reset )
                idx = findFreeVoice( Note );

            if( juce::isPositiveAndBelow( idx, MAX_POLYPHONY_PER_OBJECT ) )
            {
                // Stop clips in same mute group.
                auto notes = pDrum->getNotesInSameMuteGroup( Note );
                for( auto it : notes )
                    stopNote( it );

                int _semi = pDrum->getSemi();
                int _fine = pDrum->getFine();

                const auto srcValue = midiRange.convertTo0to1( pDrum->getLevel() );
                const auto dbValue  = dbRange.convertFrom0to1( srcValue );
                double     gain     = AudIO::utils::logToLinear( dbValue );

                double _gain = CommonUtils::midiToGain( pDrum->getLevel() ); // ((float)pDrum->getLevel() / 127.f);
                double _pan  = pDrum->getPan();

                auto noteData = pDrum->getSampleForNote( Note );

                // Start adding this note
                std::vector<AFile::Ptr> sounds;
                std::vector<int>        validLayers;
                for( auto i = 0; i < MAX_LAYERS; ++i )
                {
                    auto vPtr = AudioFileManager::Instance().GetOrLoad( noteData.layer[i].source );
                    sounds.push_back( vPtr );
                    if( vPtr )
                    {
                        validLayers.push_back( i );
                    }
                } // for

                switch( noteData.layerPlay )
                {
                    break;
                    case LayerPlayType::RANDOM:
                        noteData.idx = AudIO::utils::random<int>( 0, ( (int)validLayers.size() - 1 ) );
                        break;
                    case LayerPlayType::CYCLE:
                    {
                        auto loc = std::find( begin( validLayers ), end( validLayers ), noteData.idx );
                        if( ++loc != validLayers.end() ) noteData.idx = *loc;
                        else noteData.idx = validLayers[0];
                    }
                    break;
                    case LayerPlayType::VELOCITY:
                    {
                        noteData.idx = -1;
                        for( auto i = 0; i < validLayers.size(); ++i )
                        {
                            auto l = validLayers[i];
                            if( noteData.layer[l].velocityStart <= Velocity && Velocity < noteData.layer[l].velocityEnd )
                            {
                                noteData.idx = l;
                                break;
                            }
                        }
                    }
                }; // switch

                if( ( -1 < noteData.idx && noteData.idx < MAX_LAYERS ) && ( sounds[noteData.idx] ) )
                {
                    // Enqueue Note for playback
                    auto                  voice           = getVoiceAtIndex( idx );
                    SamplerVoice::Params* VoiceParameters = &voice->p;

                    auto vel = CommonUtils::midiToGain( Velocity );

                    _gain                 = ( noteData.gain * noteData.layer[noteData.idx].gain );
                    _gain                 = VELOCITY == noteData.layerPlay ? ( _gain ) : ( _gain * vel );
                    VoiceParameters->gain = _gain;

                    _pan                 = ( noteData.pan + noteData.layer[noteData.idx].pan );
                    _pan                 = juce::jlimit( -1.0, 1.0, _pan );
                    VoiceParameters->pan = _pan;

                    VoiceParameters->voice           = sounds[noteData.idx];
                    VoiceParameters->midiNote        = noteData.keyNote;
                    VoiceParameters->rootNote        = noteData.keyNote;
                    VoiceParameters->startSample     = noteData.layer[noteData.idx].offset;
                    VoiceParameters->lengthInSamples = sounds[noteData.idx]->getNumSamplesPerChannel();
                    VoiceParameters->isMute          = noteData.isMuted;
                    VoiceParameters->sampleRate      = (float)_sampleRate;
                    VoiceParameters->lfo             = noteData.lfo;
                    VoiceParameters->envelope        = noteData.env;

                    _semi += noteData.semi;
                    _fine += noteData.fine;
                    VoiceParameters->pitchFactor = calculatePitchFactor( _semi, _fine );

                    // Queue this voice for processing...
                    addVoiceForPlayback( idx, Note );
                }

                // Simultaneous plays...
                for( auto i = 0; i < MAX_SIMUL_ACTIONS; ++i )
                {
                    if( juce::isPositiveAndBelow( noteData.sPlay[i], 128 ) )
                        addNote( noteData.sPlay[i], Velocity );
                    if( juce::isPositiveAndBelow( noteData.sMute[i], 128 ) )
                        muteNote( noteData.sMute[i], true );
                }
            }
            else
            {
                // Not enough voices available...
            }
        }
    }

    void DrumSampler::postProcess( AudIO::Buffer<float>& buffer )
    {
        if( pDrum )
        {
            // Apply overall pitch transpose
            //int _semi = pDrum->getSemi();
            //int _fine = pDrum->getFine();

            // Apply overall Gain and Pan
            double _gain = CommonUtils::midiToGain( pDrum->getLevel() );
            double _pan  = pDrum->getPan();
            buffer.applyGainAndPan( _gain, _pan );
        }
    }
};
