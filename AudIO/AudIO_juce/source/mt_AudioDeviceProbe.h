/*
 * Copyright (c) 2022 MUSIC Tribe
 *
 * This file is part of AudIO.
 *
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

// Opt-in test tooling for the audio device, from the desktop seams (KICK_AUDIO_PROBE,
// KICK_AUDIO_INPUT). The original 0700-AAM release has no AudIO_juce implementation; this
// reconstruction is header-only (see AudIO_juce.h), so both classes are defined inline here.

#pragma once

namespace mt
{
    /// Opt-in measurement tap on the engine's master output, for tests. Enabled by setting
    /// KICK_AUDIO_PROBE=<file.wav> in the environment (relative to the working directory): every
    /// block AudIO renders is written to that WAV (32-bit float, one channel per output), and once
    /// per second of audio a line is logged:
    ///
    ///     AudioProbe: t=12s peak=-6.1dBFS rms=-21.4dBFS
    ///
    /// so a script can assert that sound happened without listening. The audio thread only copies
    /// into a lock-free FIFO; a background thread does the file I/O and the logging. Unset (as on
    /// a unit), no probe is created and the audio callback is unchanged.
    class AudioProbe : private juce::Thread
    {
    public:
        static std::unique_ptr<AudioProbe> createFromEnvironment( double SampleRate, int NumChannels )
        {
            auto path = juce::SystemStats::getEnvironmentVariable( "KICK_AUDIO_PROBE", {} );
            if( path.isEmpty() )
                return nullptr;

            auto file = juce::File::getCurrentWorkingDirectory().getChildFile( path );
            return std::make_unique<AudioProbe>( file, SampleRate, NumChannels );
        }

        AudioProbe( const juce::File& File, double SampleRate, int NumChannels )
            : juce::Thread( "AudioProbe" )
            , file( File )
            , sampleRate( SampleRate )
            , numChannels( juce::jmax( 1, NumChannels ) )
            , fifo( juce::nextPowerOfTwo( (int)SampleRate * 4 ) )
        {
            ring.setSize( numChannels, fifo.getTotalSize() );
            scratch.setSize( numChannels, fifo.getTotalSize() );

            file.deleteFile();
            file.getParentDirectory().createDirectory();
            if( auto stream = file.createOutputStream() )
            {
                juce::WavAudioFormat wav;
                writer.reset( wav.createWriterFor( stream.release(), sampleRate, (unsigned)numChannels, 32, {}, 0 ) );
            }

            if( writer )
            {
                juce::Logger::writeToLog( "AudioProbe: writing master output to " + file.getFullPathName() );
                startThread();
            }
            else
            {
                juce::Logger::writeToLog( "AudioProbe: cannot write " + file.getFullPathName() );
            }
        }

        ~AudioProbe() override
        {
            stopThread( 2000 );
            drain();
            if( framesInSecond > 0 )
                logSecond();
            writer = nullptr; // finalises the WAV header
        }

        /// Audio thread. Never blocks; drops audio if the writer falls behind (logged).
        void push( const float* const* Channels, int NumChannels, int NumSamples ) noexcept
        {
            if( !writer )
                return;

            const auto scope = fifo.write( NumSamples );
            if( scope.blockSize1 + scope.blockSize2 < NumSamples )
                dropped.fetch_add( NumSamples - ( scope.blockSize1 + scope.blockSize2 ) );

            for( int ch = 0; ch < numChannels; ++ch )
            {
                const auto* src = ch < NumChannels ? Channels[ch] : nullptr;
                auto*       dst = ring.getWritePointer( ch );
                if( scope.blockSize1 > 0 )
                {
                    if( src ) juce::FloatVectorOperations::copy( dst + scope.startIndex1, src, scope.blockSize1 );
                    else      juce::FloatVectorOperations::clear( dst + scope.startIndex1, scope.blockSize1 );
                }
                if( scope.blockSize2 > 0 )
                {
                    if( src ) juce::FloatVectorOperations::copy( dst + scope.startIndex2, src + scope.blockSize1, scope.blockSize2 );
                    else      juce::FloatVectorOperations::clear( dst + scope.startIndex2, scope.blockSize2 );
                }
            }
        }

    private:
        void run() override
        {
            while( !threadShouldExit() )
            {
                wait( 50 );
                drain();
            }
        }

        void drain()
        {
            const auto ready = fifo.getNumReady();
            if( ready == 0 || !writer )
                return;

            const auto scope = fifo.read( ready );
            int        done  = 0;
            for( auto [start, size] : { std::pair{ scope.startIndex1, scope.blockSize1 }, std::pair{ scope.startIndex2, scope.blockSize2 } } )
            {
                for( int ch = 0; ch < numChannels; ++ch )
                    scratch.copyFrom( ch, done, ring, ch, start, size );
                done += size;
            }

            // Statistics, split at whole seconds of audio.
            const auto framesPerSecond = (int64_t)sampleRate;
            for( int i = 0; i < done; ++i )
            {
                for( int ch = 0; ch < numChannels; ++ch )
                {
                    const auto s = scratch.getSample( ch, i );
                    peak         = juce::jmax( peak, std::abs( s ) );
                    sumSquares += (double)s * s;
                }
                if( ++framesInSecond == framesPerSecond )
                    logSecond();
            }

            writer->writeFromAudioSampleBuffer( scratch, 0, done );
            writer->flush(); // keeps the header valid even if the app is killed

            if( auto d = dropped.exchange( 0 ) )
                juce::Logger::writeToLog( "AudioProbe: dropped " + juce::String( d ) + " frames" );
        }

        void logSecond()
        {
            const auto rms = std::sqrt( sumSquares / (double)juce::jmax<int64_t>( 1, framesInSecond * numChannels ) );
            juce::Logger::writeToLog( "AudioProbe: t=" + juce::String( ++secondsLogged ) + "s peak=" + toDb( peak ) + " rms=" + toDb( rms ) );
            framesInSecond = 0;
            peak           = 0.f;
            sumSquares     = 0.0;
        }

        static juce::String toDb( double Linear )
        {
            return juce::String( juce::Decibels::gainToDecibels( Linear, -120.0 ), 1 ) + "dBFS";
        }

        juce::File                               file;
        const double                             sampleRate;
        const int                                numChannels;
        std::unique_ptr<juce::AudioFormatWriter> writer;
        juce::AbstractFifo                       fifo;
        juce::AudioBuffer<float>                 ring;
        juce::AudioBuffer<float>                 scratch;
        std::atomic<int>                         dropped{ 0 };

        // Per-second statistics (background thread only).
        int64_t framesInSecond{ 0 };
        int64_t secondsLogged{ 0 };
        float   peak{ 0.f };
        double  sumSquares{ 0.0 };

        JUCE_DECLARE_NON_COPYABLE( AudioProbe )
    };

    /// Opt-in replacement for the device input, for tests. Enabled by setting KICK_AUDIO_INPUT
    /// to one of:
    ///
    ///     silence       digital silence (no microphone, no feedback through the speakers)
    ///     device        the real device input, no replacement (the same as unset)
    ///     tone          1 kHz sine at -12 dBFS, continuous
    ///     tone:<hz>     sine at that frequency, -12 dBFS
    ///     pulse         1 kHz bursts of 50 ms every 500 ms (120 BPM quarter notes), -6 dBFS
    ///     pulse:<ms>    the same bursts every <ms> milliseconds (e.g. pulse:700, a period that
    ///                   is not a divisor of the bar, so a loop's repeat is visible in the output)
    ///     <file>        a WAV/AIFF/FLAC file, looped, resampled to the engine rate
    ///
    /// The source replaces whatever the audio device delivers on every input channel, so the
    /// sampler and looper can be tested without a microphone or permission. Unset (as on a unit),
    /// the device input is used and nothing is created.
    class TestInput
    {
    public:
        static std::unique_ptr<TestInput> createFromEnvironment( double SampleRate )
        {
            auto spec = juce::SystemStats::getEnvironmentVariable( "KICK_AUDIO_INPUT", {} ).trim();
            if( spec.isEmpty() )
                return nullptr;
            if( spec == "device" ) // explicit: the real device input, as when unset
            {
                juce::Logger::writeToLog( "TestInput: using the device input" );
                return nullptr;
            }

            std::unique_ptr<TestInput> in( new TestInput() );
            in->sampleRate = SampleRate;

            if( spec == "tone" || spec.startsWith( "tone:" ) )
            {
                in->kind = Kind::tone;
                if( spec.startsWith( "tone:" ) )
                    in->frequency = juce::jlimit( 1.0, SampleRate * 0.45, spec.fromFirstOccurrenceOf( ":", false, false ).getDoubleValue() );
                juce::Logger::writeToLog( "TestInput: " + juce::String( in->frequency ) + " Hz sine at -12 dBFS replaces the device input" );
                return in;
            }

            if( spec == "silence" )
            {
                in->kind = Kind::silence;
                juce::Logger::writeToLog( "TestInput: silence replaces the device input" );
                return in;
            }

            if( spec == "pulse" || spec.startsWith( "pulse:" ) )
            {
                in->kind = Kind::pulse;
                if( spec.startsWith( "pulse:" ) )
                    in->pulsePeriod = juce::jlimit( 0.06, 60.0, spec.fromFirstOccurrenceOf( ":", false, false ).getDoubleValue() / 1000.0 );
                juce::Logger::writeToLog( "TestInput: 1 kHz 50 ms pulse every " + juce::String( juce::roundToInt( in->pulsePeriod * 1000.0 ) ) + " ms replaces the device input" );
                return in;
            }

            auto file = juce::File::getCurrentWorkingDirectory().getChildFile( spec );
            juce::AudioFormatManager formats;
            formats.registerBasicFormats();
            std::unique_ptr<juce::AudioFormatReader> reader( formats.createReaderFor( file ) );
            if( !reader || reader->lengthInSamples <= 0 )
            {
                juce::Logger::writeToLog( "TestInput: cannot read " + file.getFullPathName() + ", input left alone" );
                return nullptr;
            }

            const int                srcLen = (int)reader->lengthInSamples;
            const int                numCh  = juce::jmax( 1, (int)reader->numChannels );
            juce::AudioBuffer<float> src( numCh, srcLen );
            reader->read( &src, 0, srcLen, 0, true, true );

            const double ratio  = reader->sampleRate / SampleRate;
            const int    dstLen = juce::jmax( 1, (int)std::floor( srcLen / ratio ) );
            in->data.setSize( numCh, dstLen );
            for( int ch = 0; ch < numCh; ++ch )
            {
                juce::LagrangeInterpolator interp;
                interp.process( ratio, src.getReadPointer( ch ), in->data.getWritePointer( ch ), dstLen );
            }

            in->kind = Kind::file;
            juce::Logger::writeToLog( "TestInput: looping " + file.getFullPathName() + " (" + juce::String( dstLen / SampleRate, 2 ) + " s) into the device input" );
            return in;
        }

        /// Audio thread: fill NumChannels x NumSamples.
        void render( float* const* Channels, int NumChannels, int NumSamples ) noexcept
        {
            constexpr float toneGain  = 0.2511886f; // -12 dBFS
            constexpr float pulseGain = 0.5011872f; // -6 dBFS

            for( int i = 0; i < NumSamples; ++i )
            {
                float l = 0.f, r = 0.f;
                switch( kind )
                {
                    case Kind::silence:
                        break;

                    case Kind::tone:
                        l = r = toneGain * (float)std::sin( phase );
                        phase += juce::MathConstants<double>::twoPi * frequency / sampleRate;
                        if( phase > juce::MathConstants<double>::twoPi )
                            phase -= juce::MathConstants<double>::twoPi;
                        break;

                    case Kind::pulse:
                    {
                        const auto period = (int64_t)std::llround( sampleRate * pulsePeriod );
                        const auto inBeat = position % period;
                        if( inBeat < (int64_t)( sampleRate * 0.05 ) )
                            l = r = pulseGain * (float)std::sin( juce::MathConstants<double>::twoPi * 1000.0 * (double)inBeat / sampleRate );
                        break;
                    }

                    case Kind::file:
                    {
                        const auto idx = (int)( position % data.getNumSamples() );
                        l              = data.getSample( 0, idx );
                        r              = data.getSample( juce::jmin( 1, data.getNumChannels() - 1 ), idx );
                        break;
                    }
                }
                ++position;

                for( int ch = 0; ch < NumChannels; ++ch )
                    if( auto* dst = Channels[ch] )
                        dst[i] = ( ch & 1 ) ? r : l;
            }
        }

    private:
        enum class Kind { silence, tone, pulse, file };
        TestInput() = default;

        Kind                     kind{ Kind::tone };
        double                   sampleRate{ 48000.0 };
        double                   frequency{ 1000.0 };
        double                   pulsePeriod{ 0.5 }; // seconds
        double                   phase{ 0.0 };
        int64_t                  position{ 0 };
        juce::AudioBuffer<float> data; // file contents at sampleRate
    };
} // namespace mt
