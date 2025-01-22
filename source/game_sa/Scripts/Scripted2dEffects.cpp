#include "StdInc.h"

#include "Scripted2dEffects.h"

void CScripted2dEffects::InjectHooks() {
    RH_ScopedClass(CScripted2dEffects);
    RH_ScopedCategory("Scripts");

    RH_ScopedInstall(Init, 0x6FA6F0, { .reversed = false });
    RH_ScopedInstall(GetEffectPairs, 0x6FA840);
    RH_ScopedInstall(GetIndex, 0x6F9F60);
    RH_ScopedInstall(AddScripted2DEffect, 0x6FA7C0);
    RH_ScopedInstall(ReturnScripted2DEffect, 0x6F9E80);
}

// 0x6FA6F0
// FIXME: Makes game crash for some reason
void CScripted2dEffects::Init() {
    for (auto i = 0u; i < NUM_SCRIPTED_2D_EFFECTS; i++) {
        ms_activated[i]             = false;
        ScriptReferenceIndex[i]     = 1;
        ms_effectSequenceTaskIDs[i] = -1;
        ms_userLists[i].m_bUseList  = false;
        ms_useAgainFlags[i]         = false;
        ms_radii[i]                 = -1.0f;
        rng::fill(ms_userLists[i].m_UserTypes, -1);
        rng::fill(ms_userLists[i].m_UserTypesByPedType, -1);
        ms_effectPairs[i].Flush();
    }
}

// 0x6FA840
CScriptedEffectPairs* CScripted2dEffects::GetEffectPairs(const C2dEffect* effect) {
    return &ms_effectPairs[GetIndex(effect)];
}

// 0x6F9F60
int32 CScripted2dEffects::GetIndex(const C2dEffectBase* effect) {
    if (const auto idx = IndexOfEffect(effect)) {
        return (int32)*idx;
    }
    return -1;
}

// 0x6FA7C0
int32 CScripted2dEffects::AddScripted2DEffect(float radius) {
    const auto slot = FindFreeSlot();
    if (slot == -1) {
        return NUM_SCRIPTED_2D_EFFECTS;
    }

    ms_activated[slot]             = true;
    ms_effectSequenceTaskIDs[slot] = -1;
    ms_userLists[slot].m_bUseList  = false;
    ms_useAgainFlags[slot]         = false;
    ms_radii[slot]                 = radius;
    rng::fill(ms_userLists[slot].m_UserTypes, -1);
    rng::fill(ms_userLists[slot].m_UserTypesByPedType, -1);
    ms_effectPairs[slot].Flush();
    return slot;
}

// 0x6F9E80
void CScripted2dEffects::ReturnScripted2DEffect(int32 index) {
    ms_activated[index] = false;
    return GetPedAttractorManager()->RemoveEffect(&ms_effects[index]);
}

auto CScripted2dEffects::IndexOfEffect(const C2dEffectBase* effect) -> std::optional<size_t> {
    const auto fx = reinterpret_cast<const C2dEffect*>(effect);
    if (ms_effects.data() <= fx && fx < ms_effects.data() + ms_effects.size()) {
        return (size_t)(fx - ms_effects.data());
    }
    return std::nullopt;
}
