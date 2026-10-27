#pragma once
namespace mt::ac
{
    class Locate : public juce::Component

    {
    public:
        Locate();
        ~Locate();
        void         updatePlayHead();
        void         displayPopup();
        void         refresh();
        juce::String getBarBeatTickInfo( int barValue, int beatValue, int tickValue );

    private:
        MTAudioInterface& ae{ getAudioInterface() };
        MTBarNBeats&      playHeadBnB{ getAudioInterface().getPlayhead().bnb };

        void resized() override;

    private:
        ui::KickLabel barBeatTickLabel{ "BAR : BEAT : TICK" };
    };

    class LocateSTP : public juce::Component
    {
    public:
        LocateSTP();
        ~LocateSTP();
        void updateSelection();
        void displaySTPPopup();

    private:
        MTAudioInterface& ae{ getAudioInterface() };
        const MTSelected& selection{ getAudioInterface().getSelected() };

        void resized() override;

        ui::KickLabel sequenceLable{ "SEQUENCES" };
        ui::KickLabel trackLable{ "TRACKS" };
        ui::KickLabel programLable{ "PROGRAMS" };
        int           bar  = 1;
        int           beat = 1;
        int           tick = 0;
    };
}
