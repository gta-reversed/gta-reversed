#include "StdInc.h"

#include "ScriptResourceManager.h"

void CScriptResourceManager::InjectHooks() {
    RH_ScopedClass(CScriptResourceManager);
    RH_ScopedCategory("Scripts");

    RH_ScopedInstall(Initialise, 0x470480);
    RH_ScopedInstall(AddToResourceManager, 0x4704B0);
    RH_ScopedInstall(RemoveFromResourceManager, 0x470510);
    RH_ScopedInstall(HasResourceBeenRequested, 0x470620);
    //RH_ScopedInstall(Load, 0x0, { .Reversed = false });
    //RH_ScopedInstall(Save, 0x0, { .Reversed = false });
}

// 0x470480
void CScriptResourceManager::Initialise() {
    m_aScriptResources.fill(tScriptResource());
}

// 0x4704B0
void CScriptResourceManager::AddToResourceManager(int32 modelId, eScriptResourceType type, CRunningScript* script) {
    tScriptResource* freeSlot{};
    for (auto& res : m_aScriptResources) {
        if (res.m_nModelId == modelId && res.m_nType == type && res.m_pThread == script) {
            return; // Don't allow duplicates
        }
        if (res.m_nType == RESOURCE_TYPE_DEFAULT) {
            freeSlot = &res; // NOTE: Ends up being the *last* free slot
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
    tScriptResource* ours{}; // NOTE: The original keeps the last match
    auto numMatchesInOtherScripts = 0u;
    for (auto& res : m_aScriptResources) {
        if (res.m_nModelId != modelId || res.m_nType != type) {
            continue;
        }
        if (res.m_pThread == script) {
            assert(!ours && "There can be only 1 match for a resource");
            ours = &res;
        } else {
            numMatchesInOtherScripts++;
        }
    }
    if (ours) {
        *ours = tScriptResource{}; // Resets to `-1`, `RESOURCE_TYPE_DEFAULT`, `nullptr`
    }
    return numMatchesInOtherScripts == 0; // Returns whenever this type+model was requested by any other script (Same logic as `HasResourceBeenRequested`)
}

// 0x470620
bool CScriptResourceManager::HasResourceBeenRequested(int32 modelId, eScriptResourceType type) {
    return rng::any_of(m_aScriptResources, [&](const tScriptResource& res) {
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
