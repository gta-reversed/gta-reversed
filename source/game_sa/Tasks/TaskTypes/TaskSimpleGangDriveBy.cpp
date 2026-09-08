#include "StdInc.h"

#include "TaskSimpleGangDriveBy.h"

CTaskSimpleGangDriveBy::CTaskSimpleGangDriveBy(CEntity* target, const CVector* targetPos, float abortRange, int8 frequencyPercentage, eDrivebyStyle drivebyStyle, bool seatRHS) {
    m_bSeatRHS             = seatRHS;
    m_nDrivebyStyle        = drivebyStyle;
    m_fAbortRange          = abortRange;
    m_pTargetEntity        = target;
    m_nFrequencyPercentage = frequencyPercentage;
    m_bIsFinished          = false;
    m_bAnimsReferenced     = false;
    m_bInRangeToShoot      = false;
    m_bInWeaponRange       = false;
    m_bReachedAbortRange   = false;
    m_bFromScriptCommand   = false;
    m_nBurstShots          = -1;
    m_nFakeShootDirn       = -1;
    m_nAttackTimer         = -1;
    m_nLastCommand         = 0;
    m_nNextCommand         = 1;
    m_nLOSCheckTime        = 0;
    m_nLOSBlocked          = true;
    m_pAnimAssoc           = nullptr;
    m_nRequiredAnimID      = ANIM_ID_NO_ANIMATION_SET;
    m_nRequiredAnimGroup   = ANIM_GROUP_DEFAULT;
    m_pWeaponInfo          = nullptr;
    CEntity::SafeRegisterRef(m_pTargetEntity);
    if (targetPos) {
        m_vecCoords = *targetPos;
    }
}

CTaskSimpleGangDriveBy::~CTaskSimpleGangDriveBy()
{
    if (m_bAnimsReferenced)
        CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex(m_nRequiredAnimGroup));

    if (m_pAnimAssoc)
        m_pAnimAssoc->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);

    CEntity::SafeCleanUpRef(m_pTargetEntity);
}

CTask* CTaskSimpleGangDriveBy::Clone() const {
    return plugin::CallMethodAndReturn<CTask*, 0x6236D0, const CTask*>(this);
}

bool CTaskSimpleGangDriveBy::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event)
{
    return plugin::CallMethodAndReturn<bool, 0x62D290, CTask*, CPed*, int32, const CEvent*>(this, ped, priority, event);
}

bool CTaskSimpleGangDriveBy::ProcessPed(CPed* ped)
{
    return plugin::CallMethodAndReturn<bool, 0x62D3B0, CTask*, CPed*>(this, ped);
}

void CTaskSimpleGangDriveBy::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleGangDriveBy, 0x86d944, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6217D0);
    RH_ScopedInstall(Destructor, 0x6218C0);

    RH_ScopedInstall(SetupStaticAnimForPlayer, 0x621960, { .reversed = false });
    RH_ScopedInstall(CheckIfThereNoObstaclesForFiring, 0x621B10, { .reversed = false });
    RH_ScopedInstall(SetupStaticAnimForNPC, 0x627600, { .reversed = false });
    RH_ScopedInstall(BlendAnimation, 0x627B20, { .reversed = false });
    RH_ScopedInstall(FireGun, 0x627CC0, { .reversed = false });
    RH_ScopedInstall(ProcessAiming, 0x628350, { .reversed = false });

    RH_ScopedVMTInstall(Clone, 0x6236D0);
    RH_ScopedVMTInstall(GetTaskType, 0x6218B0);
    RH_ScopedVMTInstall(MakeAbortable, 0x62D290, { .reversed = false });
    RH_ScopedVMTInstall(ProcessPed, 0x62D3B0, { .reversed = false });
}
