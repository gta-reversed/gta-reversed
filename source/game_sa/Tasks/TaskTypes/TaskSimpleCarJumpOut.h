#pragma once

#include "TaskSimple.h"

class CAnimBlendAssociation;
class CTaskUtilityLineUpPedWithCar;
class CVehicle;

// VMT: 0x86ee70 | Size: 9
class NOTSA_EXPORT_VTABLE CTaskSimpleCarJumpOut : public CTaskSimple {

public:
    bool m_finishedAnim = {};                        // 8
    CAnimBlendAssociation* m_anim = {};              // 0xC
    CVehicle* m_veh = {};                            // 0x10
    float m_initialVehSpeed = {};                    // 0x14
    uint32 m_door = {};                               // 0x18
    uint8 m_doorFlagsToClear = {};                   // 0x1C
    uint8 m_numGettingInToClear = {};                // 0x1D
    CTaskUtilityLineUpPedWithCar* m_lineUpTask = {}; // 0x20

public:
    static void InjectHooks();

    constexpr static auto Type = eTaskType::TASK_SIMPLE_CAR_JUMP_OUT;

    CTaskSimpleCarJumpOut(CVehicle* veh, uint32 door, CTaskUtilityLineUpPedWithCar* lineUpTask);
    CTaskSimpleCarJumpOut(const CTaskSimpleCarJumpOut&);
    ~CTaskSimpleCarJumpOut();

    static void FinishAnimCarJumpOutCB(CAnimBlendAssociation* anim, void* data);

    void StartAnim(const CPed* ped);
    auto ComputeAnimID() -> std::pair<AssocGroupId, AnimationId>; // NOTSA: Changed return type
    void ComputeAnimID_ToHook(AssocGroupId& animGroup, AnimationId& animId) { std::tie(animGroup, animId) = ComputeAnimID(); } // Hooking this instead

    CTask* Clone() const override { return new CTaskSimpleCarJumpOut{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool ProcessPed(CPed* ped) override;
    bool SetPedPosition(CPed* ped) override;

private: // Wrappers for hooks
    // 0x64BE40
    CTaskSimpleCarJumpOut* Constructor(CVehicle* veh, uint32 door, CTaskUtilityLineUpPedWithCar* lineUpTask) {
        this->CTaskSimpleCarJumpOut::CTaskSimpleCarJumpOut(veh, door, lineUpTask);
        return this;
    }

    // 0x647490
    CTaskSimpleCarJumpOut* Destructor() {
        this->CTaskSimpleCarJumpOut::~CTaskSimpleCarJumpOut();
        return this;
    }
};
