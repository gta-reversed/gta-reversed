#pragma once

#include "TaskSimple.h"

class CEvent;
class CAnimBlendAssociation;
class CPed;
class CTaskSimpleStandUp;

class NOTSA_EXPORT_VTABLE CTaskSimpleStandUp : public CTaskSimple {

public:
    bool m_sitAfterStep = {};           // 0x8
    bool m_hasAnimFinished = {};        // 0x9
    CAnimBlendAssociation* m_anim = {}; // 0xC

public:
    static void InjectHooks();

    constexpr static auto Type = eTaskType::TASK_SIMPLE_STAND_UP;

    CTaskSimpleStandUp(bool sitAfterStep);
    CTaskSimpleStandUp(const CTaskSimpleStandUp&);
    ~CTaskSimpleStandUp();

    static void FinishAnimStandUpCB(CAnimBlendAssociation* anim, void* data);

    void StartAnim(CPed* ped);


    CTask*    Clone() const override { return new CTaskSimpleStandUp{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool      ProcessPed(CPed* ped) override;

private: // Wrappers for hooks
    // 0x6312E0
    CTaskSimpleStandUp* Constructor(bool sitAfterStep) {
        this->CTaskSimpleStandUp::CTaskSimpleStandUp(sitAfterStep);
        return this;
    }
    // 0x631320
    CTaskSimpleStandUp* Destructor() {
        this->CTaskSimpleStandUp::~CTaskSimpleStandUp();
        return this;
    }
};
