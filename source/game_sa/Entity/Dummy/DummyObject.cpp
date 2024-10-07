#include "StdInc.h"

#include "DummyObject.h"
#include "TheScripts.h"

void CDummyObject::InjectHooks() {
    RH_ScopedVirtualClass(CDummyObject, 0x866E78, 22);
    RH_ScopedCategory("Entity/Dummy");

    RH_ScopedInstall(CreateObject, 0x59EAC0);
    RH_ScopedInstall(UpdateFromObject, 0x59EB70);
}

// 0x59EA00
CDummyObject::CDummyObject() : CDummy() {
    // NOP
}

// 0x59EA20
CDummyObject::CDummyObject(CObject* obj) : CDummy() {
    CEntity::SetModelIndexNoCreate(m_nModelIndex);
    if (obj->m_pRwObject)
        CEntity::AttachToRwObject(obj->m_pRwObject, true);

    obj->DetachFromRwObject();
    m_nIplIndex = obj->m_nIplIndex;
    m_nAreaCode = obj->m_nAreaCode;
    CIplStore::IncludeEntity(m_nIplIndex, this);
}

// 0x59EAC0
CObject* CDummyObject::CreateObject() {
    m_bImBeingRendered = true; // BUG? Seems like that flag doesn't fit here
    auto* obj = CObject::Create(this);
    m_bImBeingRendered = false;

    if (obj) {
        CTheScripts::ScriptsForBrains.CheckIfNewEntityNeedsScript(obj, 1, nullptr);
        m_bIsVisible = false;
        m_bUsesCollision = false;

        obj->m_nLodIndex = m_nLodIndex;
        m_nLodIndex = 0;
    }

    return obj;
}

// 0x59EB70
void CDummyObject::UpdateFromObject(CObject* obj) {
    m_bIsVisible = true;
    m_bUsesCollision = true;

    obj->m_bImBeingRendered = true;
    CEntity::AttachToRwObject(obj->m_pRwObject, false);
    obj->m_bImBeingRendered = false;
    CEntity::UpdateRW();
    obj->DetachFromRwObject();

    if (obj->m_nIplIndex && CIplStore::HasDynamicStreamingDisabled(obj->m_nIplIndex)) {
        m_bRenderDamaged = obj->m_bRenderDamaged;
        m_bIsVisible = obj->m_bIsVisible;
        m_bUsesCollision = obj->m_bUsesCollision;
    }

    m_nLodIndex = obj->m_nLodIndex;
    obj->m_nLodIndex = 0;
}
