#pragma once
namespace ui
{
    class FilePopup : public juce::Component
    {
    public:
        FilePopup( juce::String, bool isYes = true, bool isNo = true, int x = 100, int y = 140, int width = 400 );

    private:
        void resized() override;
        void close( int State = 0 );

        ui::KickImageButton no{ "No", "No" };
        ui::KickImageButton yes{ "Yes", "Yes" };
        ui::KickImageButton cancel{ "Cancel", "Cancel_Browser" };
        ui::KickLabel       info;
        int                 startX, startY, width;
        int                 textHeight{ 22 };
    };
}
