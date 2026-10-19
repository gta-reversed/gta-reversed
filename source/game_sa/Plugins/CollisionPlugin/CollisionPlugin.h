#pragma once

#include <rwplcore.h>

class CClumpModelInfo;

/**
 * collision plugin unique rwID
 */
#define rwID_COLLISIONPLUGIN  MAKECHUNKID(rwVENDORID_DEVELOPER, 0xFA)

/*
* Collision plugin static offset
*/
extern RwInt32& gCollisionPluginOffset;

class CCollisionPlugin {
public:
    static inline auto& ms_currentModel = StaticRef<CClumpModelInfo*>(0x9689E0);

public:
    static void InjectHooks();

    static bool PluginAttach();
    static void SetModelInfo(CClumpModelInfo* modelInfo);
};
