/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "TaskSimple.h"
class CAnimBlendAssociation;
class CEntity;
class CWeaponInfo;

enum class eDrivebyStyle : int8 {
    FIXED_LHS      = 0,
    FIXED_RHS      = 1,

    START_FROM_LHS = 2,
    START_FROM_RHS = 3,

    AI_SIDE        = 4,

    FIXED_FWD      = 5,
    FIXED_BAK      = 6,
    
    AI_FWD_BAK     = 7,
    
    AI_ALL_DIRN    = 8,
};

class NOTSA_EXPORT_VTABLE CTaskSimpleGangDriveBy : public CTaskSimple {
public:
    bool                   m_bIsFinished;
    bool                   m_bAnimsReferenced;
    bool                   m_bSeatRHS;
    bool                   m_bInRangeToShoot;
    bool                   m_bInWeaponRange;
    bool                   m_bReachedAbortRange;
    bool                   m_bFromScriptCommand;
    char                   m_nNextCommand;
    char                   m_nLastCommand;
    char                   m_nBurstShots;
    eDrivebyStyle          m_nDrivebyStyle;
    int8                   m_nFrequencyPercentage;
    char                   m_nFakeShootDirn;
    int16                  m_nAttackTimer;
    uint32                 m_nLOSCheckTime;
    bool                   m_nLOSBlocked;
    float                  m_fAbortRange;
    int32                  m_nRequiredAnimID;
    AssocGroupId           m_nRequiredAnimGroup;
    CAnimBlendAssociation* m_pAnimAssoc;
    CWeaponInfo*           m_pWeaponInfo;
    CEntity*               m_pTargetEntity;
    CVector                m_vecCoords;

public:
    static constexpr auto Type = TASK_SIMPLE_GANG_DRIVEBY;

    CTaskSimpleGangDriveBy(CEntity *target, const CVector *targetPos, float abortRange, int8 frequencyPercentage, eDrivebyStyle drivebyStyle, bool seatRHS);
    ~CTaskSimpleGangDriveBy() override;

    eTaskType GetTaskType() const override { return Type; }
    CTask* Clone() const override;
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool ProcessPed(CPed* ped) override;
};
VALIDATE_SIZE(CTaskSimpleGangDriveBy, 0x44);

