#pragma once

#include "TaskSimple.h"

class CEvent;
class CTaskSimpleSitDown;
class CPed;
class CAnimBlendAssociation;

class NOTSA_EXPORT_VTABLE CTaskSimpleSitDown : public CTaskSimple {

public:
    bool m_sitOnStep = {};              // 0x8
    bool m_animFinished = {};           // 0x9
    CAnimBlendAssociation* m_anim = {}; // 0xC

public:
    static void InjectHooks();

    constexpr static auto Type = eTaskType::TASK_SIMPLE_SIT_DOWN;

    CTaskSimpleSitDown(bool sitOnStep);
    CTaskSimpleSitDown(const CTaskSimpleSitDown&);
    ~CTaskSimpleSitDown();

    static void FinishAnimSitDownCB(CAnimBlendAssociation* a1, void* a2);

    void StartAnim(CPed* ped);

    CTask*    Clone() const override { return new CTaskSimpleSitDown{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool      ProcessPed(CPed* ped) override;

private: // Wrappers for hooks
    // 0x631030
    CTaskSimpleSitDown* Constructor(bool sitOnStep) {
        this->CTaskSimpleSitDown::CTaskSimpleSitDown(sitOnStep);
        return this;
    }
    // 0x631070
    CTaskSimpleSitDown* Destructor() {
        this->CTaskSimpleSitDown::~CTaskSimpleSitDown();
        return this;
    }
};
