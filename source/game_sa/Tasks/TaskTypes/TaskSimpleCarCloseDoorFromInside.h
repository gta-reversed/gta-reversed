#pragma once

#include "TaskSimple.h"

class CAnimBlendAssociation;
class CVehicle;
class CTaskUtilityLineUpPedWithCar;

class NOTSA_EXPORT_VTABLE CTaskSimpleCarCloseDoorFromInside : public CTaskSimple {
public:
    bool m_animHasFinished = {};                     // 8
    CAnimBlendAssociation* m_anim = {};              // 0xC
    CVehicle* m_veh = {};                            // 0x10
    uint32 m_door = {};                              // 0x14
    CTaskUtilityLineUpPedWithCar* m_lineUpTask = {}; // 0x18

public:
    static void InjectHooks();

    constexpr static auto Type = eTaskType::TASK_SIMPLE_CAR_CLOSE_DOOR_FROM_INSIDE;

    CTaskSimpleCarCloseDoorFromInside(CVehicle* veh, uint32 door, CTaskUtilityLineUpPedWithCar* lineUpTask);
    CTaskSimpleCarCloseDoorFromInside(const CTaskSimpleCarCloseDoorFromInside&);
    ~CTaskSimpleCarCloseDoorFromInside();

    static void FinishAnimCarCloseDoorFromInsideCB(CAnimBlendAssociation* anim, void* data);

    void ComputeAnimID(AssocGroupId& outGroup, AnimationId& outAnimId);
    auto ComputeAnimID_Helper() -> std::tuple<AssocGroupId, AnimationId>;
    void StartAnim(CPed const* ped);
    void ProcessDoorOpen(CPed const* ped); // NOTSA

    CTask* Clone() const override { return new CTaskSimpleCarCloseDoorFromInside{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool ProcessPed(CPed* ped) override;
    bool SetPedPosition(CPed* ped) override;

private: // Wrappers for hooks
    // 0x646300
    CTaskSimpleCarCloseDoorFromInside* Constructor(CVehicle* veh, uint32 door, CTaskUtilityLineUpPedWithCar* lineUpTask) {
        this->CTaskSimpleCarCloseDoorFromInside::CTaskSimpleCarCloseDoorFromInside(veh, door, lineUpTask);
        return this;
    }

    // 0x646380
    CTaskSimpleCarCloseDoorFromInside* Destructor() {
        this->CTaskSimpleCarCloseDoorFromInside::~CTaskSimpleCarCloseDoorFromInside();
        return this;
    }
};
