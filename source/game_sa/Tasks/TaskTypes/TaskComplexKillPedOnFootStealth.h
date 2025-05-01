#pragma once

#include "TaskComplexKillPedOnFoot.h"
#include "Vector.h"

class CPed;
class CTaskComplexKillPedOnFootStealth;

class NOTSA_EXPORT_VTABLE CTaskComplexKillPedOnFootStealth : public CTaskComplexKillPedOnFoot {

public:
    CVector m_targetPos{ m_target ? m_target->GetPosition() : CVector{0.f, 0.f, -9999.9004f} };  //< Only valid if `m_targetUnreachableTimeMs != (uint32)-1`
    uint32  m_targetUnreachableTimeMs{(uint32)-1}; //< `GetTimeMs()` at the point when the target wasn't targettable (eg.: `!CanPedTargetPed(ped, m_target)`). `-1` if target is reachable (targettable)

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_COMPLEX_KILL_PED_ON_FOOT_STEALTH;

    CTaskComplexKillPedOnFootStealth(CPed* pedToKill);
    CTaskComplexKillPedOnFootStealth(const CTaskComplexKillPedOnFootStealth&); // NOTSA
    ~CTaskComplexKillPedOnFootStealth() = default;

    CTask*    Clone() const override { return new CTaskComplexKillPedOnFootStealth{ *this }; } // 0x623400
    eTaskType GetTaskType() const override { return Type; } // 0x620FE0
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

private: // Wrappers for hooks
    // 0x620F70
    CTaskComplexKillPedOnFootStealth* Constructor(CPed* target) {
        this->CTaskComplexKillPedOnFootStealth::CTaskComplexKillPedOnFootStealth(target);
        return this;
    }

    // 0x620FF0
    CTaskComplexKillPedOnFootStealth* Destructor() {
        this->CTaskComplexKillPedOnFootStealth::~CTaskComplexKillPedOnFootStealth();
        return this;
    }
};
