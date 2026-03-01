#pragma once

#include "TaskSimple.h"
#include "Base.h"
#include "Vector.h"

class CEntity;

class NOTSA_EXPORT_VTABLE CTaskSimpleThrowControl : public CTaskSimple {
public:
    uint8    byte8{};
    uint8    m_isAttacking{};
    CEntity* m_entity{};
    CVector  m_pos{};

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_SIMPLE_THROW_CTRL;

    CTaskSimpleThrowControl(CEntity * targetEntity, CVector const* pos);
    CTaskSimpleThrowControl(const CTaskSimpleThrowControl&);
    ~CTaskSimpleThrowControl();

    eTaskType GetTaskType() const override { return Type; }
    CTask* Clone() const override { return new CTaskSimpleThrowControl{ *this }; }
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool ProcessPed(CPed* ped) override;

private: // Wrappers for hooks

// 0x61F8B0
CTaskSimpleThrowControl* Constructor(CEntity * targetEntity, CVector const* pos) {
    this->CTaskSimpleThrowControl::CTaskSimpleThrowControl(targetEntity, pos);
    return this;
}

// 0x61F950
CTaskSimpleThrowControl* Destructor() {
    this->CTaskSimpleThrowControl::~CTaskSimpleThrowControl();
    return this;
}

};
VALIDATE_SIZE(CTaskSimpleThrowControl, 0x1C);
