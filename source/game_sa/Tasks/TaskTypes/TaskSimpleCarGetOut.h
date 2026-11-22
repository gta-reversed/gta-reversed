#pragma once

#include "TaskSimple.h"

class CVehicle;
class CAnimBlendAssociation;
class CTaskUtilityLineUpPedWithCar;

// VMT: 0x86ee4c | Size: 9
class NOTSA_EXPORT_VTABLE CTaskSimpleCarGetOut : public CTaskSimple {
public:
    bool m_finished{};                            // 8
    bool m_vehHasDoorToOpen{};                    // 9
    CAnimBlendAssociation* m_anim{};              // 0xC
    bool m_isUpsideDown{};                        // 0x10
    CVehicle* m_veh{};                            // 0x14
    uint32 m_door{};                              // 0x18
    CTaskUtilityLineUpPedWithCar* m_taskLineUp{}; // 0x1C

public:
    static void InjectHooks();

    constexpr static auto Type = eTaskType::TASK_SIMPLE_CAR_GET_OUT;

    CTaskSimpleCarGetOut(CVehicle* veh, uint32 door, CTaskUtilityLineUpPedWithCar* taskLineUp);
    CTaskSimpleCarGetOut(const CTaskSimpleCarGetOut&); // NOTSA
    ~CTaskSimpleCarGetOut();

    static void FinishAnimCarGetOutCB(CAnimBlendAssociation* anim, void* data);

    auto ComputeAnimID() -> std::pair<AssocGroupId, AnimationId>; // NOTSA: return type
    void ComputeAnimID_ToHook(AssocGroupId& animGroup, AnimationId& animId);

    void StartAnim(const CPed* ped);

    CTask* Clone() const override { return new CTaskSimpleCarGetOut{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool ProcessPed(CPed* ped) override;
    bool SetPedPosition(CPed* ped) override;

private: // Wrappers for hooks
    // 0x6471D0
    CTaskSimpleCarGetOut* Constructor(CVehicle* veh, int32 door, CTaskUtilityLineUpPedWithCar* taskLineUp) {
        this->CTaskSimpleCarGetOut::CTaskSimpleCarGetOut(veh, door, taskLineUp);
        return this;
    }

    // 0x647260
    CTaskSimpleCarGetOut* Destructor() {
        this->CTaskSimpleCarGetOut::~CTaskSimpleCarGetOut();
        return this;
    }
};
