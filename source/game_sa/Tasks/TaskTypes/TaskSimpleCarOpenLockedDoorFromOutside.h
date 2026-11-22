#pragma once

#include "TaskSimple.h"

class CVehicle;
class CAnimBlendAssociation;
class CPed;
class CTaskUtilityLineUpPedWithCar;
class CEvent;

class NOTSA_EXPORT_VTABLE CTaskSimpleCarOpenLockedDoorFromOutside : public CTaskSimple {

public:
    bool m_hasAnimFinished{};                     // 0x8
    CAnimBlendAssociation* m_anim{};              // 0xC
    CVehicle* m_veh{};                            // 0x10
    int32 m_door{};                               // 0x14
    CTaskUtilityLineUpPedWithCar* m_lineUpUtil{}; // 0x18

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_SIMPLE_CAR_OPEN_LOCKED_DOOR_FROM_OUTSIDE;

    CTaskSimpleCarOpenLockedDoorFromOutside(CVehicle* veh, int32 door, CTaskUtilityLineUpPedWithCar* lineUpTask);
    CTaskSimpleCarOpenLockedDoorFromOutside(const CTaskSimpleCarOpenLockedDoorFromOutside&);
    ~CTaskSimpleCarOpenLockedDoorFromOutside();

    static void FinishAnimCarOpenLockedDoorFromOutsideCB(CAnimBlendAssociation* anim, void* data);
    void StartAnim(CPed* ped);

    CTask*    Clone() const override { return new CTaskSimpleCarOpenLockedDoorFromOutside{*this}; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool      ProcessPed(CPed* ped) override;
    bool      SetPedPosition(CPed * ped) override; 

private: // Wrappers for hooks
    // 0x646070
    CTaskSimpleCarOpenLockedDoorFromOutside* Constructor(CVehicle* veh, int32 door, CTaskUtilityLineUpPedWithCar* lineUpTask) {
        this->CTaskSimpleCarOpenLockedDoorFromOutside::CTaskSimpleCarOpenLockedDoorFromOutside(veh, door, lineUpTask);
        return this;
    }

    // 0x6460F0
    CTaskSimpleCarOpenLockedDoorFromOutside* Destructor() {
        this->CTaskSimpleCarOpenLockedDoorFromOutside::~CTaskSimpleCarOpenLockedDoorFromOutside();
        return this;
    }
};
