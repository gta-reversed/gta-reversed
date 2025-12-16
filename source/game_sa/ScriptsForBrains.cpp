#include "StdInc.h"

#include "ScriptsForBrains.h"

void CScriptsForBrains::InjectHooks() {
    RH_ScopedClass(CScriptsForBrains);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Init, 0x46A8C0, {.reversed = false});
    //RH_ScopedInstall(SwitchAllObjectBrainsWithThisID, 0x46A900, {.reversed = false});
    //RH_ScopedInstall(AddNewScriptBrain, 0x46A930, {.reversed = false});
    //RH_ScopedInstall(AddNewStreamedScriptBrainForCodeUse, 0x46A9C0, {.reversed = false});
    RH_ScopedInstall(GetIndexOfScriptBrainWithThisName, 0x46AA30);
    RH_ScopedInstall(HasAttractorScriptBrainWithThisNameLoaded, 0x46AB20);
    RH_ScopedInstall(CheckIfNewEntityNeedsScript, 0x46FF20);
    RH_ScopedInstall(StartNewStreamedScriptBrain, 0x46B270);
    RH_ScopedInstall(StartAttractorScriptBrainWithThisName, 0x46B390);
    RH_ScopedInstall(StartOrRequestNewStreamedScriptBrain, 0x46CD80, {.reversed = false});
    RH_ScopedInstall(StartOrRequestNewStreamedScriptBrainWithThisName, 0x46CED0);
    RH_ScopedInstall(IsObjectWithinBrainActivationRange, 0x46B3D0);
}

// 0x46A8C0
void CScriptsForBrains::Init() {
    for (auto& script : m_aScriptForBrains) {
        script = tScriptForBrains();
    }
}

// 0x46FF20
void CScriptsForBrains::CheckIfNewEntityNeedsScript(CEntity* entity, int8 brainType, void* unused) {
    if (brainType == CScriptsForBrains::PED_STREAMED) {
        if (entity->GetIsTypePed()) {
            const auto ped = entity->AsPed();
            if (ped->bWaitingForScriptBrainToLoad || ped->bHasAScriptBrain) {
                return;
            }
        }
    } else {
        if (entity->GetIsTypeObject()) {
            const auto object = entity->AsObject();
            if (object->objectFlags.bScriptBrainStatus) {
                return;
            }
        }
    }

    for (const auto& [i, scriptBrain] : rngv::enumerate(m_aScriptForBrains)) {
        if (scriptBrain.m_TypeOfBrain == brainType && scriptBrain.m_PedModelOrPedGeneratorIndex == entity->m_nModelIndex) {
            if (CGeneral::RandomBool(scriptBrain.m_PercentageChance)) {
                StartOrRequestNewStreamedScriptBrain(i, entity, brainType, true);
                break;
            }
        }
    }
}

// 0x46AAE0
void CScriptsForBrains::MarkAttractorScriptBrainWithThisNameAsNoLongerNeeded(const char* name) {
    if (const auto idx = GetIndexOfScriptBrainWithThisName(name, CScriptsForBrains::CODE_ATTRACTOR_PED); idx >= 0) {
        const auto streamedScriptIndex = m_aScriptForBrains[idx].m_StreamedScriptIndex;
        auto&      numberOfUsers       = CTheScripts::StreamedScripts.m_aScripts[streamedScriptIndex].m_NumberOfUsers;
        if (numberOfUsers > 0) {
            --numberOfUsers;
        }
    }
}

// 0x46AA80
void CScriptsForBrains::RequestAttractorScriptBrainWithThisName(const char* name) {
    auto idx = GetIndexOfScriptBrainWithThisName(name, CScriptsForBrains::CODE_ATTRACTOR_PED);
    if (idx < 0) {
        return;
    }
    const auto streamedScriptIndex = m_aScriptForBrains[idx].m_StreamedScriptIndex;
    if (!CStreaming::IsModelLoaded(SCMToModelId(streamedScriptIndex))) { // BUGFIX: originally IsModelLoaded(streamedScriptIndex)
        CStreaming::RequestModel(SCMToModelId(streamedScriptIndex), STREAMING_PRIORITY_REQUEST);
    }
    ++CTheScripts::StreamedScripts.m_aScripts[streamedScriptIndex].m_NumberOfUsers;
}

