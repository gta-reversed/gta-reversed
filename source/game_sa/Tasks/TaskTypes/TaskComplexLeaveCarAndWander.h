#pragma once

#include "TaskComplex.h"
#include "eTargetDoor.h"

class CTask;
class CPed;
class CTaskComplexLeaveCarAndWander;
class CVehicle;

class NOTSA_EXPORT_VTABLE CTaskComplexLeaveCarAndWander : public CTaskComplex {

public:
    CVehicle* m_veh = {};          // 0xC
    eTargetDoor m_door = {};             // 0x10
    int32 m_leaveDelayMs = {};     // 0x14
    bool m_bSensibleLeaveCar = {}; // 0x18

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_COMPLEX_LEAVE_CAR_AND_WANDER;

    CTaskComplexLeaveCarAndWander(CVehicle* veh, eTargetDoor door = TARGET_DOOR_FRONT_LEFT, int32 leaveDelayMs = 0, bool sensibleLeaveVeh = true);
    CTaskComplexLeaveCarAndWander(const CTaskComplexLeaveCarAndWander&);
    ~CTaskComplexLeaveCarAndWander();

    CTask* CreateSubTask(eTaskType taskType);

    CTask*    Clone() const override { return new CTaskComplexLeaveCarAndWander{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override { return m_pSubTask; }

private: // Wrappers for hooks
    // 0x63BDD0
    CTaskComplexLeaveCarAndWander* Constructor(CVehicle* veh, eTargetDoor door, int32 leaveDelayMs, bool sensibleLeaveVeh) {
        this->CTaskComplexLeaveCarAndWander::CTaskComplexLeaveCarAndWander(veh, door, leaveDelayMs, sensibleLeaveVeh);
        return this;
    }
    // 0x63BE50
    CTaskComplexLeaveCarAndWander* Destructor() {
        this->CTaskComplexLeaveCarAndWander::~CTaskComplexLeaveCarAndWander();
        return this;
    }
};
