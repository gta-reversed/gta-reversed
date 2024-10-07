#pragma once

#include "TaskComplex.h"

struct CScriptedEffectPair;

class NOTSA_EXPORT_VTABLE CTaskComplexAttractorPartnerWait : public CTaskComplex {
public:
    bool byteC;
    const CScriptedEffectPair* m_Pair;

public:
    static constexpr auto Type = TASK_COMPLEX_ATTRACTOR_PARTNER_WAIT;

    CTaskComplexAttractorPartnerWait(bool a2, const CScriptedEffectPair* pair); // 0x633250
    ~CTaskComplexAttractorPartnerWait() override = default;                     // 0x633290

    eTaskType GetTaskType() const override { return Type; } // 0x633280
    CTask* Clone() const override { return new CTaskComplexAttractorPartnerWait(byteC, m_Pair); } // 0x636CF0

    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
};
VALIDATE_SIZE(CTaskComplexAttractorPartnerWait, 0x14);
