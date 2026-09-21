#include "StdInc.h"

#include "TaskComplexGangFollower.h"

void CTaskComplexGangFollower::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexGangFollower, 0x86F938, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x65EAA0);
    RH_ScopedInstall(Destructor, 0x65EBB0);
    //RH_ScopedInstall(CalculateOffsetPosition, 0x65ED40, { .reversed = false }); // not hooked because i want to keep CVector return, but original function took a CVector&
    RH_ScopedInstall(Clone, 0x65ECB0);
    RH_ScopedInstall(MakeAbortable, 0x65EC30);
    RH_ScopedInstall(CreateNextSubTask, 0x665E00, { .reversed = false });
    RH_ScopedInstall(CreateFirstSubTask, 0x666160, { .reversed = false });
    RH_ScopedInstall(ControlSubTask, 0x662A10, { .reversed = false });
}

// 0x65EAA0
CTaskComplexGangFollower::CTaskComplexGangFollower(CPedGroup* pedGroup, CPed* ped, uint8 a4, CVector pos, float a6) : CTaskComplex() {
    dword40 = 0;
    dword44 = 0;
    byte48 = 0;
    byte49 = 0;
    m_PedGroup = pedGroup;
    byte3C = a4;
    dword2C = pos;
    dword20 = pos;
    dword38 = a6;
    m_Leader = ped;
    m_Flags |= 0x14;
    if (ped) {
        CEntity::RegisterReference(m_Leader);
        m_PedPosn = ped->GetPosition();
    }
    m_Flags &= 0xFC;
    if (m_Leader == FindPlayerPed(0)) {
        m_Flags |= 8;
    } else {
        m_Flags &= ~8;
    }
}


// 0x65EBB0
CTaskComplexGangFollower::~CTaskComplexGangFollower() {
    if (m_Leader) {
        CEntity::CleanUpOldReference(m_Leader);
    }
    if (m_Flags & 1) {
        CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex("gangs"));
        m_Flags &= ~1;
    }
}

// 0x65ED40
CVector CTaskComplexGangFollower::CalculateOffsetPosition() {
    CVector ret;
    plugin::CallMethod<0x65ED40, CTaskComplexGangFollower*>(this, &ret);
    return ret;
}

// 0x65ECB0
CTask* CTaskComplexGangFollower::Clone() const {
    const auto clone = new CTaskComplexGangFollower{ m_PedGroup, m_Leader, byte3C, dword2C, dword38 };
    clone->m_Flags = (clone->m_Flags & ~4) | (m_Flags & 4);
    return clone;
}
// 0x65EC30
bool CTaskComplexGangFollower::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (!m_pSubTask) {
        ped->bDontAcceptIKLookAts = false;
        ped->bMoveAnimSpeedHasBeenSetByTask = false;
        return true;
    }
    if (!m_pSubTask->MakeAbortable(ped, priority, event)) {
        return false;
    }
    ped->bMoveAnimSpeedHasBeenSetByTask = false;
    ped->bDontAcceptIKLookAts = false;
    return true;
}

// 0x665E00
CTask* CTaskComplexGangFollower::CreateNextSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x665E00, CTaskComplexGangFollower*, CPed*>(this, ped);
}

// 0x666160
CTask* CTaskComplexGangFollower::CreateFirstSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x666160, CTaskComplexGangFollower*, CPed*>(this, ped);
}

// 0x662A10
CTask* CTaskComplexGangFollower::ControlSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x662A10, CTaskComplexGangFollower*, CPed*>(this, ped);
}
