#pragma once

#include <stacktrace>

#include "../DebugModule.h"
#include <Audio/managers/AESoundManager.h>

namespace notsa { 
namespace debugmodules {
class SoundManagerDebugModule final : public DebugModule {
public:
    struct SoundInfo {
        std::stacktrace StackTrace;
    };

public:
    void RenderWindow() override;
    void RenderMenuEntry() override; 

    void SetSoundInfo(int32 id, SoundInfo info) { m_SoundsInfo[id] = info; }

    NOTSA_IMPLEMENT_DEBUG_MODULE_SERIALIZATION(SoundManagerDebugModule, m_IsOpen);

private:
    void RenderSoundsTable();

private:
    bool                                  m_IsOpen{};
    std::array<SoundInfo, MAX_NUM_SOUNDS> m_SoundsInfo;
};
}; // namespace debugmodules
}; // namespace notsa
