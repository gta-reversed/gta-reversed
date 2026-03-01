#pragma once

#include "TaskComplex.h"

class CPed;
class CTaskComplexKillPedFromBoat;

class NOTSA_EXPORT_VTABLE CTaskComplexKillPedFromBoat : public CTaskComplex {
public:
    CPed* m_Ped{};
public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_KILL_PED_FROM_BOAT;

    CTaskComplexKillPedFromBoat(CPed * ped);
    ~CTaskComplexKillPedFromBoat() override;

    CTask* Clone() const override;
    eTaskType GetTaskType() const override { return Type; }
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override;
 
private: // Wrappers for hooks
    // 0x6227C0
    CTaskComplexKillPedFromBoat* Constructor(CPed* ped) {
        this->CTaskComplexKillPedFromBoat::CTaskComplexKillPedFromBoat(ped);
        return this;
    }
    // 0x622830
    CTaskComplexKillPedFromBoat* Destructor() {
        this->CTaskComplexKillPedFromBoat::~CTaskComplexKillPedFromBoat();
        return this;
    }
};
