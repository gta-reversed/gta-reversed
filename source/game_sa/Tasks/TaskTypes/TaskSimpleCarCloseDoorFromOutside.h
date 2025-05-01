#pragma once

#include "TaskSimple.h"

class CVehicle;
class CTaskUtilityLineUpPedWithCar;
class CAnimBlendAssociation;

class NOTSA_EXPORT_VTABLE CTaskSimpleCarCloseDoorFromOutside : public CTaskSimple {
    bool m_animHasFinished = {};                     // 8
    CAnimBlendAssociation* m_anim = {};              // 0xC
    CVehicle* m_veh = {};                            // 0x10
    uint32 m_door = {};                              // 0x14
    CTaskUtilityLineUpPedWithCar* m_lineUpTask = {}; // 0x18

public:
    static void InjectHooks();

    constexpr static auto Type = eTaskType::TASK_SIMPLE_CAR_CLOSE_DOOR_FROM_OUTSIDE;

    CTaskSimpleCarCloseDoorFromOutside(CVehicle* veh, uint32 door, CTaskUtilityLineUpPedWithCar* lineUpTask);
    CTaskSimpleCarCloseDoorFromOutside(const CTaskSimpleCarCloseDoorFromOutside&);
    ~CTaskSimpleCarCloseDoorFromOutside();

    static void FinishAnimCarCloseDoorFromOutsideCB(CAnimBlendAssociation* anim, void* data);

    auto ComputeAnimID_Helper() -> std::tuple<AssocGroupId, AnimationId>;
    void ComputeAnimID(AssocGroupId& outGroup, AnimationId& outAnimId); // NOTSA
    void StartAnim(CPed const* ped);
    void ProcessDoorOpen(CPed const* ped); // NOTSA


    CTask* Clone() const override { return new CTaskSimpleCarCloseDoorFromOutside{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool ProcessPed(CPed* ped) override;
    bool SetPedPosition(CPed* ped) override;

private: // Wrappers for hooks
    // 0x6464F0
    CTaskSimpleCarCloseDoorFromOutside* Constructor(CVehicle* veh, int32 door, CTaskUtilityLineUpPedWithCar* lineUpTask) {
        this->CTaskSimpleCarCloseDoorFromOutside::CTaskSimpleCarCloseDoorFromOutside(veh, door, lineUpTask);
        return this;
    }

    // 0x646570
    CTaskSimpleCarCloseDoorFromOutside* Destructor() {
        this->CTaskSimpleCarCloseDoorFromOutside::~CTaskSimpleCarCloseDoorFromOutside();
        return this;
    }
};
