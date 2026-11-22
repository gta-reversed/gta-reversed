#pragma once

#include "TaskTimer.h"
#include "TaskComplex.h"

class CPed;
class CVehicle;
class CTaskComplexScreamInCarThenLeave;
class CEvent;
class CTaskComplexLeaveCarAndFlee;

/*
* Depending on whenever the player is in a mission,
* the ped either immidiately leaves (if in mission) or drives for 5s (if not) then
* leaves the car, while screaming like a baby.
*/
class NOTSA_EXPORT_VTABLE CTaskComplexScreamInCarThenLeave : public CTaskComplex {

public:
    CVehicle*   m_veh{};   /// The vehicle :D
    eTargetDoor m_door{};  /// Not used
    CTaskTimer  m_timer{}; /// Started when `TASK_SIMPLE_CAR_DRIVE` is created, upon expiration (of 5000ms) `TASK_COMPLEX_LEAVE_CAR_AND_FLEE` is created.

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_COMPLEX_SCREAM_IN_CAR_THEN_LEAVE;

    CTaskComplexScreamInCarThenLeave(CVehicle* veh, eTargetDoor targetDoor);
    CTaskComplexScreamInCarThenLeave(const CTaskComplexScreamInCarThenLeave&);
    ~CTaskComplexScreamInCarThenLeave();

    CTask* CreateSubTask(eTaskType taskType, CPed* ped);

    CTask*    Clone() const override { return new CTaskComplexScreamInCarThenLeave{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

private: // Wrappers for hooks
    // 0x63C240
    CTaskComplexScreamInCarThenLeave* Constructor(CVehicle* veh, eTargetDoor targetDoor) {
        this->CTaskComplexScreamInCarThenLeave::CTaskComplexScreamInCarThenLeave(veh, targetDoor);
        return this;
    }
    // 0x63C2C0
    CTaskComplexScreamInCarThenLeave* Destructor() {
        this->CTaskComplexScreamInCarThenLeave::~CTaskComplexScreamInCarThenLeave();
        return this;
    }
};
