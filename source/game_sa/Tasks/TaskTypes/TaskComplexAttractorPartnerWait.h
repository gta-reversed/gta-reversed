#pragma once

#include "TaskComplex.h"

class CScriptedEffectPair;

class NOTSA_EXPORT_VTABLE CTaskComplexAttractorPartnerWait : public CTaskComplex {
public:
    static constexpr auto Type = TASK_COMPLEX_ATTRACTOR_PARTNER_WAIT;

    static void InjectHooks();

    CTaskComplexAttractorPartnerWait(bool isLeader, const CScriptedEffectPair& pair); // 0x633250
    CTaskComplexAttractorPartnerWait(const CTaskComplexAttractorPartnerWait&);        // 0x636CF0
    ~CTaskComplexAttractorPartnerWait() override = default;                           // 0x633290

    eTaskType GetTaskType() const override { return Type; } // 0x633280
    CTask* Clone() const override { return new CTaskComplexAttractorPartnerWait(m_IsLeader, *m_CurrentFxPair); }

    CTask* CreateNextSubTask(CPed* ped) override { return nullptr; } // 0x6332A0
    CTask* ControlSubTask(CPed* ped) override { return m_pSubTask; } // 0x6332B0
    CTask* CreateFirstSubTask(CPed* ped) override;

protected:
    bool                       m_IsLeader{};
    const CScriptedEffectPair* m_CurrentFxPair{};
};
VALIDATE_SIZE(CTaskComplexAttractorPartnerWait, 0x14);
