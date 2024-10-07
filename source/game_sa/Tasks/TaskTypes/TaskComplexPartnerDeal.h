#pragma once

#include "TaskComplexPartner.h"

class NOTSA_EXPORT_VTABLE CTaskComplexPartnerDeal : public CTaskComplexPartner {
public:
    int32 field_70;

public:
    static constexpr auto Type = TASK_COMPLEX_PARTNER_DEAL;

    CTaskComplexPartnerDeal(const char* commandName, CPed* partner, bool leadSpeaker, float distanceMultiplier, CVector point);
    ~CTaskComplexPartnerDeal() override = default;

    eTaskType GetTaskType() const override { return Type; }
    CTask* Clone() const override { return new CTaskComplexPartnerDeal(m_commandName, m_partner, m_leadSpeaker, m_distanceMultiplier, m_point); }
    CTask* CreateFirstSubTask(CPed* ped) override;
    void StreamRequiredAnims() override;
    virtual CTaskComplexSequence* GetPartnerSequence();

    static void InjectHooks();
    CTaskComplexPartnerDeal* Constructor(const char* commandName, CPed* partner, bool leadSpeaker, float distanceMultiplier, CVector point);
};
VALIDATE_SIZE(CTaskComplexPartnerDeal, 0x74);
