#pragma once

#include "../Includes/Design.h"
#include "../Includes/DeviceManager.h"

#include "DawBus.h"

#if defined( AUDIO_OBJECT_AUDIO_FILE_RECORDER ) && ( AUDIO_OBJECT_AUDIO_FILE_RECORDER > 0 )
    #include "Objects/DawRecorder.h"

namespace AudIO
{
    struct DawRecorderListener
    {
        std::function<void()> onRecorderStarted;
        std::function<void()> onRecorderStopped;
        std::function<void()> onRecorderPaused;
        std::function<void()> onRecorderResumed;
        std::function<void()> onRecorderPreviewStarted;
        std::function<void()> onRecorderPreviewStopped;

        std::function<void( double TimeInSeconds )> onRecorderLengthChanged;
    };
};
#endif

namespace AudIO
{
    class DawSequence;
    class DawTrack;

    class DawEngine : public Design
    {
    public:
        DawEngine( unsigned int ninputs, unsigned int noutputs, unsigned int nsendreturns = 0, unsigned int nsubmix = 0, unsigned int naux = 0 );
        ~DawEngine();

        // Memory...
        float getFeeMemory() const; // In Percentage.
        float getUsedMemory() const; // In Percentage.
        int   getTotalMemory() const; // In no of audio pools.

        // Bus Manager
        inline DawBusManager* getBusManager() { return _busMgr.get(); }

        // Effects...
        const std::map<const std::string, ObjectInfo>& getAudioEffects();

        // Audio Design
        void reset();
        bool addAudioDevice( const std::string& name, IAudioDevice* device, bool master );

        void startAudioDevice( unsigned int SampleRate, unsigned int BlockSize ) const;
        void pauseAudioDevice();
        void stopAudioDevice();

#if defined( AUDIO_DAW_EXTENSIONS ) && ( AUDIO_DAW_EXTENSIONS > 0 )
        // Sequence API's...
        DawSequence* createSequence( const std::string& Name );
        int          deleteSequence( const std::string& Name );
        int          deleteSequence( uint32_t Index );

        // Sequence api's
        bool         isSeqExists( const std::string& Name );
        int          getSequenceIndex( const std::string& Name ) const;
        DawSequence* getSequence( const std::string& Name ) const;
        DawSequence* getSequence( uint32_t Index ) const;

        const std::vector<std::unique_ptr<DawSequence>>& getSequences() const;
        std::vector<std::string>                         getSequenceNames() const;

        // Sequence related functions...
        void playSequence( const std::string& Name );
        void playSequence( uint32_t Index );
        void playSequence( DawSequence* Seq );
        void stopSequence();
        void pauseSequence();
        void resumeSequence();

        // Track api's
        bool                                          isTrackExists( uint32_t Sequence, const std::string& Name );
        int                                           getTrackIndex( uint32_t Sequence, const std::string& Name ) const;
        DawTrack*                                     getTrack( uint32_t Sequence, const std::string& Name ) const;
        DawTrack*                                     getTrack( uint32_t Sequence, uint32_t Index ) const;
        const std::vector<std::unique_ptr<DawTrack>>& getTracks( uint32_t Sequence ) const;
        std::vector<std::string>                      getTrackNames( uint32_t Sequence ) const;

        // Iterators...
        void forAllSequences( std::function<void( DawSequence* S )>& func );
        void forAllTracksInSequence( const uint32_t Seq, std::function<void( DawTrack* S )>& func );
#endif

#if defined( AUDIO_OBJECT_AUDIO_FILE_PLAYBACK ) && ( AUDIO_OBJECT_AUDIO_FILE_PLAYBACK > 0 )
        // Temporary Editors...
        unsigned int createAudioPreview( const std::string& File );
        void         deleteAudioPreview( unsigned int ID );
        void         deleteAllAudioPreviews();
        void         playAudioPreview( unsigned int ID, bool FromBeginning = true );
        void         stopAudioPreview( unsigned int ID );
        void         setAudioPreviewGainAndPan( unsigned int ID, float Gain, float Pan );
        void         setAudioPreviewMute( unsigned int ID, bool Mute );
        void         setAudioPreviewBypass( unsigned int ID, bool Bypass );
        void         setAudioPreviewLoop( unsigned int ID, bool Loop );

        const MeterData& getAudioPreviewMeter( unsigned int ID ) const;
#endif

#if defined( AUDIO_OBJECT_AUDIO_FILE_RECORDER ) && ( AUDIO_OBJECT_AUDIO_FILE_RECORDER > 0 )
        // Recorder interface API's
        void initializeRecorder();
        void setEnableRecorder( bool Enable );
        void setupRecorder( double timeInSeconds, float Gain, float Pan = 0.f, bool Looping = false, bool Overdub = false );
        void setMonitorOn( bool Monitor );
        void setRecorderLoop( bool Looping );
        void setRecorderOverdub( bool Overdub );
        void setRecorderDirection( bool ForwardDirection );
        void setRecorderPan( float Pan );
        void setRecorderGain( float Gain );
        void setRecorderGainDB( float GainDB );
        void setRecroderThreshold( float Gain );
        void setRecroderThresholdDB( float DB );

        void startRecorder( int Mode, bool ForwardDirection = true, bool FromBeginning = true );
        void stopRecorder();
        void saveRecording( const std::string& FilePath );

        float getRecorderGain() const;
        float getRecorderProgress() const;
        float getRecorderThreshold() const;
        float getRecorderThresholdDB() const;

        bool isRecorderEnabled() const;
        bool isRecorderMonitorOn() const;
        bool isRecorderLooping() const;
        bool isRecorderOverdubbing() const;
        bool isRecorderInForwardDirection() const;
        bool isRecorderRunning() const;
        bool isRecorderPreviewing() const;

        const MeterData& getRecorderInputMeter();
        const MeterData& getRecorderOutputMeter();

        void startRecorderPreview( double fromStartTimeInSeconds );
        void stopRecorderPreview();
        void setRecorderListeners( DawRecorderListener& Listener );

        const std::vector<std::vector<float>>& getRecorderChannelsData();
#endif
        // Developer/Debug interfaces...
        void dumpAllMeters();
        void readMeters( const std::function<void( const std::string& meterName, MeterData& data )>& MeterCallback );

    private:
        void addDefaultMeters();

    private:
        std::atomic<bool>              _isInitialized{ false };
        std::unique_ptr<DawBusManager> _busMgr;

        DeviceManager _devmgr;
        uint32_t      mainOut{ 0 };
        uint32_t      inputChain{ 0 };
        uint32_t      recorderId{ 0 };

#if defined( AUDIO_DAW_EXTENSIONS ) && ( AUDIO_DAW_EXTENSIONS > 0 )
        std::vector<std::unique_ptr<DawSequence>> _sequences;
        DawSequence*                              playingSequence{ nullptr };
#endif

#if defined( AUDIO_OBJECT_AUDIO_FILE_PLAYBACK ) && ( AUDIO_OBJECT_AUDIO_FILE_PLAYBACK > 0 )
        std::vector<unsigned int> _AudioPreviews;
#endif

#if defined( AUDIO_OBJECT_AUDIO_FILE_RECORDER ) && ( AUDIO_OBJECT_AUDIO_FILE_RECORDER > 0 )
        DRecorder* recorder{ nullptr };
#endif
    };
};
