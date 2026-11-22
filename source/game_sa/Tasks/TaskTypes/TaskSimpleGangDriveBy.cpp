#include "StdInc.h"

#include "TaskSimpleGangDriveBy.h"

// 0x6217D0
CTaskSimpleGangDriveBy::CTaskSimpleGangDriveBy(CEntity* target, const CVector* targetPos, float abortRange, int8 frequencyPercentage, eDrivebyStyle drivebyStyle, bool seatRHS) :
    m_bSeatRHS{ seatRHS },
    m_nDrivebyStyle{ drivebyStyle },
    m_fAbortRange{ abortRange },
    m_pTargetEntity{ target },
    m_nFrequencyPercentage{ frequencyPercentage },
    m_vecCoords{ targetPos ? *targetPos : CVector{} } {
}

// 0x6236D0
CTaskSimpleGangDriveBy::CTaskSimpleGangDriveBy(const CTaskSimpleGangDriveBy& o) :
    CTaskSimpleGangDriveBy{
        o.m_pTargetEntity,
        &o.m_vecCoords,
        o.m_fAbortRange,
        o.m_nFrequencyPercentage,
        o.m_nDrivebyStyle,
        o.m_bSeatRHS
    }
{
    m_bFromScriptCommand = o.m_bFromScriptCommand;
}

// 0x6218C0
CTaskSimpleGangDriveBy::~CTaskSimpleGangDriveBy() {
    if (m_bAnimsReferenced) {
        CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex(m_nRequiredAnimGroup));
    }
    if (m_pAnimAssoc) {
        m_pAnimAssoc->SetDefaultDeleteCallback();
    }
}

// 0x621960
int8 CTaskSimpleGangDriveBy::SetupStaticAnimForPlayer(CPlayerPed* ped) {
    return plugin::CallMethodAndReturn<int8, 0x621960, CTaskSimpleGangDriveBy*, CPlayerPed*>(this, ped);
}

// 0x621B10
bool CTaskSimpleGangDriveBy::CheckIfThereNoObstaclesForFiring(CPed* ped, CVector* posn) {
    return plugin::CallMethodAndReturn<bool, 0x621B10, CTaskSimpleGangDriveBy*, CPed*, CVector*>(this, ped, posn);
}

// 0x627600
void CTaskSimpleGangDriveBy::SetupStaticAnimForNPC(CPed* ped) {
    return plugin::CallMethodAndReturn<void, 0x627600, CTaskSimpleGangDriveBy*, CPed*>(this, ped);
}

// 0x627B20
bool CTaskSimpleGangDriveBy::BlendAnimation(CPed* ped) {
    return plugin::CallMethodAndReturn<bool, 0x627B20, CTaskSimpleGangDriveBy*, CPed*>(this, ped);
}

// 0x627CC0
int8 CTaskSimpleGangDriveBy::FireGun(CPed* ped) {
    return plugin::CallMethodAndReturn<int8, 0x627CC0, CTaskSimpleGangDriveBy*, CPed*>(this, ped);
}

// 0x628350
int8 CTaskSimpleGangDriveBy::ProcessAiming(CPed* ped) {
    return plugin::CallMethodAndReturn<int8, 0x628350, CTaskSimpleGangDriveBy*, CPed*>(this, ped);
}

// 0x62D290
bool CTaskSimpleGangDriveBy::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    return plugin::CallMethodAndReturn<bool, 0x62D290, CTaskSimpleGangDriveBy*, CPed*, eAbortPriority, const CEvent*>(this, ped, priority, event);
}

// 0x62D3B0
bool CTaskSimpleGangDriveBy::ProcessPed(CPed* ped) {
    return plugin::CallMethodAndReturn<bool, 0x62D3B0, CTaskSimpleGangDriveBy*, CPed*>(this, ped);
}

void CTaskSimpleGangDriveBy::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleGangDriveBy, 0x86d944, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x6217D0, "1", {}, CEntity*, const CVector*, float, char, eDrivebyStyle, bool);
    RH_ScopedDestructorInstall(0x6218C0);

    RH_ScopedInstall(SetupStaticAnimForPlayer, 0x621960, { .Reversed = false });
    RH_ScopedInstall(CheckIfThereNoObstaclesForFiring, 0x621B10, { .Reversed = false });
    RH_ScopedInstall(SetupStaticAnimForNPC, 0x627600, { .Reversed = false });
    RH_ScopedInstall(BlendAnimation, 0x627B20, { .Reversed = false });
    RH_ScopedInstall(FireGun, 0x627CC0, { .Reversed = false });
    RH_ScopedInstall(ProcessAiming, 0x628350, { .Reversed = false });
    RH_ScopedVMTInstall(Clone, 0x6236D0);
    RH_ScopedVMTInstall(GetTaskType, 0x6218B0);
    RH_ScopedVMTInstall(MakeAbortable, 0x62D290, { .Reversed = false });
    RH_ScopedVMTInstall(ProcessPed, 0x62D3B0, { .Reversed = false });
}
