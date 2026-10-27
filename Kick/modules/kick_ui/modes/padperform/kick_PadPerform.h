#pragma once
#include "contracts/dcl/dcl_signals.h"
#include "contracts/dcl/dcl_contracts_helper.h"

namespace kick_ui
{
    class PadPerformContent : public juce::Component
    {
    public:
        PadPerformContent();
        ~PadPerformContent() = default;
        void selectionChanged();

        void handleCommand( CPktMgr::HWMsg_t& );
        void knobCommand( int knob, int value );

        inline int              transposeNote( int Note );
        inline std::vector<int> transpose( int Note );

    private:
        enum enumNoteMode
        {
            EN_NOTE_NOT_AVAILABLE = 0,
            EN_NOTE_INACTIVE,
            EN_NOTE_ROOT,
            EN_NOTE_ACTIVE
        };

        // Map these enums to  Controls::PadPerformType
        //  "Off", "Chromatic", "Notes", "Chords", "Progression", "Chromatic Chords"
        enum enumNoteTypes
        {
            EN_TYPE_OFF = 0,
            EN_TYPE_CHROMATIC,
            EN_TYPE_NOTES,
            EN_TYPE_CHORDS,
            EN_TYPE_PROGRESSION,
            EN_TYPE_CHROMATIC_CHORDS
        };

        //"Continuous", "Start on Root"
        enum enumStartSeq
        {
            EN_START_SEQ_CONTINOUS = 0,
            EN_START_SEQ_ROOT
        };

        struct noteData_t
        {
            int          colour{ 0 };
            int          activeDisplayIndex{ 0 };
            juce::String noteName{ "" };
            juce::Colour borderColour{ juce::Colours::transparentBlack };
            enumNoteMode enNoteMode{ EN_NOTE_NOT_AVAILABLE };

            inline void clear()
            {
                colour             = 0;
                activeDisplayIndex = 0;
                noteName           = "";
                borderColour       = juce::Colours::transparentBlack;
                enNoteMode         = EN_NOTE_NOT_AVAILABLE;
            }
        };

        mt::MTAudioInterface& ae{ mt::getAudioInterface() };

        int octaveIndex;
        int bankIndex;
        int activeDisplayIndex;
        int selectedType;
        int selectedScale;
        int selectedOctave;
        int selectedRootNote;
        int selectedStartOn;
        int selectedChord;
        int startOfNotes{ 0 };

        std::unique_ptr<ui::KickPadButtonGroupComponent> pads;
        ui::KickListBox                                  type{ ui::KickListBox::BoxType::withTitleBar, "TYPE", mt::Kick::PadPerformType };
        ui::KickLabel                                    scale_octave{ "SCALE AND OCTAVE" };
        ui::KickListBox                                  rootNote{ ui::KickListBox::BoxType::withTitleBar, "ROOT NOTE", mt::Note::rootNames(), 0, 3 };
        ui::KickListBox                                  scale{ ui::KickListBox::BoxType::withTitleBar, "SCALE", mt::ScaleAndNotes::scaleNames, 0, 3 };
        ui::KickListBox                                  chord{ ui::KickListBox::BoxType::withTitleBar, "CHORDS", mt::ScaleAndNotes::Chord, 0, 2 };
        ui::KickListBox                                  startOn{ ui::KickListBox::BoxType::withTitleBar, "BANKS", mt::Kick::PadPerformStartOn };
        ui::KickListBox                                  octave{ ui::KickListBox::BoxType::withTitleBar, "START ON OCTAVE", mt::ScaleAndNotes::octave, 0, 3 };

        ui::KickImageButton bankInc{ "Inc", "Plus", "Plus_Active" };
        ui::KickImageButton bankDec{ "Dec", "Minus", "Minus_Active" };
        ui::KickLabel       bank{ "A" };

        ui::KickToggleButton chromaticBtn{ "Chromatic", "Chromatic", "Chromatic" };
        ui::KickToggleButton notesBtn{ "Notes", "Notes", "Notes" };
        ui::KickToggleButton chordsBtn{ "Chords", "Chords", "Chords" };
        ui::KickToggleButton progressionBtn{ "Progressions", "Progressions", "Progressions" };

        noteData_t                  st_NoteData[128];
        std::array<noteData_t, 128> noteData;
        mt::MTIPadPerformanceData*  pPadPerform{ nullptr };

        int pushPadInfoToUI( int id, const juce::String& text, enumNoteMode f_enNoteMode );

        void updateNoteData( void );
        void clearNoteData( void );
        void pushToUI( void );
        void pushToHW( void );
        void paint( juce::Graphics& g ) override;

        void refresh();
        void resized() override;
        void visibilityChanged() override;

        void refreshData();
        void displayFromNote( int );
        void updateNoteNames();
        void pushPadInfoToDisplay();
        void pushPadInfoToHw( int id, enumNoteMode f_enNoteMode );
        int  convertToHwColour( int colourId );

        inline void IncrementBankIndex( void )
        {
            // Clear all Pads
            contracts::DclSignals::sendCommandToDcl( contracts::DclContractsHelper::clearAllPadsMessage() );
            if( bankIndex > 0 )
                bankIndex--;
            pads->setBank( mt::Kick::padBankNames[bankIndex] );
        }

        inline void DecrementBankIndex( void )
        {
        }
    };

    class PadPerform : public kick_ui::IScreen
        , public juce::Component
    {
    public:
        PadPerform();
        ~PadPerform() = default;

        bool canClose() override { return true; }
        bool requiresControlBar() override { return false; }
        bool requiresStatusBar() override { return true; }
        bool requiresHandleCommand() override { return true; }

        void handleCommand( CPktMgr::HWMsg_t& ) override;

        int              transposeNote( int Note );
        std::vector<int> transpose( int Note );

    private:
        mt::MTAudioInterface& ae{ mt::getAudioInterface() };
        KickModeHeader        header{ "Pad Notes", "Icon_PadPerform" };
        PadPerformContent     content;

        void resized() override;
        void visibilityChanged() override;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( PadPerform )
    };
}
