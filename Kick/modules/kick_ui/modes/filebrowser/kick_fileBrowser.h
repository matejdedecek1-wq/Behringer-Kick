#pragma once
#include "kick_ui/recent_project/kick_RecentProject.h"
constexpr int   numButtonWidth = 44;
constexpr float cornerSizeX    = 6.0f;
constexpr float cornerSizeY    = 6.0f;
constexpr float lineThickness  = 1.0f;

//Horizontal Line
constexpr int lineX      = 0;
constexpr int lineY      = 54;
constexpr int lineWidth  = 342;
constexpr int lineHeight = 1;

constexpr int headerLabelFontHeight = 17;

constexpr int leftTableRowHeight  = 53;
constexpr int rightTableRowHeight = 53;

constexpr int   labelLeftMargin = 10;
constexpr float labelFontHeight = 14.0f;

constexpr int fileWidth  = 39;
constexpr int fileHeight = 46;

constexpr int directoryWidth  = 54;
constexpr int directoryHeight = 40;

constexpr int textHeight = 22;

#define DEMOSROW 2

namespace ui
{
    class AuditionComponent : public juce::Component
    {
    public:
        AuditionComponent();

        std::function<void( bool )>   onMuteChanged = []( bool ) {};
        std::function<void( double )> onGainChanged = []( double ) {};

        inline bool   isMuted() const noexcept { return volume.getToggleState(); }
        inline double getVolume() const noexcept { return slider.getNormalized(); }
        inline void   setVolume( double Volume ) { slider.setNormalized( Volume ); }

    private:
        mt::KickSettings& settings{ mt::getSettings() };
        void              resized() override;
        void              paint( juce::Graphics& ) override;

    private:
        ui::KickSlider  slider{ "SLIDER", juce::Slider::LinearBarVertical, juce::Slider::TextBoxLeft };
        KickImageButton volume{ "Volume", "Volume_BrowserActive", "Button_VolumeMute" };
    };

    enum filterSystem
    {
        Null = -1,
        Projects,
        Samples,
        All
    };
    class FileBrowserLeftPanel : public juce::Component
    {
    public:
        KickImageButton                places{ "Places", "Places" };
        KickImageButton                content{ "Content", "Content" };
        KickImageButton                expansion{ "Expansion", "Expansion" };
        KickTableViewProvider          provider;
        std::unique_ptr<KickTableView> table;

        FileBrowserLeftPanel();

    private:
        juce::Rectangle<int> leftTopBounds    = { 0, 0, 342, 54 };
        juce::Rectangle<int> leftCentreBounds = { 0, 54, 342, 290 };
        KickLabel            location{ "Location" };

        void paint( juce::Graphics& g ) override;
        void resized() override;
    };

    class FileBrowserRightPanel : public juce::Component
    {
    public:
        KickTableViewProvider          provider;
        std::unique_ptr<KickTableView> table;
        KickImageButton                backButton{ "Back", "Back_Browser", "", "", "Back_Browser_Disabled" };

        FileBrowserRightPanel();

    private:
        juce::Rectangle<int> rightTopBounds    = { 0, 0, 652, 54 };
        juce::Rectangle<int> rightCentreBounds = { 0, 54, 652, 290 };
        void                 paint( juce::Graphics& g ) override;
        void                 resized() override;
        KickLabel            name{ "Name" }, date{ "Date" }, timeStamp{ "Time Stamp" }, size{ "Size" };
    };

    class FileBrowserTopControls : public juce::Component
    {
    public:
        std::function<void( void )> onElementChangedCallback;
        KickImageButton             searchButton{ "Search", "Search_Browser", "Search_BrowserActive  " };
        KickImageButton             backButton{ "Back", "Back_Browser", "Back_Browser", "Back_BrowserActive" };
        KickImageButton             projectButton{ "Project", "Project_Browser", "Project_BrowserActive" };
        KickImageButton             sampleButton{ "Sample", "Sample_Browser", "Sample_BrowserActive" };
        KickImageButton             allButton{ "All", "All_Browser", "All_BrowserActive" };

