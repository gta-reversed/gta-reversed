#include "StdInc.h"

#include "ScriptsForBrains.h"

void CScriptsForBrains::InjectHooks() {
    RH_ScopedClass(CScriptsForBrains);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Init, 0x46A8C0);
    RH_ScopedInstall(SwitchAllObjectBrainsWithThisID, 0x46A900);
    RH_ScopedInstall(AddNewScriptBrain, 0x46A930);
    RH_ScopedInstall(AddNewStreamedScriptBrainForCodeUse, 0x46A9C0);
    RH_ScopedInstall(GetIndexOfScriptBrainWithThisName, 0x46AA30);
    RH_ScopedInstall(HasAttractorScriptBrainWithThisNameLoaded, 0x46AB20);
    RH_ScopedInstall(RequestAttractorScriptBrainWithThisName, 0x46AA80);
    RH_ScopedInstall(MarkAttractorScriptBrainWithThisNameAsNoLongerNeeded, 0x46AAE0);
    RH_ScopedInstall(StartNewStreamedScriptBrain, 0x46B270);
    RH_ScopedInstall(StartAttractorScriptBrainWithThisName, 0x46B390);
    RH_ScopedInstall(StartOrRequestNewStreamedScriptBrain, 0x46CD80);
    RH_ScopedInstall(StartOrRequestNewStreamedScriptBrainWithThisName, 0x46CED0);
    RH_ScopedInstall(IsObjectWithinBrainActivationRange, 0x46B3D0);
    RH_ScopedInstall(CheckIfNewEntityNeedsScript, 0x46FF20);
}


// 0x46A8C0
void CScriptsForBrains::Init() {
    for (auto& script : m_aScriptForBrains) {
        script = tScriptForBrains();
    }
}
// 0x46A900
void CScriptsForBrains::SwitchAllObjectBrainsWithThisID(int8 ID, bool bStatus) {
    if (ID < 0) {
        return;
    }
    for (auto& brain : m_aScriptForBrains) {
        if (brain.m_ObjectGroupingId == ID) {
            brain.m_bBrainActive = bStatus;
        }
    }
}

// 0x46A930
void CScriptsForBrains::AddNewScriptBrain(int16 ImgIndex, int16 Model, uint16 priority, int8 attachType, int8 Type, float Radius) {
    for (auto& brain : m_aScriptForBrains) {
        if (brain.m_StreamedScriptIndex != -1) {
            continue;
        }
        brain.m_StreamedScriptIndex        = ImgIndex;
        brain.m_TypeOfBrain                = attachType;
        brain.m_ObjectGroupingId           = Type;
        brain.m_bBrainActive               = true;
        brain.m_PedModelOrPedGeneratorIndex = Model;
        brain.m_PercentageChance           = priority;
        brain.m_ObjectBrainActivationRadius = Radius <= 0.0f ? 5.0f : Radius;
        return;
    }
}

// 0x46A9C0
void CScriptsForBrains::AddNewStreamedScriptBrainForCodeUse(int16 a2, char* a3, int8 attachtype) {
    for (auto& brain : m_aScriptForBrains) {
        if (brain.m_StreamedScriptIndex != -1) {
            continue;
        }
        brain.m_StreamedScriptIndex = a2;
        strcpy(brain.m_ScriptName, a3);
        brain.m_TypeOfBrain                 = attachtype;
        brain.m_ObjectGroupingId            = -1;
        brain.m_bBrainActive                = true;
        brain.m_ObjectBrainActivationRadius = 5.0f;
        return;
    }
}
// 0x46FF20
void CScriptsForBrains::CheckIfNewEntityNeedsScript(CEntity* entity, int8 attachType, void* unused) {
    if (!attachType) {
        if (entity->AsPed()->bHasAScriptBrain || entity->AsPed()->bWaitingForScriptBrainToLoad)
            return;
    } else if (entity->AsObject()->objectFlags.b0x100000_0x200000) {
        return;
    }
    for (auto i = 0u; i < m_aScriptForBrains.size(); i++) {
        const auto& brain = m_aScriptForBrains[i];
        if (brain.m_TypeOfBrain != attachType || brain.m_ObjectGroupingId != entity->m_nModelIndex)
            continue;
        // Binary: (rand() & 0xFFFF) * 3.0517578125e-05 * 100.0, truncated to int by FUN_00821B40,
        // compared against m_PercentageChance. Same shape as CGeneral::GetRandomNumberInRange(0.0f, 100.0f).
        const auto roll = static_cast<int32>(CGeneral::GetRandomNumberInRange(0.0f, 100.0f));
        if (roll < brain.m_PercentageChance) {
            StartOrRequestNewStreamedScriptBrain(static_cast<uint8>(i), entity, attachType, true);
            return;
        }
    }
}

