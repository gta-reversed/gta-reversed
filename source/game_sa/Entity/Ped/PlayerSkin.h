#pragma once

#include "RenderWare.h"

class CPlayerSkin {
public:
    static inline auto& m_txdSlot = StaticRef<int32>(0xC3F03C);

public:
    static void InjectHooks();

    static void       Initialise();
    static void       Shutdown();
    static void       RenderFrontendSkinEdit();
    static RwTexture* GetSkinTexture(const char* name);
};
