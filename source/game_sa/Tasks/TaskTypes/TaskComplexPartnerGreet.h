#pragma once

#include "TaskComplexPartner.h"

class NOTSA_EXPORT_VTABLE CTaskComplexPartnerGreet final : public CTaskComplexPartner {
public:
    int32 field_70;
    int32 m_handShakeType;

public:
    static constexpr auto Type = TASK_COMPLEX_PARTNER_GREET;

    static void InjectHooks();

    CTaskComplexPartnerGreet(const char* commandName, CPed* partner, bool leadSpeaker, float distanceMultiplier, int32 handShakeType, CVector point);
    CTaskComplexPartnerGreet(const CTaskComplexPartnerGreet&);
    ~CTaskComplexPartnerGreet() override = default;

    eTaskType GetTaskType()         const override { return Type; }
    CTask*    Clone()               const override { return new CTaskComplexPartnerGreet{*this}; }
    void      StreamRequiredAnims()       override;

    virtual CTaskComplexSequence* GetPartnerSequence();
private: // Wrappers for hooks
    // 0x684210
    CTaskComplexPartnerGreet* Constructor(char * commandName, CPed * partner, uint8 leadSpeaker, float distanceMultiplier, int32 handShakeType, CVector point) {
        this->CTaskComplexPartnerGreet::CTaskComplexPartnerGreet(commandName, partner, leadSpeaker, distanceMultiplier, handShakeType, point);
        return this;
    }

    // 0x684280
    CTaskComplexPartnerGreet* Destructor() {
        this->CTaskComplexPartnerGreet::~CTaskComplexPartnerGreet();
        return this;
    }
};
VALIDATE_SIZE(CTaskComplexPartnerGreet, 0x78);