// 0x46AA80
// Binary: byte at 0xA47B64 + scm*0x20 = CTheScripts::StreamedScripts.m_aScripts[scm].m_NumberOfUsers
// (CStreamedScriptInfo is 0x20 bytes; counter at +4). Flags arg of RequestModel is crack-obfuscated;
// value follows this file's existing precedent.
void CScriptsForBrains::RequestAttractorScriptBrainWithThisName(const char* name) {
    if (const auto idx = GetIndexOfScriptBrainWithThisName(name, 5); idx >= 0) {
        const auto scm = m_aScriptForBrains[idx].m_StreamedScriptIndex;
        if (!CStreaming::IsModelLoaded(SCMToModelId(scm)))
            CStreaming::RequestModel(SCMToModelId(scm), STREAMING_GAME_REQUIRED);
        CTheScripts::StreamedScripts.m_aScripts[scm].m_NumberOfUsers++;
    }
}

// 0x46AAE0
void CScriptsForBrains::MarkAttractorScriptBrainWithThisNameAsNoLongerNeeded(const char* name) {
    if (const auto idx = GetIndexOfScriptBrainWithThisName(name, 5); idx >= 0) {
        auto& users = CTheScripts::StreamedScripts.m_aScripts[m_aScriptForBrains[idx].m_StreamedScriptIndex].m_NumberOfUsers;
        if (users)
            users--;
    }
}

// 0x46B270
void CScriptsForBrains::StartNewStreamedScriptBrain(uint8 index, CEntity* entity, bool bHasAScriptBrain) {
    auto& brain = m_aScriptForBrains[index];
    auto* script = CTheScripts::StreamedScripts.StartNewStreamedScript(brain.m_StreamedScriptIndex);
    script->m_ExternalType = brain.m_TypeOfBrain;
    switch (brain.m_TypeOfBrain) {
    case 0:
    case 3:
    case 5:
        script->m_LocalVars[0].iParam = GetPedPool()->GetRef(entity->AsPed());
        entity->AsPed()->bHasAScriptBrain = true;
        if (brain.m_TypeOfBrain == 5)
            script->m_LocalVars[1].iParam = bHasAScriptBrain;
        entity->AsPed()->bWaitingForScriptBrainToLoad = false;
        CTheScripts::RemoveFromWaitingForScriptBrainArray(entity, entity->AsPed()->m_StreamedScriptBrainToLoad);
        entity->AsPed()->m_StreamedScriptBrainToLoad = -1;
        return;
    case 1:
    case 4:
        script->m_LocalVars[0].iParam = GetObjectPool()->GetRef(entity->AsObject());
        entity->AsObject()->objectFlags.b0x100000_0x200000 = 3;
        return;
    default:
        return;
    }
}

