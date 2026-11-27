#include "StdInc.h"

#include "ScriptsForBrains.h"

// 0x46A8C0
void CScriptsForBrains::Init() {
    m_aScriptForBrains.fill({});
}

// 0x46A900
void CScriptsForBrains::SwitchAllObjectBrainsWithThisID(int8 objectGroupingId, bool isBrainActive) {
    if (objectGroupingId < 0) {
        return;
    }
    for (auto& brain : m_aScriptForBrains) {
        if (brain.m_ObjectGroupingId == objectGroupingId) {
            brain.m_IsBrainActive = isBrainActive;
        }
    }
}

// 0x46A930
void CScriptsForBrains::AddNewScriptBrain(int16 streamedScriptIndex, int16 modelOrPedGeneratorIndex, uint16 percentageChance, int8 brainType, int8 objectGroupingId, float objectActivationRadius) {
    const auto brain = rng::find(m_aScriptForBrains, -1, &tScriptForBrains::m_StreamedScriptIndex);
    if (brain == m_aScriptForBrains.end()) {
        return;
    }
    brain->m_StreamedScriptIndex         = streamedScriptIndex;
    brain->m_PedModelOrPedGeneratorIndex = modelOrPedGeneratorIndex;
    brain->m_PercentageChance            = percentageChance;
    brain->m_ObjectGroupingId            = objectGroupingId;
    brain->m_TypeOfBrain                 = brainType;
    brain->m_IsBrainActive               = true;
    brain->m_ObjectBrainActivationRadius = objectActivationRadius <= 0.f
        ? 5.f
        : objectActivationRadius;
}

// 0x46A9C0
void CScriptsForBrains::AddNewStreamedScriptBrainForCodeUse(int16 streamedScriptIndex, const char* scriptName, int8 brainType) {
    const auto brain = rng::find(m_aScriptForBrains, -1, &tScriptForBrains::m_StreamedScriptIndex);
    if (brain == m_aScriptForBrains.end()) {
        return;
    }
    brain->m_StreamedScriptIndex = streamedScriptIndex;
    if (notsa::bugfixes::GenericOOB) {
        const auto len = std::min(std::strlen(scriptName), std::size(brain->m_ScriptName) - 1);
        std::memcpy(brain->m_ScriptName, scriptName, len);
        brain->m_ScriptName[len] = '\0';
    } else {
        strcpy(brain->m_ScriptName, scriptName); // NOTE/BUG: Unbounded copy into the 8 byte name
    }
    brain->m_TypeOfBrain                 = brainType;
    brain->m_ObjectGroupingId            = -1;
    brain->m_IsBrainActive               = true;
    brain->m_ObjectBrainActivationRadius = 5.f;
}

// 0x46AA30
int16 CScriptsForBrains::GetIndexOfScriptBrainWithThisName(const char* name, int8 brainType) {
    const auto it = rng::find_if(m_aScriptForBrains, [=](tScriptForBrains& brain) {
        return brain.m_TypeOfBrain == brainType && !_stricmp(brain.m_ScriptName, name);
    });
    return it != m_aScriptForBrains.end()
        ? rng::distance(m_aScriptForBrains.begin(), it)
        : -1;
}

// 0x46AA80
void CScriptsForBrains::RequestAttractorScriptBrainWithThisName(const char* name) {
    const auto idx = GetIndexOfScriptBrainWithThisName(name, SCRIPT_BRAIN_TYPE_CODE_ATTRACTOR_PED);
    if (idx < 0) {
        return;
    }
    const auto scriptIdx = m_aScriptForBrains[idx].m_StreamedScriptIndex;
    if (!CStreaming::IsModelLoaded(SCMToModelId(scriptIdx))) {
        CStreaming::RequestModel(SCMToModelId(scriptIdx), STREAMING_PRIORITY_REQUEST);
    }
    CTheScripts::StreamedScripts.m_aScripts[scriptIdx].m_NumberOfUsers++;
}

// 0x46AAE0
void CScriptsForBrains::MarkAttractorScriptBrainWithThisNameAsNoLongerNeeded(const char* name) {
    const auto idx = GetIndexOfScriptBrainWithThisName(name, SCRIPT_BRAIN_TYPE_CODE_ATTRACTOR_PED);
    if (idx < 0) {
        return;
    }
    auto& numUsers = CTheScripts::StreamedScripts.m_aScripts[m_aScriptForBrains[idx].m_StreamedScriptIndex].m_NumberOfUsers;
    if (numUsers > 0) {
        numUsers--;
    }
}

