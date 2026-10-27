#pragma once
#include "kick_AudioProgramEditGrid.h"
#include "kick_AudioSelectTool.h"
#include "kick_AudioEraseTool.h"
#include "kick_AudioDragTool.h"
#include "kick_AudioCutTool.h"
#include "kick_AudioPencilTool.h"
#include "kick_AudioMagnifyTool.h"
namespace kick_ui
{
    class AudioProgramEdit : public ProgEditInterface
    {
    public:
        AudioProgramEdit();
        ~AudioProgramEdit() = default;

        void             refresh() override { audioPgmContent.refresh(); };
        void             setupHeader( kick_ui::KickModeHeader& Hdr ) override{};
        void             handleCommand( CPktMgr::HWMsg_t& f_HwMsg ) override { audioPgmContent.handleCommand( f_HwMsg ); };
        juce::Component& getHeaderComponent() { return audioPgmContent.getHeaderComponent(); }

    private:
        void resized() override;
        void visibilityChanged() override;

        mt::MTAudioProgram* pAudio{ nullptr };
        AudioProgramGrid    audioPgmContent{ pAudio };

    private:
    };
}
