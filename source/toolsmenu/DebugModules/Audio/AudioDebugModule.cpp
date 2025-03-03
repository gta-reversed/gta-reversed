#include <StdInc.h>
#include <Audio/hardware/AEAudioHardware.h>
#include "./AudioDebugModule.hpp"

namespace {
template<typename T, T First, T End>
void DrawEnumSelect(const char* lbl, T& selected) {
    if (ImGui::BeginCombo(lbl, EnumToString(selected).value_or("NOT SELECTED"))) {
        for (int32 i = First; i < End; i++) {
            const auto slot = (T)(i);
            if (const auto name = EnumToString(slot)) {
                if (ImGui::Selectable(*name, slot == selected)) {
                    selected = slot;
                }
            }
        }
        ImGui::EndCombo();
    }
}

void DrawBankSlotSelect(const char* lbl, eSoundBankSlot& selected) {
    DrawEnumSelect<eSoundBankSlot, SND_BANK_SLOT_FIRST, SND_BANK_SLOT_END>(lbl, selected);
}

void DrawBankSelect(const char* lbl, eSoundBank& selected) {
    DrawEnumSelect<eSoundBank, SND_BANK_FIRST, SND_BANK_END>(lbl, selected);
}
};

namespace notsa { 
namespace debugmodules {
void AudioDebugModule::RenderWindow() {
    const ::notsa::ui::ScopedWindow window{ "AudioDebugModule", {}, m_IsOpen };
    if (!m_IsOpen) {
        return;
    }
    if (ImGui::BeginTabBar("TabBar")) {
        if (ImGui::BeginTabItem("Slots")) {
            DrawBankSlots();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}

void AudioDebugModule::RenderMenuEntry() {
    notsa::ui::DoNestedMenuIL({ "Extra", "Audio" }, [&] {
        ImGui::MenuItem("Audio", nullptr, &m_IsOpen);
    });
}

void AudioDebugModule::DrawBankSlots() {
    auto* const d = &BankSlotsMenuData;

    DrawBankSlotSelect("Selected Slot", d->SelectedSlot);

    if (d->SelectedSlot == SND_BANK_SLOT_NONE) {
        return;
    }

    const auto info = AEAudioHardware.GetBankSlot(d->SelectedSlot);
    if (!info) {
        ImGui::Text("SLOT NOT IN USE");
        return;
    }

    // Bank selection (This will also load a different bank into the slot upon change)
    {
        auto loaded = (eSoundBank)(info->Bank);
        DrawBankSelect("Loaded Bank", loaded);
        if (loaded != info->Bank) {
            AEAudioHardware.LoadSoundBank(loaded, d->SelectedSlot);
        }
    }
    
    if (ImGui::TreeNode("Slot Info")) {
        ImGui::Text("Offset: %i", (int32)(info->Offset));
        ImGui::Text("NumBytes: %i", (int32)(info->NumBytes));
        ImGui::Text("NumSounds: %i", (int32)(info->NumSounds));

        ImGui::TreePop();
    }

    if (!info->IsSingleSound()) {
        const auto disabled = info->NumSounds < 1;
        notsa::ui::ScopedDisable g{disabled};

        ImGui::InputInt("Sound ID", &d->SoundToPlayID);
        if (!disabled) {
            d->SoundToPlayID = std::clamp(d->SoundToPlayID, 0, info->NumSounds - 1);
        }

        const auto StopCurrentSoundIfAny = [&] {
            if (auto* const s = std::exchange(d->PlayedSound, nullptr)) {
                s->StopSoundAndForget();
            }
        };

        ImGui::SameLine();
        if (ImGui::Button("Play Sound")) {
            StopCurrentSoundIfAny();
            d->PlayedSound = AESoundManager.PlaySound({
                .BankSlot    = d->SelectedSlot,
                .SoundID     = (eSoundID)(d->SoundToPlayID),
                .AudioEntity = nullptr,
                .Pos         = TheCamera.GetPosition(),
                .Volume      = 20.f
            });
        }

        ImGui::SameLine();
        if (notsa::ui::ScopedDisable disable{ !d->PlayedSound }; ImGui::Button("Stop Sound")) {
            StopCurrentSoundIfAny();
        }
    }
}
}; // namespace debugmodules
}; // namespace notsa
