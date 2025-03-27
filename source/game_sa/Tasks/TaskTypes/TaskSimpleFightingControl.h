#pragma once

#include "TaskSimple.h"
#include "Base.h"

class CEntity;

class NOTSA_EXPORT_VTABLE CTaskSimpleFightingControl : public CTaskSimple {
public:
    bool m_bool{};
    bool m_bool2{};
    uint16 m_uint16{};
    CEntity* m_target{};
    float m_angleRad{};
    float m_maxAttackRange{};
    float m_unk3{};
    uint32 m_nextAttackTime{};
    uint32 m_someTime{};

public:
    static void InjectHooks();

    constexpr static auto Type = eTaskType::TASK_SIMPLE_FIGHT_CTRL;

    CTaskSimpleFightingControl(CEntity* entityToFight, float angleRad, float dist);
    CTaskSimpleFightingControl(const CTaskSimpleFightingControl&);
    ~CTaskSimpleFightingControl();

    int16 CalcMoveCommand(CPed* ped);

    CTask* Clone() const override { return new CTaskSimpleFightingControl{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool ProcessPed(CPed* ped) override;

private: // Wrappers for hooks
    // 0x61DC10
    CTaskSimpleFightingControl* Constructor(CEntity* entityToFight, float angleRad, float dist) { this->CTaskSimpleFightingControl::CTaskSimpleFightingControl(entityToFight, angleRad, dist); return this; }

    // 0x61DCA0
    CTaskSimpleFightingControl* Destructor() { this->CTaskSimpleFightingControl::~CTaskSimpleFightingControl(); return this; }
};
VALIDATE_SIZE(CTaskSimpleFightingControl, 0x24);
