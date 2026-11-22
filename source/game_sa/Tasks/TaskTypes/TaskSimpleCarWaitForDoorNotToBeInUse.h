#pragma once

#include "TaskSimple.h"

class CVehicle ;

/*! Task to wait for a vehicle's door to be opened */

// VMT: 0x86edbc | Size: 9
class NOTSA_EXPORT_VTABLE CTaskSimpleCarWaitForDoorNotToBeInUse : public CTaskSimple {
    
public:
    CVehicle* m_veh = {};               ///< The vehicle whose door we have to wait for
    uint32    m_doorsToWaitFor[2] = {}; ///< Wait for any of these doors to be ready (Not all)

public:
    static void InjectHooks();

    constexpr static auto Type = eTaskType::TASK_SIMPLE_CAR_WAIT_FOR_DOOR_NOT_TO_BE_IN_USE;

    CTaskSimpleCarWaitForDoorNotToBeInUse(CVehicle * veh, uint32 doorToWaitFor1, uint32 doorToWaitFor2 = 0);
    CTaskSimpleCarWaitForDoorNotToBeInUse(const CTaskSimpleCarWaitForDoorNotToBeInUse&);
    ~CTaskSimpleCarWaitForDoorNotToBeInUse();

    CTask*    Clone() const override { return new CTaskSimpleCarWaitForDoorNotToBeInUse{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool      ProcessPed(CPed* ped) override;
    bool      SetPedPosition(CPed* ped) override;  
 
private: // Wrappers for hooks

// 0x646B70
CTaskSimpleCarWaitForDoorNotToBeInUse* Constructor(CVehicle * veh, uint32 doorToWaitFor1, uint32 doorToWaitFor2) {
    this->CTaskSimpleCarWaitForDoorNotToBeInUse::CTaskSimpleCarWaitForDoorNotToBeInUse(veh, doorToWaitFor1, doorToWaitFor2);
    return this;
}

// 0x646C00
CTaskSimpleCarWaitForDoorNotToBeInUse* Destructor() {
    this->CTaskSimpleCarWaitForDoorNotToBeInUse::~CTaskSimpleCarWaitForDoorNotToBeInUse();
    return this;
}


};
