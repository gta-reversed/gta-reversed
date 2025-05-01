#pragma once

#include "TaskComplex.h"
#include "Enums/eDirection.h"

class NOTSA_EXPORT_VTABLE CTaskComplexFallToDeath : public CTaskComplex {
public:
    CVector     m_Posn;
    AnimationId m_nAnimId;
    AnimationId m_nAnimId1;
    eDirection    m_nFallToDeathDir;
    union {
        struct {
            uint8 b0x1  : 1;
            uint8 b0x2  : 1;
            uint8 b0x4 : 1; // bFallToDeathOverRailing
            uint8 b0x8  : 1;
            uint8 b0x10 : 1;
        };
        uint8 m_nFlags;
    };

    static inline float ms_NoRailingVerticalForce   = 4.5f; // 0x8D2F10
    static inline float ms_OverRailingVerticalForce = 9.0f; // 0x8D2F14
    static inline float ms_LateralForceMagnitude    = 6.0f; // 0x8D2F18

public:
    static constexpr auto Type = TASK_COMPLEX_FALL_TO_DEATH;

    CTaskComplexFallToDeath(int32 direction, const CVector& posn, bool bFallToDeathOverRailing, bool a5);
    ~CTaskComplexFallToDeath() override = default; // 0x6790B0

    eTaskType GetTaskType() const override { return Type; }; // 0x6790A0
    CTask* Clone() const override { return new CTaskComplexFallToDeath(static_cast<int32>(m_nFallToDeathDir), m_Posn, b0x8, b0x10); } // 0x67C480
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask* ControlSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* CreateNextSubTask(CPed* ped) override;

    static bool CalcFall(CPed* ped, int32& fallDir, bool& bFallToDeathOverRailing);

private:
    void UpdateAnims(CPed* ped);

    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskComplexFallToDeath* Constructor(int32 direction, const CVector& posn, bool a4, bool a5);
};
VALIDATE_SIZE(CTaskComplexFallToDeath, 0x24);
