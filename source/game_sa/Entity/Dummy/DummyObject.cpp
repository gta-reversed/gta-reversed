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
    CEntity::SetModelIndexNoCreate(GetModelIndex());
    if (obj->GetRwObject())
        CEntity::AttachToRwObject(obj->GetRwObject(), true);

    obj->DetachFromRwObject();
    SetIplIndex(obj->GetIplIndex());
    SetAreaCode(obj->GetAreaCode());
    CIplStore::IncludeEntity(GetIplIndex(), this);
}

// 0x59EAC0
CObject* CDummyObject::CreateObject() {
    m_bImBeingRendered = true; // BUG? Seems like that flag doesn't fit here
    auto* obj = CObject::Create(this);
    m_bImBeingRendered = false;

    if (obj) {
        CTheScripts::ScriptsForBrains.CheckIfNewEntityNeedsScript(obj, 1, nullptr);
        SetIsVisible(false);
        SetUsesCollision(false);

        obj->SetLod(GetLod());
        SetLod(nullptr);
    }

    return obj;
}

// 0x59EB70
void CDummyObject::UpdateFromObject(CObject* obj) {
    SetIsVisible(true);
    SetUsesCollision(true);

    obj->m_bImBeingRendered = true;
    CEntity::AttachToRwObject(obj->GetRwObject(), false);
    obj->m_bImBeingRendered = false;
    CEntity::UpdateRwMatrix();
    obj->DetachFromRwObject();

    if (obj->GetIplIndex() && CIplStore::HasDynamicStreamingDisabled(obj->GetIplIndex())) {
        m_bRenderDamaged = obj->m_bRenderDamaged;
        SetIsVisible(obj->GetIsVisible());
        SetUsesCollision(obj->GetUsesCollision());
    }

    SetLod(obj->GetLod());
    obj->SetLod(nullptr);
}
