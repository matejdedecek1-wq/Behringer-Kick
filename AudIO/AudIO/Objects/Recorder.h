#pragma once

#if !defined( JUCE_VERSION )
    #include "../3rdparty/AudioFile.h"
    #include "VolumeAndPan.h"
#endif

namespace AudIO
{

    class AudioRecorder : public Object
    {
    public:
        AudioRecorder();
        ~AudioRecorder();

        enum Direction
        {
            FORWARD,
            REVERSE,
        };

        enum Mode
        {
            L_MONO,
            R_MONO,
            LR_MONO,
            L_STEREO,
            R_STEREO,
            LR_STEREO,
        };

        std::function<void()>                       onRecorderStarted;
        std::function<void()>                       onRecorderStopped;
        std::function<void()>                       onRecorderPaused;
        std::function<void()>                       onRecorderResumed;
        std::function<void()>                       onRecorderPreviewStarted;
        std::function<void()>                       onRecorderPreviewStopped;
        std::function<void( double TimeInSeconds )> onRecorderLengthChanged;

        void setVolumeAndPan( float Volume, float Pan );
        void setBypass( bool Bypass ) { m_bypass.store( Bypass ); };
        void setMute( bool Mute ) { m_mute.store( Mute ); };
        void setLooping( bool Loop ) { m_loop.store( Loop ); };
        void setOverdub( bool Overdub ) { m_overdub.store( Overdub ); };
        void setMonitor( bool Monitor ) { m_monitor.store( Monitor ); };

        void setPan( float Pan );
        void setVolume( float Gain );
        void setVolumeDB( float GainDB );
        void setThreshold( float Threshold ) { m_threshold = utils::clamp( Threshold, 0.f, 1.f ); }
        void setThresholdDB( float DB );

        void      setDirection( bool Forward ) { dir = Forward ? FORWARD : REVERSE; };
        void      setDirection( Direction Mode ) { dir = Mode; };
        Direction getDirection() const { return dir; };

        void setRecordMode( Mode Mode ) { mode_ = Mode; };
        Mode getRecordMode() const { return mode_; };

        float getVolume() { return vp.getParameter( VolumeAndPan::kParamGain ); };
        float getPan() { return vp.getParameter( VolumeAndPan::kParamPan ); };
        float getThreshold() const { return m_threshold.load(); };
        float getThresholdDB() const { return utils::linearTolog( m_threshold.load() ); };

        bool isBypassed() const { return m_bypass.load(); };
        bool isMuted() const { return m_mute.load(); };
        bool isLooping() const { return m_loop.load(); };
        bool isRecording() const { return m_recording.load(); };
        bool isOverdubbing() const { return m_overdub.load(); };
        bool isMonitorOn() const { return m_monitor.load(); };
        bool isPreviewing() const { return m_preview.load(); };
        bool isForwardDirection() const { return dir == FORWARD; };
        bool isReverse() const { return dir == REVERSE; };

        virtual void reset(); // If overriding this, then must call the this function.
        void         reInitialize();
        void         setLength( double timeInSeconds );

        void start( Mode RecMode = LR_STEREO, Direction Dir = FORWARD, bool FromBegin = true );
        void stop() override;
        void pause();
        void resume();

        void startPreview( double fromStratTimeInSeconds = 0 );
        void stopPreview();

        void  save( const std::string& Path );
        float readProgress();

        const std::vector<std::vector<float>>& getRecorderData();

    protected:
        void prepareToPlay( uint32_t samplerate, uint32_t blocksize ) override;
        void process( Buffer<float>& buffer ) override;

    private:
        void         resetIndex();
        void         advanceIndex();
        unsigned int availableSpace() const;

        std::unique_ptr<AudioFile<float>> m_file;
        std::atomic<bool>                 m_bypass{ false }, m_mute{ false }, m_recording{ false }, m_loop{ false }, m_overdub{ false };
        std::atomic<bool>                 m_monitor{ false }, m_preview{ false };
        double                            recorderLength{ 0 }, progress{ 0 };
        std::atomic<unsigned int>         writeIndex_{ 0 }, readIndex_{ 0 }, maxSamples_{ 0 };
        std::atomic<float>                m_threshold{ 0.2 };
        std::atomic<bool>                 m_thresholdCrossed{ false };

        VolumeAndPan vp;
        Direction    dir{ FORWARD };
        Mode         mode_{ LR_STEREO };
    };

    REGISTER_OBJECT( "AudioRecorder", AudioRecorder );
};