// 0x46AB20
bool CScriptsForBrains::HasAttractorScriptBrainWithThisNameLoaded(const char* name) {
    if (const auto idx = GetIndexOfScriptBrainWithThisName(name, SCRIPT_BRAIN_TYPE_CODE_ATTRACTOR_PED); idx >= 0) {
        return CStreaming::IsModelLoaded(SCMToModelId(m_aScriptForBrains[idx].m_StreamedScriptIndex));
    }
    return false;
}

// 0x46B270
void CScriptsForBrains::StartNewStreamedScriptBrain(uint8 index, CEntity* entity, bool useAttractorInstantly) {
    const auto& brain  = m_aScriptForBrains[index];

    auto* const script = CTheScripts::StreamedScripts.StartNewStreamedScript(brain.m_StreamedScriptIndex);
    if (notsa::bugfixes::GenericCrashing && !script) {
        return;
    }
    script->m_ExternalType = brain.m_TypeOfBrain;

    switch (brain.m_TypeOfBrain) {
    case SCRIPT_BRAIN_TYPE_PED_STREAMED:
    case SCRIPT_BRAIN_TYPE_CODE_PED:
    case SCRIPT_BRAIN_TYPE_CODE_ATTRACTOR_PED: { // 0x46B2AB
        auto* const ped               = entity->AsPed();

        script->m_LocalVars[0].iParam = GetPedPool()->GetRef(ped);
        ped->bHasAScriptBrain         = true;
        if (brain.m_TypeOfBrain == SCRIPT_BRAIN_TYPE_CODE_ATTRACTOR_PED) {
            script->m_LocalVars[1].iParam = useAttractorInstantly;
        }
        ped->bWaitingForScriptBrainToLoad = false;
        CTheScripts::RemoveFromWaitingForScriptBrainArray(ped, ped->m_StreamedScriptBrainToLoad);
        ped->m_StreamedScriptBrainToLoad = -1;
        break;
    }
    case SCRIPT_BRAIN_TYPE_OBJECT_STREAMED:
    case SCRIPT_BRAIN_TYPE_CODE_OBJECT:     { // 0x46B32A
        auto* const obj                     = entity->AsObject();

        script->m_LocalVars[0].iParam       = GetObjectPool()->GetRef(obj);
        obj->objectFlags.b0x100000_0x200000 = OBJECT_SCRIPT_BRAIN_RUNNING;
        break;
    }
    case SCRIPT_BRAIN_TYPE_PED_GENERATOR_STREAMED: // 0x46B364
        break;
    }
}

// 0x46B390
void CScriptsForBrains::StartAttractorScriptBrainWithThisName(const char* name, CPed* ped, bool useAttractorInstantly) {
    if (ped->bWaitingForScriptBrainToLoad || ped->bHasAScriptBrain) {
        return;
    }
    if (const auto idx = GetIndexOfScriptBrainWithThisName(name, SCRIPT_BRAIN_TYPE_CODE_ATTRACTOR_PED); idx >= 0) {
        StartNewStreamedScriptBrain((uint8)(idx), ped, useAttractorInstantly);
    }
}

// 0x46B3D0
bool CScriptsForBrains::IsObjectWithinBrainActivationRange(CObject* object, const CVector& point) {
    const auto& brain = m_aScriptForBrains[object->m_nStreamedScriptBrainToLoad];
    if (brain.m_TypeOfBrain != SCRIPT_BRAIN_TYPE_OBJECT_STREAMED) {
        return false;
    }
    return (point - object->GetPosition()).Magnitude() < brain.m_ObjectBrainActivationRadius; // NOTE: 3D distance, not `CVector::Dist` (2D on master)
}

