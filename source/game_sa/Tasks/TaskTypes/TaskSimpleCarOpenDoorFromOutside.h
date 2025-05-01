#pragma once

#include "TaskSimple.h"
#include "AnimationEnums.h"

class CTaskSimpleCarOpenDoorFromOutside;
class CTaskUtilityLineUpPedWithCar;
class CAnimBlendAssociation;
class CPed;
class CVehicle;
class CEvent;

class NOTSA_EXPORT_VTABLE CTaskSimpleCarOpenDoorFromOutside : public CTaskSimple {
public:
    bool m_finished = {};                            // 0x8
    CAnimBlendAssociation* m_anim = {};              // 0xC
    CVehicle* m_veh = {};                            // 0x10
    int32 m_door = {};                               // 0x14
    bool m_disallowPlayerDriverToExitCar = {};       // 0x18
    bool m_hasSetPlayerCanExitCarFlag = {};             // 0x19
    CTaskUtilityLineUpPedWithCar* m_lineUpTask = {}; // 0x1C
    float m_doorOpenAngleRatio = {};                 // 0x20

public:
    static void InjectHooks();

    constexpr static auto Type = eTaskType::TASK_SIMPLE_CAR_OPEN_DOOR_FROM_OUTSIDE;

    CTaskSimpleCarOpenDoorFromOutside(CVehicle* veh, int32 door, CTaskUtilityLineUpPedWithCar* lineUpTask, bool disallowPlayerDriverToExitCar);
    CTaskSimpleCarOpenDoorFromOutside(const CTaskSimpleCarOpenDoorFromOutside&); // NOTSA
    ~CTaskSimpleCarOpenDoorFromOutside();

    static void FinishAnimCarOpenDoorFromOutsideCB(CAnimBlendAssociation* anim, void* data);

    void ComputeAnimID_ToHook(AssocGroupId& grp, AnimationId& id);
    auto ComputeAnimID() -> std::pair<AssocGroupId, AnimationId>;
    void StartAnim(CPed* ped);
    void IfNotAlreadySetPlayerCanExitCarFlag(); // NOTSA?

    CTask* Clone() const override { return new CTaskSimpleCarOpenDoorFromOutside{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool ProcessPed(CPed* ped) override;
    bool SetPedPosition(CPed* ped) override;

private: // Wrappers for hooks
    // 0x645E50
    CTaskSimpleCarOpenDoorFromOutside* Constructor(CVehicle* veh, int32 door, CTaskUtilityLineUpPedWithCar* lineUpTask, bool disallowPlayerDriverToExitCar) {
        this->CTaskSimpleCarOpenDoorFromOutside::CTaskSimpleCarOpenDoorFromOutside(veh, door, lineUpTask, disallowPlayerDriverToExitCar);
        return this;
    }
    // 0x645EE0
    CTaskSimpleCarOpenDoorFromOutside* Destructor() {
        this->CTaskSimpleCarOpenDoorFromOutside::~CTaskSimpleCarOpenDoorFromOutside();
        return this;
    }
};
