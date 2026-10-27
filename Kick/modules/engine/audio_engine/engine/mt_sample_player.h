#pragma once
namespace mt{
    class MTSamplePlayer : public juce::PositionableAudioSource,
        public juce::ChangeBroadcaster,
        public juce::ChangeListener{
    public:
        MTSamplePlayer();
        ~MTSamplePlayer();

        void setSource( juce::File source );
        void start();
        void stop();
        bool isPlaying();

        //void setPosition(double p);
        void start( const MTSampleRegion& region, MTSamplePlayMode playMode = MTSamplePlayMode::samplePlayFromStart);

        virtual void setNextReadPosition( juce::int64 newPosition ) override{ pos = newPosition; }

        /** Returns the position from which the next block will be returned.

            @see setNextReadPosition
        */
        virtual juce::int64 getNextReadPosition() const override;

        /** Returns the total length of the stream (in samples). */
        virtual juce::int64 getTotalLength() const override;

        /** Returns true if this source is actually playing in a loop. */
        virtual bool isLooping() const override;

        // audio source functions
        virtual void prepareToPlay( int samplesPerBlockExpected, double sampleRate ) override;

        /** Allows the source to release anything it no longer needs after playback has stopped.

            This will be called when the source is no longer going to have its getNextAudioBlock()
            method called, so it should release any spare memory, etc. that it might have
            allocated during the prepareToPlay() call.

            Note that there's no guarantee that prepareToPlay() will actually have been called before
            releaseResources(), and it may be called more than once in succession, so make sure your
            code is robust and doesn't make any assumptions about when it will be called.

            @see prepareToPlay, getNextAudioBlock
        */
        virtual void releaseResources() override;

        /** Called repeatedly to fetch subsequent blocks of audio data.

            After calling the prepareToPlay() method, this callback will be made each
            time the audio playback hardware (or whatever other destination the audio
            data is going to) needs another block of data.

            It will generally be called on a high-priority system thread, or possibly even
            an interrupt, so be careful not to do too much work here, as that will cause
            audio glitches!

            @see AudioSourceChannelInfo, prepareToPlay, releaseResources
        */
        virtual void getNextAudioBlock( const juce::AudioSourceChannelInfo& bufferToFill ) override;

        double getSampleRate();

    private:
        void changeListenerCallback( ChangeBroadcaster* source );

    private:
        juce::AudioSourcePlayer player;
        juce::AudioTransportSource transportSource;
        juce::AudioDeviceManager& devmgr{ getAudioInterface().getAudioDeviceManager() };
        juce::AudioFormatManager formatManager;

        bool looping = false;
        juce::int64 pos = 0;
        juce::int64 length = 0;
        juce::int64 regionStart = 0;
        juce::int64 regionEnd = 0;
        bool playingBackwords = false;
        int samplesPerBlock = 0;

        juce::File sourceFile;
        std::unique_ptr<juce::AudioFormatReader> reader;
        MTSampleRegion region;
        MTSamplePlayMode playMode = MTSamplePlayMode::samplePlayFromStart;

        int broadcastElapsedSamples = 0;
        int numSamplesElapsed = 0;
    };
}
