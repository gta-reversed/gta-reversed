#pragma once

#include "TaskSimple.h"

class CEvent;
class CAnimBlendAssociation;
class CPed;

class NOTSA_EXPORT_VTABLE CTaskSimpleShakeFist : public CTaskSimple {

public:
    bool m_hasAnimFinished = {};                // 0x8
    CAnimBlendAssociation* m_anim = {}; // 0xC

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_SIMPLE_SHAKE_FIST;

    CTaskSimpleShakeFist() = default;
    CTaskSimpleShakeFist(const CTaskSimpleShakeFist&);
    ~CTaskSimpleShakeFist();

    static void FinishAnimShakeFistCB(CAnimBlendAssociation*, void*);

    void StartAnim(CPed* ped);

    CTask*    Clone() const override { return new CTaskSimpleShakeFist{*this}; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool      ProcessPed(CPed* ped) override;

private: // Wrappers for hooks
    // 0x690B80
    CTaskSimpleShakeFist* Constructor() {
        this->CTaskSimpleShakeFist::CTaskSimpleShakeFist();
        return this;
    }

    // 0x690BB0
    CTaskSimpleShakeFist* Destructor() {
        this->CTaskSimpleShakeFist::~CTaskSimpleShakeFist();
        return this;
    }
};
