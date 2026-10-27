#pragma once

namespace kick_ui
{
    class InsertsSelector
        : public juce::Component
    {
    public:
        InsertsSelector();
        ~InsertsSelector() = default;

        std::function<void( const juce::String& )> onClick = []( const juce::String& ) {};

        const juce::String& getSelected() { return insertList.getSelected(); };

    private:
        void paint( juce::Graphics& g ) override;
        void resized() override;

        ui::KickList insertCategories{ "INSERT CATEGORY" };
        ui::KickList insertList{ "INSERTS", {}, 0, 2 };
    };

    class InsertsOptions
        : public juce::Component
    {
    public:
        InsertsOptions();
        ~InsertsOptions() = default;

        std::function<void( int, bool )>       onEnableClicked = []( int, bool ) {};
        std::function<void( int, int, float )> onRotaryChanged = []( int, int, float ) {};

        void setupControls( mt::MTIInsertsInterface* pEffect, int Position );
        void refresh();

    private:
        void setupControls( int NumControls );

        void visibilityChanged() override;
        void paint( juce::Graphics& g ) override;
        void resized() override;

    private:
        int                      pos{ 0 };
        mt::MTIInsertsInterface* pInsert{ nullptr };
        mt::MTInsertsInfo        info;

        ui::KickLabel                                insertName;
        ui::KickToggleButton                         enable;
        std::vector<std::unique_ptr<ui::KickRotary>> rotary;
    };

    class InsertsList
        : public juce::Component
    {
    public:
        explicit InsertsList( InsertsSelector& selector, InsertsOptions& options );
        ~InsertsList() = default;

        std::function<void( int, const juce::String&, bool )> onInsertDelete  = []( int, const juce::String&, bool ) {};
        std::function<void( int, const juce::String&, bool )> onInsertChanged = []( int, const juce::String&, bool ) {};

    private:
        void refresh();
        int  getDefaultIndex();

        void visibilityChanged() override;
        void paint( juce::Graphics& g ) override;
        void resized() override;

    private:
        InsertsSelector& selector;
        InsertsOptions&  options;

        mt::MTIInsertsInterface*        pEffects;
        std::vector<mt::MTInsertsInfo>* pInfo;

        static constexpr int                      numRows{ 4 };
        std::array<ui::KickButton, numRows>       inserts;
        std::array<ui::KickToggleButton, numRows> enable;
        std::array<ui::KickToggleButton, numRows> add;
        int                                       selected{ -1 };

        ui::KickLabel        allInserts{ "ALL INSERTS" };
        ui::KickButton       allDelete{ "DELETE" };
        ui::KickToggleButton allEnable{ "All Enable" };
    };

    class Inserts
        : public kick_ui::IScreen
        , public juce::Component
    {
    public:
        Inserts();
        ~Inserts() = default;

        std::function<void( int, const juce::String&, bool )> onInsertChanged = []( int, const juce::String&, bool ) {};
        std::function<void( int, const juce::String&, bool )> onInsertDelete  = []( int, const juce::String&, bool ) {};

        void handleCommand( CPktMgr::HWMsg_t& ) override{};
        void knobCommand( int knob, int value ) override{};

    private:
        kick_ui::KickModeHeader header{ "Inserts", "Icon_Mixer " };
        InsertsSelector         selector;
        InsertsOptions          options;
        InsertsList             list{ selector, options };

        bool canClose() override { return true; }
        bool requiresControlBar() override { return false; }
        bool requiresStatusBar() override { return true; }

        void visibilityChanged() override;
        void resized() override;
    };

}
