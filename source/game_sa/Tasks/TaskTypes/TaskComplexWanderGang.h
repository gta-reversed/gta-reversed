#pragma

#include "TaskComplexWander.h"
#include "Ped.h"

class NOTSA_EXPORT_VTABLE CTaskComplexWanderGang : public CTaskComplexWander {
public:
    CTaskTimer m_TaskTimer;
    uint32     m_NextScanTime;

public:
    CTaskComplexWanderGang(eMoveState moveState, uint8 dir, uint32 scanTime, bool bWanderSensibly, float fTargetRadius);
    ~CTaskComplexWanderGang() override = default; // 0x66F650

    eWanderType GetWanderType() override { return WANDER_TYPE_GANG; } // 0x66F630
    CTask* Clone() const override { return new CTaskComplexWanderGang(m_nMoveState, m_nDir, m_NextScanTime, m_bWanderSensibly, 0.5f); } // 0x671470
    void ScanForStuff(CPed* ped) override; // 0x66F640

    bool CanJoinGang() { return m_TaskTimer.IsOutOfTime(); }
};
VALIDATE_SIZE(CTaskComplexWanderGang, 0x38);
