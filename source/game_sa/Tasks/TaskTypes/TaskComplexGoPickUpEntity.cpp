#include "StdInc.h"
#include "TaskComplexGoPickUpEntity.h"

void CTaskComplexGoPickUpEntity::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexGoPickUpEntity, 0x870b98, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x6919C0, "1", {}, CEntity*, AssocGroupId);
    RH_ScopedDestructorInstall(0x691A50);

    RH_ScopedVMTInstall(Clone, 0x692C80);
    RH_ScopedVMTInstall(GetTaskType, 0x691A40);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x691AE0, { .Reversed = false });
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x693610, { .Reversed = false });
    RH_ScopedVMTInstall(ControlSubTask, 0x691D50, { .Reversed = false });
}

// 0x6919C0
CTaskComplexGoPickUpEntity::CTaskComplexGoPickUpEntity(CEntity* pEntity, AssocGroupId animGroupId) :
    m_AnimGroupId{ animGroupId },
    m_EntityToPickUp{ pEntity } {
}

// 0x692C80
CTaskComplexGoPickUpEntity::CTaskComplexGoPickUpEntity(const CTaskComplexGoPickUpEntity& o) :
    CTaskComplexGoPickUpEntity{ o.m_EntityToPickUp, o.m_AnimGroupId } {
}

// 0x691A50
CTaskComplexGoPickUpEntity::~CTaskComplexGoPickUpEntity() {
    if (m_IsAnimBlockReferenced) {
        CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex(m_AnimGroupId));
    }
}

// 0x691AE0
CTask* CTaskComplexGoPickUpEntity::CreateNextSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x691AE0, CTaskComplexGoPickUpEntity*, CPed*>(this, ped);
}

// 0x693610
CTask* CTaskComplexGoPickUpEntity::CreateFirstSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x693610, CTaskComplexGoPickUpEntity*, CPed*>(this, ped);
}

// 0x691D50
CTask* CTaskComplexGoPickUpEntity::ControlSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x691D50, CTaskComplexGoPickUpEntity*, CPed*>(this, ped);
}
