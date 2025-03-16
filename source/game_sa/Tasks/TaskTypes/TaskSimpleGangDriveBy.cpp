#include "StdInc.h"

#include "TaskSimpleGangDriveBy.h"

CTaskSimpleGangDriveBy::CTaskSimpleGangDriveBy(CEntity* target, const CVector* targetPos, float abortRange,
    int8 frequencyPercentage, eDrivebyStyle drivebyStyle, bool seatRHS)
{
    m_bSeatRHS = seatRHS;
    m_nDrivebyStyle = drivebyStyle;
    m_fAbortRange = abortRange;
    m_pTargetEntity = target;
    m_nFrequencyPercentage = frequencyPercentage;
    m_bIsFinished = false;
    m_bAnimsReferenced = false;
    m_bInRangeToShoot = false;
    m_bInWeaponRange = false;
    m_bReachedAbortRange = false;
    m_bFromScriptCommand = false;
    m_nBurstShots = -1;
    m_nFakeShootDirn = -1;
    m_nAttackTimer = -1;
    m_nLastCommand = 0;
    m_nNextCommand = 1;
    m_nLOSCheckTime = 0;
    m_nLOSBlocked = true;
    m_pAnimAssoc = nullptr;
    m_nRequiredAnimID = ANIM_ID_NO_ANIMATION_SET;
    m_nRequiredAnimGroup = ANIM_GROUP_DEFAULT;
    m_pWeaponInfo = nullptr;
    CEntity::SafeRegisterRef(m_pTargetEntity);
    if (targetPos)
        m_vecCoords = *targetPos;
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