        KickImageButton     cutBtn{ "Cut Button", "Cut Button", "Cut Button_Active", "", "Cut Disabled" };
        KickImageButton     copyBtn{ "Copy Button", "Copy Button", "Copy Button_Active" };
        KickImageButton     pasteBtn{ "Paste Button", "Paste Button", "Paste Button_Active", "", "Paste Disabled" };
        KickImageButton     deleteBtn{ "Delete Button", "Trash", "Trash_Active", "", "Trash Disabled" };
        KickLabel           searchLabel{ "Search Directory" };
        KickImageButton     renameBtn{ "Rename", "Rename", "Rename_Active", "", "Rename Disabled" };
        ui::KickImageButton selectBtn{ "Select", "Select_Normal", "Select_Active" };

        FileBrowserTopControls();

    private:
        void paint( juce::Graphics& g ) override;
        void resized() override;
    };

    class FileBrowserBottomControls : public juce::TabbedComponent
    {
    public:
        KickImageButton projectButton{ "Project", "Project_Browser", "Project_BrowserActive" };
        KickImageButton sampleButton{ "Sample", "Sample_Browser", "Sample_BrowserActive" };
        KickImageButton allButton{ "All", "All_Browser", "All_BrowserActive" };

        KickButton      sampleAssignButton{ "Sample Assign", "Sample Assign" };
        KickImageButton playIconButton{ "Play", "Play_browser", "Play_browser_Active" };

        KickMultiSelectTabBarButton auditionButton{ 3, getTabbedButtonBar(), "OFF" };

        KickButton      loadButton{ "Load", "Load" };
        KickButton      loadAllButton{ "Load All", "Load All" };
        KickImageButton volumeBtn{ "Volume", "Volume_Browser", "Volume_BrowserActive" };
        KickImageButton volumeMuteBtn{ "VolumeMute", "Button_VolumeMute", "Button_VolumeMute" };

        FileBrowserBottomControls();

    private:
        void paint( juce::Graphics& g ) override;
        void resized() override;
    };

    struct DeviceData
    {
        juce::String imgBtn;
        juce::String imgBtnActive;
        float        width;
        float        height;
        juce::String fileName;
        bool         isEjectBtn;
    };

    class FolderSizeCalculator;
    class FileBrowser : public KickControlBase<juce::Component>
        , juce::Timer
        , public kick_ui::IScreen
    {
    public:
        std::function<void()>                       sizeCalculationEnd;
        std::function<void()>                       closeScreenCallback;
        std::function<void( ui::KickImageButton* )> onFirstClick;
        std::function<void( ui::KickImageButton* )> onSecondClick;
        std::function<void( ui::KickImageButton* )> onDoubleClick;

        FileBrowser();
        ~FileBrowser();

        bool requiresControlBar() override { return false; }
        bool requiresStatusBar() override { return true; }
        bool requiresHandleCommand() override { return true; }

        void handleCommand( CPktMgr::HWMsg_t& ) override;
        void sliderMoved( float ) override{};
        void knobRotated( double ) override;
        void knobClicked() override;

        void load( juce::String );
        void fileExtension( juce::String );
        void updateTemp( int );
        void deleteSystem();
        void searchImplementation();
        void visibilityChanged() override;

        void startFolderSizeCalculation( const juce::File& folder, std::function<void( juce::int64 )> callback );

        bool sdCardOn{ false }, usbOn{ false };
        int  sdCardRow{ -1 }, USBRow{ -1 };
        bool isSdCardEject{ false }, isUSBEject{ false };
        bool sdCardFirstTime{ true }, usbFirstTime{ true };

        std::vector<DeviceData> deviceData;

        mt::MTAudioInterface& ae;

        FileBrowserLeftPanel      left;
        std::vector<juce::String> row;

    private:
        kick_ui::AppStatusbar&         statusBar{ kick_ui::AppStatusbar::Instance() };
        bool                           onFirstLaunch{ false };
        const juce::Array<juce::File>  fileList;
        const juce::Array<juce::File>& dirList;
        juce::Array<juce::File>        tempList;
        juce::Array<juce::File>        selectedFile{ "" };
        juce::File                     sourceFile{ "" };
        int                            filterNum{ 2 }, editNum{ -1 };
        juce::String                   replacementPath = "[Internal]";
        bool                           searchMode      = false;
        static int                     numRows;

        kick_ui::KickModeHeader header{ "File Browser", "Icon_Browser" };
        KickLabel               pathLabel;

        FileBrowserRightPanel     right;
        FileBrowserTopControls    top;
        FileBrowserBottomControls bottom;
        AuditionComponent         auditionComp;

        int timerInt = 1;
        int buttonId;

        mt::KickSettings&                             settings{ mt::getSettings() };
        std::unique_ptr<LoadSamplesAndAnimation>      sampleAnimation;
        juce::String                                  pwd, root;
        juce::Array<juce::File>                       source;
        juce::File                                    destination;
        juce::CriticalSection                         lock;
        std::vector<std::unique_ptr<KickImageButton>> grpNum;
        juce::String                                  projectFileList{ ".kickPrj" };
        juce::String                                  sampleFileList{ ".wav;bwf;aif;aiff;flac;ogg;mp3;wmv;asf;wm;wma" };

        void paint( juce::Graphics& g ) override;
        void resized() override;
        void enterDirectory( int );
        void exitDirectory();
        void audition( juce::File f );
        void renamePopup();
        void renameImplementation( juce::String );
        void favouriteButtonImplementation();
        bool boolLoad{ false };
        void setupProvider();
        void selectBtnOnClick();
        void addData( const juce::String& buttonText, const juce::String& buttonTextActive, float width, float height, const juce::String& itemName, bool isEject );
        bool checkIsCurrProj();
        void hideAuditionComponent();
        void showAuditionComponent();
        void addMultipleAssets( juce::Array<juce::File> assetFile );
        void createProject( juce::String value );
        void loadSample( const juce::Array<juce::File>& assetFiles, bool IsMultiple );
        void mouseDown( const juce::MouseEvent& e ) override;
        void timerCallback() override;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( FileBrowser )
    };