// 0x46CD80
// ped +0x474 bits 0x800000/0x1000000 and object +0x140 bits 0x100000/0x200000 have no named
// fields in the current headers (verified: no m_nPedFlags/m_nEntityFlags members exist),
// so raw offsets are used here to stay faithful to the binary.
void CScriptsForBrains::StartOrRequestNewStreamedScriptBrain(uint8 index, CEntity* entity, int8 attachType, bool bAddToWaitingArray) {
    if (bAddToWaitingArray) {
        if (!m_aScriptForBrains[index].m_bBrainActive)
            return;
        switch (attachType) {
        case 0:
        case 3: {
            const auto flags = *reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(entity) + 0x474);
            if (flags & 0x800000)
                return;
            if (flags & 0x1000000)
                return;
            break;
        }
        case 1:
        case 4:
            if (*reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(entity) + 0x140) & 0x300000)
                return;
            break;
        default:
            break;
        }
    }
    if (attachType == 1) {
        auto* flags = reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(entity) + 0x140);
        if (!bAddToWaitingArray) {
            *flags = (*flags & ~0x100000u) | 0x200000u;
        } else {
            entity->AsObject()->m_nStreamedScriptBrainToLoad = index;
            *flags = (*flags & ~0x200000u) | 0x100000u;
            CTheScripts::AddToWaitingForScriptBrainArray(entity, index);
        }
    }
    const auto scmIndex = m_aScriptForBrains[index].m_StreamedScriptIndex;
    if (CStreaming::ms_aInfoForModel[SCMToModelId(scmIndex)].m_LoadState != eStreamingLoadState::LOADSTATE_LOADED) {
        CStreaming::RequestModel(SCMToModelId(scmIndex), STREAMING_GAME_REQUIRED);
        if ((attachType == 0 || (attachType > 2 && attachType < 5)) && bAddToWaitingArray) {
            entity->AsPed()->m_StreamedScriptBrainToLoad = index;
            *reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(entity) + 0x474) |= 0x1000000u;
            CTheScripts::AddToWaitingForScriptBrainArray(entity, index);
        }
        return;
    }
    StartNewStreamedScriptBrain(index, entity, false);
}

// 0x46CED0
void CScriptsForBrains::StartOrRequestNewStreamedScriptBrainWithThisName(const char* name, CEntity* entity, int8 attachType) {
    if (const auto idx = GetIndexOfScriptBrainWithThisName(name, attachType); idx >= 0)
        StartOrRequestNewStreamedScriptBrain(static_cast<uint8>(idx), entity, attachType, true);
}

bool CScriptsForBrains::HasAttractorScriptBrainWithThisNameLoaded(const char* name) {
    if (const auto idx = GetIndexOfScriptBrainWithThisName(name, 5); idx >= 0) {
        return CStreaming::IsModelLoaded(SCMToModelId(m_aScriptForBrains[idx].m_StreamedScriptIndex));
    }
    return false;
}

// 0x46B3D0
bool CScriptsForBrains::IsObjectWithinBrainActivationRange(CObject* entity, const CVector& point) {
    const auto& brain = m_aScriptForBrains[entity->CObject::m_nStreamedScriptBrainToLoad];
    if (brain.m_TypeOfBrain != 1)
        return false;
    const auto* lod = entity->GetLod();
    const auto& pos = lod ? lod->GetPosition() : entity->GetPosition();
    return (point - pos).Magnitude() < brain.m_ObjectBrainActivationRadius;
}

int16 CScriptsForBrains::GetIndexOfScriptBrainWithThisName(const char* name, int8 type) {
    const auto it = rng::find_if(m_aScriptForBrains, [=](tScriptForBrains& script) {
        return script.m_TypeOfBrain == type && !_stricmp(script.m_ScriptName, name);
    });
    return it != m_aScriptForBrains.end()
        ? rng::distance(m_aScriptForBrains.begin(), it)
        : -1;
}

void CScriptsForBrains::StartAttractorScriptBrainWithThisName(const char* name, CPed* ped, bool bHasAScriptBrain) {
    if (!ped->bWaitingForScriptBrainToLoad && !ped->bHasAScriptBrain) {
        if (const auto idx = GetIndexOfScriptBrainWithThisName(name, 5); idx >= 0) {
            StartNewStreamedScriptBrain(static_cast<uint8>(idx), ped, bHasAScriptBrain);
        }
    }
}
