#include "StdInc.h"

#include "ScriptResourceManager.h"

void CScriptResourceManager::InjectHooks() {
    RH_ScopedClass(CScriptResourceManager);
    RH_ScopedCategory("Scripts");

    RH_ScopedInstall(Initialise, 0x470480);
    RH_ScopedInstall(AddToResourceManager, 0x4704B0, { .Reversed = false });
    RH_ScopedInstall(RemoveFromResourceManager, 0x470510, { .Reversed = false });
    RH_ScopedInstall(HasResourceBeenRequested, 0x470620, { .Reversed = false });
    //RH_ScopedInstall(Load, 0x0, { .Reversed = false });
    //RH_ScopedInstall(Save, 0x0, { .Reversed = false });
}

// 0x470480
void CScriptResourceManager::Initialise() {
    m_aScriptResources.fill(tScriptResource());
}

// 0x4704B0
void CScriptResourceManager::AddToResourceManager(int32 modelId, eScriptResourceType type, CRunningScript* script) {
    plugin::Call<0x4704B0, int32, eScriptResourceType, CRunningScript*>(modelId, type, script);
}

// 0x470510
bool CScriptResourceManager::RemoveFromResourceManager(int32 modelId, eScriptResourceType type, CRunningScript* script) {
    return plugin::CallAndReturn<bool, 0x470510, int32, eScriptResourceType, CRunningScript*>(modelId, type, script);
}

// 0x470620
bool CScriptResourceManager::HasResourceBeenRequested(int32 modelId, eScriptResourceType type) {
    return plugin::CallAndReturn<bool, 0x470620, int32, eScriptResourceType>(modelId, type);
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
