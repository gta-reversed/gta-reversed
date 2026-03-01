#pragma once

#include "TaskComplex.h"

class CVehicle;

// VMT: 0x86e854 | Size: 11
class NOTSA_EXPORT_VTABLE CTaskComplexLeaveBoat : public CTaskComplex {
public:
    CVehicle* m_veh = {};     // 0xC
    uint32    m_delayTimeMs = {}; // 0x10

public:
    static void InjectHooks();

    constexpr static auto Type = eTaskType::TASK_COMPLEX_LEAVE_BOAT;

    CTaskComplexLeaveBoat(CVehicle* vehicle, uint32 delay);
    CTaskComplexLeaveBoat(const CTaskComplexLeaveBoat&); // NOTSA
    ~CTaskComplexLeaveBoat();

    CTask* CreateSubTask(eTaskType taskType);

    CTask*    Clone() const override { return new CTaskComplexLeaveBoat{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

private: // Wrappers for hooks
    // 0x63BB60
    CTaskComplexLeaveBoat* Constructor(CVehicle* vehicle, uint32 delay) {
        this->CTaskComplexLeaveBoat::CTaskComplexLeaveBoat(vehicle, delay);
        return this;
    }

    // 0x642250
    CTaskComplexLeaveBoat* Destructor() {
        this->CTaskComplexLeaveBoat::~CTaskComplexLeaveBoat();
        return this;
    }
};
