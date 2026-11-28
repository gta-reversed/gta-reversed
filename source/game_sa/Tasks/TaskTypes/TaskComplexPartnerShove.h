#pragma once

#include "TaskComplexPartner.h"

class NOTSA_EXPORT_VTABLE CTaskComplexPartnerShove : public CTaskComplexPartner {
public:

public:
    static constexpr auto Type = TASK_COMPLEX_PARTNER_SHOVE;

    CTaskComplexPartnerShove(const char* commandName, CPed* partner, bool leadSpeaker, float distanceMultiplier, int8 updateDirectionCount, CVector point);
    ~CTaskComplexPartnerShove() override = default;

    eTaskType GetTaskType() const override { return Type; }
    CTask* Clone() const override { return new CTaskComplexPartnerShove(m_commandName, m_partner, m_leadSpeaker, m_distanceMultiplier, m_updateDirectionCount, m_point); }
    CTaskComplexSequence* GetPartnerSequence() override;

    static void InjectHooks();
    CTaskComplexPartnerShove* Constructor(const char* commandName, CPed* partner, bool leadSpeaker, float distanceMultiplier, int8 updateDirectionCount, CVector point);
};
VALIDATE_SIZE(CTaskComplexPartnerShove, 0x74);
