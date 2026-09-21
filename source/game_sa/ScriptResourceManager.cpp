#include "StdInc.h"

#include "ScriptResourceManager.h"

void CScriptResourceManager::InjectHooks() {
    RH_ScopedClass(CScriptResourceManager);
    RH_ScopedCategory("Scripts");

    RH_ScopedInstall(Initialise, 0x470480);
    RH_ScopedInstall(AddToResourceManager, 0x4704B0);
    RH_ScopedInstall(RemoveFromResourceManager, 0x470510);
    RH_ScopedInstall(HasResourceBeenRequested, 0x470620);
    //RH_ScopedInstall(Load, 0x0, { .reversed = false });
    //RH_ScopedInstall(Save, 0x0, { .reversed = false });
}

// 0x470480
void CScriptResourceManager::Initialise() {
    m_aScriptResources.fill(tScriptResource());
}

// 0x4704B0
void CScriptResourceManager::AddToResourceManager(int32 modelId, eScriptResourceType type, CRunningScript* script) {
    // NOTE: The vanilla code uses the last free slot it comes across (not the first one)
    tScriptResource* freeSlot{};
    for (auto& res : m_aScriptResources) {
        if (res.m_nModelId == modelId && res.m_nType == type && res.m_pThread == script) {
            return; // Already requested by this very script
        }
        if (res.m_nType == RESOURCE_TYPE_DEFAULT) {
            freeSlot = &res;
        }
    }
    if (freeSlot) {
        freeSlot->m_nModelId = modelId;
        freeSlot->m_nType    = type;
        freeSlot->m_pThread  = script;
    }
}

// 0x470510
bool CScriptResourceManager::RemoveFromResourceManager(int32 modelId, eScriptResourceType type, CRunningScript* script) {
    int32  removeIdx{ -1 };
    uint32 otherScriptsCount{};

    for (const auto& [i, res] : rngv::enumerate(m_aScriptResources)) {
        if (res.m_nModelId != modelId || res.m_nType != type) {
            continue;
        }
        if (res.m_pThread == script) {
            removeIdx = (int32)i; // The last matching entry wins
        } else {
            otherScriptsCount++;
        }
    }

    if (removeIdx != -1) {
        m_aScriptResources[removeIdx] = tScriptResource();
    }

    // It only counts as removed once no other script references it anymore
    return otherScriptsCount == 0;
}

// 0x470620
bool CScriptResourceManager::HasResourceBeenRequested(int32 modelId, eScriptResourceType type) {
    return rng::any_of(m_aScriptResources, [=](const tScriptResource& res) {
        return res.m_nModelId == modelId && res.m_nType == type;
    });
}

// 0x0
bool CScriptResourceManager::Load() {
    assert(false);
    return true;
}

// 0x0
bool CScriptResourceManager::Save() {
    assert(false);
    return true;
}