// 0x46CD80
void CScriptsForBrains::StartOrRequestNewStreamedScriptBrain(uint8 index, CEntity* entity, int8 brainType, bool isFirstTime) {
    const auto& brain = m_aScriptForBrains[index];

    if (isFirstTime) {
        if (!brain.m_IsBrainActive) {
            return;
        }
        switch (brainType) {
        case SCRIPT_BRAIN_TYPE_PED_STREAMED:
        case SCRIPT_BRAIN_TYPE_CODE_PED:     { // 0x46CDB8
            const auto* const ped = entity->AsPed();
            if (ped->bHasAScriptBrain || ped->bWaitingForScriptBrainToLoad) {
                return;
            }
            break;
        }
        case SCRIPT_BRAIN_TYPE_OBJECT_STREAMED:
        case SCRIPT_BRAIN_TYPE_CODE_OBJECT:     { // 0x46CDD6
            if (entity->AsObject()->objectFlags.b0x100000_0x200000 != OBJECT_SCRIPT_BRAIN_NONE) {
                return;
            }
            break;
        }
        default: // 0x46CDE6
            break;
        }
    }

    if (brainType == SCRIPT_BRAIN_TYPE_OBJECT_STREAMED) { // 0x46CDE6
        auto* const obj = entity->AsObject();
        if (isFirstTime) {
            obj->objectFlags.b0x100000_0x200000 = OBJECT_SCRIPT_BRAIN_WAITING_FOR_PLAYER;
            obj->m_nStreamedScriptBrainToLoad   = index;
            CTheScripts::AddToWaitingForScriptBrainArray(obj, index);
        } else {
            obj->objectFlags.b0x100000_0x200000 = OBJECT_SCRIPT_BRAIN_LOADING;
        }
    }

    const auto scriptModel = SCMToModelId(brain.m_StreamedScriptIndex);
    if (CStreaming::IsModelLoaded(scriptModel)) {
        StartNewStreamedScriptBrain(index, entity, false);
        return;
    }
    CStreaming::RequestModel(scriptModel, STREAMING_MISSION_REQUIRED);

    switch (brainType) {
    case SCRIPT_BRAIN_TYPE_CODE_OBJECT:    // 0x46CE7C
        if (notsa::bugfixes::GenericOOB) { // The original writes the ped fields below into the object
            break;
        }
        [[fallthrough]];
    case SCRIPT_BRAIN_TYPE_PED_STREAMED:
    case SCRIPT_BRAIN_TYPE_CODE_PED:     { // 0x46CE80
        if (!isFirstTime) {
            break;
        }
        auto* const ped                   = entity->AsPed();
        ped->bWaitingForScriptBrainToLoad = true;
        ped->m_StreamedScriptBrainToLoad  = index;
        CTheScripts::AddToWaitingForScriptBrainArray(ped, index);
        break;
    }
    default: // 0x46CEAF
        break;
    }
}

// 0x46CED0
void CScriptsForBrains::StartOrRequestNewStreamedScriptBrainWithThisName(const char* name, CEntity* entity, int8 brainType) {
    if (const auto idx = GetIndexOfScriptBrainWithThisName(name, brainType); idx >= 0) {
        StartOrRequestNewStreamedScriptBrain((uint8)(idx), entity, brainType, true);
    }
}

// 0x46FF20
void CScriptsForBrains::CheckIfNewEntityNeedsScript(CEntity* entity, int8 brainType, void* unused) {
    if (brainType == SCRIPT_BRAIN_TYPE_PED_STREAMED) {
        const auto* const ped = entity->AsPed();
        if (ped->bHasAScriptBrain || ped->bWaitingForScriptBrainToLoad) {
            return;
        }
    } else if (entity->AsObject()->objectFlags.b0x100000_0x200000 != OBJECT_SCRIPT_BRAIN_NONE) { // NOTE: Every other type is checked as an object
        return;
    }

    for (auto&& [i, brain] : rngv::enumerate(m_aScriptForBrains)) {
        if (brain.m_TypeOfBrain != brainType || brain.m_PedModelOrPedGeneratorIndex != (int16)(entity->GetModelIndex())) {
            continue;
        }
        // NOTE: Not `CGeneral::RandomBool` - the original truncates the roll to [0, 99] and compares with `<`
        if ((int32)((float)(CGeneral::GetRandomNumber()) * (1.f / 32768.f) * 100.f) < brain.m_PercentageChance) {
            StartOrRequestNewStreamedScriptBrain((uint8)(i), entity, brainType, true);
            break;
        }
    }
}

void CScriptsForBrains::InjectHooks() {
    RH_ScopedClass(CScriptsForBrains);
    RH_ScopedCategory("Scripts");

    RH_ScopedInstall(Init, 0x46A8C0);
    RH_ScopedInstall(SwitchAllObjectBrainsWithThisID, 0x46A900);
    RH_ScopedInstall(AddNewScriptBrain, 0x46A930);
    RH_ScopedInstall(AddNewStreamedScriptBrainForCodeUse, 0x46A9C0);
    RH_ScopedInstall(GetIndexOfScriptBrainWithThisName, 0x46AA30);
    RH_ScopedInstall(RequestAttractorScriptBrainWithThisName, 0x46AA80);
    RH_ScopedInstall(MarkAttractorScriptBrainWithThisNameAsNoLongerNeeded, 0x46AAE0);
    RH_ScopedInstall(HasAttractorScriptBrainWithThisNameLoaded, 0x46AB20);
    RH_ScopedInstall(StartNewStreamedScriptBrain, 0x46B270);
    RH_ScopedInstall(StartAttractorScriptBrainWithThisName, 0x46B390);
    RH_ScopedInstall(IsObjectWithinBrainActivationRange, 0x46B3D0);
    RH_ScopedInstall(StartOrRequestNewStreamedScriptBrain, 0x46CD80);
    RH_ScopedInstall(StartOrRequestNewStreamedScriptBrainWithThisName, 0x46CED0);
    RH_ScopedInstall(CheckIfNewEntityNeedsScript, 0x46FF20);
}
