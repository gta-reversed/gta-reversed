#include "StdInc.h"

#include "ScriptsForBrains.h"

void CScriptsForBrains::InjectHooks() {
    RH_ScopedClass(CScriptsForBrains);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Init, 0x46A8C0, {.reversed = false});
    //RH_ScopedInstall(SwitchAllObjectBrainsWithThisID, 0x46A900, {.reversed = false});
    RH_ScopedInstall(AddNewScriptBrain, 0x46A930);
    RH_ScopedInstall(AddNewStreamedScriptBrainForCodeUse, 0x46A9C0);
    RH_ScopedInstall(GetIndexOfScriptBrainWithThisName, 0x46AA30);
    RH_ScopedInstall(HasAttractorScriptBrainWithThisNameLoaded, 0x46AB20);
    //RH_ScopedInstall(StartNewStreamedScriptBrain, 0x46B270, {.reversed = false});
    RH_ScopedInstall(StartAttractorScriptBrainWithThisName, 0x46B390);
    RH_ScopedInstall(StartOrRequestNewStreamedScriptBrain, 0x46CD80, {.reversed = false});
    RH_ScopedInstall(StartOrRequestNewStreamedScriptBrainWithThisName, 0x46CED0);
    RH_ScopedInstall(IsObjectWithinBrainActivationRange, 0x46B3D0, {.reversed=false});
}


// 0x46A8C0
void CScriptsForBrains::Init() {
    for (auto& script : m_aScriptForBrains) {
        script = tScriptForBrains();
    }
}

// 0x46A9C0
void CScriptsForBrains::AddNewStreamedScriptBrainForCodeUse(int16 streamedScriptIndex, const char *scriptName, int8 brainType) {
    for (auto& script : m_aScriptForBrains) {
        if (script.m_StreamedScriptIndex == -1) {
            script.m_StreamedScriptIndex         = streamedScriptIndex;
            *script.m_ScriptName                 = *scriptName;
            script.m_TypeOfBrain                 = brainType;
            script.m_ObjectGroupingId            = -1;
            script.m_bBrainActive                = true;
            script.m_ObjectBrainActivationRadius = 5.0f;
            return;
        }
    }
}

// 0x46A930
void CScriptsForBrains::AddNewScriptBrain(int16 ImgIndex, int16 Model, uint16 priority, int8 attachType, int8 Type, float Radius) {
    for (auto& script : m_aScriptForBrains) {
        if (script.m_StreamedScriptIndex == -1) {
            script.m_StreamedScriptIndex = ImgIndex;
            script.m_PercentageChance = priority;
            script.m_ObjectGroupingId = Type;
            script.m_PedModelOrPedGeneratorIndex = Model;
            script.m_TypeOfBrain = attachType;
            script.m_bBrainActive = true;
            script.m_ObjectBrainActivationRadius = (Radius > 0.0f ? Radius : 5.0f);
            return;
        }
    }
}

void CScriptsForBrains::CheckIfNewEntityNeedsScript(CEntity* entity, int8 attachType, void* unused) {
    plugin::CallMethod<0x46FF20, CScriptsForBrains*, CEntity*, int8, void*>(this, entity, attachType, unused);
}

void CScriptsForBrains::MarkAttractorScriptBrainWithThisNameAsNoLongerNeeded(const char* name) {
    plugin::CallMethod<0x46AAE0, CScriptsForBrains*, const char*>(this, name);
}

void CScriptsForBrains::RequestAttractorScriptBrainWithThisName(const char* name) {
    plugin::CallMethod<0x46AA80, CScriptsForBrains*, const char*>(this, name);
}

void CScriptsForBrains::StartNewStreamedScriptBrain(uint8 index, CEntity* entity, bool bHasAScriptBrain) {
    plugin::CallMethodAndReturn<void, 0x46B270, CScriptsForBrains*, uint8, CEntity*, uint8>(this, index, entity, bHasAScriptBrain);
}

void CScriptsForBrains::StartOrRequestNewStreamedScriptBrain(uint8 index, CEntity* entity, int8 attachType, bool bAddToWaitingArray) {
    NOTSA_UNREACHABLE();
}

// 0x46CED0
void CScriptsForBrains::StartOrRequestNewStreamedScriptBrainWithThisName(const char* name, CEntity* entity, int8 attachType) {
    if (const auto IndexOfScriptBrainWithThisName = GetIndexOfScriptBrainWithThisName(name, attachType) >= 0) {
        StartOrRequestNewStreamedScriptBrain(IndexOfScriptBrainWithThisName, entity, attachType, true);
    }
}

bool CScriptsForBrains::HasAttractorScriptBrainWithThisNameLoaded(const char* name) {
    if (const auto idx = GetIndexOfScriptBrainWithThisName(name, 5); idx >= 0) {
        return CStreaming::IsModelLoaded(SCMToModelId(m_aScriptForBrains[idx].m_StreamedScriptIndex));
    }
    return false;
}

// 0x46B3D0
bool CScriptsForBrains::IsObjectWithinBrainActivationRange(CObject* entity, const CVector& point) {
    const auto script = &m_aScriptForBrains[entity->m_nStreamedScriptBrainToLoad];
    if (script->m_TypeOfBrain == 1) {
        const auto& v5 = entity->GetPosition();
        const auto v6_v7 = point - v5;
        if (sqrt(v6_v7.z * v6_v7.z + v6_v7.y * v6_v7.y + (point.x - v5.x) * (point.x - v5.x)) < script->m_ObjectBrainActivationRadius) {
            return true;
        }
    }
    return false;
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