// 0x46B270
void CScriptsForBrains::StartNewStreamedScriptBrain(uint8 index, CEntity* entity, bool bPedCreatedAtAttractor) {
    const auto& brain  = m_aScriptForBrains[index];
    const auto  script = CTheScripts::StreamedScripts.StartNewStreamedScript(brain.m_StreamedScriptIndex);

    assert(script);
    script->m_ScriptBrainType = brain.m_TypeOfBrain;

    switch (brain.m_TypeOfBrain) {
    case CScriptsForBrains::PED_STREAMED:
    case CScriptsForBrains::CODE_PED:
    case CScriptsForBrains::CODE_ATTRACTOR_PED: {
        const auto ped                    = entity->AsPed();

        ped->bHasAScriptBrain             = true;
        ped->bWaitingForScriptBrainToLoad = false;

        script->m_LocalVars[0].uParam     = GetPedPool()->GetRef(ped);
        if (brain.m_TypeOfBrain == CScriptsForBrains::CODE_ATTRACTOR_PED) {
            script->m_LocalVars[1].bParam = bPedCreatedAtAttractor;
        }

        const auto scriptBrainToLoad = ped->m_StreamedScriptBrainToLoad;
        CTheScripts::RemoveFromWaitingForScriptBrainArray(ped, scriptBrainToLoad);
        ped->m_StreamedScriptBrainToLoad = -1;
        break;
    }
    case CScriptsForBrains::OBJECT_STREAMED:
    case CScriptsForBrains::CODE_OBJECT:     {
        const auto object                      = entity->AsObject();

        script->m_LocalVars[0].uParam          = GetObjectPool()->GetRef(object);
        object->objectFlags.bScriptBrainStatus = 2;
        break;
    }
    }
}

// 0x46CED0
void CScriptsForBrains::StartOrRequestNewStreamedScriptBrainWithThisName(const char* name, CEntity* entity, int8 brainType) {
    const auto idx = GetIndexOfScriptBrainWithThisName(name, brainType);
    if (idx >= 0) {
        StartOrRequestNewStreamedScriptBrain(static_cast<uint8>(idx), entity, brainType, true);
    }
}

// 0x46CD80
void CScriptsForBrains::StartOrRequestNewStreamedScriptBrain(uint8 index, CEntity* entity, int8 brainType, bool bFirstTime) {

    // todo: verify
    switch (brainType) {
    case CScriptsForBrains::OBJECT_STREAMED: {
        const auto object = entity->AsObject();
        if (bFirstTime) {
            object->objectFlags.bScriptBrainStatus = 1;
            object->m_nStreamedScriptBrainToLoad   = index;
            CTheScripts::AddToWaitingForScriptBrainArray(object, index);
        } else {
            object->objectFlags.bScriptBrainStatus = 2;
        }
        break;
    }
    case CScriptsForBrains::PED_STREAMED:
    case CScriptsForBrains::CODE_PED:
    case CScriptsForBrains::CODE_ATTRACTOR_PED: {
        const auto ped = entity->AsPed();
        if (bFirstTime) {
            ped->bWaitingForScriptBrainToLoad = true;
            ped->m_StreamedScriptBrainToLoad  = index;
            CTheScripts::AddToWaitingForScriptBrainArray(ped, index);
        } else {
            ped->bHasAScriptBrain             = true;
            ped->bWaitingForScriptBrainToLoad = false;
        }
        break;
    }
    }
    const auto& brain = m_aScriptForBrains[index];
    if (!brain.m_bBrainActive) {
        return;
    }
    const auto modelIdx = SCMToModelId(brain.m_StreamedScriptIndex);
    if (CStreaming::IsModelLoaded(modelIdx)) {
        StartNewStreamedScriptBrain(index, entity, false);
    } else {
        CStreaming::RequestModel(modelIdx, STREAMING_MISSION_REQUIRED);
    }
}

bool CScriptsForBrains::HasAttractorScriptBrainWithThisNameLoaded(const char* name) {
    if (const auto idx = GetIndexOfScriptBrainWithThisName(name, CScriptsForBrains::CODE_ATTRACTOR_PED); idx >= 0) {
        return CStreaming::IsModelLoaded(SCMToModelId(m_aScriptForBrains[idx].m_StreamedScriptIndex));
    }
    return false;
}

// 0x46B3D0
bool CScriptsForBrains::IsObjectWithinBrainActivationRange(CObject* entity, const CVector& point) {
    const auto& brain = m_aScriptForBrains[entity->m_nStreamedScriptBrainToLoad];
    if (brain.m_TypeOfBrain != CScriptsForBrains::OBJECT_STREAMED) {
        return false;
    }

    return ((point - entity->GetPosition()).Magnitude() < brain.m_ObjectBrainActivationRadius);
}

int16 CScriptsForBrains::GetIndexOfScriptBrainWithThisName(const char* name, int8 brainType) {
    const auto it = rng::find_if(m_aScriptForBrains, [=](tScriptForBrains& script) {
        return script.m_TypeOfBrain == brainType && !_stricmp(script.m_ScriptName, name);
    });
    return it != m_aScriptForBrains.end()
        ? rng::distance(m_aScriptForBrains.begin(), it)
        : -1;
}

void CScriptsForBrains::StartAttractorScriptBrainWithThisName(const char* name, CPed* ped, bool bHasAScriptBrain) {
    if (!ped->bWaitingForScriptBrainToLoad && !ped->bHasAScriptBrain) {
        if (const auto idx = GetIndexOfScriptBrainWithThisName(name, CScriptsForBrains::CODE_ATTRACTOR_PED); idx >= 0) {
            StartNewStreamedScriptBrain(static_cast<uint8>(idx), ped, bHasAScriptBrain);
        }
    }
}
