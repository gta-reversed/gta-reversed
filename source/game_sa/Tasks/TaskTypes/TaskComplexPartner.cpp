#include "StdInc.h"

#include "TaskComplexPartner.h"

void CTaskComplexPartner::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexPartner, 0x870664, 14);
    RH_ScopedCategory("Tasks/TaskTypes");
  
    RH_ScopedConstructorInstall(0x681E70, "1", {}, char *, CPed *, uint8, float, uint8, int32, CVector);
    RH_ScopedDestructorInstall(0x683A40);

    RH_ScopedInstall(CalcTargetPositions, 0x681FE0, { .Reversed = false });
    RH_ScopedInstall(GetPartnerState, 0x6822B0, { .Reversed = false });
    RH_ScopedVMTInstall(GetTaskType, 0x681F10);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x683AD0, { .Reversed = false });
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x681F20, { .Reversed = false });
    RH_ScopedVMTInstall(ControlSubTask, 0x6840D0, { .Reversed = false });
    RH_ScopedVMTInstall(StreamRequiredAnims, 0x682310, { .Reversed = false });
    RH_ScopedVMTInstall(RemoveStreamedAnims, 0x682370, { .Reversed = false });
}

// 0x681E70
CTaskComplexPartner::CTaskComplexPartner(const char* commandName, CPed* partner, bool leadSpeaker, float distanceMultiplier, bool makePedAlwaysFacePartner, int8 updateDirectionCount, CVector point) :
    m_leadSpeaker{ leadSpeaker },
    m_makePedAlwaysFacePartner{ makePedAlwaysFacePartner },
    m_distanceMultiplier{ distanceMultiplier },
    m_updateDirectionCount{ updateDirectionCount },
    m_point{ point },
    m_partner{ partner } {
}

// 0x683A40
CTaskComplexPartner::~CTaskComplexPartner() {
    if (m_requiredAnimsStreamedIn) {
        if (strcmp(m_animBlockName, "") != 0) {
            CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex(m_animBlockName));
        }
        m_requiredAnimsStreamedIn = false;
    }
}


// 0x681FE0
void CTaskComplexPartner::CalcTargetPositions(CPed * ped, CVector * point, CVector * targetPoint) {
    return plugin::CallMethodAndReturn<void, 0x681FE0, CTaskComplexPartner*, CPed *, CVector *, CVector *>(this, ped, point, targetPoint);
}

// 0x6822B0
int32 CTaskComplexPartner::GetPartnerState() {
    return plugin::CallMethodAndReturn<int32, 0x6822B0, CTaskComplexPartner*>(this);
}

// 0x683AD0
CTask * CTaskComplexPartner::CreateNextSubTask(CPed * ped) {
    return plugin::CallMethodAndReturn< CTask *, 0x683AD0, CTaskComplexPartner*, CPed *>(this, ped);
}

// 0x681F20
CTask * CTaskComplexPartner::CreateFirstSubTask(CPed * ped) {
    return plugin::CallMethodAndReturn< CTask *, 0x681F20, CTaskComplexPartner*, CPed *>(this, ped);
}

// 0x6840D0
CTask * CTaskComplexPartner::ControlSubTask(CPed * ped) {
    return plugin::CallMethodAndReturn< CTask *, 0x6840D0, CTaskComplexPartner*, CPed *>(this, ped);
}

// 0x682310
void CTaskComplexPartner::StreamRequiredAnims() {
    return plugin::CallMethodAndReturn<void, 0x682310, CTaskComplexPartner*>(this);
}

// 0x682370
void CTaskComplexPartner::RemoveStreamedAnims() {
    return plugin::CallMethodAndReturn<void, 0x682370, CTaskComplexPartner*>(this);
}
