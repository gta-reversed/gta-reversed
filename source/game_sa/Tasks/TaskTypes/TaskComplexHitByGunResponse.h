#pragma once

#include "TaskComplex.h"
#include "Enums/eDirection.h"
#include <extensions/WEnum.hpp>

class CPed;
class CEvent;

class NOTSA_EXPORT_VTABLE CTaskComplexHitByGunResponse : public CTaskComplex {
public:
    notsa::WEnumS32<eDirection> m_HitSide{};

public:
    static constexpr auto Type = eTaskType::TASK_COMPLEX_HIT_BY_GUN_RESPONSE;

    static void InjectHooks();

    CTaskComplexHitByGunResponse(eDirection hitSide);
    CTaskComplexHitByGunResponse(const CTaskComplexHitByGunResponse&);
    ~CTaskComplexHitByGunResponse() override = default;

    CTask*    Clone() const override { return new CTaskComplexHitByGunResponse{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override { return nullptr; }
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override { return m_pSubTask; }

private: // Wrappers for hooks
    // 0x631DD0
    CTaskComplexHitByGunResponse* Constructor(eDirection hs) {
        this->CTaskComplexHitByGunResponse::CTaskComplexHitByGunResponse(hs);
        return this;
    }

    // 0x631E30
    CTaskComplexHitByGunResponse* Destructor() {
        this->CTaskComplexHitByGunResponse::~CTaskComplexHitByGunResponse();
        return this;
    }
};
