#pragma once

#include "TaskSimple.h"

class CAnimBlendAssociation;
class CEvent;
class CPed;
class CVehicle;
class CTaskSimplePickUpBike;
class CTaskUtilityLineUpPedWithCar;


class NOTSA_EXPORT_VTABLE CTaskSimplePickUpBike : public CTaskSimple {
 
public:
    bool m_hasAnimFinished = {};  // 0x8
    CAnimBlendAssociation * m_anim = {};  // 0xC
    CVehicle * m_veh = {};  // 0x10
    int32 m_targetDoor = {};  // 0x14
    CTaskUtilityLineUpPedWithCar * m_lineUpUtil = {};  // 0x18

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_SIMPLE_BIKE_PICK_UP;

    CTaskSimplePickUpBike(CVehicle * veh, int32 targetDoor, CTaskUtilityLineUpPedWithCar * lineUpUtil);
    CTaskSimplePickUpBike(const CTaskSimplePickUpBike&);
    ~CTaskSimplePickUpBike();
    static void FinishAnimPickUpBikeCB(CAnimBlendAssociation * anim, void * data);

    void StartAnim(CPed const* ped);

    CTask*    Clone() const override { return new CTaskSimplePickUpBike{*this}; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool      ProcessPed(CPed* ped) override;
    bool      SetPedPosition(CPed * ped) override;
 
private: // Wrappers for hooks
    // 0x6461B0
    CTaskSimplePickUpBike* Constructor(CVehicle * veh, int32 targetDoor, CTaskUtilityLineUpPedWithCar * lineUpUtil) {
        this->CTaskSimplePickUpBike::CTaskSimplePickUpBike(veh, targetDoor, lineUpUtil);
        return this;
    }
    // 0x646230
    CTaskSimplePickUpBike* Destructor() {
        this->CTaskSimplePickUpBike::~CTaskSimplePickUpBike();
        return this;
    }
};
