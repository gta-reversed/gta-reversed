#pragma once

#include "TaskSimple.h"
#include "Base.h"
#include "Vector.h"
#include <extensions/EntityRef.hpp>

class CEntity;

class NOTSA_EXPORT_VTABLE CTaskSimpleThrowControl : public CTaskSimple {
public:
    static constexpr auto Type = eTaskType::TASK_SIMPLE_THROW_CTRL;

    CTaskSimpleThrowControl(CEntity* targetEntity, const CVector* targetPos);
    CTaskSimpleThrowControl(const CTaskSimpleThrowControl&);
    ~CTaskSimpleThrowControl() override = default;

    eTaskType GetTaskType() const override { return Type; }
    CTask*    Clone() const override { return new CTaskSimpleThrowControl{ *this }; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool      ProcessPed(CPed* ped) override;

    static void InjectHooks();

private:
    bool               m_IsFinished{};  //!< Never set by the game, only read by `ProcessPed`
    bool               m_IsAttacking{}; //!< The throw projectile task was given to the ped
    notsa::EntityRef<> m_TargetEntity{};
    CVector            m_TargetPos{};
};
VALIDATE_SIZE(CTaskSimpleThrowControl, 0x1C);
