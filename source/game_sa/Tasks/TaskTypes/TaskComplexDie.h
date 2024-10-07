#pragma once

#include "TaskComplex.h"
#include "eWeaponType.h"
#include "eDirection.h"

class CVehicle;

class NOTSA_EXPORT_VTABLE CTaskComplexDie : public CTaskComplex {
public:
    eWeaponType  m_nWeaponType;
    AssocGroupId m_nAnimGroup;
    AnimationId  m_nAnimID;
    float        m_fBlendDelta;
    float        m_fAnimSpeed;

    union {
        uint32 m_nFlags;
        struct {
            uint32 m_bBeingKilledByStealth : 1;
            uint32 m_bFallingToDeath : 1;
            uint32 m_bFallToDeathOverRailing : 1;
        };
    };
    eDirection m_nFallToDeathDir;

public:
    static constexpr auto Type = TASK_COMPLEX_DIE;

    CTaskComplexDie(
        eWeaponType nWeaponType,
        AssocGroupId animGroup,
        AnimationId animID,
        float fBlendDelta = 4.f,
        float fAnimSpeed = 0.f,
        bool bBeingKilledByStealth = false,
        bool bFallingToDeath = false,
        eDirection nFallToDeathDir = eDirection::FORWARD,
        bool bFallToDeathOverRailing = false
    );
    ~CTaskComplexDie() override = default; // 0x6300C0 0x637910

    eTaskType GetTaskType() const override { return Type; } // 0x6300B0
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed*) override { return m_pSubTask; } // 0x630580
    CTask* Clone() const override {
        return new CTaskComplexDie(m_nWeaponType, m_nAnimGroup, m_nAnimID, m_fBlendDelta, m_fAnimSpeed, m_bBeingKilledByStealth,
                                   m_bFallingToDeath, m_nFallToDeathDir, m_bFallToDeathOverRailing);
    } // 0x636060

    void SayDeathSample(CPed* ped) const;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskComplexDie* Constructor(eWeaponType nWeaponType, AssocGroupId animGroup, AnimationId animID, float fBlendDelta, float fAnimSpeed, bool bBeingKilledByStealth, bool bFallingToDeath, eDirection nFallToDeathDir, bool bFallToDeathOverRailing);
};

VALIDATE_SIZE(CTaskComplexDie, 0x28);
