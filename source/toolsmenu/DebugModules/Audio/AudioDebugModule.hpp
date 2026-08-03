#pragma once

#include <DebugModules/DebugModule.h>
#include <Audio/managers/AESoundManager.h>
#include <Audio/Enums/eSoundBankSlot.h>

namespace notsa { 
namespace debugmodules {
class AudioDebugModule final : public DebugModule {
public:
    void RenderWindow() override;
    void RenderMenuEntry() override;

    NOTSA_IMPLEMENT_DEBUG_MODULE_SERIALIZATION(AudioDebugModule, m_IsOpen, BankSlotsMenuData);

private:
    void DrawBankSlots();

private:
    bool m_IsOpen{};

    struct tBankSlotsMenuData {
        eSoundBankSlot SelectedSlot{SND_BANK_SLOT_NONE};
        int32          SoundToPlayID{0};
        CAESound*      PlayedSound{};

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(tBankSlotsMenuData, SelectedSlot, SoundToPlayID);
    } BankSlotsMenuData;
};
}; // namespace debugmodules
}; // namespace notsa