    class FileLeftEntryPopup : public juce::Component
    {
    public:
        FileLeftEntryPopup( juce::String, juce::String image, int, int, juce::String fileName, juce::String date );
        ~FileLeftEntryPopup(){};
        void close();

        ui::KickImageButton button1{ "No", "No" };
        ui::KickImageButton button2{ "Yes", "Yes" };

    private:
        void            resized() override;
        void            paint( juce::Graphics& g ) override;
        ui::KickLabel   info, fileName, date;
        KickImageButton folder{ "Folder" };
        int             width, height;
        int             x = 98, y;
    };

    class FolderSizeCalculator : public juce::Thread
    {
    public:
        FolderSizeCalculator( const juce::File& folder, std::function<void( juce::int64 )> callback )
            : juce::Thread( "FolderSizeCalculator" ), folderToCalculate( folder ), resultCallback( callback )
        {
            calculateFolderSize();
        }
        ~FolderSizeCalculator()
        {
            stopThread( 5000 );
        }
        void run() override{};
        void calculateFolderSize();

    private:
        juce::File                         folderToCalculate;
        std::function<void( juce::int64 )> resultCallback;
        juce::int64                        totalSize = 0;
    };

    class FileLeftEntry : public juce::Component
    {
    public:
        std::function<void( int )> ejectCallback;
        FileLeftEntry( int rowId );
        ~FileLeftEntry() {}
        void            setProperties( const DeviceData& fileData );
        KickLabel       file;
        int             getRowID() { return row; }
        void            setHighlight( const bool state );
        KickImageButton ejectButton{ "Eject", "Eject", "Eject", "Eject_Active" };

    private:
        void resized() override;
        void paint( juce::Graphics& g ) override;

        KickImageButton imageButton;
        KickImageButton formatButton{ "Format", "Format", "Format", "Format_Active" };
        bool            enableEjectBtn{ false };
        float           width{ 0.0f }, height{ 0.0f };
        int             row;
    };

    class FileRightEntry : public juce::Component
    {
    public:
        FileRightEntry();
        ~FileRightEntry() {}
        void setProperties( const juce::String& imageButtonFile, bool, const float Width, const float Height, const juce::String& File, const juce::String& Date, const juce::String& Time, const juce::String& Size );

    private:
        float width{ 0.0f }, height{ 0.0f };
        bool  isFolder{ false };

        KickImageButton imageBtn;
        KickLabel       file;
        KickLabel       date;
        KickLabel       time;
        KickLabel       size;

        void resized() override;
        void paint( juce::Graphics& g ) override;
    };

    class NewProjectPopupData : public juce::Component
    {
    public:
        NewProjectPopupData( juce::String, juce::String );
        ~NewProjectPopupData() {}
        void         close();
        juce::String getInputText();

    private:
        void      paint( juce::Graphics& ) override;
        void      resized() override;
        KickLabel info;
        KickLabel text;
    };

}
