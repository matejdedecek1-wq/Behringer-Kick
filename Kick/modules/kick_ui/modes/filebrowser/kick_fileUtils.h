#pragma once
#if defined( LINUX ) || defined( __linux__ )
    #include <unistd.h>
#endif
namespace ui
{
    namespace fileUtils
    {
        juce::String getSdCardMountedPath();
        juce::String getUsbMountedPath();
        bool         getSdCardStatus();
        bool         getUsbStatus();
        juce::int32  calculateFolderSize( juce::File filePath );
        void         setSdCardEjectStatus( bool );
        void         setUsbEjectStatus( bool );
        bool         getSdCardEjectStatus();
        bool         getUsbEjectStatus();
        juce::String getFilePath( juce::String originalFullPath, juce::String replacementPath );
        static bool  isSdCardEject{ false };
        static bool  isUSBEject{ false };
        void         setHighlightLED( int index );
        bool         hasSampleAssignedToPad( int LastPressedPad );

        extern int lastPressedPad;
    }

    class LoadSamplesAndAnimation
        : public juce::Thread
        , public mt::utils::LoadAnimation
    {
    public:
        LoadSamplesAndAnimation();
        ~LoadSamplesAndAnimation();

        void startLoading( const juce::Array<juce::File>& assetsFiles, bool IsMultiple, std::function<void()> onCompleteCallback );
        bool isSampleLoaded() { return sampleLoaded; }

    private:
        void                    run() override;
        void                    addMultipleAssets();
        void                    addAsset();
        bool                    isMultiple{ true };
        bool                    sampleLoaded{ false };
        juce::Array<juce::File> assetsFile;
        juce::String            sampleFileList{ ".wav;bwf;aif;aiff;flac;ogg;mp3;wmv;asf;wm;wma" };
        std::function<void()>   completionCallback;
    };

}
