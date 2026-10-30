// mt::AudioDevice, reconstructed: AudIO's engine driven by a JUCE audio device.
//
// MTAudioManager makes one with ( ins, outs, deviceName ), registers it with
// DawEngine::addAudioDevice and later asks it for getJuceAudioDeviceMgr(). AudIO's
// IAudioDevice defines the contract (start / resume / pause / stop, and a Buffer<float>
// handed to the engine's callback); this fills it in with juce::AudioDeviceManager. The
// engine works in fixed blocks of Buffer::getBlockSize(), so the JUCE callback is cut into
// blocks of that size.
//
// From the community macOS port (github.com/Eight4aWish/0700-AAM, branch macos-build);
// reused unchanged for the Windows build.

#pragma once

namespace mt
{
    class AudioDevice
        : public AudIO::IAudioDevice
        , private juce::AudioIODeviceCallback
    {
    public:
        AudioDevice( uint32_t nInputs, uint32_t nOutputs, const juce::String& deviceName )
            : AudIO::IAudioDevice( nInputs, nOutputs ), numIns( (int)nInputs ), numOuts( (int)nOutputs ), name( deviceName )
        {
        }

        ~AudioDevice() override { stop(); }

        juce::AudioDeviceManager& getJuceAudioDeviceMgr() noexcept { return deviceManager; }

        void start( uint32_t samplerate, uint32_t blocksize ) override
        {
            juce::AudioDeviceManager::AudioDeviceSetup setup;
            deviceManager.initialise( numIns, numOuts, nullptr, true, name, nullptr );
            deviceManager.getAudioDeviceSetup( setup );
            setup.sampleRate = (double)samplerate;
            setup.bufferSize = (int)blocksize;
            deviceManager.setAudioDeviceSetup( setup, true );
            // Opt-in test tooling (KICK_AUDIO_PROBE, KICK_AUDIO_INPUT), see mt_AudioDeviceProbe.h.
            probe     = AudioProbe::createFromEnvironment( (double)samplerate, numOuts );
            testInput = TestInput::createFromEnvironment( (double)samplerate );
            prepareToPlay( samplerate, blocksize );
            deviceManager.addAudioCallback( this );
            setState( AudIO::AudioDeviceState::playing );
        }

        void resume() override { setState( AudIO::AudioDeviceState::playing ); }
        void pause() override { setState( AudIO::AudioDeviceState::paused ); }

        void stop() override
        {
            if( !isInitialized() ) return;
            deviceManager.removeAudioCallback( this );
            deviceManager.closeAudioDevice();
            probe     = nullptr;
            testInput = nullptr;
            teardown();
            setState( AudIO::AudioDeviceState::stopped );
        }

    private:
        void audioDeviceIOCallbackWithContext( const float* const* inputs, int numInputChannels, float* const* outputs,
                                               int numOutputChannels, int numSamples,
                                               const juce::AudioIODeviceCallbackContext& ) override
        {
            for( int ch = 0; ch < numOutputChannels; ++ch )
                if( outputs[ch] ) juce::FloatVectorOperations::clear( outputs[ch], numSamples );
            if( !isPlaying() ) return;

            auto&     buf   = getBuffer();
            const int block = (int)buf.getBlockSize();
            if( block <= 0 ) return;

            for( int pos = 0; pos < numSamples; pos += block )
            {
                const int n = juce::jmin( block, numSamples - pos );
                for( int ch = 0; ch < (int)buf.getNumInputChannels(); ++ch )
                    if( auto in = buf.getInputBuffer( (uint32_t)ch ) )
                    {
                        juce::FloatVectorOperations::clear( in, block );
                        if( ch < numInputChannels && inputs[ch] ) juce::FloatVectorOperations::copy( in, inputs[ch] + pos, n );
                    }
                if( testInput )
                {
                    float* ins[8] = {};
                    const int nIns = (int)juce::jmin<uint32_t>( 8, buf.getNumInputChannels() );
                    for( int ch = 0; ch < nIns; ++ch )
                        ins[ch] = buf.getInputBuffer( (uint32_t)ch );
                    testInput->render( ins, nIns, n );
                }
                buf.clearOutputs();
                process( buf );
                if( probe )
                {
                    float* outs[8] = {};
                    const int nOuts = (int)juce::jmin<uint32_t>( 8, buf.getNumOutputChannels() );
                    for( int ch = 0; ch < nOuts; ++ch )
                        outs[ch] = buf.getOutputBuffer( (uint32_t)ch );
                    probe->push( outs, nOuts, n );
                }
                for( int ch = 0; ch < juce::jmin( numOutputChannels, (int)buf.getNumOutputChannels() ); ++ch )
                    if( auto out = buf.getOutputBuffer( (uint32_t)ch ) )
                        if( outputs[ch] ) juce::FloatVectorOperations::copy( outputs[ch] + pos, out, n );
            }
        }

        void audioDeviceAboutToStart( juce::AudioIODevice* ) override {}
        void audioDeviceStopped() override {}

        juce::AudioDeviceManager deviceManager;
        int                      numIns, numOuts;
        juce::String             name;

        // Opt-in test tooling (KICK_AUDIO_PROBE, KICK_AUDIO_INPUT), see mt_AudioDeviceProbe.h.
        // Both stay null unless the variable is set.
        std::unique_ptr<AudioProbe> probe;
        std::unique_ptr<TestInput>  testInput;
    };
} // namespace mt
